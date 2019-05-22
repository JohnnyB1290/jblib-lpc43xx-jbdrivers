/**
 * @file
 * @brief Ethernet Driver Realization
 *
 *
 * @note
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
 * All rights reserved.
 * @note
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 * @note
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note
 * This file is a part of JB_Lib.
 */

// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <string.h>
#include "lpc_phy.h"
#include "ethernet/EthernetPhy.hpp"
#include "JbController.hpp"

#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
#include <stdio.h>
#endif
#if USE_LWIP
#include "lwip/pbuf.h"
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;

EthernetPhy* EthernetPhy::ethernetPhy_ = (EthernetPhy*)NULL;


EthernetPhy* EthernetPhy::getEthernetPhy(void)
{
	if(EthernetPhy::ethernetPhy_ == (EthernetPhy*)NULL)
		EthernetPhy::ethernetPhy_ = new EthernetPhy();
	return EthernetPhy::ethernetPhy_;
}



EthernetPhy::EthernetPhy(void) : IVoidEthernet(), IIrqListener()
{
	EthernetPhy::mac_[0] = 0x12;
	EthernetPhy::mac_[1] = (std::rand())&0xff;
	EthernetPhy::mac_[2] = (std::rand())&0xff;
	EthernetPhy::mac_[3] = (std::rand())&0xff;
	EthernetPhy::mac_[4] = (std::rand())&0xff;
	EthernetPhy::mac_[5] = (std::rand())&0xff;

#ifndef CORE_M0SUB
	this->setCode((uint64_t)1<<ETHERNET_IRQn);
	IrqController::getIrqController()->addPeripheralIrqListener(this);
#endif
}



void EthernetPhy::irqHandler(int8_t irqNumber)
{
	if(LPC_ETHERNET->DMA_STAT & DMA_ST_AIE)
		this->errorHandler();
	if(LPC_ETHERNET->DMA_STAT & DMA_ST_TI)
		this->sendHandler();
	LPC_ETHERNET->DMA_STAT = DMA_ST_ALL;
}



void EthernetPhy::sendHandler(void)
{
	for(uint32_t i = 0; i < ETHERNET_PHY_TX_QUEUE_SIZE; i++) {
		if(((this->txQueue_.bdTxPtrs[this->txQueue_.br]->CTRLSTAT) & TDES_OWN) == 0) {
			this->txQueue_.sizes[this->txQueue_.br] = 0;
			this->txQueue_.br++;
			if(this->txQueue_.br == ETHERNET_PHY_TX_QUEUE_SIZE)
				this->txQueue_.br = 0;
			if(this->txQueue_.bw == this->txQueue_.br)
				break;
		}
		else
			break;
	}
}



uint16_t EthernetPhy::pullOutRxFrame(EthernetFrame* const frame)
{
	if((this->bdsRx_[this->rxBdIndex_].STATUS) & RDES_OWN)
		return 0;

	uint16_t size = 0;
	while(((this->bdsRx_[this->rxBdIndex_].STATUS) & RDES_OWN) == 0) {
		if( (this->bdsRx_[this->rxBdIndex_].STATUS & RDES_FS) &&
				(this->bdsRx_[this->rxBdIndex_].STATUS & RDES_LS)) {
			this->outFrameSize_ = 0;
			size = RDES_FLMSK(this->bdsRx_[this->rxBdIndex_].STATUS);
			memcpy(frame, (uint8_t*)this->rxBdBuffers_[this->rxBdIndex_], size);
			this->bdsRx_[this->rxBdIndex_].STATUS = RDES_OWN;
			this->rxBdIndex_++;
			if(this->rxBdIndex_ == ETHERNET_PHY_RX_QUEUE_SIZE)
				this->rxBdIndex_ = 0;
			return size;
		}

		if(this->bdsRx_[this->rxBdIndex_].STATUS & RDES_FS)
			this->outFrameSize_ = 0;

		size = RDES_FLMSK(this->bdsRx_[this->rxBdIndex_].STATUS) - this->outFrameSize_;

		if((this->outFrameSize_ + size) > EMAC_ETH_MAX_FLEN)
			this->outFrameSize_ = 0;
		else {
			memcpy(&this->outFrame_[this->outFrameSize_], this->rxBdBuffers_[this->rxBdIndex_], size);
			this->outFrameSize_ = this->outFrameSize_ + size;

			if(this->bdsRx_[this->rxBdIndex_].STATUS & RDES_LS) {
				if(this->outFrameSize_ == RDES_FLMSK(this->bdsRx_[this->rxBdIndex_].STATUS)) {
					memcpy(frame, this->outFrame_, this->outFrameSize_);
					this->bdsRx_[this->rxBdIndex_].STATUS = RDES_OWN;
					this->rxBdIndex_++;
					if(this->rxBdIndex_ == ETHERNET_PHY_RX_QUEUE_SIZE)
						this->rxBdIndex_ = 0;
					return this->outFrameSize_;
				}
				else
					this->outFrameSize_ = 0;
			}
		}

		this->bdsRx_[this->rxBdIndex_].STATUS = RDES_OWN;
		this->rxBdIndex_++;
		if(this->rxBdIndex_ == ETHERNET_PHY_RX_QUEUE_SIZE)
			this->rxBdIndex_ = 0;
	}

	return 0;
}



