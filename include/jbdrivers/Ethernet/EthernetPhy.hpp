/**
 * @file
 * @brief Ethernet Driver Description
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

#ifndef ETHERNETPHY_HPP_
#define ETHERNETPHY_HPP_

#include "jbkernel/jb_common.h"
#include "jbkernel/callback_interfaces.hpp"
#include "jbkernel/IVoidEthernet.hpp"
#include "jbdrivers/IrqController.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

typedef enum
{
	PHY_TYPE_UNDEFINED = 0,
	PHY_TYPE_LAN8720 =   1,
	PHY_TYPE_DP83848M =  2,
}EthernetPhyType_t;

class EthernetPhy : public IVoidEthernet, protected IIrqListener
{
public:
	static EthernetPhy* getEthernetPhy(void);
	virtual void initialize(void);
	virtual void start(void);
	virtual void reset(void);
	virtual void getParameter(const uint8_t number, void* const value);
	virtual void setParameter(const uint8_t number, void* const value);
	virtual bool isTxQueueFull(void);
	virtual void addToTxQueue(EthernetFrame* const frame, uint16_t frameSize);
#if USE_LWIP
	virtual void addToTxQueue(struct pbuf* p);
#endif
	virtual uint16_t pullOutRxFrame(EthernetFrame* const frame);

private:
	static EthernetPhy* ethernetPhy_;
	static uint8_t mac_[6];

	EthernetPhy(void);
	virtual void irqHandler(int irqNumber);
	bool checkLink(void);
	void sendHandler(void);
	void errorHandler(void);
	uint16_t rmiiRead(uint8_t reg);
	void initializeDescriptors(void);
	void initializeTxDescriptors(void);
	void initializeRxDescriptors(void);

	#pragma pack(push, 4)
	typedef struct
	{
		EthernetFrame frames[ETHERNET_PHY_TX_QUEUE_SIZE];
		uint16_t sizes[ETHERNET_PHY_TX_QUEUE_SIZE];
		ENET_TXDESC_T* bdTxPtrs[ETHERNET_PHY_TX_QUEUE_SIZE];
		uint16_t bw = 0;
		uint16_t br = 0;
	}EthernetPhyTxQueue_t;
	#pragma pack(pop)

	bool isInitialized_ = false;
	bool isTxUnlocked_ = true;
	uint32_t dmaErrorsCounter_ = 0;
	char name_[9] = "Eth_phy";
	IVoidEthernetSpeed_t speed_ = ETHERNET_PHY_SPEED;
	EthernetPhyType_t phyType_ = PHY_TYPE_UNDEFINED;
	uint32_t phyStatus_ = 0;

	ENET_TXDESC_T bdsTx_[ETHERNET_PHY_TX_QUEUE_SIZE] __attribute__ ((aligned(4)));
	EthernetPhyTxQueue_t txQueue_ __attribute__ ((aligned(4)));

	ENET_RXDESC_T bdsRx_[ETHERNET_PHY_RX_QUEUE_SIZE] __attribute__ ((aligned(4)));
	uint8_t rxBdBuffers_[ETHERNET_PHY_RX_QUEUE_SIZE][ETHERNET_PHY_RX_DESCRIPTOR_BUFFER_SIZE] __attribute__ ((aligned(4)));
	uint16_t rxBdIndex_ = 0;
	uint8_t outFrame_[EMAC_ETH_MAX_FLEN];
	uint16_t outFrameSize_ = 0;
};

}

#endif /* ETHERNETPHY_HPP_ */
