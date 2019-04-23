/*
 * RNDIS_Device.cpp
 *
 *  Created on: 20.10.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "USB/USB_Device_use_rom/RNDIS_Device.hpp"
#include "stdio.h"
#include "stdint.h"
#include "string.h"

#ifdef USE_LWIP
#include "lwip/pbuf.h"
#endif

uint32_t RNDIS_device_module_t::AdapterSupportedOIDList[28]  =
{
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

char RNDIS_device_module_t::AdapterVendorDescription[] = "Johnny Bravo RNDIS Adapter";

const USBD_API_T* RNDIS_device_module_t::g_pUsbApi = (const USBD_API_T*) LPC_ROM_API->usbdApiBase;


static ErrorCode_t C_RNDIS_EP0_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event);
static ErrorCode_t C_RNDIS_BulkOUT_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event);
static ErrorCode_t C_RNDIS_BulkIN_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event);

RNDIS_device_module_t::RNDIS_device_module_t(void):Ethernet_t(),USB_Devices_module_t()
{
	uint8_t Temp_MAC[6] = ETH_RNDIS_DEFAULT_MAC;
	MAC_Address_t Temp_Adapter_MAC = ETH_RNDIS_ADAPTER_MAC;

	this->USB_num = 0;
	memcpy(this->MAC, Temp_MAC, 6);
	memcpy((void*)(this->AdapterMACAddress.Octets), (void*)Temp_Adapter_MAC.Octets, sizeof(MAC_Address_t));
	this->MessageHeader = (RNDIS_Message_Header_t*)&this->RNDISMessageBuffer;
	this->ResponseReady = false;
	this->CurrRNDISState = RNDIS_Uninitialized;
	this->CurrPacketFilter = 0;
	this->Rx_packet_counter = 0;
	this->Tx_packet_counter = 0;
	this->Tx_Unlocked = 0;
	memcpy(this->Adapter_name, "RNDIS   ",9);
}

ErrorCode_t RNDIS_device_module_t::USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam)
{
	uint32_t ep_index;
	ErrorCode_t ret = ERR_USBD_UNHANDLED;

	this->l_hUsb = hUsb;
	this->Tx_queue_ptr = (RNDIS_TX_Queue_t*)pUsbParam->mem_base;
	pUsbParam->mem_base += sizeof(RNDIS_TX_Queue_t);
	pUsbParam->mem_size -= sizeof(RNDIS_TX_Queue_t);
	this->Rx_queue_ptr = (RNDIS_RX_Queue_t*)pUsbParam->mem_base;
	pUsbParam->mem_base += sizeof(RNDIS_RX_Queue_t);
	pUsbParam->mem_size -= sizeof(RNDIS_RX_Queue_t);

	memset((void *)this->Tx_queue_ptr, 0, sizeof(RNDIS_TX_Queue_t));
	memset((void *)this->Rx_queue_ptr, 0, sizeof(RNDIS_RX_Queue_t));

	ret = RNDIS_device_module_t::g_pUsbApi->core->RegisterClassHandler(hUsb, C_RNDIS_EP0_hdlr, (void*)this);
	if (ret != LPC_OK)
	{
		#ifdef USE_CONSOLE
		#ifdef RNDIS_console
		printf("ERROR in RNDIS! RegisterClassHandler 0x%X",ret);
		#endif
		#endif
	}
	else
	{
		if(this->USB_num == 0) ep_index = ((USB0_RNDIS_OUT_EP & 0x0F) << 1);
		if(this->USB_num == 1) ep_index = ((USB1_RNDIS_OUT_EP & 0x0F) << 1);

		ret = RNDIS_device_module_t::g_pUsbApi->core->RegisterEpHandler(hUsb, ep_index, C_RNDIS_BulkOUT_hdlr, (void*)this);
		if (ret != LPC_OK)
		{
			#ifdef USE_CONSOLE
			#ifdef RNDIS_console
			printf("ERROR in RNDIS! Register BulkOut Ep Handler 0x%X",ret);
			#endif
			#endif
		}
		else
		{
			if(this->USB_num == 0) ep_index = (((USB0_RNDIS_IN_EP & 0x0F) << 1) + 1);
			if(this->USB_num == 1) ep_index = (((USB1_RNDIS_IN_EP & 0x0F) << 1) + 1);

			ret = RNDIS_device_module_t::g_pUsbApi->core->RegisterEpHandler(hUsb, ep_index, C_RNDIS_BulkIN_hdlr, (void*)this);
			if (ret != LPC_OK)
			{
				#ifdef USE_CONSOLE
				#ifdef RNDIS_console
				printf("ERROR in RNDIS! Register BulkIN Ep Handler 0x%X",ret);
				#endif
				#endif
			}
		}
	}
	return ret;
}


void RNDIS_device_module_t::USB_GetParameter(uint8_t ParamName, void* ParamValue)
{

}

void RNDIS_device_module_t::USB_SetParameter(uint8_t ParamName, void* ParamValue)
{
	if(ParamName == USB_num_param)
	{
		this->USB_num = *((uint8_t*)ParamValue);
	}
	if(ParamName == RNDIS_Adapter_MAC_param)
	{
		memcpy(this->AdapterMACAddress.Octets, (uint8_t*)ParamValue, 6);
	}
}

void RNDIS_device_module_t::USB_Tasks(void)
{
	if(!USB_IsConfigured(this->l_hUsb)) return;

	if (this->ResponseReady)
	{
		MY_USB_Request_Header_t Notification;
		Notification.bmRequestType = ((1 << 7)/*REQDIR_DEVICETOHOST*/ | (1 << 5)/*REQTYPE_CLASS*/ | (1 << 0)/*REQREC_INTERFACE*/);
		Notification.bRequest      = RNDIS_NOTIF_ResponseAvailable;
		Notification.wValue        = 0;
		Notification.wIndex        = 0;
		Notification.wLength       = 0;


		if(this->USB_num == 0)
		{
			NVIC_DisableIRQ(USB0_IRQn);
			RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB0_RNDIS_INT_EP, (uint8_t *)&Notification, sizeof(MY_USB_Request_Header_t));
			NVIC_EnableIRQ(USB0_IRQn);
		}
		if(this->USB_num == 1)
		{
			NVIC_DisableIRQ(USB1_IRQn);
			RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB1_RNDIS_INT_EP, (uint8_t *)&Notification, sizeof(MY_USB_Request_Header_t));
			NVIC_EnableIRQ(USB1_IRQn);
		}
		this->ResponseReady = false;
	}
}

