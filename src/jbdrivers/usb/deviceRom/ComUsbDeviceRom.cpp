/**
 * @file
 * @brief USB Device Rom Com Device Realization
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
#include "jbdrivers/usb/deviceRom/ComUsbDeviceRom.hpp"
#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
#include <stdio.h>
#endif

#define CTRL_LINE_STATE_DTR_bm 		(1 << 0)
#define CTRL_LINE_STATE_RTS_bm 		(1 << 1)

#define LINE_CODDING_STOP_BIT_1		0
#define LINE_CODDING_STOP_BIT_1_5	1
#define LINE_CODDING_STOP_BIT_2		2

#define LINE_CODDING_PARITY_NONE	0
#define LINE_CODDING_PARITY_ODD		1
#define LINE_CODDING_PARITY_EVEN	2
#define LINE_CODDING_PARITY_MARK	3
#define LINE_CODDING_PARITY_SPACE	4

namespace jblib::jbdrivers
{

using namespace jbkernel;

static ErrorCode_t setControlLineState(USBD_HANDLE_T cdcHandle, uint16_t state);
static ErrorCode_t sendBreak(USBD_HANDLE_T cdcHandle, uint16_t msTime);
static ErrorCode_t setLineCode(USBD_HANDLE_T cdcHandle, CDC_LINE_CODING* lineCoding);
static ErrorCode_t cdcEp0Handler(USBD_HANDLE_T usbHandle, void* data, uint32_t event);
static ErrorCode_t cdcBulkInHandler(USBD_HANDLE_T usbHandle, void* data, uint32_t event);
static ErrorCode_t cdcBulkOutHandler(USBD_HANDLE_T usbHandle, void* data, uint32_t event);



const USBD_API_T* ComUsbDeviceRom::usbApi =
		(const USBD_API_T*) LPC_ROM_API->usbdApiBase;
ComUsbDeviceRom* ComUsbDeviceRom::comUsbRomDevices[COM_USB_DEVICE_ROM_NUM_DEVICES];
USBD_HANDLE_T ComUsbDeviceRom::cdcHandlers[COM_USB_DEVICE_ROM_NUM_DEVICES];
uint8_t ComUsbDeviceRom::devicesCounter = 0;



ComUsbDeviceRom::ComUsbDeviceRom(void) : IVoidChannel(), IUsbDeviceRom()
{
	this->number_ = devicesCounter;
	comUsbRomDevices[this->number_] = this;
	devicesCounter++;
	if(devicesCounter == COM_USB_DEVICE_ROM_NUM_DEVICES)
		devicesCounter = 0;
}



ErrorCode_t ComUsbDeviceRom::initialize(USBD_HANDLE_T usbHandle, USB_CORE_DESCS_T* descriptors,
		USBD_API_INIT_PARAM_T* initParameters)
{
	USBD_CDC_INIT_PARAM_T cdcInitParameters;
	memset((void *) &cdcInitParameters, 0, sizeof(USBD_CDC_INIT_PARAM_T));
	this->usbHandle = usbHandle;

	cdcInitParameters.mem_base = initParameters->mem_base;
	cdcInitParameters.mem_size = initParameters->mem_size;
	if(this->usbNumber == 0) {
		cdcInitParameters.cif_intf_desc =
				(uint8_t *)findInterfaceDescriptor(descriptors->high_speed_desc,
						COM_USB_DEVICE_ROM_CIF_NUM_USB_0);
		cdcInitParameters.dif_intf_desc =
				(uint8_t *)findInterfaceDescriptor(descriptors->high_speed_desc,
						COM_USB_DEVICE_ROM_DIF_NUM_USB_0);
	}
	if(this->usbNumber == 1) {
		cdcInitParameters.cif_intf_desc =
				(uint8_t *)findInterfaceDescriptor(descriptors->full_speed_desc,
						COM_USB_DEVICE_ROM_CIF_NUM_USB_1);
		cdcInitParameters.dif_intf_desc =
				(uint8_t *)findInterfaceDescriptor(descriptors->full_speed_desc,
						COM_USB_DEVICE_ROM_DIF_NUM_USB_1);
	}
	cdcInitParameters.SetCtrlLineState = setControlLineState;
	cdcInitParameters.SetLineCode =  setLineCode;
	cdcInitParameters.SendBreak = sendBreak;
	ErrorCode_t ret = usbApi->cdc->init(usbHandle, &cdcInitParameters,
			&cdcHandlers[this->number_]);
	if (ret != LPC_OK)
		return ret;

	/*	WORKAROUND for artf42016 ROM driver BUG:
		The default CDC class handler in initial ROM (REV A silicon) was not
		sending proper handshake after processing SET_REQUEST messages targeted
		to CDC interfaces. The workaround will send the proper handshake to host.
		Due to this bug some terminal applications such as Putty have problem
		establishing connection.
	 */
	USB_CORE_CTRL_T* coreControl = (USB_CORE_CTRL_T *) usbHandle;
	this->defaultCdcHandler = coreControl->ep0_hdlr_cb[coreControl->num_ep0_hdlrs - 1];
	coreControl->ep0_hdlr_cb[coreControl->num_ep0_hdlrs - 1] = cdcEp0Handler;

	uint32_t epIndex = 0;
	if(this->usbNumber == 0)
		epIndex = (((COM_USB_DEVICE_ROM_IN_EP_USB_0 & 0x0F) << 1) + 1);
	if(this->usbNumber == 1)
		epIndex = (((COM_USB_DEVICE_ROM_IN_EP_USB_1 & 0x0F) << 1) + 1);
	ret = usbApi->core->RegisterEpHandler(usbHandle, epIndex, cdcBulkInHandler, this);
	if (ret != LPC_OK)
		return ret;
	if(this->usbNumber == 0)
		epIndex = ((COM_USB_DEVICE_ROM_OUT_EP_USB_0 & 0x0F) << 1);
	if(this->usbNumber == 1)
		epIndex = ((COM_USB_DEVICE_ROM_OUT_EP_USB_1 & 0x0F) << 1);
	ret = usbApi->core->RegisterEpHandler(usbHandle, epIndex, cdcBulkOutHandler, this);
	if (ret != LPC_OK)
		return ret;

	initParameters->mem_base = cdcInitParameters.mem_base;
	initParameters->mem_size = cdcInitParameters.mem_size;

	return ret;
}