void EthernetPhy::addToTxQueue(EthernetFrame* const frame, uint16_t frameSize)
{
	if(frameSize){
		if(this->isTxUnlocked_) {
			__disable_irq();
			if(!this->isTxQueueFull()) {
				memcpy((uint8_t*)&(this->txQueue_.frames[this->txQueue_.bw]), frame, frameSize);
				this->txQueue_.bdTxPtrs[this->txQueue_.bw]->BSIZE = frameSize;
				this->txQueue_.sizes[this->txQueue_.bw] = frameSize;
				this->txQueue_.bdTxPtrs[this->txQueue_.bw]->CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC|TDES_OWN;
				this->txQueue_.bw++;
				if(this->txQueue_.bw == ETHERNET_PHY_TX_QUEUE_SIZE)
					this->txQueue_.bw = 0;
				LPC_ETHERNET->DMA_TRANS_POLL_DEMAND = 1;
			}
			else {
				#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
				printf("Ethernet PHY: Error! TX Queue is FULL\n\r");
				#endif
			}
			__enable_irq();
		}
	}
}



#if USE_LWIP
void EthernetPhy::addToTxQueue(struct pbuf* p)
{
	if(this->isTxUnlocked_ && (p != NULL)) {
		uint16_t size = p->tot_len;
		if(size == 0)
			return;

		__disable_irq();
		if(!this->isTxQueueFull()) {
			if(p->next != NULL) {
				uint16_t frameIndex = 0;
				while (p != NULL) {
					memcpy((uint8_t*)&(this->txQueue_.frames[this->txQueue_.bw][frameIndex]), p->payload, p->len);
					frameIndex += p->len;
				    p = p->next;
				}
			}
			else
				memcpy((uint8_t*)&(this->txQueue_.frames[this->txQueue_.bw]), p->payload, p->tot_len);

			this->txQueue_.bdTxPtrs[this->txQueue_.bw]->BSIZE = size;
			this->txQueue_.sizes[this->txQueue_.bw] = size;
			this->txQueue_.bdTxPtrs[this->txQueue_.bw]->CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC|TDES_OWN;
			this->txQueue_.bw++;
			if(this->txQueue_.bw == ETHERNET_PHY_TX_QUEUE_SIZE)
				this->txQueue_.bw = 0;
			LPC_ETHERNET->DMA_TRANS_POLL_DEMAND = 1;
		}
		else {
			#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
			printf("Ethernet PHY: Error! TX Queue is FULL\n\r");
			#endif
		}
		__enable_irq();
	}
}
#endif



bool EthernetPhy::isTxQueueFull(void)
{
	return (this->txQueue_.bdTxPtrs[this->txQueue_.bw]->CTRLSTAT) & TDES_OWN;
}



