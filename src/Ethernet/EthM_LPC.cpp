/*
 * EthM_LPC.cpp
 *
 *  Created on: 13 сен. 2017 г.
 *      Author: Stalker1290
 */

#include "Ethernet/EthM_LPC.hpp"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "lpc_phy.h"
#include "CONTROLLER.hpp"

#ifdef USE_LWIP
#include "lwip/pbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static void _delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

Eth_phy_t* Eth_phy_t::Eth_phy_ptr = (Eth_phy_t*)NULL;
uint8_t Eth_phy_t::EmacPsMAC[6] = ETH_PHY_DEFAULT_MAC;

Eth_phy_t* Eth_phy_t::get_Ethernet_phy(void)
{
	if(Eth_phy_t::Eth_phy_ptr == (Eth_phy_t*)NULL) Eth_phy_t::Eth_phy_ptr = new Eth_phy_t();
	return Eth_phy_t::Eth_phy_ptr;
}

Eth_phy_t::Eth_phy_t(void):Ethernet_t(),Callback_Interface_t(),IRQ_LISTENER_t()
{
	this->Rx_Bd_index = 0;
	this->Out_Frame_size = 0;
	this->Eth_DMA_errors_counter = 0;
	this->Initialized = 0;
	this->speed = Eth_phy_Default_speed;
	this->Tx_Unlocked = 1;
	this->PHY_Status = 0;
	this->PHY_type = Undefined;
	memcpy(this->Adapter_name, "Eth_phy ",9);
	this->setCode((uint64_t)1<<ETHERNET_IRQn);
	IRQ_CONTROLLER_t::getIRQController().Add_Peripheral_IRQ_Listener(this);
}

void Eth_phy_t::IRQ(int8_t IRQ_num)
{
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_AIE) != 0) this->ErrorHandler();
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_NIS) != 0)
	{
		#ifdef ETH_ACT_LED
		if((LPC_ETHERNET->DMA_STAT & DMA_ST_TI) || (LPC_ETHERNET->DMA_STAT & DMA_ST_RI)) CONTROLLER_t::get_CONTROLLER()->GPIO_TGL(ETH_ACT_LED);
		#endif

		if((LPC_ETHERNET->DMA_STAT & DMA_ST_TI) != 0) this->SendHandler();
	}
	LPC_ETHERNET->DMA_STAT = DMA_ST_ALL;
}


void Eth_phy_t::SendHandler(void)
{
	uint8_t i;
	for(i=0; i<Eth_phy_Tx_Queue_Len;i++)
	{
		if(((this->TxQueue.BdTxPtr[this->TxQueue.br]->CTRLSTAT)&TDES_OWN) == 0)
		{
			this->TxQueue.frame_size[this->TxQueue.br] = 0;
			this->TxQueue.br++;
			if(this->TxQueue.br == Eth_phy_Tx_Queue_Len) this->TxQueue.br = 0;
			if(this->TxQueue.bw == this->TxQueue.br) break;
		}
		else break;
	}
}

