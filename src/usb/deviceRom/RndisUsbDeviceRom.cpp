/**
 * @file
 * @brief USB Device Rom RNDIS Device Realization
 *
 *
 * @note
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
 * Copyright © 2015 Dean Camera (dean [at] fourwalledcubicle [dot] com)
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

#include <stdint.h>
#include <string.h>
#include "usb/deviceRom/RndisUsbDeviceRom.hpp"

#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
#include <stdio.h>
#endif
#if USE_LWIP
#include "lwip/pbuf.h"
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;

uint32_t RndisUsbDeviceRom::adapterSupportedOidList_[28] = {
		OID_GEN_SUPPORTED_LIST,
		OID_GEN_PHYSICAL_MEDIUM,
		OID_GEN_HARDWARE_STATUS,
		OID_GEN_MEDIA_SUPPORTED,
		OID_GEN_MEDIA_IN_USE,
		OID_GEN_MAXIMUM_FRAME_SIZE,
		OID_GEN_MAXIMUM_TOTAL_SIZE,
		OID_GEN_LINK_SPEED,
		OID_GEN_TRANSMIT_BLOCK_SIZE,
		OID_GEN_RECEIVE_BLOCK_SIZE,
		OID_GEN_VENDOR_ID,
		OID_GEN_VENDOR_DESCRIPTION,
		OID_GEN_CURRENT_PACKET_FILTER,
		OID_GEN_MAXIMUM_TOTAL_SIZE,
		OID_GEN_MEDIA_CONNECT_STATUS,
		OID_GEN_XMIT_OK,
		OID_GEN_RCV_OK,
		OID_GEN_XMIT_ERROR,
		OID_GEN_RCV_ERROR,
		OID_GEN_RCV_NO_BUFFER,
		OID_802_3_PERMANENT_ADDRESS,
		OID_802_3_CURRENT_ADDRESS,
		OID_802_3_MULTICAST_LIST,
		OID_802_3_MAXIMUM_LIST_SIZE,
		OID_802_3_RCV_ERROR_ALIGNMENT,
		OID_802_3_XMIT_ONE_COLLISION,
		OID_802_3_XMIT_MORE_COLLISIONS,
		OID_GEN_CURRENT_LOOKAHEAD,
};

char RndisUsbDeviceRom::adapterVendorDescription_[] =
		"Johnny Bravo RNDIS Adapter";
const USBD_API_T* RndisUsbDeviceRom::usbApi_ =
		(const USBD_API_T*) LPC_ROM_API->usbdApiBase;



RndisUsbDeviceRom::RndisUsbDeviceRom(void) : IVoidEthernet(), IUsbDeviceRom()
{
	this->mac_[0] = 0x14;
	this->mac_[1] = (std::rand())&0xff;
	this->mac_[2] = (std::rand())&0xff;
	this->mac_[3] = (std::rand())&0xff;
	this->mac_[4] = (std::rand())&0xff;
	this->mac_[5] = (std::rand())&0xff;
	this->adapterMac_.Octets[0] = 0x0C;
	this->adapterMac_.Octets[1] = (std::rand())&0xff;
	this->adapterMac_.Octets[2] = (std::rand())&0xff;
	this->adapterMac_.Octets[3] = (std::rand())&0xff;
	this->adapterMac_.Octets[4] = (std::rand())&0xff;
	this->adapterMac_.Octets[5] = (std::rand())&0xff;
	memset(&this->rndisMessageBuffer_, 0, sizeof(this->rndisMessageBuffer_));
}



ErrorCode_t RndisUsbDeviceRom::initialize(USBD_HANDLE_T usbHandle, USB_CORE_DESCS_T* descriptors,
		USBD_API_INIT_PARAM_T* initParameters)
{
	this->usbHandle_ = usbHandle;
	if(initParameters->mem_size < (sizeof(RndisTxQueue_t) + sizeof(RndisRxQueue_t)))
		return ERR_FAILED;
	this->txQueue_ = (RndisTxQueue_t*)initParameters->mem_base;
	initParameters->mem_base += sizeof(RndisTxQueue_t);
	initParameters->mem_size -= sizeof(RndisTxQueue_t);
	this->rxQueue_ = (RndisRxQueue_t*)initParameters->mem_base;
	initParameters->mem_base += sizeof(RndisRxQueue_t);
	initParameters->mem_size -= sizeof(RndisRxQueue_t);
	memset((void *)this->txQueue_, 0, sizeof(RndisTxQueue_t));
	memset((void *)this->rxQueue_, 0, sizeof(RndisRxQueue_t));

	ErrorCode_t ret = usbApi_->core->RegisterClassHandler(usbHandle,
			ep0Handler, (void*)this);
	if (ret != LPC_OK) {
		#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
		printf("RNDIS USB Device Rom Error: RegisterClassHandler 0x%X", ret);
		#endif
	}
	else {
		uint32_t epIndex = (((this->usbNumber_) ? RNDIS_DEVICE_ROM_OUT_EP_USB_1 :
				RNDIS_DEVICE_ROM_OUT_EP_USB_0) & 0x0F) << 1;
		ret = usbApi_->core->RegisterEpHandler(usbHandle, epIndex, bulkOutHandler, this);
		if (ret != LPC_OK) {
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("RNDIS USB Device Rom Error: Register BulkOut Ep Handler 0x%X",ret);
			#endif
		}
		else {
			epIndex = (((((this->usbNumber_) ? RNDIS_DEVICE_ROM_IN_EP_USB_1 :
					RNDIS_DEVICE_ROM_IN_EP_USB_0) & 0x0F) << 1) + 1);
			ret = usbApi_->core->RegisterEpHandler(usbHandle, epIndex, bulkInHandler, this);
			if (ret != LPC_OK){
				#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
				printf("RNDIS USB Device Rom Error: Register BulkIN Ep Handler 0x%X",ret);
				#endif
			}
		}
	}
	return ret;
}



void RndisUsbDeviceRom::getParameter(const UsbRomDeviceParameters_t parameter,
		void* const value)
{
	if(parameter == USB_DEVICE_PARAMETER_RNDIS_ADAPTER_MAC)
		*((uint8_t**)value) = this->adapterMac_.Octets;
}



void RndisUsbDeviceRom::setParameter(const UsbRomDeviceParameters_t parameter,
		void* const value)
{
	if(parameter == USB_DEVICE_PARAMETER_USB_NUMBER)
		this->usbNumber_ = *((uint32_t*)value);
	else if(parameter == USB_DEVICE_PARAMETER_RNDIS_ADAPTER_MAC)
		memcpy(this->adapterMac_.Octets, (uint8_t*)value, 6);
}



void RndisUsbDeviceRom::doTasks(void)
{
	if(!USB_IsConfigured(this->usbHandle_))
		return;
	if (this->isResponseReady_) {

		MyUsbRequestHeader_t notification = {
			.bmRequestType = ((1 << 7)/*REQDIR_DEVICETOHOST*/ |
					(1 << 5)/*REQTYPE_CLASS*/ | (1 << 0)/*REQREC_INTERFACE*/),
			.bRequest      = RNDIS_NOTIF_ResponseAvailable,
			.wValue        = 0,
			.wIndex        = 0,
			.wLength       = 0
		};

		if(this->usbNumber_ == 0) {
			NVIC_DisableIRQ(USB0_IRQn);
			RndisUsbDeviceRom::usbApi_->hw->WriteEP(this->usbHandle_,
					RNDIS_DEVICE_ROM_INT_EP_USB_0,
					(uint8_t *)&notification, sizeof(MyUsbRequestHeader_t));
			NVIC_EnableIRQ(USB0_IRQn);
		}
		if(this->usbNumber_ == 1) {
			NVIC_DisableIRQ(USB1_IRQn);
			RndisUsbDeviceRom::usbApi_->hw->WriteEP(this->usbHandle_,
					RNDIS_DEVICE_ROM_INT_EP_USB_1,
					(uint8_t *)&notification, sizeof(MyUsbRequestHeader_t));
			NVIC_EnableIRQ(USB1_IRQn);
		}
		this->isResponseReady_ = false;
	}
}