void RNDIS_device_module_t::Initialize(void)
{

}

void RNDIS_device_module_t::Start(void)
{

}

void RNDIS_device_module_t::ResetDevice(void)
{

}

void RNDIS_device_module_t::GetParameter(uint8_t ParamName, void* ParamValue)
{
	if(ParamName == MAC_param)
	{
		*((uint8_t**)ParamValue) = this->MAC;
	}
	if(ParamName == LINK_param)
	{
		if(this->CurrRNDISState == RNDIS_Data_Initialized) *(uint8_t*)ParamValue = 1;
		else *(uint8_t*)ParamValue = 0;
	}
	if(ParamName == name_param)
	{
		*((char**)ParamValue) = (char*)this->Adapter_name;
	}
}

void RNDIS_device_module_t::SetParameter(uint8_t ParamName, void* ParamValue)
{
	uint8_t* Param = (uint8_t*)ParamValue;
	if(ParamName == Tx_Unlock_param)
	{
		this->Tx_Unlocked = *Param;
	}
	if(ParamName == MAC_param)
	{
		memcpy((char*)this->MAC,Param,6);
	}
	if(ParamName == name_param)
	{
		memcpy(this->Adapter_name, ParamValue, 9);
	}
}

uint8_t RNDIS_device_module_t::Check_if_TX_queue_not_full(void)
{
	if(this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->bw]) return 0;
	return 1;
}

void RNDIS_device_module_t::Add_to_TX_queue(EthernetFrame* mes,uint16_t m_size)
{
	RNDIS_Packet_Message_t* Header;

	if(this->Tx_Unlocked&&(this->CurrRNDISState == RNDIS_Data_Initialized))
	{
		if(this->Check_if_TX_queue_not_full())
		{
			Header = (RNDIS_Packet_Message_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->bw][0]);
			memset((void*)Header, 0, sizeof(RNDIS_Packet_Message_t));

			Header->MessageType   = REMOTE_NDIS_PACKET_MSG;
			Header->MessageLength = sizeof(RNDIS_Packet_Message_t) + m_size;
			Header->DataOffset    = sizeof(RNDIS_Packet_Message_t) - sizeof(RNDIS_Message_Header_t);
			Header->DataLength    = m_size;
			memcpy((uint8_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->bw][sizeof(RNDIS_Packet_Message_t)]),mes,m_size);
			this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->bw] = 1;
			this->Tx_queue_ptr->bw++;
			if(this->Tx_queue_ptr->bw == Eth_RNDIS_Tx_Queue_Len) this->Tx_queue_ptr->bw = 0;
			if(this->Tx_queue_ptr->tx_busy == 0)
			{
				if(this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->br] == 1)
				{
					Header = (RNDIS_Packet_Message_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]);
					this->Tx_queue_ptr->tx_busy = 1;
					if(this->USB_num == 0)
					{
						NVIC_DisableIRQ(USB0_IRQn);
						RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB0_RNDIS_IN_EP, (uint8_t *)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]), Header->MessageLength);
						NVIC_EnableIRQ(USB0_IRQn);
					}
					if(this->USB_num == 1)
					{
						NVIC_DisableIRQ(USB1_IRQn);
						RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB1_RNDIS_IN_EP, (uint8_t *)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]), Header->MessageLength);
						NVIC_EnableIRQ(USB1_IRQn);
					}
				}
			}
		}
		else
		{
			#ifdef USE_CONSOLE
			#ifdef RNDIS_console
			printf("ERROR in RNDIS! TX Queue OVERFLOW\n\r");
			#endif
			#endif
		}
	}
}

