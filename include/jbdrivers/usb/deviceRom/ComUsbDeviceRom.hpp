/**
 * @file
 * @brief USB Device Rom Com Device Description
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

#ifndef COM_USB_DEVICE_ROM_HPP_
#define COM_USB_DEVICE_ROM_HPP_

#include "jb_common.h"
#include "IUsbDeviceRom.hpp"
#include "IVoidChannel.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

typedef enum{
	COM_USB_CHANNEL_PARAMETER_RTS = 0,
	COM_USB_CHANNEL_PARAMETER_CONNECTED = 1,
	COM_USB_CHANNEL_PARAMETER_TX_FULL = 2,
	COM_USB_CHANNEL_PARAMETER_CTS = 3,
}ComUsbChannelParameters_t;



class ComUsbDeviceRom : public IVoidChannel, public IUsbDeviceRom
{
public:
	ComUsbDeviceRom(void);
	virtual ErrorCode_t initialize(USBD_HANDLE_T usbHandle, USB_CORE_DESCS_T* descriptors,
			USBD_API_INIT_PARAM_T* initParameters);
	virtual void doTasks(void);
	virtual void getParameter(const UsbRomDeviceParameters_t parameter,
			void* const value);
	virtual void setParameter(const UsbRomDeviceParameters_t parameter,
			void* const value);
	virtual void initialize(void* (* const mallocFunc)(size_t),
			const uint16_t txBufferSize, IChannelCallback* const callback);
	virtual void deinitialize(void);
	virtual void tx(uint8_t* const buffer, const uint16_t size, void* parameter);
	virtual void getParameter(const uint8_t number, void* const value);
	virtual void setParameter(const uint8_t number, void* const value);

	static ComUsbDeviceRom* comUsbRomDevices[COM_USB_DEVICE_ROM_NUM_DEVICES];
	static USBD_HANDLE_T cdcHandlers[COM_USB_DEVICE_ROM_NUM_DEVICES];
	static const USBD_API_T* usbApi;
	static uint8_t devicesCounter;
	uint8_t usbNumber = 0;
	IChannelCallback* callback = NULL;
	USBD_HANDLE_T usbHandle = NULL;
	USB_EP_HANDLER_T defaultCdcHandler = NULL;
	uint8_t txBusyFlag = 0;
	uint8_t isChannelInitialized = 0;
	uint16_t controlLineState = 0;
	CDC_LINE_CODING lineCode;
	uint32_t isConnected = 0;
	uint32_t cts = 0;
	RINGBUFF_T txRingBuffer;
	uint8_t usbTxBuffer[USB_HS_MAX_BULK_PACKET];
	uint8_t usbRxBuffer[USB_HS_MAX_BULK_PACKET];
	uint8_t isUsbRxBufferQueued = 0;

private:
	static USB_INTERFACE_DESCRIPTOR* findInterfaceDescriptor(const uint8_t* descriptor,
			uint32_t interfaceNumber);

	uint8_t number_ = 0;
	uint8_t* txBuffer_ = NULL;
	uint16_t txBufferSize_ = 0;
};

}

#endif /* COM_USB_DEVICE_ROM_HPP_ */