void RndisUsbDeviceRom::initialize(void)
{

}



void RndisUsbDeviceRom::start(void)
{

}



void RndisUsbDeviceRom::reset(void)
{

}



void RndisUsbDeviceRom::getParameter(const uint8_t number, void* const value)
{
	if(number == PARAMETER_MAC)
		*((uint8_t**)value) = this->mac_;
	else if(number == PARAMETER_LINK) {
		if(this->currentRndisState_ == RNDIS_Data_Initialized)
			*(uint32_t*)value = 1;
		else
			*(uint32_t*)value = 0;
	}
	else if(number == PARAMETER_NAME)
		*((char**)value) = (char*)this->name_;
}



void RndisUsbDeviceRom::setParameter(const uint8_t number, void* const value)
{
	if(number == PARAMETER_TX_UNLOCK){
		if(*(uint32_t*)value)
			this->isTxUnlocked_ = true;
		else
			this->isTxUnlocked_ = false;
	}
	else if(number == PARAMETER_MAC)
		memcpy((char*)this->mac_, value, 6);
	else if(number == PARAMETER_NAME)
		memcpy(this->name_, value, 9);
}



bool RndisUsbDeviceRom::isTxQueueFull(void)
{
	if(this->txQueue_->ownFlags[this->txQueue_->bw])
		return true;
	else
		return false;
}



void RndisUsbDeviceRom::addToTxQueue(EthernetFrame* const frame, uint16_t frameSize)
{
	if(this->isTxUnlocked_ && (this->currentRndisState_ == RNDIS_Data_Initialized)) {
		if(!this->isTxQueueFull()) {
			RNDIS_Packet_Message_t* header =
					(RNDIS_Packet_Message_t*)&(this->txQueue_->packets[this->txQueue_->bw][0]);
			memset((void*)header, 0, sizeof(RNDIS_Packet_Message_t));

			header->MessageType   = REMOTE_NDIS_PACKET_MSG;
			header->MessageLength = sizeof(RNDIS_Packet_Message_t) + frameSize;
			header->DataOffset    = sizeof(RNDIS_Packet_Message_t) -
					sizeof(RNDIS_Message_Header_t);
			header->DataLength    = frameSize;
			memcpy((uint8_t*)&(this->txQueue_->packets[this->txQueue_->bw][sizeof(RNDIS_Packet_Message_t)]),
					frame, frameSize);
			this->txQueue_->ownFlags[this->txQueue_->bw] = 1;
			this->txQueue_->bw++;
			if(this->txQueue_->bw == RNDIS_DEVICE_ROM_TX_QUEUE_SIZE)
				this->txQueue_->bw = 0;
			if(this->txQueue_->isTxbusy == 0) {
				if(this->txQueue_->ownFlags[this->txQueue_->br] == 1) {
					header = (RNDIS_Packet_Message_t*)&(this->txQueue_->packets[this->txQueue_->br][0]);
					this->txQueue_->isTxbusy = 1;
					if(this->usbNumber_ == 0){
						NVIC_DisableIRQ(USB0_IRQn);
						RndisUsbDeviceRom::usbApi_->hw->WriteEP(this->usbHandle_,
								RNDIS_DEVICE_ROM_IN_EP_USB_0,
								(uint8_t *)&(this->txQueue_->packets[this->txQueue_->br][0]),
								header->MessageLength);
						NVIC_EnableIRQ(USB0_IRQn);
					}
					if(this->usbNumber_ == 1) {
						NVIC_DisableIRQ(USB1_IRQn);
						RndisUsbDeviceRom::usbApi_->hw->WriteEP(this->usbHandle_,
								RNDIS_DEVICE_ROM_IN_EP_USB_1,
								(uint8_t *)&(this->txQueue_->packets[this->txQueue_->br][0]),
								header->MessageLength);
						NVIC_EnableIRQ(USB1_IRQn);
					}
				}
			}
		}
		else {
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("RNDIS USB Device Rom Error: TX Queue OVERFLOW\r\n");
			#endif
		}
	}
}