#ifdef USE_LWIP
void RNDIS_device_module_t::Add_to_TX_queue(struct pbuf* p)
{
	RNDIS_Packet_Message_t* Header;
	uint16_t frame_index = 0;
	uint16_t m_size = 0;

	if(p == NULL) return;

	m_size = p->tot_len;
	if(m_size == 0) return;

	if(this->Tx_Unlocked&&(this->CurrRNDISState == RNDIS_Data_Initialized))
	{
		if(this->Check_if_TX_queue_not_full())
		{
			Header = (RNDIS_Packet_Message_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->bw][0]);
			memset((void*)Header, 0, sizeof(RNDIS_Packet_Message_t));

			Header->MessageType   = REMOTE_NDIS_PACKET_MSG;
			Header->MessageLength = sizeof(RNDIS_Packet_Message_t) + m_size;
			Header->DataOffset    = sizeof(RNDIS_Packet_Message_t) - sizeof(RNDIS_Message_Header_t);
			Header->DataLength    = m_size;

			if(p->next != NULL)
			{
				while (p != NULL)
				{
					memcpy((uint8_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->bw][sizeof(RNDIS_Packet_Message_t) + frame_index]),p->payload,p->len);
					frame_index += p->len;
				    p = p->next;
				}
			}
			else
			{
				memcpy((uint8_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->bw][sizeof(RNDIS_Packet_Message_t)]),p->payload,p->tot_len);
			}

			this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->bw] = 1;
			this->Tx_queue_ptr->bw++;
			if(this->Tx_queue_ptr->bw == Eth_RNDIS_Tx_Queue_Len) this->Tx_queue_ptr->bw = 0;
			if(this->Tx_queue_ptr->tx_busy == 0)
			{
				if(this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->br] == 1)
				{
					Header = (RNDIS_Packet_Message_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]);
					this->Tx_queue_ptr->tx_busy = 1;
					if(this->USB_num == 0)
					{
						NVIC_DisableIRQ(USB0_IRQn);
						RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB0_RNDIS_IN_EP, (uint8_t *)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]), Header->MessageLength);
						NVIC_EnableIRQ(USB0_IRQn);
					}
					if(this->USB_num == 1)
					{
						NVIC_DisableIRQ(USB1_IRQn);
						RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB1_RNDIS_IN_EP, (uint8_t *)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]), Header->MessageLength);
						NVIC_EnableIRQ(USB1_IRQn);
					}
				}
			}
		}
		else
		{
			#ifdef USE_CONSOLE
			#ifdef RNDIS_console
			printf("ERROR in RNDIS! TX Queue OVERFLOW\n\r");
			#endif
			#endif
		}
	}
}
#endif

uint16_t RNDIS_device_module_t::Get_num_frames_in_RX_queue(void)
{
	uint16_t count;
	count = (this->Rx_queue_ptr->bw>=this->Rx_queue_ptr->br ?
				((this->Rx_queue_ptr->bw - this->Rx_queue_ptr->br) % Eth_RNDIS_Rx_Queue_Len) : (Eth_RNDIS_Rx_Queue_Len - this->Rx_queue_ptr->br + this->Rx_queue_ptr->bw));
	return count;
}

uint16_t RNDIS_device_module_t::Pull_out_RX_Frame(EthernetFrame* Frame)
{
	uint16_t count;
	int16_t size;
	RNDIS_Packet_Message_t* Header;

	count = (this->Rx_queue_ptr->bw>=this->Rx_queue_ptr->br ?
				((this->Rx_queue_ptr->bw - this->Rx_queue_ptr->br) % Eth_RNDIS_Rx_Queue_Len) : (Eth_RNDIS_Rx_Queue_Len - this->Rx_queue_ptr->br + this->Rx_queue_ptr->bw));
	if(count == 0) return 0;
	else
	{
		Header = (RNDIS_Packet_Message_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->br][0]);
		size = Header->DataLength;
		memcpy(Frame,(uint8_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->br][sizeof(RNDIS_Message_Header_t) + Header->DataOffset]),size);
		this->Rx_queue_ptr->OWN_flag[this->Rx_queue_ptr->br] = 0;
		this->Rx_queue_ptr->br++;
		if(this->Rx_queue_ptr->br == Eth_RNDIS_Rx_Queue_Len) this->Rx_queue_ptr->br = 0;
		this->Rx_packet_counter++;
		return size;
	}
}

static ErrorCode_t C_RNDIS_BulkIN_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	RNDIS_device_module_t* This_ptr = (RNDIS_device_module_t*)data;
	return This_ptr->RNDIS_BulkIN_hdlr(hUsb, data, event);
}