void ComUsbDeviceRom::doTasks(void)
{

}



void ComUsbDeviceRom::getParameter(const UsbRomDeviceParameters_t parameter,
		void* const value)
{

}



void ComUsbDeviceRom::setParameter(const UsbRomDeviceParameters_t parameter,
		void* const value)
{
	if(parameter == USB_DEVICE_PARAMETER_USB_NUMBER)
		this->usbNumber = *((uint32_t*)value);
}



void ComUsbDeviceRom::initialize(void* (* const mallocFunc)(size_t),
		const uint16_t txBufferSize, IChannelCallback* const callback)
{
	if(this->isChannelInitialized == 0){
		if(txBufferSize == 0)
			return;
		this->txBufferSize_ = txBufferSize;
		this->txBuffer_ = (uint8_t*)mallocFunc(this->txBufferSize_);
		if(this->txBuffer_ == (uint8_t*)NULL)
			return;
		if(callback == (IChannelCallback*)NULL)
			return;
		this->callback = callback;
		RingBuffer_Init(&(this->txRingBuffer), this->txBuffer_, 1,
				this->txBufferSize_);
		this->isChannelInitialized = 1;
	}
}



void ComUsbDeviceRom::deinitialize(void)
{
	this->isChannelInitialized = 0;
}



void ComUsbDeviceRom::tx(uint8_t* const buffer, const uint16_t size, void* parameter)
{
	if(this->isChannelInitialized == 1) {
		if(size == 0)
			return;
		uint32_t count = RingBuffer_InsertMult(&this->txRingBuffer, buffer, size);
		if(count == 0) {
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("COM USB Device ROM Error: TX ring buffer is full!\r\n");
			#endif
		}
		if((this->isConnected) && (this->txBusyFlag == 0)) {
			count = RingBuffer_PopMult(&this->txRingBuffer,
					this->usbTxBuffer, USB_HS_MAX_BULK_PACKET);
			if(count){
				this->txBusyFlag = 1;
				if(this->usbNumber == 0){
					NVIC_DisableIRQ(USB0_IRQn);
					ComUsbDeviceRom::usbApi->hw->WriteEP(this->usbHandle,
							COM_USB_DEVICE_ROM_IN_EP_USB_0, this->usbTxBuffer, count);
					NVIC_EnableIRQ(USB0_IRQn);
				}
				else if(this->usbNumber == 1) {
					NVIC_DisableIRQ(USB1_IRQn);
					ComUsbDeviceRom::usbApi->hw->WriteEP(this->usbHandle,
							COM_USB_DEVICE_ROM_IN_EP_USB_1, this->usbTxBuffer, count);
					NVIC_EnableIRQ(USB1_IRQn);
				}
			}
		}
	}
}