#if USE_LWIP
void RndisUsbDeviceRom::addToTxQueue(struct pbuf* p)
{
	if(p == NULL)
		return;
	uint16_t frameSize = p->tot_len;
	if(frameSize == 0)
		return;
	if(this->isTxUnlocked_ && (this->currentRndisState_ == RNDIS_Data_Initialized)) {
		if(! this->isTxQueueFull()) {
			RNDIS_Packet_Message_t* header =
					(RNDIS_Packet_Message_t*)&(this->txQueue_->packets[this->txQueue_->bw][0]);
			memset((void*)header, 0, sizeof(RNDIS_Packet_Message_t));
			header->MessageType   = REMOTE_NDIS_PACKET_MSG;
			header->MessageLength = sizeof(RNDIS_Packet_Message_t) + frameSize;
			header->DataOffset    = sizeof(RNDIS_Packet_Message_t) -
					sizeof(RNDIS_Message_Header_t);
			header->DataLength    = frameSize;

			if(p->next != NULL) {
				uint16_t frameIndex = 0;
				while (p != NULL)
				{
					memcpy((uint8_t*)&( this->txQueue_->packets[this->txQueue_->bw][sizeof(RNDIS_Packet_Message_t) + frameIndex]),
							p->payload, p->len);
					frameIndex += p->len;
				    p = p->next;
				}
			}
			else
				memcpy((uint8_t*)&(this->txQueue_->packets[this->txQueue_->bw][sizeof(RNDIS_Packet_Message_t)]),
						p->payload,p->tot_len);
			this->txQueue_->ownFlags[this->txQueue_->bw] = 1;
			this->txQueue_->bw++;
			if(this->txQueue_->bw == RNDIS_DEVICE_ROM_TX_QUEUE_SIZE)
				this->txQueue_->bw = 0;
			if(this->txQueue_->isTxbusy == 0) {
				if(this->txQueue_->ownFlags[this->txQueue_->br] == 1) {
					header = (RNDIS_Packet_Message_t*)&(this->txQueue_->packets[this->txQueue_->br][0]);
					this->txQueue_->isTxbusy = 1;
					if(this->usbNumber_ == 0) {
						NVIC_DisableIRQ(USB0_IRQn);
						RndisUsbDeviceRom::usbApi_->hw->WriteEP(this->usbHandle_,
								RNDIS_DEVICE_ROM_IN_EP_USB_0,
								(uint8_t *)&(this->txQueue_->packets[this->txQueue_->br][0]),
								header->MessageLength);
						NVIC_EnableIRQ(USB0_IRQn);
					}
					if(this->usbNumber_ == 1) {
						NVIC_DisableIRQ(USB1_IRQn);
						RndisUsbDeviceRom::usbApi_->hw->WriteEP(this->usbHandle_,
								RNDIS_DEVICE_ROM_IN_EP_USB_1,
								(uint8_t *)&(this->txQueue_->packets[this->txQueue_->br][0]),
								header->MessageLength);
						NVIC_EnableIRQ(USB1_IRQn);
					}
				}
			}
		}
		else
		{
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("RNDIS USB Device Rom Error: TX Queue OVERFLOW\r\n");
			#endif
		}
	}
}
#endif



uint16_t RndisUsbDeviceRom::pullOutRxFrame(EthernetFrame* const frame)
{
	uint16_t count = D_A_MIN_B_MOD_C(this->rxQueue_->bw, this->rxQueue_->br,
			RNDIS_DEVICE_ROM_RX_QUEUE_SIZE);
	if(count == 0)
		return 0;
	else {
		RNDIS_Packet_Message_t* header = (RNDIS_Packet_Message_t*)&(this->rxQueue_->packets[this->rxQueue_->br][0]);
		uint16_t size = header->DataLength;
		memcpy(frame, (uint8_t*)&(this->rxQueue_->packets[this->rxQueue_->br][sizeof(RNDIS_Message_Header_t) + header->DataOffset]),
				size);
		this->rxQueue_->ownFlags[this->rxQueue_->br] = 0;
		this->rxQueue_->br++;
		if(this->rxQueue_->br == RNDIS_DEVICE_ROM_RX_QUEUE_SIZE)
			this->rxQueue_->br = 0;
		this->rxFramesCounter_++;
		return size;
	}
}



