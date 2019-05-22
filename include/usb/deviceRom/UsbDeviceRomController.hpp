/**
 * @file
 * @brief USB Device ROM Controller Description
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

#ifndef USB_DEVICE_ROM_CONTROLLER_HPP_
#define USB_DEVICE_ROM_CONTROLLER_HPP_

#include "jb_common.h"
#include "IrqController.hpp"
#include "IUsbDeviceRom.hpp"

#if !defined USB_NUM_MODULES
#define USB_NUM_MODULES			2
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;

class UsbDeviceRomController : protected IIrqListener, public IVoidCallback
{
public:
	static UsbDeviceRomController* getUsbDeviceRomController(uint8_t usbNumber);
	void initialize(IUsbDeviceRom** devices, uint8_t devicesSize);
	void connect(void);
	void disconnect(void);
	void reset(void);
	virtual void voidCallback(void* const source, void* parameter);
	/* Endpoint 0 patch that prevents nested NAK event processing */
	ErrorCode_t ep0Patch(USBD_HANDLE_T handle, void* data, uint32_t event);

private:
	UsbDeviceRomController(uint8_t usbNumber);
	virtual void irqHandler(int8_t irqNumber);
	static ErrorCode_t resetEvent(USBD_HANDLE_T handle);

	static uint8_t deviceDescriptorUsb0_[];
	static uint8_t hsConfigDescriptorUsb0_[];
	static uint8_t fsConfigDescriptorUsb0_[];
	static uint8_t stringDescriptorUsb0_[];
	static uint8_t deviceQualifierUsb0_[];
	static uint8_t deviceDescriptorUsb1_[];
	static uint8_t fsConfigDescriptorUsb1_[];
	static uint8_t stringDescriptorUsb1_[];
	static const USBD_API_T* usbRomApi_;
	static UsbDeviceRomController* usbDeviceRomControllers_[USB_NUM_MODULES];
	IUsbDeviceRom** devices_ = NULL;
	uint8_t devicesSize_ = 0;
	uint8_t usbNumber_ = 0;
	USBD_HANDLE_T handle_ = NULL;
	uint32_t ep0RxBusyFlag_ = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
	USB_EP_HANDLER_T ep0BaseHandler_ = NULL;	/* variable to store the pointer to base EP0 handler */
};

}

#endif /* USB_DEVICE_ROM_CONTROLLER_HPP_ */