void ComUsbDeviceRom::getParameter(const uint8_t number, void* const value)
{
	switch(number)
	{
		case COM_USB_CHANNEL_PARAMETER_RTS:
		{
			*((uint32_t*)value) = (this->controlLineState & CTRL_LINE_STATE_RTS_bm) >> 1;
		}
		break;

		case COM_USB_CHANNEL_PARAMETER_CONNECTED:
		{
			*((uint32_t*)value) = this->isConnected;
		}
		break;

		case COM_USB_CHANNEL_PARAMETER_TX_FULL:
		{
			*((uint32_t*)value) = RingBuffer_IsFull(&this->txRingBuffer);
		}
		break;
	}
}



void ComUsbDeviceRom::setParameter(const uint8_t number, void* const value)
{
	if(number == COM_USB_CHANNEL_PARAMETER_CTS)
		this->cts = *((uint32_t*)value);
}



static ErrorCode_t cdcBulkInHandler(USBD_HANDLE_T usbHandle, void* data, uint32_t event)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	if (event == USB_EVT_IN_NAK)   /**< 5    IN Packet - Not Acknowledged */
		printf("COM USB Device ROM Error: BULK IN USB_EVT_IN_NAK\r\n");
	if (event == USB_EVT_IN_STALL)  /**< 7    IN Packet - Stalled */
		printf("COM USB Device ROM Error: BULK IN USB_EVT_IN_STALL\r\n");
	if (event == USB_EVT_IN_DMA_ERR) /**< 13  DMA  IN EP - Error */
		printf("COM USB Device ROM Error: BULK IN USB_EVT_IN_DMA_ERR\r\n");
	#endif
	if(event == USB_EVT_IN) {
		ComUsbDeviceRom* comDevice = (ComUsbDeviceRom*)data;
		if(comDevice->isChannelInitialized == 1) {
			int count = RingBuffer_PopMult(&comDevice->txRingBuffer,
					comDevice->usbTxBuffer, USB_HS_MAX_BULK_PACKET);
			if(count > 0){
				ComUsbDeviceRom::usbApi->hw->WriteEP(usbHandle,
						(comDevice->usbNumber) ? COM_USB_DEVICE_ROM_IN_EP_USB_1 :
								COM_USB_DEVICE_ROM_IN_EP_USB_0,
						comDevice->usbTxBuffer, count);
			}
			else
				comDevice->txBusyFlag = 0;
		}
		return LPC_OK;
	}
	return ERR_USBD_UNHANDLED;
}