ErrorCode_t RndisUsbDeviceRom::bulkInHandler(USBD_HANDLE_T usbHandle, void* data,
		uint32_t event)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
		if (event == USB_EVT_IN_NAK) /**< 5    IN Packet - Not Acknowledged */
			printf("RNDIS USB Device Rom Error: BULK IN USB_EVT_IN_NAK\r\n");
		if (event == USB_EVT_IN_STALL) /**< 7    IN Packet - Stalled */
			printf("RNDIS USB Device Rom Error: BULK IN USB_EVT_IN_STALL\r\n");
		if (event == USB_EVT_IN_DMA_ERR)  /**< 13  DMA  IN EP - Error */
			printf("RNDIS USB Device Rom Error: BULK IN USB_EVT_IN_DMA_ERR\r\n");
	#endif
	if(event == USB_EVT_IN) {
		RndisUsbDeviceRom* rndisDevice = (RndisUsbDeviceRom*)data;
		rndisDevice->txFramesCounter_++;
		rndisDevice->txQueue_->ownFlags[rndisDevice->txQueue_->br] = 0;
		rndisDevice->txQueue_->br++;
		if(rndisDevice->txQueue_->br == RNDIS_DEVICE_ROM_TX_QUEUE_SIZE)
			rndisDevice->txQueue_->br = 0;
		if(rndisDevice->txQueue_->ownFlags[rndisDevice->txQueue_->br]) {
			RNDIS_Packet_Message_t* header =
					(RNDIS_Packet_Message_t*)&(rndisDevice->txQueue_->packets[rndisDevice->txQueue_->br][0]);
			usbApi_->hw->WriteEP(rndisDevice->usbHandle_,
					(rndisDevice->usbNumber_) ?
							RNDIS_DEVICE_ROM_IN_EP_USB_1 :
							RNDIS_DEVICE_ROM_IN_EP_USB_0,
					(uint8_t *)&(rndisDevice->txQueue_->packets[rndisDevice->txQueue_->br][0]),
					header->MessageLength);
		}
		else
			rndisDevice->txQueue_->isTxbusy = 0;
		return LPC_OK;
	}
	return ERR_USBD_UNHANDLED;
}



ErrorCode_t RndisUsbDeviceRom::bulkOutHandler(USBD_HANDLE_T usbHandle, void* data,
		uint32_t event)
{
	#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
		if (event == USB_EVT_OUT_STALL) /**< 6   OUT Packet - Stalled */
			printf("RNDIS USB Device Rom Error: BULK OUT USB_EVT_OUT_STALL\n\r\n\r\n\r");
		if (event == USB_EVT_OUT_DMA_ERR) /**< 12  DMA OUT EP - Error */
			printf("RNDIS USB Device Rom Error: BULK OUT USB_EVT_OUT_DMA_ERR\n\r\n\r\n\r");
	#endif
	RndisUsbDeviceRom* rndisDevice = (RndisUsbDeviceRom*)data;
	if (event == USB_EVT_OUT) {

		usbApi_->hw->ReadEP(rndisDevice->usbHandle_,
				(rndisDevice->usbNumber_) ?
						RNDIS_DEVICE_ROM_OUT_EP_USB_1 :
						RNDIS_DEVICE_ROM_OUT_EP_USB_0,
				(uint8_t*)&(rndisDevice->rxQueue_->packets[rndisDevice->rxQueue_->bw][0]));
		rndisDevice->rxQueue_->isBufferQueued = 0;
		RNDIS_Packet_Message_t* header =
				(RNDIS_Packet_Message_t*)&(rndisDevice->rxQueue_->packets[rndisDevice->rxQueue_->bw][0]);
		if(header->MessageType == REMOTE_NDIS_PACKET_MSG){
			rndisDevice->rxQueue_->ownFlags[rndisDevice->rxQueue_->bw] = 1;
			rndisDevice->rxQueue_->bw++;
			if(rndisDevice->rxQueue_->bw == RNDIS_DEVICE_ROM_RX_QUEUE_SIZE)
				rndisDevice->rxQueue_->bw = 0;
		}
	}
	if(event == USB_EVT_OUT_NAK) {
		if(rndisDevice->rxQueue_->isBufferQueued == 0) {
			uint16_t tempBw = rndisDevice->rxQueue_->bw + 1;
			if(tempBw == RNDIS_DEVICE_ROM_RX_QUEUE_SIZE)
				tempBw = 0;
			if(rndisDevice->rxQueue_->ownFlags[tempBw] == 0) {

				usbApi_->hw->ReadReqEP(rndisDevice->usbHandle_,
						(rndisDevice->usbNumber_) ?
								RNDIS_DEVICE_ROM_OUT_EP_USB_1 :
								RNDIS_DEVICE_ROM_OUT_EP_USB_0,
						(uint8_t*)&(rndisDevice->rxQueue_->packets[rndisDevice->rxQueue_->bw][0]),
						EMAC_ETH_MAX_FLEN + sizeof(RNDIS_Packet_Message_t));
				rndisDevice->rxQueue_->isBufferQueued = 1;
			}
			else {
				#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
				printf("RNDIS USB Device Rom Error: RX Queue OVERFLOW\n\r");
				#endif
			}
		}
	}
	return LPC_OK;
}