uint16_t Eth_phy_t::Pull_out_RX_Frame(EthernetFrame* Frame)
{
	uint16_t size = 0;

	if((this->BdRx[this->Rx_Bd_index].STATUS)&RDES_OWN) return 0;

	while(((this->BdRx[this->Rx_Bd_index].STATUS)&RDES_OWN) == 0)
	{
		if( (this->BdRx[this->Rx_Bd_index].STATUS & RDES_FS) && (this->BdRx[this->Rx_Bd_index].STATUS & RDES_LS))
		{
			this->Out_Frame_size = 0;
			size = RDES_FLMSK(this->BdRx[this->Rx_Bd_index].STATUS);
			memcpy(Frame,(uint8_t*)this->RxBd_buf[this->Rx_Bd_index],size);
			this->BdRx[this->Rx_Bd_index].STATUS = RDES_OWN;
			this->Rx_Bd_index++;
			if(this->Rx_Bd_index == Eth_phy_Rx_Queue_Len) this->Rx_Bd_index = 0;
			return size;
		}

		if(this->BdRx[this->Rx_Bd_index].STATUS & RDES_FS) this->Out_Frame_size = 0;

		size = RDES_FLMSK(this->BdRx[this->Rx_Bd_index].STATUS) - this->Out_Frame_size;

		if((this->Out_Frame_size + size) > EMAC_ETH_MAX_FLEN) this->Out_Frame_size = 0;
		else
		{
			memcpy(&this->Out_Frame[this->Out_Frame_size],this->RxBd_buf[this->Rx_Bd_index],size);
			this->Out_Frame_size = this->Out_Frame_size + size;

			if(this->BdRx[this->Rx_Bd_index].STATUS & RDES_LS)
			{
				if(this->Out_Frame_size == RDES_FLMSK(this->BdRx[this->Rx_Bd_index].STATUS))
				{
					memcpy(Frame,this->Out_Frame,this->Out_Frame_size);
					this->BdRx[this->Rx_Bd_index].STATUS = RDES_OWN;
					this->Rx_Bd_index++;
					if(this->Rx_Bd_index == Eth_phy_Rx_Queue_Len) this->Rx_Bd_index = 0;
					return this->Out_Frame_size;
				}
				else this->Out_Frame_size = 0;
			}
		}

		this->BdRx[this->Rx_Bd_index].STATUS = RDES_OWN;
		this->Rx_Bd_index++;
		if(this->Rx_Bd_index == Eth_phy_Rx_Queue_Len) this->Rx_Bd_index = 0;
	}

	return 0;
}

void Eth_phy_t::Add_to_TX_queue(EthernetFrame* mes,uint16_t m_size)
{
	if(m_size == 0) return;
	if(this->Tx_Unlocked)
	{
		__disable_irq();
		if(this->Check_if_TX_queue_not_full())
		{
			memcpy((uint8_t*)&(this->TxQueue.Frames[this->TxQueue.bw]),mes,m_size);
			this->TxQueue.BdTxPtr[this->TxQueue.bw]->BSIZE = m_size;
			this->TxQueue.frame_size[this->TxQueue.bw] = m_size;
			this->TxQueue.BdTxPtr[this->TxQueue.bw]->CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC|TDES_OWN;
			this->TxQueue.bw++;
			if(this->TxQueue.bw == Eth_phy_Tx_Queue_Len) this->TxQueue.bw = 0;
			LPC_ETHERNET->DMA_TRANS_POLL_DEMAND = 1;
		}
		else
		{
			#ifdef USE_CONSOLE
			#ifdef EthM_console
			printf("ERROR! Ethernet: TxQueue is FULL\n\r");
			#endif
			#endif
		}
		__enable_irq();
	}
}
#ifdef USE_LWIP
void Eth_phy_t::Add_to_TX_queue(struct pbuf* p)
{
	uint16_t frame_index = 0;
	uint16_t m_size = 0;

	if(p == NULL) return;
	m_size = p->tot_len;

	if(m_size == 0) return;

	if(this->Tx_Unlocked)
	{
		__disable_irq();
		if(this->Check_if_TX_queue_not_full())
		{
			if(p->next != NULL)
			{
				while (p != NULL)
				{
					memcpy((uint8_t*)&(this->TxQueue.Frames[this->TxQueue.bw][frame_index]),p->payload,p->len);
					frame_index += p->len;
				    p = p->next;
				}
			}
			else
			{
				memcpy((uint8_t*)&(this->TxQueue.Frames[this->TxQueue.bw]),p->payload,p->tot_len);
			}

			this->TxQueue.BdTxPtr[this->TxQueue.bw]->BSIZE = m_size;
			this->TxQueue.frame_size[this->TxQueue.bw] = m_size;
			this->TxQueue.BdTxPtr[this->TxQueue.bw]->CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC|TDES_OWN;
			this->TxQueue.bw++;
			if(this->TxQueue.bw == Eth_phy_Tx_Queue_Len) this->TxQueue.bw = 0;
			LPC_ETHERNET->DMA_TRANS_POLL_DEMAND = 1;
		}
		else
		{
			#ifdef USE_CONSOLE
			#ifdef EthM_console
			printf("ERROR! Ethernet: TxQueue is FULL\n\r");
			#endif
			#endif
		}
		__enable_irq();
	}
}
#endif