static ErrorCode_t cdcBulkOutHandler(USBD_HANDLE_T usbHandle, void* data, uint32_t event)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	if (event == USB_EVT_OUT_STALL) /**< 6   OUT Packet - Stalled */
		printf("COM USB Device ROM Error: BULK OUT USB_EVT_OUT_STALL\r\n");
	if (event == USB_EVT_OUT_DMA_ERR) /**< 12  DMA OUT EP - Error */
		printf("COM USB Device ROM Error: BULK OUT USB_EVT_OUT_DMA_ERR\r\n");
	#endif
	ComUsbDeviceRom* comDevice = (ComUsbDeviceRom*)data;
	if (event == USB_EVT_OUT) {
		uint32_t count = ComUsbDeviceRom::usbApi->hw->ReadEP(
				comDevice->usbHandle,
				(comDevice->usbNumber) ? COM_USB_DEVICE_ROM_OUT_EP_USB_1 :
						COM_USB_DEVICE_ROM_OUT_EP_USB_0,
				comDevice->usbRxBuffer);
		comDevice->isUsbRxBufferQueued = 0;
		if((comDevice->isChannelInitialized) && (comDevice->callback)){
			comDevice->callback->channelCallback(comDevice->usbRxBuffer,
					count, comDevice, NULL);
		}
	}
	if( (event == USB_EVT_OUT_NAK) &&
			(comDevice->cts == 0) &&
			(comDevice->isUsbRxBufferQueued == 0)){
		ComUsbDeviceRom::usbApi->hw->ReadReqEP(comDevice->usbHandle,
				(comDevice->usbNumber) ? COM_USB_DEVICE_ROM_OUT_EP_USB_1 :
						COM_USB_DEVICE_ROM_OUT_EP_USB_0,
				comDevice->usbRxBuffer, USB_HS_MAX_BULK_PACKET);
		comDevice->isUsbRxBufferQueued = 1;
	}
	return LPC_OK;
}



/* CDC EP0_patch part of WORKAROUND for artf42016. */
static ErrorCode_t cdcEp0Handler(USBD_HANDLE_T usbHandle, void* data, uint32_t event)
{
	ErrorCode_t ret = ERR_USBD_UNHANDLED;
	USB_CORE_CTRL_T* coreControl = (USB_CORE_CTRL_T*)usbHandle;
	if ( (event == USB_EVT_OUT) &&
		 (coreControl->SetupPacket.bmRequestType.BM.Type == REQUEST_CLASS) &&
		 (coreControl->SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_INTERFACE) ) {
		/* Check which CDC control structure to use. If epin_num doesn't have BIT7 set then we are
		   at wrong offset so use the old CDC control structure. BIT7 is set for all EP_IN endpoints.

		 */
		USB_CDC_CTRL_T* cdcControl = (USB_CDC_CTRL_T *) data;
		USB_CDC0_CTRL_T* cdc0Control = (USB_CDC0_CTRL_T *) data;
		CIC_SetRequest_t setReq;
		uint8_t cifNum = 0;
		uint8_t difNum = 0;
		if ((cdcControl->epin_num & 0x80) == 0) {
			cifNum = cdc0Control->cif_num;
			difNum = cdc0Control->dif_num;
			setReq = cdc0Control->CIC_SetRequest;
		}
		else {
			cifNum = cdcControl->cif_num;
			difNum = cdcControl->dif_num;
			setReq = cdcControl->CIC_SetRequest;
		}
		/* is the request target is our interfaces */
		if (((coreControl->SetupPacket.wIndex.WB.L == cifNum)  ||
			 (coreControl->SetupPacket.wIndex.WB.L == difNum)) ) {
			coreControl->EP0Data.pData -= coreControl->SetupPacket.wLength;
			ret = setReq(cdcControl, &coreControl->SetupPacket,
					&coreControl->EP0Data.pData, coreControl->SetupPacket.wLength);
			if ( ret == LPC_OK) {
				/* send Acknowledge */
				ComUsbDeviceRom::usbApi->core->StatusInStage(coreControl);
			}
		}
	}
	else {
		uint8_t index = 0;
		for(uint8_t i = 0; i < ComUsbDeviceRom::devicesCounter; i++){
			if(data == ComUsbDeviceRom::cdcHandlers[i])
				break;
			index++;
		}
		ret = ComUsbDeviceRom::comUsbRomDevices[index]->defaultCdcHandler(
				usbHandle, data, event);
	}
	return ret;
}