ErrorCode_t RNDIS_device_module_t::RNDIS_BulkIN_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	RNDIS_Packet_Message_t* Header;

	#ifdef USE_CONSOLE
	#ifdef RNDIS_console
		if (event == USB_EVT_IN_NAK) printf("Error in RNDIS! BULK IN USB_EVT_IN_NAK\n\r\n\r\n\r");      /**< 5    IN Packet - Not Acknowledged */
		if (event == USB_EVT_IN_STALL) printf("Error in RNDIS! BULK IN USB_EVT_IN_STALL\n\r\n\r\n\r");   /**< 7    IN Packet - Stalled */
		if (event == USB_EVT_IN_DMA_ERR) printf("Error in RNDIS! BULK IN USB_EVT_IN_DMA_ERR\n\r\n\r\n\r"); /**< 13  DMA  IN EP - Error */
	#endif
	#endif
	if(event == USB_EVT_IN)
	{
		this->Tx_packet_counter++;
		this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->br] = 0;
		this->Tx_queue_ptr->br++;
		if(this->Tx_queue_ptr->br == Eth_RNDIS_Tx_Queue_Len) this->Tx_queue_ptr->br = 0;
		if(this->Tx_queue_ptr->OWN_flag[this->Tx_queue_ptr->br])
		{
			Header = (RNDIS_Packet_Message_t*)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]);
			if(this->USB_num == 0) RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB0_RNDIS_IN_EP,
					(uint8_t *)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]), Header->MessageLength);
			if(this->USB_num == 1) RNDIS_device_module_t::g_pUsbApi->hw->WriteEP(this->l_hUsb, USB1_RNDIS_IN_EP,
					(uint8_t *)&(this->Tx_queue_ptr->Packets[this->Tx_queue_ptr->br][0]), Header->MessageLength);
		}
		else this->Tx_queue_ptr->tx_busy = 0;

		return LPC_OK;
	}
	return ERR_USBD_UNHANDLED;
}

static ErrorCode_t C_RNDIS_BulkOUT_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	RNDIS_device_module_t* This_ptr = (RNDIS_device_module_t*)data;
	return This_ptr->RNDIS_BulkOUT_hdlr(hUsb, data, event);
}

ErrorCode_t RNDIS_device_module_t::RNDIS_BulkOUT_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	RNDIS_Packet_Message_t* Header;
	uint16_t temp_bw;

	#ifdef USE_CONSOLE
	#ifdef RNDIS_console
		if (event == USB_EVT_OUT_STALL) printf("Error in RNDIS! BULK OUT USB_EVT_OUT_STALL\n\r\n\r\n\r");   /**< 6   OUT Packet - Stalled */
		if (event == USB_EVT_OUT_DMA_ERR) printf("BError in RNDIS! BULK OUT USB_EVT_OUT_DMA_ERR\n\r\n\r\n\r"); /**< 12  DMA OUT EP - Error */
	#endif
	#endif

	if (event == USB_EVT_OUT)
	{
		if(this->USB_num == 0) RNDIS_device_module_t::g_pUsbApi->hw->ReadEP(this->l_hUsb, USB0_RNDIS_OUT_EP,
				(uint8_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->bw][0]));
		if(this->USB_num == 1) RNDIS_device_module_t::g_pUsbApi->hw->ReadEP(this->l_hUsb, USB1_RNDIS_OUT_EP,
				(uint8_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->bw][0]));
		this->Rx_queue_ptr->RX_buf_queued = 0;
		Header = (RNDIS_Packet_Message_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->bw][0]);
		if(Header->MessageType == REMOTE_NDIS_PACKET_MSG)
		{
			this->Rx_queue_ptr->OWN_flag[this->Rx_queue_ptr->bw] = 1;
			this->Rx_queue_ptr->bw++;
			if(this->Rx_queue_ptr->bw == Eth_RNDIS_Rx_Queue_Len) this->Rx_queue_ptr->bw = 0;
		}
	}
	if(event == USB_EVT_OUT_NAK)
	{
		if(this->Rx_queue_ptr->RX_buf_queued == 0)
		{
			temp_bw = this->Rx_queue_ptr->bw + 1;
			if(temp_bw == Eth_RNDIS_Rx_Queue_Len) temp_bw = 0;
			if(this->Rx_queue_ptr->OWN_flag[temp_bw] == 0)
			{
				if(this->USB_num == 0) RNDIS_device_module_t::g_pUsbApi->hw->ReadReqEP(this->l_hUsb, USB0_RNDIS_OUT_EP,
						(uint8_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->bw][0]), EMAC_ETH_MAX_FLEN + sizeof(RNDIS_Packet_Message_t));
				if(this->USB_num == 1) RNDIS_device_module_t::g_pUsbApi->hw->ReadReqEP(this->l_hUsb, USB1_RNDIS_OUT_EP,
						(uint8_t*)&(this->Rx_queue_ptr->Packets[this->Rx_queue_ptr->bw][0]), EMAC_ETH_MAX_FLEN + sizeof(RNDIS_Packet_Message_t));
				this->Rx_queue_ptr->RX_buf_queued = 1;
			}
			else
			{
				#ifdef USE_CONSOLE
				#ifdef RNDIS_console
				printf("ERROR in RNDIS! RX Queue OVERFLOW\n\r");
				#endif
				#endif
			}
		}
	}
	return LPC_OK;
}


