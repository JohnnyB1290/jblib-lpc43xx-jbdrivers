/**
 * @file
 * @brief USB Device ROM Controller Realization
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
#include "jbdrivers/usb/deviceRom/UsbDeviceRomController.hpp"
#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
#include <stdio.h>
#endif


namespace jblib::jbdrivers
{

using namespace jbkernel;

static ErrorCode_t ep0PatchUsb0(USBD_HANDLE_T handle, void* data, uint32_t event);
static ErrorCode_t ep0PatchUsb1(USBD_HANDLE_T handle, void* data, uint32_t event);



const USBD_API_T* UsbDeviceRomController::usbRomApi_ =
		(const USBD_API_T*) LPC_ROM_API->usbdApiBase;
UsbDeviceRomController* UsbDeviceRomController::usbDeviceRomControllers_[USB_NUM_MODULES] = {
		(UsbDeviceRomController*)NULL, (UsbDeviceRomController*)NULL
};



UsbDeviceRomController* UsbDeviceRomController::getUsbDeviceRomController(uint8_t usbNumber)
{
	if(usbNumber >= USB_NUM_MODULES)
		return (UsbDeviceRomController*)NULL;
	if(usbDeviceRomControllers_[usbNumber] == (UsbDeviceRomController*)NULL)
		usbDeviceRomControllers_[usbNumber] = new UsbDeviceRomController(usbNumber);
	return usbDeviceRomControllers_[usbNumber];
}



UsbDeviceRomController::UsbDeviceRomController(uint8_t usbNumber) : IIrqListener(), IVoidCallback()
{
	this->usbNumber_ = usbNumber;
}



void UsbDeviceRomController::voidCallback(void* const source, void* parameter)
{
	for(uint8_t i = 0; i < this->devicesSize_; i++)
		this->devices_[i]->doTasks();
}



void UsbDeviceRomController::disconnect(void)
{
	usbRomApi_->hw->Connect(this->handle_, 0);
}



void UsbDeviceRomController::connect(void)
{
	usbRomApi_->hw->Connect(this->handle_, 1);
}



void UsbDeviceRomController::reset(void)
{
	IrqController::getIrqController()->deleteIrqListener(this);
	if(this->usbNumber_ == 0)
		IrqController::getIrqController()->disableInterrupt(USB0_IRQn);
	else if(this->usbNumber_ == 1)
		IrqController::getIrqController()->disableInterrupt(USB1_IRQn);
	usbRomApi_->hw->Reset(this->handle_);
}



void UsbDeviceRomController::irqHandler(int irqNumber)
{
	usbRomApi_->hw->ISR(this->handle_);
}



ErrorCode_t UsbDeviceRomController::ep0Patch(USBD_HANDLE_T handle,
		void* data, uint32_t event)
{
	switch (event) {
		case USB_EVT_OUT_NAK:
		{
			if (this->ep0RxBusyFlag_)
				return LPC_OK; /* we already queued the buffer so ignore this NAK event. */
			else
				this->ep0RxBusyFlag_ = 1; /* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
		}
		break;
		case USB_EVT_SETUP:	/* reset the flag when new setup sequence starts */
		case USB_EVT_OUT: /* we received the packet so clear the flag. */
			this->ep0RxBusyFlag_ = 0;
		break;
	}
	return this->ep0BaseHandler_(handle, data, event);
}



static ErrorCode_t ep0PatchUsb0(USBD_HANDLE_T handle, void* data, uint32_t event)
{
	return UsbDeviceRomController::getUsbDeviceRomController(0)->
			ep0Patch(handle, data, event);
}



static ErrorCode_t ep0PatchUsb1(USBD_HANDLE_T handle, void* data, uint32_t event)
{
	return UsbDeviceRomController::getUsbDeviceRomController(1)->
			ep0Patch(handle, data, event);
}



ErrorCode_t UsbDeviceRomController::resetEvent(USBD_HANDLE_T hUsb)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	printf("USB Reset event\r\n");
	#endif
	return ERR_USBD_UNHANDLED;
}