void EthernetPhy::initialize(void)
{
	#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
	printf("Start Ethernet PHY Initialization\n\r");
	#endif
	this->dmaErrorsCounter_ = 0;

#if ETHERNET_PHY_USE_RESET_PIN
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RESET_PORT, ETHERNET_PHY_RESET_PIN, ETHERNET_PHY_RESET_SCU_MODE);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PIN, (bool) true);
#endif

	Chip_SCU_PinMuxSet(ETHERNET_PHY_TXD0_PORT, ETHERNET_PHY_TXD0_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETHERNET_PHY_TXD0_SCU_MODE); /* TXD0 */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_TXD1_PORT, ETHERNET_PHY_TXD1_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETHERNET_PHY_TXD1_SCU_MODE); /* TXD1 */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_TXEN_PORT, ETHERNET_PHY_TXEN_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETHERNET_PHY_TXEN_SCU_MODE); /* TXEN */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RXD0_PORT, ETHERNET_PHY_RXD0_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RXD0_SCU_MODE); /* RXD0 */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RXD1_PORT, ETHERNET_PHY_RXD1_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RXD1_SCU_MODE); /* RXD1 */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_CRS_DV_PORT, ETHERNET_PHY_CRS_DV_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_CRS_DV_SCU_MODE); /* CRS_DV */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_MDIO_PORT, ETHERNET_PHY_MDIO_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_MDIO_SCU_MODE); /* MDIO */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_MDC_PORT, ETHERNET_PHY_MDC_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETHERNET_PHY_MDC_SCU_MODE); /* MDC */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_REF_CLK_PORT, ETHERNET_PHY_REF_CLK_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_REF_CLK_SCU_MODE); /* REF_CLK */

#if ETHERNET_PHY_USE_MII
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RX_ER_PORT, ETHERNET_PHY_RX_ER_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RX_ER_SCU_MODE); /* RX_ER */
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RXDV_PORT, ETHERNET_PHY_RXDV_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RXDV_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RXD2_PORT, ETHERNET_PHY_RXD2_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RXD2_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RXD3_PORT, ETHERNET_PHY_RXD3_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RXD3_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_RXCLK_PORT, ETHERNET_PHY_RXCLK_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_RXCLK_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_TXD2_PORT, ETHERNET_PHY_TXD2_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_TXD2_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_TXD3_PORT, ETHERNET_PHY_TXD3_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_TXD3_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_TX_ER_PORT, ETHERNET_PHY_TX_ER_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_TX_ER_SCU_MODE);
	Chip_SCU_PinMuxSet(ETHERNET_PHY_COL_PORT, ETHERNET_PHY_COL_PIN,
			SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETHERNET_PHY_COL_SCU_MODE);
#endif

	// Enable Clocks
	Chip_Clock_SetBaseClock(CLK_BASE_PHY_TX, CLKIN_ENET_TX, true, false);
	Chip_Clock_SetBaseClock(CLK_BASE_PHY_RX, CLKIN_ENET_TX, true, false);
	Chip_Clock_EnableOpts(CLK_MX_ETHERNET, true, true, 1);

#if ETHERNET_PHY_USE_RESET_PIN
	//PHY Reset
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PIN);
	JbController::delayUs(200);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PORT, ETHERNET_PHY_RESET_GPIO_PIN);
	JbController::delayMs(1);
#endif
	
	// Find PHY
#if ETHERNET_PHY_USE_MII
	Chip_ENET_MIIEnable(LPC_ETHERNET);
#else
	Chip_ENET_RMIIEnable(LPC_ETHERNET);