ErrorCode_t RndisUsbDeviceRom::ep0Handler(USBD_HANDLE_T usbHandle, void* data,
		uint32_t event)
{
	USB_CORE_CTRL_T* coreControl = (USB_CORE_CTRL_T *)usbHandle;
	RndisUsbDeviceRom* rndisDevice = (RndisUsbDeviceRom*)data;
	if ((coreControl->SetupPacket.bmRequestType.BM.Type == REQUEST_CLASS)&&
			(coreControl->SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_INTERFACE ) ) {
		if (coreControl->SetupPacket.bmRequestType.BM.Dir == REQUEST_HOST_TO_DEVICE) {
			if (coreControl->SetupPacket.bRequest == RNDIS_REQ_SendEncapsulatedCommand) {
				if (event == USB_EVT_SETUP) {
					rndisDevice->ep0DataCounter_ = coreControl->SetupPacket.wLength;
					rndisDevice->ep0DataIndex_ = 0;
					coreControl->EP0Data.pData = &coreControl->EP0Buf[0];
					if(rndisDevice->ep0DataCounter_ > USB_MAX_PACKET_EP0)
						coreControl->EP0Data.Count = USB_MAX_PACKET_EP0;
					else
						coreControl->EP0Data.Count = rndisDevice->ep0DataCounter_;
					return LPC_OK;
				}
				if(event == USB_EVT_OUT) {
					usbApi_->core->DataOutStage(usbHandle);
					if(rndisDevice->ep0DataCounter_ > USB_MAX_PACKET_EP0) {
						memcpy(&rndisDevice->rndisMessageBuffer_[rndisDevice->ep0DataIndex_],
								&coreControl->EP0Buf[0], USB_MAX_PACKET_EP0);
						coreControl->EP0Data.pData = &coreControl->EP0Buf[0];
						rndisDevice->ep0DataCounter_ -= USB_MAX_PACKET_EP0;
						rndisDevice->ep0DataIndex_ += USB_MAX_PACKET_EP0;
						if(rndisDevice->ep0DataCounter_ > USB_MAX_PACKET_EP0)
							coreControl->EP0Data.Count = USB_MAX_PACKET_EP0;
						else
							coreControl->EP0Data.Count = rndisDevice->ep0DataCounter_;
					}
					else {
						memcpy(&rndisDevice->rndisMessageBuffer_[rndisDevice->ep0DataIndex_],
								&coreControl->EP0Buf[0], rndisDevice->ep0DataCounter_);
						rndisDevice->processRndisControlMessage();
						coreControl->EP0Data.pData = &coreControl->EP0Buf[0];
						coreControl->EP0Data.Count = 0;
						rndisDevice->ep0DataCounter_ = 0;
						rndisDevice->ep0DataIndex_ = 0;
						usbApi_->core->StatusInStage(usbHandle);
					}
					return LPC_OK;
				}
			}
		}
		else if (coreControl->SetupPacket.bmRequestType.BM.Dir == REQUEST_DEVICE_TO_HOST) {
			if (coreControl->SetupPacket.bRequest == RNDIS_REQ_GetEncapsulatedResponse) {
				rndisDevice->messageHeader_ =
						(RNDIS_Message_Header_t*)rndisDevice->rndisMessageBuffer_;
				if (event == USB_EVT_SETUP) {
					if (!(rndisDevice->messageHeader_->MessageLength)) {
						rndisDevice->rndisMessageBuffer_[0] = 0;
						rndisDevice->messageHeader_->MessageLength = 1;
					}
					rndisDevice->ep0DataCounter_ =
							rndisDevice->messageHeader_->MessageLength;
					rndisDevice->ep0DataIndex_ = 0;
					coreControl->EP0Data.pData = &coreControl->EP0Buf[0];
					if(rndisDevice->ep0DataCounter_ > USB_MAX_PACKET_EP0) {
						coreControl->EP0Data.Count = USB_MAX_PACKET_EP0;
						memcpy(&coreControl->EP0Buf[0],
								&rndisDevice->rndisMessageBuffer_[rndisDevice->ep0DataIndex_],
								USB_MAX_PACKET_EP0);
						rndisDevice->ep0DataCounter_ -= USB_MAX_PACKET_EP0;
						rndisDevice->ep0DataIndex_ += USB_MAX_PACKET_EP0;
					}
					else {
						coreControl->EP0Data.Count = rndisDevice->ep0DataCounter_;
						memcpy(&coreControl->EP0Buf[0],
								&rndisDevice->rndisMessageBuffer_[rndisDevice->ep0DataIndex_],
								rndisDevice->ep0DataCounter_);
						rndisDevice->ep0DataCounter_ = 0;
						rndisDevice->ep0DataIndex_ = 0;
					}
					usbApi_->core->DataInStage(usbHandle);
					return LPC_OK;
				}
				if(event == USB_EVT_IN) {
					coreControl->EP0Data.pData = &coreControl->EP0Buf[0];
					if(rndisDevice->ep0DataCounter_ > 0) {
						if(rndisDevice->ep0DataCounter_ > USB_MAX_PACKET_EP0) {
							coreControl->EP0Data.Count = USB_MAX_PACKET_EP0;
							memcpy(&coreControl->EP0Buf[0],
									&rndisDevice->rndisMessageBuffer_[rndisDevice->ep0DataIndex_],
									USB_MAX_PACKET_EP0);
							rndisDevice->ep0DataCounter_ -= USB_MAX_PACKET_EP0;
							rndisDevice->ep0DataIndex_ += USB_MAX_PACKET_EP0;
						}
						else {
							coreControl->EP0Data.Count = rndisDevice->ep0DataCounter_;
							memcpy(&coreControl->EP0Buf[0],
									&rndisDevice->rndisMessageBuffer_[rndisDevice->ep0DataIndex_],
									rndisDevice->ep0DataCounter_);
							rndisDevice->ep0DataCounter_ = 0;
							rndisDevice->ep0DataIndex_ = 0;
						}
						usbApi_->core->DataInStage(usbHandle);
					}
					else {
						coreControl->EP0Data.Count = 0;
						usbApi_->core->StatusOutStage(usbHandle);
						rndisDevice->messageHeader_->MessageLength = 0;
						rndisDevice->ep0DataCounter_ = 0;
						rndisDevice->ep0DataIndex_ = 0;
					}
					return LPC_OK;
				}
			}
		}
	}
	return ERR_USBD_UNHANDLED;
}