static ErrorCode_t C_RNDIS_EP0_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
	RNDIS_device_module_t* This_ptr = (RNDIS_device_module_t*)data;
	return This_ptr->RNDIS_EP0_hdlr(hUsb, data, event);
}


ErrorCode_t RNDIS_device_module_t::RNDIS_EP0_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_CORE_CTRL_T* pCtrl_hndl = (USB_CORE_CTRL_T *)hUsb;

	if ((pCtrl_hndl->SetupPacket.bmRequestType.BM.Type == REQUEST_CLASS)&&(pCtrl_hndl->SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_INTERFACE ) )
	{
		if (pCtrl_hndl->SetupPacket.bmRequestType.BM.Dir == REQUEST_HOST_TO_DEVICE)
		{
			if (pCtrl_hndl->SetupPacket.bRequest == RNDIS_REQ_SendEncapsulatedCommand)
			{
				if (event == USB_EVT_SETUP)
				{
					this->EP0_data_count = pCtrl_hndl->SetupPacket.wLength;
					this->EP0_data_index = 0;
					pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];

					if(this->EP0_data_count > USB_MAX_PACKET0) pCtrl_hndl->EP0Data.Count = USB_MAX_PACKET0;
					else pCtrl_hndl->EP0Data.Count = this->EP0_data_count;

					return LPC_OK;
				}
				if(event == USB_EVT_OUT)
				{
					if(this->EP0_data_count>USB_MAX_PACKET0)
					{
						RNDIS_device_module_t::g_pUsbApi->core->DataOutStage(hUsb);
						memcpy(&this->RNDISMessageBuffer[this->EP0_data_index], &pCtrl_hndl->EP0Buf[0], USB_MAX_PACKET0);
						pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
						this->EP0_data_count -= USB_MAX_PACKET0;
						this->EP0_data_index += USB_MAX_PACKET0;
						if(this->EP0_data_count > USB_MAX_PACKET0) pCtrl_hndl->EP0Data.Count = USB_MAX_PACKET0;
						else pCtrl_hndl->EP0Data.Count = this->EP0_data_count;
					}
					else
					{
						RNDIS_device_module_t::g_pUsbApi->core->DataOutStage(hUsb);
						memcpy(&this->RNDISMessageBuffer[this->EP0_data_index], &pCtrl_hndl->EP0Buf[0], this->EP0_data_count);
						ProcessRNDISControlMessage();
						pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
						pCtrl_hndl->EP0Data.Count = 0;
						this->EP0_data_count = 0;
						this->EP0_data_index = 0;
						RNDIS_device_module_t::g_pUsbApi->core->StatusInStage(hUsb);
					}
					return LPC_OK;
				}
			}
		}
		if (pCtrl_hndl->SetupPacket.bmRequestType.BM.Dir == REQUEST_DEVICE_TO_HOST)
		{
			if (pCtrl_hndl->SetupPacket.bRequest == RNDIS_REQ_GetEncapsulatedResponse)
			{
				this->MessageHeader = (RNDIS_Message_Header_t*)this->RNDISMessageBuffer;

				if (event == USB_EVT_SETUP)
				{
					if (!(this->MessageHeader->MessageLength))
					{
						this->RNDISMessageBuffer[0] = 0;
						this->MessageHeader->MessageLength = 1;
					}

					this->EP0_data_count = this->MessageHeader->MessageLength;
					this->EP0_data_index = 0;
					if(this->EP0_data_count>USB_MAX_PACKET0)
					{
						pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
						pCtrl_hndl->EP0Data.Count = USB_MAX_PACKET0;
						memcpy(&pCtrl_hndl->EP0Buf[0],&this->RNDISMessageBuffer[this->EP0_data_index], USB_MAX_PACKET0);
						this->EP0_data_count -= USB_MAX_PACKET0;
						this->EP0_data_index += USB_MAX_PACKET0;
					}
					else
					{
						pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
						pCtrl_hndl->EP0Data.Count = this->EP0_data_count;
						memcpy(&pCtrl_hndl->EP0Buf[0],&this->RNDISMessageBuffer[this->EP0_data_index], this->EP0_data_count);
						this->EP0_data_count = 0;
						this->EP0_data_index = 0;
					}
					RNDIS_device_module_t::g_pUsbApi->core->DataInStage(hUsb);
					return LPC_OK;
				}
				if(event == USB_EVT_IN)
				{
					if(this->EP0_data_count>0)
					{
						if(this->EP0_data_count>USB_MAX_PACKET0)
						{
							pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
							pCtrl_hndl->EP0Data.Count = USB_MAX_PACKET0;
							memcpy(&pCtrl_hndl->EP0Buf[0],&this->RNDISMessageBuffer[this->EP0_data_index], USB_MAX_PACKET0);
							this->EP0_data_count -= USB_MAX_PACKET0;
							this->EP0_data_index += USB_MAX_PACKET0;
						}
						else
						{
							pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
							pCtrl_hndl->EP0Data.Count = this->EP0_data_count;
							memcpy(&pCtrl_hndl->EP0Buf[0],&this->RNDISMessageBuffer[this->EP0_data_index], this->EP0_data_count);
							this->EP0_data_count = 0;
							this->EP0_data_index = 0;
						}
						RNDIS_device_module_t::g_pUsbApi->core->DataInStage(hUsb);
					}
					else
					{
						pCtrl_hndl->EP0Data.pData = &pCtrl_hndl->EP0Buf[0];
						pCtrl_hndl->EP0Data.Count = 0;
						RNDIS_device_module_t::g_pUsbApi->core->StatusOutStage(hUsb);
						this->MessageHeader->MessageLength = 0;
						this->EP0_data_count = 0;
						this->EP0_data_index = 0;
					}
					return LPC_OK;
				}
			}
		}
	}
	return ERR_USBD_UNHANDLED;
}