static ErrorCode_t setControlLineState(USBD_HANDLE_T cdcHandle, uint16_t state)
{
	uint8_t index = 0;
	for(uint8_t i = 0; i < ComUsbDeviceRom::devicesCounter; i++) {
		if(cdcHandle == ComUsbDeviceRom::cdcHandlers[i])
			break;
		index++;
	}
	ComUsbDeviceRom::comUsbRomDevices[index]->controlLineState = state;

	if(ComUsbDeviceRom:: comUsbRomDevices[index]->controlLineState &
			CTRL_LINE_STATE_DTR_bm)
		ComUsbDeviceRom::comUsbRomDevices[index]->isConnected = 1;
	else
		ComUsbDeviceRom::comUsbRomDevices[index]->isConnected = 0;

	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	printf("COM USB Device ROM setControlLineState\r\n");
	if(ComUsbDeviceRom::comUsbRomDevices[index]->controlLineState &
			CTRL_LINE_STATE_DTR_bm)
		printf("DTR ON! CONNECTED\r\n");
	else
		printf("DTR OFF! DISCONNECTED\r\n");
	if(ComUsbDeviceRom::comUsbRomDevices[index]->controlLineState &
			CTRL_LINE_STATE_RTS_bm)
		printf("RTS ON\r\n");
	else
		printf("RTS OFF\r\n");
	#endif
	return LPC_OK;
}



static ErrorCode_t sendBreak(USBD_HANDLE_T cdcHandle, uint16_t msTime)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	printf("COM USB Device ROM SendBreak\r\n");
	#endif
	return LPC_OK;
}



static ErrorCode_t setLineCode(USBD_HANDLE_T cdcHandle, CDC_LINE_CODING* lineCoding)
{
	uint8_t index = 0;
	for(uint8_t i = 0; i < ComUsbDeviceRom::devicesCounter; i++) {
		if(cdcHandle == ComUsbDeviceRom::cdcHandlers[i])
			break;
		index++;
	}
	memcpy(&ComUsbDeviceRom::comUsbRomDevices[index]->lineCode,
			lineCoding, sizeof(CDC_LINE_CODING));

	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
	printf("COM USB Device ROM Set Line Codding\r\n");
	printf("Speed = %lu bit/s\r\n",
			ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.dwDTERate);
	if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bCharFormat ==
			LINE_CODDING_STOP_BIT_1)
		printf("1 Stop bit\r\n");
	else if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bCharFormat ==
			LINE_CODDING_STOP_BIT_1_5)
		printf("1.5 Stop bit\r\n");
	else if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bCharFormat ==
			LINE_CODDING_STOP_BIT_2)
		printf("2 Stop bit\r\n");
	if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bParityType ==
			 LINE_CODDING_PARITY_NONE)
		printf("None Parity\r\n");
	else if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bParityType ==
			 LINE_CODDING_PARITY_ODD)
		printf("Odd Parity\r\n");
	else if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bParityType ==
			 LINE_CODDING_PARITY_EVEN)
		printf("Even Parity\r\n");
	else if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bParityType ==
			 LINE_CODDING_PARITY_MARK)
		printf("Mark Parity\r\n");
	else if(ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bParityType ==
			 LINE_CODDING_PARITY_SPACE)
		printf("Space Parity\r\n");
	printf("Number of databits = %u bits\r\n",
			ComUsbDeviceRom::comUsbRomDevices[index]->lineCode.bDataBits);
	#endif
	return LPC_OK;
}



USB_INTERFACE_DESCRIPTOR* ComUsbDeviceRom::findInterfaceDescriptor(const uint8_t* descriptor,
		uint32_t interfaceNumber)
{
	USB_COMMON_DESCRIPTOR* pD = (USB_COMMON_DESCRIPTOR*)descriptor;
	USB_INTERFACE_DESCRIPTOR* interfaceDescriptor = NULL;

	while(pD->bLength) {
		if(pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {
			interfaceDescriptor = (USB_INTERFACE_DESCRIPTOR*) pD;
			if (interfaceDescriptor->bInterfaceNumber == interfaceNumber)
				break;
		}
		interfaceDescriptor = NULL;
		pD = (USB_COMMON_DESCRIPTOR*)((uint32_t)pD + pD->bLength);
	}
	return interfaceDescriptor;
}

}