uint8_t Eth_phy_t::Check_if_TX_queue_not_full(void)
{
	if(((this->TxQueue.BdTxPtr[this->TxQueue.bw]->CTRLSTAT)&TDES_OWN) == 0) return 1;
	return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

static void _delay_ms(uint32_t ms)
{
	CONTROLLER_t::get_CONTROLLER()->_delay_ms(ms);
}

#ifdef __cplusplus
}
#endif


void Eth_phy_t::Initialize(void)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif
	static uint8_t i;
	uint32_t PHY_ID = 0;
	uint32_t clk_mhz = 0;
	uint32_t mdcclk = 0;


	#ifdef USE_CONSOLE
	#ifdef EthM_console
	printf("Start Ethernet_Physical Initialization\n\r");
	#endif
	#endif

	this->Eth_DMA_errors_counter = 0;

	//PinMux ini
#ifdef USE_PHY_RESET_PIN
	Chip_SCU_PinMuxSet(ETH_PHY_RESET_PORT, ETH_PHY_RESET_PIN, ETH_PHY_RESET_MODE_FUNC);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, ETH_PHY_RESET_GPIO_PORT, ETH_PHY_RESET_GPIO_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, ETH_PHY_RESET_GPIO_PORT, ETH_PHY_RESET_GPIO_PIN, (bool) true);
#endif

	Chip_SCU_PinMuxSet(ETH_TXD0_PORT, ETH_TXD0_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETH_TXD0_MODE_FUNC); /* TXD0 */
	Chip_SCU_PinMuxSet(ETH_TXD1_PORT, ETH_TXD1_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETH_TXD1_MODE_FUNC); /* TXD1 */
	Chip_SCU_PinMuxSet(ETH_TXEN_PORT, ETH_TXEN_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETH_TXEN_MODE_FUNC); /* TXEN */
	Chip_SCU_PinMuxSet(ETH_RXD0_PORT, ETH_RXD0_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETH_RXD0_MODE_FUNC); /* RXD0 */
	Chip_SCU_PinMuxSet(ETH_RXD1_PORT, ETH_RXD1_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETH_RXD1_MODE_FUNC); /* RXD1 */
	Chip_SCU_PinMuxSet(ETH_CRS_DV_PORT, ETH_CRS_DV_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETH_CRS_DV_MODE_FUNC); /* CRS_DV */
	Chip_SCU_PinMuxSet(ETH_MDIO_PORT, ETH_MDIO_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETH_MDIO_MODE_FUNC); /* MDIO */
	Chip_SCU_PinMuxSet(ETH_MDC_PORT, ETH_MDC_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | ETH_MDC_MODE_FUNC); /* MDC */
	Chip_SCU_PinMuxSet(ETH_REF_CLK_PORT, ETH_REF_CLK_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | ETH_REF_CLK_MODE_FUNC); /* REF_CLK */

#ifdef USE_MII
	Chip_SCU_PinMuxSet(ETH_RX_ER_PORT, ETH_RX_ER_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_RX_ER_MODE_FUNC); /* RX_ER */
	Chip_SCU_PinMuxSet(ETH_RXDV_PORT, ETH_RXDV_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_RXDV_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_RXD2_PORT, ETH_RXD2_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_RXD2_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_RXD3_PORT, ETH_RXD3_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_RXD3_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_RXCLK_PORT, ETH_RXCLK_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_RXCLK_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_TXD2_PORT, ETH_TXD2_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_TXD2_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_TXD3_PORT, ETH_TXD3_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_TXD3_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_TX_ER_PORT, ETH_TX_ER_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_TX_ER_MODE_FUNC);
	Chip_SCU_PinMuxSet(ETH_COL_PORT, ETH_COL_PIN, SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
			ETH_COL_MODE_FUNC);