#endif

	uint32_t clkMhz = SystemCoreClock / 1000000UL;
	uint32_t mdcclk = 0;

	if (clkMhz >= 20 && clkMhz < 35)
		mdcclk =  2;
	else if (clkMhz >= 35 && clkMhz < 60)
		mdcclk = 3;
	else if (clkMhz >= 60 && clkMhz < 100)
		mdcclk = 0;
	else if (clkMhz >= 100 && clkMhz < 150)
		mdcclk = 1;
	else if (clkMhz >= 150 && clkMhz < 250)
		mdcclk = 4;
	else if (clkMhz >= 250 && clkMhz < 300)
		mdcclk = 5;

	uint8_t phyAddr = 0;
	for(uint8_t i = 0; i < 32; i++) {
		uint32_t phyId = 0;
		phyAddr = i;
		Chip_ENET_SetupMII(LPC_ETHERNET, mdcclk, phyAddr);
		phyId = (this->rmiiRead(0x02))<<8;
		phyId |= (this->rmiiRead(0x03)>>10);
		phyId = phyId&0xffff;
		
		if(phyId == 0x0730) {
			this->phyType_ = PHY_TYPE_LAN8720;
			#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
			printf("Ethernet PHY LAN8720 Founded!\n\r");
			#endif
			break;
		}
		else if(phyId == 0x0017) {
			this->phyType_ = PHY_TYPE_DP83848M;
			#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
			printf("Ethernet PHY DP83848M Founded!\n\r");
			#endif
			break;
		}
	}
	if(phyAddr == 31) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Ethernet PHY Don't fond\n\r");
		#endif
		return;
	}

	//PHY Initialization
	this->phyStatus_ = lpc_phy_init(true, JbController::delayMs);
	switch (this->speed_) {
		case SPEED_AUTONEG:
			this->phyStatus_ = lpc_mii_write(LAN8_BCR_REG, LAN8_AUTONEG);
			break;
		case SPEED_10_M:
			this->phyStatus_ = lpc_mii_write(LAN8_BCR_REG, LAN8_DUPLEX_MODE);
			break;
		case SPEED_100_M:
			this->phyStatus_ = lpc_mii_write(LAN8_BCR_REG, LAN8_DUPLEX_MODE|LAN8_SPEED_SELECT);
			break;
		default:
			this->phyStatus_ = lpc_mii_write(LAN8_BCR_REG, LAN8_AUTONEG);
			break;
	}

#if !ETHERNET_PHY_USE_MII
	if(this->phyType_ == DP83848M)
		lpc_mii_write(DP8_PHY_RBR_REG, DP8_RBR_RMII_MODE);
#endif

	if(this->phyStatus_ == ERROR) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! PHY Initialization\n\r");
		#endif
		return;
	}

	this->txQueue_.br = 0;
	this->txQueue_.bw = 0;

	this->initializeDescriptors();
	
	//Reset Ethernet module
	Chip_RGU_TriggerReset(RGU_ETHERNET_RST);
	while (Chip_RGU_InReset(RGU_ETHERNET_RST));

	Chip_ENET_Reset(LPC_ETHERNET);
	JbController::delayMs(1);
	
#if ETHERNET_PHY_USE_MII
	Chip_ENET_MIIEnable(LPC_ETHERNET);
#else
	Chip_ENET_RMIIEnable(LPC_ETHERNET);
#endif

	Chip_ENET_SetupMII(LPC_ETHERNET, mdcclk, phyAddr);
	
	Chip_ENET_InitDescriptors(LPC_ETHERNET, (ENET_ENHTXDESC_T*) &this->bdsTx_, (ENET_ENHRXDESC_T*) &this->bdsRx_);
	
	Chip_ENET_SetADDR(LPC_ETHERNET, (uint8_t*)EthernetPhy::mac_);

	/* Burst length = 1 */
	LPC_ETHERNET->DMA_BUS_MODE = DMA_BM_PBL(1) | DMA_BM_RPBL(1);
	/* Initial MAC configuration for checksum offload, full duplex,
	   100Mbps, inter-frame gap 72 bit times
	   of 64-bits */
	LPC_ETHERNET->MAC_CONFIG = MAC_CFG_ACS/*| MAC_CFG_IPC*/ | MAC_CFG_DM | /*MAC_CFG_FES |*/ MAC_CFG_IFG(3);

	if(this->speed_ != SPEED_10_M)
		LPC_ETHERNET->MAC_CONFIG |= MAC_CFG_FES;

	/* Promiscuous Mode */
	LPC_ETHERNET->MAC_FRAME_FILTER = MAC_FF_PR;
	/* Flush transmit FIFO */
	LPC_ETHERNET->DMA_OP_MODE = DMA_OM_FTF;
	/* Setup DMA to flush receive FIFOs at 32 bytes, service TX FIFOs at
	   64 bytes */
	LPC_ETHERNET->DMA_OP_MODE |= DMA_OM_RTC(1) | DMA_OM_TTC(0);
	/* Clear all MAC interrupts */
	LPC_ETHERNET->DMA_STAT = DMA_ST_ALL;
	/* Enable MAC interrupts */
	LPC_ETHERNET->DMA_INT_EN = DMA_IE_TIE|DMA_IE_TSE|DMA_IE_TJE|DMA_IE_OVE|DMA_IE_UNE|DMA_IE_RUE|DMA_IE_RSE|DMA_IE_RWE|DMA_IE_ETE|DMA_IE_FBE|DMA_IE_AIE|DMA_IE_NIE;

	#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(ETHERNET_IRQn, NVIC_EncodePriority(prioritygroup, ETHERNET_PHY_INTERRUPT_PRIORITY, 0));
	#endif

	#ifndef CORE_M0SUB
	#ifdef CORE_M0
	NVIC_SetPriority(ETHERNET_IRQn, ETHERNET_PHY_INTERRUPT_PRIORITY);
	#endif
	NVIC_ClearPendingIRQ(ETHERNET_IRQn);
	NVIC_EnableIRQ(ETHERNET_IRQn);
	#endif
	
	this->isInitialized_ = true;
	#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
	printf("Ethernet PHY Initialization Done!\n\r");
	#endif
}