void RNDIS_device_module_t::ProcessRNDISControlMessage(void)
{
	switch (this->MessageHeader->MessageType)
	{
		case REMOTE_NDIS_INITIALIZE_MSG:
		{
			/* Initialize the adapter - return information about the supported RNDIS version and buffer sizes */

			this->ResponseReady = true;

			RNDIS_Initialize_Message_t*  INITIALIZE_Message  = (RNDIS_Initialize_Message_t*)&this->RNDISMessageBuffer;
			RNDIS_Initialize_Complete_t* INITIALIZE_Response = (RNDIS_Initialize_Complete_t*)&this->RNDISMessageBuffer;

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

			if(this->USB_num == 0)
			{
				NVIC_DisableIRQ(USB0_IRQn);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB0_RNDIS_IN_EP);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB0_RNDIS_OUT_EP);
				memset((void *)this->Tx_queue_ptr, 0, sizeof(RNDIS_TX_Queue_t));
				memset((void *)this->Rx_queue_ptr, 0, sizeof(RNDIS_RX_Queue_t));
				NVIC_EnableIRQ(USB0_IRQn);
			}
			if(this->USB_num == 1)
			{
				NVIC_DisableIRQ(USB1_IRQn);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB1_RNDIS_IN_EP);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB1_RNDIS_OUT_EP);
				memset((void *)this->Tx_queue_ptr, 0, sizeof(RNDIS_TX_Queue_t));
				memset((void *)this->Rx_queue_ptr, 0, sizeof(RNDIS_RX_Queue_t));
				NVIC_EnableIRQ(USB1_IRQn);
			}

			this->CurrRNDISState = RNDIS_Initialized;

			break;
		}
		case REMOTE_NDIS_HALT_MSG:
		{
			/* Halt the adapter, reset the adapter state - note that no response should be returned when completed */

			this->ResponseReady = false;
			this->MessageHeader->MessageLength = 0;

			this->CurrRNDISState = RNDIS_Uninitialized;

			break;
		}
		case REMOTE_NDIS_QUERY_MSG:
		{
			/* Request for information about a parameter about the adapter, specified as an OID token */

			this->ResponseReady = true;

			RNDIS_Query_Message_t*  QUERY_Message  = (RNDIS_Query_Message_t*)&this->RNDISMessageBuffer;
			RNDIS_Query_Complete_t* QUERY_Response = (RNDIS_Query_Complete_t*)&this->RNDISMessageBuffer;
			uint32_t                Query_Oid      = QUERY_Message->Oid;

			void*     QueryData                 = &this->RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) + QUERY_Message->InformationBufferOffset];
			void*     ResponseData              = &this->RNDISMessageBuffer[sizeof(RNDIS_Query_Complete_t)];
			uint16_t  ResponseSize;

			QUERY_Response->MessageType         = REMOTE_NDIS_QUERY_CMPLT;
			QUERY_Response->MessageLength       = sizeof(RNDIS_Query_Complete_t);

			if (ProcessNDISQuery(Query_Oid, QueryData, QUERY_Message->InformationBufferLength, ResponseData, &ResponseSize))
			{
				QUERY_Response->Status                  = REMOTE_NDIS_STATUS_SUCCESS;
				QUERY_Response->MessageLength          += ResponseSize;

				QUERY_Response->InformationBufferLength = ResponseSize;
				QUERY_Response->InformationBufferOffset = (sizeof(RNDIS_Query_Complete_t) - sizeof(RNDIS_Message_Header_t));
			}
			else
			{
				QUERY_Response->Status                  = REMOTE_NDIS_STATUS_NOT_SUPPORTED;

				QUERY_Response->InformationBufferLength = 0;
				QUERY_Response->InformationBufferOffset = 0;
			}

			break;
		}
		case REMOTE_NDIS_SET_MSG:
		{
			/* Request to set a parameter of the adapter, specified as an OID token */

			this->ResponseReady = true;

			RNDIS_Set_Message_t*  SET_Message  = (RNDIS_Set_Message_t*)&this->RNDISMessageBuffer;
			RNDIS_Set_Complete_t* SET_Response = (RNDIS_Set_Complete_t*)&this->RNDISMessageBuffer;
			uint32_t              SET_Oid      = SET_Message->Oid;

			SET_Response->MessageType       = REMOTE_NDIS_SET_CMPLT;
			SET_Response->MessageLength     = sizeof(RNDIS_Set_Complete_t);
			SET_Response->RequestId         = SET_Message->RequestId;

			void* SetData                   = &this->RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) + SET_Message->InformationBufferOffset];

			if (ProcessNDISSet(SET_Oid, SetData, SET_Message->InformationBufferLength))
			  SET_Response->Status = REMOTE_NDIS_STATUS_SUCCESS;
			else
			  SET_Response->Status = REMOTE_NDIS_STATUS_NOT_SUPPORTED;

			break;
		}
		case REMOTE_NDIS_RESET_MSG:
		{
			/* Soft reset the adapter */

			this->Rx_packet_counter = 0;
			this->Tx_packet_counter = 0;

			if(this->USB_num == 0)
			{
				NVIC_DisableIRQ(USB0_IRQn);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB0_RNDIS_IN_EP);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB0_RNDIS_OUT_EP);
				memset((void *)this->Tx_queue_ptr, 0, sizeof(RNDIS_TX_Queue_t));
				memset((void *)this->Rx_queue_ptr, 0, sizeof(RNDIS_RX_Queue_t));
				NVIC_EnableIRQ(USB0_IRQn);
			}
			if(this->USB_num == 1)
			{
				NVIC_DisableIRQ(USB1_IRQn);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB1_RNDIS_IN_EP);
				RNDIS_device_module_t::g_pUsbApi->hw->ResetEP(this->l_hUsb, USB1_RNDIS_OUT_EP);
				memset((void *)this->Tx_queue_ptr, 0, sizeof(RNDIS_TX_Queue_t));
				memset((void *)this->Rx_queue_ptr, 0, sizeof(RNDIS_RX_Queue_t));
				NVIC_EnableIRQ(USB1_IRQn);
			}

			this->ResponseReady = true;

			RNDIS_Reset_Complete_t* RESET_Response = (RNDIS_Reset_Complete_t*)&this->RNDISMessageBuffer;

			RESET_Response->MessageType         = REMOTE_NDIS_RESET_CMPLT;
			RESET_Response->MessageLength       = sizeof(RNDIS_Reset_Complete_t);
			RESET_Response->Status              = REMOTE_NDIS_STATUS_SUCCESS;
			RESET_Response->AddressingReset     = 0;

			break;
		}
		case REMOTE_NDIS_KEEPALIVE_MSG:
		{
			/* Keep alive message sent to the adapter every 5 seconds when idle to ensure it is still responding */
			this->ResponseReady = true;

			RNDIS_KeepAlive_Message_t*  KEEPALIVE_Message  = (RNDIS_KeepAlive_Message_t*)&this->RNDISMessageBuffer;
			RNDIS_KeepAlive_Complete_t* KEEPALIVE_Response = (RNDIS_KeepAlive_Complete_t*)&this->RNDISMessageBuffer;

			KEEPALIVE_Response->MessageType     = REMOTE_NDIS_KEEPALIVE_CMPLT;
			KEEPALIVE_Response->MessageLength   = sizeof(RNDIS_KeepAlive_Complete_t);
			KEEPALIVE_Response->RequestId       = KEEPALIVE_Message->RequestId;
			KEEPALIVE_Response->Status          = REMOTE_NDIS_STATUS_SUCCESS;

			break;
		}
	}
}