#endif

	// Enable Clocks
	Chip_Clock_SetBaseClock(CLK_BASE_PHY_TX, CLKIN_ENET_TX, true, false);
	Chip_Clock_SetBaseClock(CLK_BASE_PHY_RX, CLKIN_ENET_TX, true, false);
	Chip_Clock_EnableOpts(CLK_MX_ETHERNET, true, true, 1);
#ifdef USE_PHY_RESET_PIN
	//PHY Reset
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, ETH_PHY_RESET_GPIO_PORT, ETH_PHY_RESET_GPIO_PIN);
	CONTROLLER_t::get_CONTROLLER()->_delay_us(200);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, ETH_PHY_RESET_GPIO_PORT, ETH_PHY_RESET_GPIO_PIN);
	CONTROLLER_t::get_CONTROLLER()->_delay_ms(1);
#endif
	
	// Find PHY
#ifdef USE_RMII
	Chip_ENET_RMIIEnable(LPC_ETHERNET);
#endif
#ifdef USE_MII
	Chip_ENET_MIIEnable(LPC_ETHERNET);
#endif

	clk_mhz = SystemCoreClock / 1000000UL;
	if (clk_mhz >= 20 && clk_mhz < 35)
		mdcclk =  2;
	if (clk_mhz >= 35 && clk_mhz < 60)
		mdcclk = 3;
	if (clk_mhz >= 60 && clk_mhz < 100)
		mdcclk = 0;
	if (clk_mhz >= 100 && clk_mhz < 150)
		mdcclk = 1;
	if (clk_mhz >= 150 && clk_mhz < 250)
		mdcclk = 4;
	if (clk_mhz >= 250 && clk_mhz < 300)
		mdcclk = 5;

	for(i=0; i<32; i++)
	{
		PHY_ID = 0;
		Chip_ENET_SetupMII(LPC_ETHERNET, mdcclk, i);
		PHY_ID = (this->lpc_rmii_read(0x02))<<8;
		PHY_ID |= (this->lpc_rmii_read(0x03)>>10);
		PHY_ID = PHY_ID&0xffff;
		
		if(PHY_ID == 0x0730)
		{
			this->PHY_type = LAN8720;
			#ifdef USE_CONSOLE
			#ifdef EthM_console
			printf("Ethernet PHY LAN8720 Founded!\n\r");
			#endif
			#endif
			break;
		}
		if(PHY_ID == 0x0017)
		{
			this->PHY_type = DP83848M;
			#ifdef USE_CONSOLE
			#ifdef EthM_console
			printf("Ethernet PHY DP83848M Founded!\n\r");
			#endif
			#endif
			break;
		}
	}
	if(i == 31)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet PHY Don't fond\n\r");
		#endif
		#endif
		return;
	}


	//PHY Initialization
	this->PHY_Status = lpc_phy_init(true, _delay_ms);
	switch (this->speed)
	{
		case speed_autoneg:
			if(this->PHY_type == LAN8720) this->PHY_Status = lpc_mii_write(LAN8_BCR_REG, LAN8_AUTONEG);
			if(this->PHY_type == DP83848M) this->PHY_Status = lpc_mii_write(DP8_BMCR_REG, DP8_AUTONEG);
			break;
		case speed_10Mbit:
			if(this->PHY_type == LAN8720) this->PHY_Status = lpc_mii_write(LAN8_BCR_REG, LAN8_DUPLEX_MODE);
			if(this->PHY_type == DP83848M) this->PHY_Status = lpc_mii_write(DP8_BMCR_REG, DP8_DUPLEX_MODE);
			break;
		case speed_100Mbit:
			if(this->PHY_type == LAN8720) this->PHY_Status = lpc_mii_write(LAN8_BCR_REG, LAN8_DUPLEX_MODE|LAN8_SPEED_SELECT);
			if(this->PHY_type == DP83848M) this->PHY_Status = lpc_mii_write(DP8_BMCR_REG, DP8_DUPLEX_MODE|DP8_SPEED_SELECT);
			break;
		default:
			if(this->PHY_type == LAN8720) this->PHY_Status = lpc_mii_write(LAN8_BCR_REG, LAN8_AUTONEG);
			if(this->PHY_type == DP83848M) this->PHY_Status = lpc_mii_write(DP8_BMCR_REG, DP8_AUTONEG);
			break;
	}