void EthernetPhy::start(void)
{
	Chip_ENET_TXEnable(LPC_ETHERNET);
	Chip_ENET_RXEnable(LPC_ETHERNET);
	Chip_ENET_TXStart(LPC_ETHERNET);
	Chip_ENET_RXStart(LPC_ETHERNET);
}



void EthernetPhy::reset(void)
{
	#ifndef CORE_M0SUB
	NVIC_ClearPendingIRQ(ETHERNET_IRQn);
	NVIC_DisableIRQ(ETHERNET_IRQn);
	#endif
	this->isInitialized_ = false;
	#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
	printf("Ethernet PHY Reset!\n\r");
	#endif
	this->initialize();
	this->start();
}



void EthernetPhy::getParameter(const uint8_t number, void* const value)
{
	if(number == PARAMETER_MAC)
		*((uint8_t**)value) = EthernetPhy::mac_;
	else if(number == PARAMETER_LINK)
		*((uint8_t*)value) = this->checkLink();
	else if(number == PARAMETER_TX_UNLOCK)
		*((uint8_t*)value) = this->isTxUnlocked_;
	else if(number == PARAMETER_NAME)
		*((char**)value) = (char*)this->name_;
}



void EthernetPhy::setParameter(const uint8_t number, void* const value)
{
	if(number == PARAMETER_MAC)
		memcpy((char*)(&(EthernetPhy::mac_[0])), value, 6);
	else if(number == PARAMETER_TX_UNLOCK)
		this->isTxUnlocked_ = *((bool*)value);
	else if(number == PARAMETER_NAME)
		memcpy(this->name_, value, 9);
	else if(number == PARAMETER_SPEED)
		this->speed_ = *((IVoidEthernetSpeed_t*)value);
}



bool EthernetPhy::checkLink(void)
{
	this->phyStatus_ = lpcPHYStsPoll();
	if( (this->phyStatus_ & PHY_LINK_BUSY) == 0 ) {
		if((this->phyStatus_ & PHY_LINK_CONNECTED) == 0) {
			#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
			printf("Ethernet PHY: NO LINK \n\r");
			#endif
			return false;
		}
		else {
			#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
			printf("Ethernet PHY: LINK UP\n\r");
			#endif
			if((this->phyStatus_ & PHY_LINK_FULLDUPLX) == 0) {
				#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
				printf("HALF-DUPLEX ");
				#endif
				Chip_ENET_SetDuplex(LPC_ETHERNET, false);
			}
			else {
				#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
				printf("FULL DUPLEX ");
				#endif
				Chip_ENET_SetDuplex(LPC_ETHERNET, true);
			}
			if((this->phyStatus_ & PHY_LINK_SPEED100) == 0) {
				#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
				printf("10 Mb/s\n\r");
				#endif
				Chip_ENET_SetSpeed(LPC_ETHERNET, false);
			}
			else {
				#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
				printf("100 Mb/s\n\r");
				#endif
				Chip_ENET_SetSpeed(LPC_ETHERNET, true);
			}
			return true;
		}
	}
	return false;
}



uint16_t EthernetPhy::rmiiRead(uint8_t reg)
{
	int32_t mst = 250;

	Chip_ENET_StartMIIRead(LPC_ETHERNET, reg);
	/* Wait for unbusy status */
	while (mst > 0) {
		if (!Chip_ENET_IsMIIBusy(LPC_ETHERNET)) {
			mst = 0;
			return Chip_ENET_ReadMIIData(LPC_ETHERNET);
		}
		else {
			mst--;
			JbController::delayMs(1);
		}
	}
	return 0;
}