void RndisUsbDeviceRom::processRndisControlMessage(void)
{
	switch (this->messageHeader_->MessageType)
	{
		case REMOTE_NDIS_INITIALIZE_MSG:
		{
			/* Initialize the adapter - return
			 * information about the supported RNDIS version and buffer sizes */
			this->isResponseReady_ = true;
			RNDIS_Initialize_Message_t*  INITIALIZE_Message  =
					(RNDIS_Initialize_Message_t*)&this->rndisMessageBuffer_;
			RNDIS_Initialize_Complete_t* INITIALIZE_Response =
					(RNDIS_Initialize_Complete_t*)&this->rndisMessageBuffer_;

			INITIALIZE_Response->MessageType           = REMOTE_NDIS_INITIALIZE_CMPLT;
			INITIALIZE_Response->MessageLength         = sizeof(RNDIS_Initialize_Complete_t);
			INITIALIZE_Response->RequestId             = INITIALIZE_Message->RequestId;
			INITIALIZE_Response->Status                = REMOTE_NDIS_STATUS_SUCCESS;

			INITIALIZE_Response->MajorVersion          = REMOTE_NDIS_VERSION_MAJOR;
			INITIALIZE_Response->MinorVersion          = REMOTE_NDIS_VERSION_MINOR;
			INITIALIZE_Response->DeviceFlags           = REMOTE_NDIS_DF_CONNECTIONLESS;
			INITIALIZE_Response->Medium                = REMOTE_NDIS_MEDIUM_802_3;
			INITIALIZE_Response->MaxPacketsPerTransfer = 1;
			INITIALIZE_Response->MaxTransferSize       = (sizeof(RNDIS_Packet_Message_t) + EMAC_ETH_MAX_FLEN);
			INITIALIZE_Response->PacketAlignmentFactor = 0;
			INITIALIZE_Response->AFListOffset          = 0;
			INITIALIZE_Response->AFListSize            = 0;

			if(this->usbNumber_ == 0) {
				NVIC_DisableIRQ(USB0_IRQn);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_IN_EP_USB_0);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_OUT_EP_USB_0);
				memset((void *)this->txQueue_, 0, sizeof(RndisTxQueue_t));
				memset((void *)this->rxQueue_, 0, sizeof(RndisRxQueue_t));
				NVIC_EnableIRQ(USB0_IRQn);
			}
			else if(this->usbNumber_ == 1) {
				NVIC_DisableIRQ(USB1_IRQn);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_IN_EP_USB_1);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_OUT_EP_USB_1);
				memset((void *)this->txQueue_, 0, sizeof(RndisTxQueue_t));
				memset((void *)this->rxQueue_, 0, sizeof(RndisRxQueue_t));
				NVIC_EnableIRQ(USB1_IRQn);
			}
			this->currentRndisState_ = RNDIS_Initialized;
		}
		break;

		case REMOTE_NDIS_HALT_MSG:
		{
			/* Halt the adapter, reset the adapter state -
			 * note that no response should be returned when completed */
			this->isResponseReady_ = false;
			this->messageHeader_->MessageLength = 0;
			this->currentRndisState_ = RNDIS_Uninitialized;
		}
		break;

		case REMOTE_NDIS_QUERY_MSG:
		{
			/* Request for information about a parameter about the adapter,
			 * specified as an OID token */
			this->isResponseReady_ = true;
			RNDIS_Query_Message_t*  QUERY_Message  = (RNDIS_Query_Message_t*)&this->rndisMessageBuffer_;
			RNDIS_Query_Complete_t* QUERY_Response = (RNDIS_Query_Complete_t*)&this->rndisMessageBuffer_;
			uint32_t                Query_Oid      = QUERY_Message->Oid;

			void*     QueryData                 = &this->rndisMessageBuffer_[sizeof(RNDIS_Message_Header_t) + QUERY_Message->InformationBufferOffset];
			void*     ResponseData              = &this->rndisMessageBuffer_[sizeof(RNDIS_Query_Complete_t)];
			uint16_t  ResponseSize;

			QUERY_Response->MessageType         = REMOTE_NDIS_QUERY_CMPLT;
			QUERY_Response->MessageLength       = sizeof(RNDIS_Query_Complete_t);

			if (processRndisQuery(Query_Oid, QueryData, QUERY_Message->InformationBufferLength, ResponseData, &ResponseSize)) {
				QUERY_Response->Status                  = REMOTE_NDIS_STATUS_SUCCESS;
				QUERY_Response->MessageLength          += ResponseSize;
				QUERY_Response->InformationBufferLength = ResponseSize;
				QUERY_Response->InformationBufferOffset = (sizeof(RNDIS_Query_Complete_t) - sizeof(RNDIS_Message_Header_t));
			}
			else {
				QUERY_Response->Status                  = REMOTE_NDIS_STATUS_NOT_SUPPORTED;
				QUERY_Response->InformationBufferLength = 0;
				QUERY_Response->InformationBufferOffset = 0;
			}
		}
		break;

		case REMOTE_NDIS_SET_MSG:
		{
			/* Request to set a parameter of the adapter, specified as an OID token */
			this->isResponseReady_ = true;
			RNDIS_Set_Message_t*  SET_Message  = (RNDIS_Set_Message_t*)&this->rndisMessageBuffer_;
			RNDIS_Set_Complete_t* SET_Response = (RNDIS_Set_Complete_t*)&this->rndisMessageBuffer_;
			uint32_t              SET_Oid      = SET_Message->Oid;

			SET_Response->MessageType       = REMOTE_NDIS_SET_CMPLT;
			SET_Response->MessageLength     = sizeof(RNDIS_Set_Complete_t);
			SET_Response->RequestId         = SET_Message->RequestId;

			void* SetData                   = &this->rndisMessageBuffer_[sizeof(RNDIS_Message_Header_t) + SET_Message->InformationBufferOffset];

			if (processRndisSet(SET_Oid, SetData, SET_Message->InformationBufferLength))
			  SET_Response->Status = REMOTE_NDIS_STATUS_SUCCESS;
			else
			  SET_Response->Status = REMOTE_NDIS_STATUS_NOT_SUPPORTED;
		}
		break;

		case REMOTE_NDIS_RESET_MSG:
		{
			/* Soft reset the adapter */
			this->rxFramesCounter_ = 0;
			this->txFramesCounter_ = 0;
			if(this->usbNumber_ == 0) {
				NVIC_DisableIRQ(USB0_IRQn);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_IN_EP_USB_0);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_OUT_EP_USB_0);
				memset((void *)this->txQueue_, 0, sizeof(RndisTxQueue_t));
				memset((void *)this->rxQueue_, 0, sizeof(RndisRxQueue_t));
				NVIC_EnableIRQ(USB0_IRQn);
			}
			else if(this->usbNumber_ == 1) {
				NVIC_DisableIRQ(USB1_IRQn);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_IN_EP_USB_1);
				RndisUsbDeviceRom::usbApi_->hw->ResetEP(this->usbHandle_,
						RNDIS_DEVICE_ROM_OUT_EP_USB_1);
				memset((void *)this->txQueue_, 0, sizeof(RndisTxQueue_t));
				memset((void *)this->rxQueue_, 0, sizeof(RndisRxQueue_t));
				NVIC_EnableIRQ(USB1_IRQn);
			}
			this->isResponseReady_ = true;
			RNDIS_Reset_Complete_t* RESET_Response = (RNDIS_Reset_Complete_t*)&this->rndisMessageBuffer_;

			RESET_Response->MessageType         = REMOTE_NDIS_RESET_CMPLT;
			RESET_Response->MessageLength       = sizeof(RNDIS_Reset_Complete_t);
			RESET_Response->Status              = REMOTE_NDIS_STATUS_SUCCESS;
			RESET_Response->AddressingReset     = 0;
		}
		break;

		case REMOTE_NDIS_KEEPALIVE_MSG:
		{
			/* Keep alive message sent to the adapter every 5 seconds when idle
			 * to ensure it is still responding */
			this->isResponseReady_ = true;
			RNDIS_KeepAlive_Message_t*  KEEPALIVE_Message  = (RNDIS_KeepAlive_Message_t*)&this->rndisMessageBuffer_;
			RNDIS_KeepAlive_Complete_t* KEEPALIVE_Response = (RNDIS_KeepAlive_Complete_t*)&this->rndisMessageBuffer_;

			KEEPALIVE_Response->MessageType     = REMOTE_NDIS_KEEPALIVE_CMPLT;
			KEEPALIVE_Response->MessageLength   = sizeof(RNDIS_KeepAlive_Complete_t);
			KEEPALIVE_Response->RequestId       = KEEPALIVE_Message->RequestId;
			KEEPALIVE_Response->Status          = REMOTE_NDIS_STATUS_SUCCESS;
		}
		break;
	}
}



