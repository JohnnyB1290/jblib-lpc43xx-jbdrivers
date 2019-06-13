/**
 * @file
 * @brief USB Device Rom RNDIS Device Description
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

#ifndef RNDIS_USB_DEVICE_ROM_HPP_
#define RNDIS_USB_DEVICE_ROM_HPP_

#include "jbkernel/jb_common.h"
#include "jbdrivers/usb/deviceRom/RndisUsbDeviceRomDefines.h"
#include "jbdrivers/usb/deviceRom/IUsbDeviceRom.hpp"
#include "jbkernel/IVoidEthernet.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

class RndisUsbDeviceRom : public IVoidEthernet, public IUsbDeviceRom
{
public:
	RndisUsbDeviceRom(void);
	virtual ErrorCode_t initialize(USBD_HANDLE_T usbHandle, USB_CORE_DESCS_T* descriptors,
			USBD_API_INIT_PARAM_T* initParameters);
	virtual void doTasks(void);
	virtual void getParameter(const UsbRomDeviceParameters_t parameter,
			void* const value);
	virtual void setParameter(const UsbRomDeviceParameters_t parameter,
			void* const value);
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
	static ErrorCode_t ep0Handler(USBD_HANDLE_T usbHandle, void* data,
			uint32_t event);
	static ErrorCode_t bulkInHandler(USBD_HANDLE_T usbHandle, void* data,
			uint32_t event);
	static ErrorCode_t bulkOutHandler(USBD_HANDLE_T usbHandle, void* data,
			uint32_t event);
	void processRndisControlMessage(void);
	bool processRndisQuery(const uint32_t oid, void* queryData, uint16_t querySize,
			void* responseData, uint16_t* responseSize);
	bool processRndisSet(uint32_t oid, void* setData, uint16_t setSize);

	static const USBD_API_T* usbApi_;
	static uint32_t adapterSupportedOidList_[28];
	static char adapterVendorDescription_[];
	uint8_t mac_[6];
	MAC_Address_t adapterMac_;
	uint32_t usbNumber_ = 0;
	uint8_t rndisMessageBuffer_[sizeof(adapterSupportedOidList_) +
							   sizeof(RNDIS_Query_Complete_t)];
	RNDIS_Message_Header_t* messageHeader_ =
			(RNDIS_Message_Header_t*)&this->rndisMessageBuffer_;
	bool isResponseReady_ = false;
	uint8_t currentRndisState_ = RNDIS_Uninitialized;
	uint32_t currentPacketFilter_ = 0;
	bool isTxUnlocked_ = false;
	char name_[9] = "RNDIS   ";
	USBD_HANDLE_T usbHandle_ = NULL;
	RndisRxQueue_t* rxQueue_ = NULL;
	RndisTxQueue_t* txQueue_ = NULL;
	uint8_t ep0DataCounter_ = 0;
	uint16_t ep0DataIndex_ = 0;
};

}

#endif /* RNDIS_USB_DEVICE_ROM_HPP_ */