void EthernetPhy::initializeTxDescriptors(void)
{
	for(uint32_t i = 0; i < (ETHERNET_PHY_TX_QUEUE_SIZE-1); i++) {
		this->bdsTx_[i].B1ADD = (uint32_t)(this->txQueue_.frames[i]);
		this->bdsTx_[i].B2ADD = (uint32_t)(&(this->bdsTx_[i+1]));
		this->bdsTx_[i].BSIZE = 0;
		this->bdsTx_[i].CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC;
		this->txQueue_.bdTxPtrs[i] = (ENET_TXDESC_T*)&(this->bdsTx_[i]);
		this->txQueue_.sizes[i] = 0;
	}
	this->bdsTx_[(ETHERNET_PHY_TX_QUEUE_SIZE-1)].B1ADD =
			(uint32_t)(this->txQueue_.frames[(ETHERNET_PHY_TX_QUEUE_SIZE-1)]);
	this->bdsTx_[(ETHERNET_PHY_TX_QUEUE_SIZE-1)].B2ADD = (uint32_t)(&(this->bdsTx_[0]));
	this->bdsTx_[(ETHERNET_PHY_TX_QUEUE_SIZE-1)].BSIZE = 0;
	this->bdsTx_[(ETHERNET_PHY_TX_QUEUE_SIZE-1)].CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC;
	this->txQueue_.bdTxPtrs[(ETHERNET_PHY_TX_QUEUE_SIZE-1)] = (ENET_TXDESC_T*)&(this->bdsTx_[(ETHERNET_PHY_TX_QUEUE_SIZE-1)]);
	this->txQueue_.sizes[(ETHERNET_PHY_TX_QUEUE_SIZE-1)] = 0;
}



void EthernetPhy::initializeRxDescriptors(void)
{
	for(uint32_t i = 0; i < (ETHERNET_PHY_RX_QUEUE_SIZE-1); i++) {
		this->bdsRx_[i].STATUS = RDES_OWN;
		this->bdsRx_[i].CTRL = RDES_ENH_RCH | ETHERNET_PHY_RX_DESCRIPTOR_BUFFER_SIZE;
		this->bdsRx_[i].B1ADD = (uint32_t)(this->rxBdBuffers_[i]);
		this->bdsRx_[i].B2ADD = (uint32_t)(&(this->bdsRx_[i+1]));
	}
	this->bdsRx_[(ETHERNET_PHY_RX_QUEUE_SIZE-1)].STATUS = RDES_OWN;
	this->bdsRx_[(ETHERNET_PHY_RX_QUEUE_SIZE-1)].CTRL = RDES_ENH_RCH|ETHERNET_PHY_RX_DESCRIPTOR_BUFFER_SIZE;
	this->bdsRx_[(ETHERNET_PHY_RX_QUEUE_SIZE-1)].B1ADD = (uint32_t)(this->rxBdBuffers_[(ETHERNET_PHY_RX_QUEUE_SIZE-1)]);
	this->bdsRx_[(ETHERNET_PHY_RX_QUEUE_SIZE-1)].B2ADD = (uint32_t)(&(this->bdsRx_[0]));
	this->rxBdIndex_ = 0;
}



void EthernetPhy::initializeDescriptors(void)
{
	this->initializeTxDescriptors();
	this->initializeRxDescriptors();
}



void EthernetPhy::errorHandler(void)
{
#if ETHERNET_PHY_USE_DMA_ERRORS_CONTROL
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_ETI) == 0) {
		this->dmaErrorsCounter_++;
		if(this->dmaErrorsCounter_ >= ETHERNET_PHY_DMA_ERRORS_CONTROL_TRESHOLD)
			this->reset();
	}
#endif
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_TPS) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Transmit process stopped\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_TJT) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Transmit jabber timeout\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_OVF) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Receive overflow\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_UNF) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Transmit underflow\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_RU) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Receive buffer unavailable\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_RPS) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Received process stopped\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_RWT) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Receive watchdog timeout\n\r");
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_FBI) != 0) {
		#if (USE_CONSOLE && ETHERNET_PHY_USE_CONSOLE)
		printf("Ethernet PHY: Error! Fatal bus error interrupt\n\r");
		#endif
	}
}

}
