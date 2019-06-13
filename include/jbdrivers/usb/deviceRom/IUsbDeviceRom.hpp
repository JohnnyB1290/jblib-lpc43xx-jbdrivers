/**
 * @file
 * @brief USB Device ROM Interface Description
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

#ifndef IUSB_DEVICE_ROM_HPP_
#define IUSB_DEVICE_ROM_HPP_

#include "jb_common.h"
#include "error.h"
#include "usbd_rom/usbd_rom_api.h"

typedef enum
{
	USB_DEVICE_PARAMETER_USB_NUMBER = 0,
	USB_DEVICE_PARAMETER_RNDIS_ADAPTER_MAC = 1,
}UsbRomDeviceParameters_t;

namespace jblib::jbdrivers
{

class  IUsbDeviceRom
{
public:
	IUsbDeviceRom(void){}
	virtual ~IUsbDeviceRom(void){}
	virtual ErrorCode_t initialize(USBD_HANDLE_T usbHandle, USB_CORE_DESCS_T* descriptors,
			USBD_API_INIT_PARAM_T* initParameters) = 0;
	virtual void doTasks(void) = 0;
	virtual void getParameter(const UsbRomDeviceParameters_t parameter,
			void* const value) = 0;
	virtual void setParameter(const UsbRomDeviceParameters_t parameter,
			void* const value) = 0;
};

}

#endif /* IUSB_DEVICE_ROM_HPP_ */