#ifdef USE_RMII
	if(this->PHY_type == DP83848M) lpc_mii_write(DP8_PHY_RBR_REG, DP8_RBR_RMII_MODE);
#endif

	if(this->PHY_Status == ERROR)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! PHY Initialization\n\r");
		#endif
		#endif
		return;
	}

	this->TxQueue.br = 0;
	this->TxQueue.bw = 0;

	this->Init_Descriptors();
	
	//Reset Ethernet module
	Chip_RGU_TriggerReset(RGU_ETHERNET_RST);
	while (Chip_RGU_InReset(RGU_ETHERNET_RST));

	Chip_ENET_Reset(LPC_ETHERNET);
	CONTROLLER_t::get_CONTROLLER()->_delay_ms(1);
	
#ifdef USE_RMII
	Chip_ENET_RMIIEnable(LPC_ETHERNET);
#endif
#ifdef USE_MII
	Chip_ENET_MIIEnable(LPC_ETHERNET);
#endif

	Chip_ENET_SetupMII(LPC_ETHERNET, mdcclk, i);
	
	Chip_ENET_InitDescriptors(LPC_ETHERNET, (ENET_ENHTXDESC_T*) &this->BdTx, (ENET_ENHRXDESC_T*) &this->BdRx);
	
	Chip_ENET_SetADDR(LPC_ETHERNET, (uint8_t*)Eth_phy_t::EmacPsMAC);

	/* Burst length = 1 */
	LPC_ETHERNET->DMA_BUS_MODE = DMA_BM_PBL(1) | DMA_BM_RPBL(1);
	/* Initial MAC configuration for checksum offload, full duplex,
	   100Mbps, inter-frame gap 72 bit times
	   of 64-bits */
	LPC_ETHERNET->MAC_CONFIG = MAC_CFG_ACS/*| MAC_CFG_IPC*/ | MAC_CFG_DM | /*MAC_CFG_FES |*/ MAC_CFG_IFG(3);

	switch (this->speed)
	{
		case speed_autoneg:
			LPC_ETHERNET->MAC_CONFIG |= MAC_CFG_FES;
			break;
		case speed_10Mbit:
			break;
		case speed_100Mbit:
			LPC_ETHERNET->MAC_CONFIG |= MAC_CFG_FES;
			break;
		default:
			LPC_ETHERNET->MAC_CONFIG |= MAC_CFG_FES;
			break;
	}

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

	CONTROLLER_t::get_Time_Engine()->NRT_setEvent(Lowest_priority_delay, Eth_phy_Check_link_event_period_us, this, NULL);

	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(ETHERNET_IRQn, NVIC_EncodePriority(prioritygroup, Ethernet_Interrrupt_priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(ETHERNET_IRQn, Ethernet_Interrrupt_priority);
	#endif
	NVIC_ClearPendingIRQ(ETHERNET_IRQn);
	NVIC_EnableIRQ(ETHERNET_IRQn);
	
	this->Initialized = 1;
	#ifdef USE_CONSOLE
	#ifdef EthM_console
	printf("Ethernet Physical Initialization Done!\n\r");
	#endif
	#endif
}

void Eth_phy_t::Start(void)
{
	Chip_ENET_TXEnable(LPC_ETHERNET);
	Chip_ENET_RXEnable(LPC_ETHERNET);

	Chip_ENET_TXStart(LPC_ETHERNET);
	Chip_ENET_RXStart(LPC_ETHERNET);
}

void Eth_phy_t::ResetDevice(void)
{
	NVIC_ClearPendingIRQ(ETHERNET_IRQn);
	NVIC_DisableIRQ(ETHERNET_IRQn);
	this->Initialized = 0;

	#ifdef USE_CONSOLE
	#ifdef EthM_console
	printf("Ethernet PHY Reset!\n\r");
	#endif
	#endif

	this->Initialize();
	this->Start();
}


void Eth_phy_t::GetParameter(uint8_t ParamName, void* ParamValue)
{
	if(ParamName == MAC_param)
	{
		*((uint8_t**)ParamValue) = Eth_phy_t::EmacPsMAC;
	}
	if(ParamName == LINK_param)
	{
		*((uint8_t*)ParamValue) = this->Tx_Unlocked;
	}
	if(ParamName == Tx_Unlock_param)
	{
		*((uint8_t*)ParamValue) = this->Tx_Unlocked;
	}

	if(ParamName == name_param)
	{
		*((char**)ParamValue) = (char*)this->Adapter_name;
	}
}

void Eth_phy_t::SetParameter(uint8_t ParamName, void* ParamValue)
{
	uint8_t* Param = (uint8_t*)ParamValue;
	if(ParamName == MAC_param)
	{
		memcpy((char*)(&(Eth_phy_t::EmacPsMAC[0])),Param,6);
	}
	if(ParamName == Tx_Unlock_param)
	{
		this->Tx_Unlocked = *Param;
	}

	if(ParamName == name_param)
	{
		memcpy(this->Adapter_name, ParamValue, 9);
	}
	if(ParamName == speed_param)
	{
		this->speed = *((Ethernet_speed_t*)ParamValue);
	}

}

void Eth_phy_t::void_callback(void* Intf_ptr, void* parameters)
{
	this->Check_link();
	CONTROLLER_t::get_Time_Engine()->NRT_setEvent(Lowest_priority_delay, Eth_phy_Check_link_event_period_us, this, NULL);
}

void Eth_phy_t::Check_link(void)
{
	this->PHY_Status = lpcPHYStsPoll();
	if( ((this->PHY_Status&PHY_LINK_BUSY) == 0) && ((this->PHY_Status&PHY_LINK_CHANGED) != 0)  )
	{
		if((this->PHY_Status&PHY_LINK_CONNECTED) == 0)
		{
			#ifdef USE_CONSOLE
			#ifdef EthM_console
			printf("NO LINK ON ETHERNET \n\r");
			#endif
			#endif
			this->Tx_Unlocked = 0;
		}
		else
		{
			#ifdef USE_CONSOLE
			#ifdef EthM_console
			printf("ETHERNET LINK UP\n\r");
			#endif
			#endif
			this->Tx_Unlocked = 1;
			if((this->PHY_Status&PHY_LINK_FULLDUPLX) == 0)
			{
				#ifdef USE_CONSOLE
				#ifdef EthM_console
				printf("HALF-DUPLEX ");
				#endif
				#endif
				Chip_ENET_SetDuplex(LPC_ETHERNET, false);
			}
			else
			{
				#ifdef USE_CONSOLE
				#ifdef EthM_console
				printf("FULL DUPLEX ");
				#endif
				#endif
				Chip_ENET_SetDuplex(LPC_ETHERNET, true);
			}

			if((this->PHY_Status&PHY_LINK_SPEED100) == 0)
			{
				#ifdef USE_CONSOLE
				#ifdef EthM_console
				printf("10 Mb/s\n\r");
				#endif
				#endif
				Chip_ENET_SetSpeed(LPC_ETHERNET, false);
			}
			else
			{
				#ifdef USE_CONSOLE
				#ifdef EthM_console
				printf("100 Mb/s\n\r");
				#endif
				#endif
				Chip_ENET_SetSpeed(LPC_ETHERNET, true);
			}
		}
	}
}


uint16_t Eth_phy_t::lpc_rmii_read(uint8_t reg)
{
	int32_t mst = 250;
	uint16_t data = 0;
	/* Start register read */
	Chip_ENET_StartMIIRead(LPC_ETHERNET, reg);

	/* Wait for unbusy status */
	while (mst > 0) {
		if (!Chip_ENET_IsMIIBusy(LPC_ETHERNET)) {
			mst = 0;
			data = Chip_ENET_ReadMIIData(LPC_ETHERNET);
			return data;
		}
		else {
			mst--;
			CONTROLLER_t::get_CONTROLLER()->_delay_ms(1);
		}
	}

	return 0;
}

void Eth_phy_t::Init_Tx_Descriptors(void)
{
	uint8_t i;
	for(i=0;i<(Eth_phy_Tx_Queue_Len-1);i++)
	{
		this->BdTx[i].B1ADD = (uint32_t)(this->TxQueue.Frames[i]);
		this->BdTx[i].B2ADD = (uint32_t)(&(this->BdTx[i+1]));
		this->BdTx[i].BSIZE = 0;
		this->BdTx[i].CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC;
		this->TxQueue.BdTxPtr[i] = (ENET_TXDESC_T*)&(this->BdTx[i]);
		this->TxQueue.frame_size[i] = 0;
	}
	this->BdTx[(Eth_phy_Tx_Queue_Len-1)].B1ADD = (uint32_t)(this->TxQueue.Frames[(Eth_phy_Tx_Queue_Len-1)]);
	this->BdTx[(Eth_phy_Tx_Queue_Len-1)].B2ADD = (uint32_t)(&(this->BdTx[0]));
	this->BdTx[(Eth_phy_Tx_Queue_Len-1)].BSIZE = 0;
	this->BdTx[(Eth_phy_Tx_Queue_Len-1)].CTRLSTAT = TDES_ENH_TCH|TDES_ENH_FS|TDES_ENH_LS|TDES_ENH_IC;
	this->TxQueue.BdTxPtr[(Eth_phy_Tx_Queue_Len-1)] = (ENET_TXDESC_T*)&(this->BdTx[(Eth_phy_Tx_Queue_Len-1)]);
	this->TxQueue.frame_size[(Eth_phy_Tx_Queue_Len-1)] = 0;
}

void Eth_phy_t::Init_Rx_Descriptors(void)
{
	uint8_t i;
	for(i=0;i<(Eth_phy_Rx_Queue_Len-1);i++)
	{
		this->BdRx[i].STATUS = RDES_OWN;
		this->BdRx[i].CTRL = RDES_ENH_RCH | Eth_Rx_desc_buf_size;
		this->BdRx[i].B1ADD = (uint32_t)(this->RxBd_buf[i]);
		this->BdRx[i].B2ADD = (uint32_t)(&(this->BdRx[i+1]));
	}
	this->BdRx[(Eth_phy_Rx_Queue_Len-1)].STATUS = RDES_OWN;
	this->BdRx[(Eth_phy_Rx_Queue_Len-1)].CTRL = RDES_ENH_RCH|Eth_Rx_desc_buf_size;
	this->BdRx[(Eth_phy_Rx_Queue_Len-1)].B1ADD = (uint32_t)(this->RxBd_buf[(Eth_phy_Rx_Queue_Len-1)]);
	this->BdRx[(Eth_phy_Rx_Queue_Len-1)].B2ADD = (uint32_t)(&(this->BdRx[0]));
	this->Rx_Bd_index = 0;
}

void Eth_phy_t::Init_Descriptors(void)
{
	this->Init_Tx_Descriptors();
	this->Init_Rx_Descriptors();
}


void Eth_phy_t::ErrorHandler(void)
{
#ifdef Eth_phy_USE_DMA_errors_control

	if((LPC_ETHERNET->DMA_STAT & DMA_ST_ETI) == 0)
	{
		this->Eth_DMA_errors_counter++;
		if(this->Eth_DMA_errors_counter>=Eth_phy_DMA_errors_treshhold)
		{
			this->ResetDevice();
		}
	}

#endif

	if((LPC_ETHERNET->DMA_STAT & DMA_ST_TPS) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Transmit process stopped\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_TJT) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Transmit jabber timeout\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_OVF) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Receive overflow\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_UNF) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Transmit underflow\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_RU) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Receive buffer unavailable\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_RPS) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Received process stopped\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_RWT) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Receive watchdog timeout\n\r");
		#endif
		#endif
	}
	if((LPC_ETHERNET->DMA_STAT & DMA_ST_FBI) != 0)
	{
		#ifdef USE_CONSOLE
		#ifdef EthM_console
		printf("ERROR! Ethernet: Fatal bus error interrupt\n\r");
		#endif
		#endif
	}
}