void UsbDeviceRomController::initialize(IUsbDeviceRom** devices, uint8_t devicesSize)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	printf("Initialize USB Device Rom Controller\r\n");
	#endif
	if((devicesSize == 0)||(devices == (IUsbDeviceRom**)NULL)) {
		#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
		printf("USB Device Rom Controller ERROR: No USB devices\r\n");
		#endif
		return;
	}
	this->devicesSize_ = devicesSize;
	this->devices_ = devices;

	USBD_API_INIT_PARAM_T initParameters;
	memset((void *) &initParameters, 0, sizeof(USBD_API_INIT_PARAM_T));
	USB_CORE_DESCS_T descriptors;
	if(this->usbNumber_ == 0){
		Chip_USB0_Init();
		initParameters.usb_reg_base = LPC_USB0_BASE;
		initParameters.mem_base = USB_DEVICE_ROM_USB0_STACK_MEM_BASE;
		initParameters.mem_size = USB_DEVICE_ROM_USB0_STACK_MEM_SIZE;
		initParameters.max_num_ep = USB_MAX_EP_NUM;
		initParameters.USB_Reset_Event = resetEvent;
		/* Set the USB descriptors */
		descriptors.device_desc = (uint8_t *)deviceDescriptorUsb0_;
		descriptors.string_desc = (uint8_t *)stringDescriptorUsb0_;
		descriptors.high_speed_desc = hsConfigDescriptorUsb0_;
		descriptors.full_speed_desc = fsConfigDescriptorUsb0_;
		descriptors.device_qualifier = (uint8_t *)deviceQualifierUsb0_;
	}
	if(this->usbNumber_ == 1) {
		Chip_USB1_Init();
		initParameters.usb_reg_base = LPC_USB1_BASE;
		initParameters.mem_base = USB_DEVICE_ROM_USB1_STACK_MEM_BASE;
		initParameters.mem_size = USB_DEVICE_ROM_USB1_STACK_MEM_SIZE;
		initParameters.max_num_ep = USB_MAX_EP_NUM;
		initParameters.USB_Reset_Event = resetEvent;
		/* Set the USB descriptors */
		descriptors.device_desc = (uint8_t *) deviceDescriptorUsb1_;
		descriptors.string_desc = (uint8_t *) stringDescriptorUsb1_;
		descriptors.high_speed_desc = fsConfigDescriptorUsb1_;
		descriptors.full_speed_desc = fsConfigDescriptorUsb1_;
		descriptors.device_qualifier = 0;
	}
	/* USB Initialization */
	ErrorCode_t ret = usbRomApi_->hw->Init(&(this->handle_),
			&descriptors, &initParameters);
	if (ret != LPC_OK) {
		#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
		printf("USB Device Rom Controller ERROR: "
				"HW Initialization! 0x%X\r\n", ret);
		#endif
	}
	else {
		#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
		printf("New mem base after HW Initialization = 0x%lX\r\n",
				initParameters.mem_base);
		printf("New mem size after HW Initialization = 0x%lX\r\n",
				initParameters.mem_size);
		#endif
	}

	/*	WORKAROUND for artf45032 ROM driver BUG:
	    Due to a race condition there is the chance that a second NAK event will
	    occur before the default endpoint0 handler has completed its preparation
	    of the DMA engine for the first NAK event. This can cause certain fields
	    in the DMA descriptors to be in an invalid state when the USB controller
	    reads them, thereby causing a hang.
	 */
	USB_CORE_CTRL_T* coreControl = (USB_CORE_CTRL_T *) this->handle_;
	this->ep0BaseHandler_ = coreControl->ep_event_hdlr[0];
	if(this->usbNumber_ == 0)
		coreControl->ep_event_hdlr[0] = ep0PatchUsb0;
	if(this->usbNumber_ == 1)
		coreControl->ep_event_hdlr[0] = ep0PatchUsb1;
	
	for(uint8_t i = 0; i < this->devicesSize_; i++) {
		if(this->devices_[i]) {
			this->devices_[i]->setParameter(USB_DEVICE_PARAMETER_USB_NUMBER,
					(void*)&this->usbNumber_);
			ret = this->devices_[i]->initialize(this->handle_,
					&descriptors, &initParameters);
		}
		else
			break;
		if (ret != LPC_OK) {
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("USB Device Rom Controller ERROR:"
					" in USB device %i Initialize! 0x%X\r\n", i, ret);
			#endif
		}
		else {
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("New mem base after USB device %i ini = 0x%lX\r\n",
					i,initParameters.mem_base);
			printf("New mem size after USB device %i ini = 0x%lX\r\n",
					i,initParameters.mem_size);
			#endif
		}
	}

	if(usbNumber_ == 0)
		IrqController::getIrqController()->addIrqListener(this, USB0_IRQn);
	else if(usbNumber_ == 1)
		IrqController::getIrqController()->addIrqListener(this, USB1_IRQn);

	if(this->usbNumber_ == 0) {
		IrqController::getIrqController()->
				setPriority(USB0_IRQn, USB_0_INTERRUPT_PRIORITY);
		IrqController::getIrqController()->enableInterrupt(USB0_IRQn);
	}
	else if(this->usbNumber_ == 1) {
		IrqController::getIrqController()->
				setPriority(USB1_IRQn, USB_1_INTERRUPT_PRIORITY);
		IrqController::getIrqController()->enableInterrupt(USB1_IRQn);
	}
	UsbDeviceRomController::usbRomApi_->hw->Connect(this->handle_, 1);
}

}