bool RNDIS_device_module_t::ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize, void* ResponseData, uint16_t* ResponseSize)
{
	switch (OId)
	{
		case OID_GEN_VENDOR_DRIVER_VERSION:
			*ResponseSize = sizeof(uint32_t);
			*((uint32_t*)ResponseData) = 0x00001000;
			return true;

		case OID_GEN_MAC_OPTIONS:
			*ResponseSize = sizeof(uint32_t);
			*((uint32_t*)ResponseData) = 0;
			return true;

		case OID_GEN_SUPPORTED_LIST:
			*ResponseSize = sizeof(RNDIS_device_module_t::AdapterSupportedOIDList);
			/* Copy the list of supported NDIS OID tokens to the response buffer */
			memcpy(ResponseData, RNDIS_device_module_t::AdapterSupportedOIDList, sizeof(RNDIS_device_module_t::AdapterSupportedOIDList));

			return true;

		case OID_GEN_PHYSICAL_MEDIUM:
			*ResponseSize = sizeof(uint32_t);
			/* Indicate that the device is a true ethernet link */
			*((uint32_t*)ResponseData) = 0;

			return true;

		case OID_GEN_HARDWARE_STATUS:
			*ResponseSize = sizeof(uint32_t);
			/* Always indicate hardware ready */
			*((uint32_t*)ResponseData) = NDIS_HardwareStatus_Ready;

			return true;

		case OID_GEN_MEDIA_SUPPORTED:
		case OID_GEN_MEDIA_IN_USE:
			*ResponseSize = sizeof(uint32_t);
			/* Indicate 802.3 (Ethernet) supported by the adapter */
			*((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIUM_802_3;

			return true;

		case OID_GEN_VENDOR_ID:
			*ResponseSize = sizeof(uint32_t);
			/* Vendor ID 0x0xFFFFFF is reserved for vendors who have not purchased a NDIS VID */
			*((uint32_t*)ResponseData) = 0x00FFFFFF;

			return true;

		case OID_GEN_MAXIMUM_FRAME_SIZE:
		case OID_GEN_TRANSMIT_BLOCK_SIZE:
		case OID_GEN_RECEIVE_BLOCK_SIZE:
			*ResponseSize = sizeof(uint32_t);
			/* Indicate that the maximum frame size is the size of the ethernet frame buffer */
			*((uint32_t*)ResponseData) = EMAC_ETH_MAX_FLEN;
			return true;

		case OID_GEN_VENDOR_DESCRIPTION:
			*ResponseSize = sizeof(RNDIS_device_module_t::AdapterVendorDescription);
			/* Copy vendor description string to the response buffer */
			memcpy(ResponseData, RNDIS_device_module_t::AdapterVendorDescription, sizeof(RNDIS_device_module_t::AdapterVendorDescription));

			return true;

		case OID_GEN_MEDIA_CONNECT_STATUS:
			*ResponseSize = sizeof(uint32_t);
			/* Always indicate that the adapter is connected to a network */
			*((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIA_STATE_CONNECTED;
			return true;

		case OID_GEN_LINK_SPEED:
			*ResponseSize = sizeof(uint32_t);
			/* Indicate 10Mb/s link speed */
			//*((uint32_t*)ResponseData) = 100000;
			*((uint32_t*)ResponseData) = 20000;
			/* Indicate 90Mb/s link speed */
			*((uint32_t*)ResponseData) = 900000;

			return true;

		case OID_802_3_PERMANENT_ADDRESS:
		case OID_802_3_CURRENT_ADDRESS:
			*ResponseSize = sizeof(MAC_Address_t);
			memcpy(ResponseData, &this->AdapterMACAddress, sizeof(MAC_Address_t));
			return true;

		case OID_802_3_MAXIMUM_LIST_SIZE:
			*ResponseSize = sizeof(uint32_t);
			/* Indicate only one multicast address supported */
			*((uint32_t*)ResponseData) = 1;
			return true;

		case OID_GEN_CURRENT_PACKET_FILTER:
			*ResponseSize = sizeof(uint32_t);
			*((uint32_t*)ResponseData) = this->CurrPacketFilter;
			return true;

		case OID_GEN_XMIT_OK:
			*ResponseSize = sizeof(uint32_t);
			*((uint32_t*)ResponseData) = this->Rx_packet_counter;
			return true;

		case OID_GEN_RCV_OK:
			*ResponseSize = sizeof(uint32_t);
			*((uint32_t*)ResponseData) = this->Tx_packet_counter;
			return true;

		case OID_GEN_XMIT_ERROR:
		case OID_GEN_RCV_ERROR:
		case OID_GEN_RCV_NO_BUFFER:
		case OID_802_3_RCV_ERROR_ALIGNMENT:
		case OID_802_3_XMIT_ONE_COLLISION:
		case OID_802_3_XMIT_MORE_COLLISIONS:
			*ResponseSize = sizeof(uint32_t);
			*((uint32_t*)ResponseData) = 0;
			return true;

		case OID_GEN_MAXIMUM_TOTAL_SIZE:
			*ResponseSize = sizeof(uint32_t);
			/* Indicate maximum overall buffer (Ethernet frame and RNDIS header) the adapter can handle */
			*((uint32_t*)ResponseData) = (sizeof(this->RNDISMessageBuffer) + EMAC_ETH_MAX_FLEN);
			return true;

		default:
			#ifdef USE_CONSOLE
			#ifdef RNDIS_console
			printf("ERROR in RNDIS! GET DOESNT SUPPORT OID!\n\r\n\r");
			#endif
			#endif
		return false;
	}
}

bool RNDIS_device_module_t::ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize)
{
	switch (OId)
	{
		case OID_GEN_CURRENT_PACKET_FILTER:
			/* Save the packet filter mask in case the host queries it again later */
			this->CurrPacketFilter = *((uint32_t*)SetData);
			/* Set the RNDIS state to initialized if the packet filter is non-zero */
			if(this->CurrPacketFilter)
			{
				this->CurrRNDISState = RNDIS_Data_Initialized;
				this->Tx_Unlocked = 1;
			}
			else this->CurrRNDISState = RNDIS_Initialized;

			return true;
		case OID_802_3_MULTICAST_LIST:
			/* Do nothing - throw away the value from the host as it is unused */
			return true;
		case OID_GEN_CURRENT_LOOKAHEAD:
			return true;
		default:
			#ifdef USE_CONSOLE
			#ifdef RNDIS_console
			printf("ERROR in RNDIS! GET DOESNT SUPPORT OID!\n\r\n\r");
			#endif
			#endif
		return false;
	}
}