bool RndisUsbDeviceRom::processRndisQuery(const uint32_t oid, void* queryData, uint16_t querySize,
		void* responseData, uint16_t* responseSize)
{
	switch (oid)
	{
		case OID_GEN_VENDOR_DRIVER_VERSION:
		{
			*responseSize = sizeof(uint32_t);
			*((uint32_t*)responseData) = 0x00001000;
		}
		return true;

		case OID_GEN_MAC_OPTIONS:
		{
			*responseSize = sizeof(uint32_t);
			*((uint32_t*)responseData) = 0;
		}
		return true;

		case OID_GEN_SUPPORTED_LIST:
		{
			*responseSize = sizeof(adapterSupportedOidList_);
			/* Copy the list of supported NDIS OID tokens to the response buffer */
			memcpy(responseData, adapterSupportedOidList_,
					sizeof(adapterSupportedOidList_));
		}
		return true;

		case OID_GEN_PHYSICAL_MEDIUM:
		{
			*responseSize = sizeof(uint32_t);
			/* Indicate that the device is a true ethernet link */
			*((uint32_t*)responseData) = 0;
		}
		return true;

		case OID_GEN_HARDWARE_STATUS:
		{
			*responseSize = sizeof(uint32_t);
			/* Always indicate hardware ready */
			*((uint32_t*)responseData) = NDIS_HardwareStatus_Ready;
		}
		return true;

		case OID_GEN_MEDIA_SUPPORTED:
		case OID_GEN_MEDIA_IN_USE:
		{
			*responseSize = sizeof(uint32_t);
			/* Indicate 802.3 (Ethernet) supported by the adapter */
			*((uint32_t*)responseData) = REMOTE_NDIS_MEDIUM_802_3;
		}
		return true;

		case OID_GEN_VENDOR_ID:
		{
			*responseSize = sizeof(uint32_t);
			/* Vendor ID 0x0xFFFFFF is reserved for vendors who have not
			 * purchased a NDIS VID */
			*((uint32_t*)responseData) = 0x00FFFFFF;
		}
		return true;

		case OID_GEN_MAXIMUM_FRAME_SIZE:
		case OID_GEN_TRANSMIT_BLOCK_SIZE:
		case OID_GEN_RECEIVE_BLOCK_SIZE:
		{
			*responseSize = sizeof(uint32_t);
			/* Indicate that the maximum frame size is the size of
			 * the ethernet frame buffer */
			*((uint32_t*)responseData) = EMAC_ETH_MAX_FLEN;
		}
		return true;

		case OID_GEN_VENDOR_DESCRIPTION:
		{
			*responseSize = sizeof(adapterVendorDescription_);
			/* Copy vendor description string to the response buffer */
			memcpy(responseData, adapterVendorDescription_,
					sizeof(adapterVendorDescription_));
		}
		return true;

		case OID_GEN_MEDIA_CONNECT_STATUS:
		{
			*responseSize = sizeof(uint32_t);
			/* Always indicate that the adapter is connected to a network */
			*((uint32_t*)responseData) = REMOTE_NDIS_MEDIA_STATE_CONNECTED;
		}
		return true;

		case OID_GEN_LINK_SPEED:
		{
			*responseSize = sizeof(uint32_t);
			/* Indicate 90Mb/s link speed */
			*((uint32_t*)responseData) = 900000;
		}
		return true;

		case OID_802_3_PERMANENT_ADDRESS:
		case OID_802_3_CURRENT_ADDRESS:
		{
			*responseSize = sizeof(MAC_Address_t);
			memcpy(responseData, &this->adapterMac_, sizeof(MAC_Address_t));
		}
		return true;

		case OID_802_3_MAXIMUM_LIST_SIZE:
		{
			*responseSize = sizeof(uint32_t);
			/* Indicate only one multicast address supported */
			*((uint32_t*)responseData) = 1;
		}
		return true;

		case OID_GEN_CURRENT_PACKET_FILTER:
		{
			*responseSize = sizeof(uint32_t);
			*((uint32_t*)responseData) = this->currentPacketFilter_;
		}
		return true;

		case OID_GEN_XMIT_OK:
		{
			*responseSize = sizeof(uint32_t);
			*((uint32_t*)responseData) = this->rxFramesCounter_;
		}
		return true;

		case OID_GEN_RCV_OK:
		{
			*responseSize = sizeof(uint32_t);
			*((uint32_t*)responseData) = this->txFramesCounter_;
		}
		return true;

		case OID_GEN_XMIT_ERROR:
		case OID_GEN_RCV_ERROR:
		case OID_GEN_RCV_NO_BUFFER:
		case OID_802_3_RCV_ERROR_ALIGNMENT:
		case OID_802_3_XMIT_ONE_COLLISION:
		case OID_802_3_XMIT_MORE_COLLISIONS:
		{
			*responseSize = sizeof(uint32_t);
			*((uint32_t*)responseData) = 0;
		}
		return true;

		case OID_GEN_MAXIMUM_TOTAL_SIZE:
		{
			*responseSize = sizeof(uint32_t);
			/* Indicate maximum overall buffer (Ethernet frame and RNDIS header) the adapter can handle */
			*((uint32_t*)responseData) =
					(sizeof(this->rndisMessageBuffer_) + EMAC_ETH_MAX_FLEN);
		}
		return true;

		default:
		{
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("RNDIS USB Device Rom Error: request for oid, that"
					"doesn't support %lu!n\r", oid);
			#endif
		}
		return false;
	}
}



bool RndisUsbDeviceRom::processRndisSet(uint32_t oid, void* setData, uint16_t setSize)
{
	switch (oid)
	{
		case OID_GEN_CURRENT_PACKET_FILTER:
		{
			/* Save the packet filter mask in case the host queries it again later */
			this->currentPacketFilter_ = *((uint32_t*)setData);
			/* Set the RNDIS state to initialized if the packet filter is non-zero */
			if(this->currentPacketFilter_) {
				this->currentRndisState_ = RNDIS_Data_Initialized;
				this->isTxUnlocked_ = true;
			}
			else
				this->currentRndisState_ = RNDIS_Initialized;
		}
		return true;

		case OID_802_3_MULTICAST_LIST:
		/* Do nothing - throw away the value from the host as it is unused */
		return true;

		case OID_GEN_CURRENT_LOOKAHEAD:
		return true;

		default:
		{
			#if (USE_CONSOLE && USB_DEVICE_ROM_USE_CONSOLE)
			printf("RNDIS USB Device Rom Error: set for oid, that"
					"doesn't support %lu!n\r", oid);
			#endif
		}
		return false;
	}
}

}
