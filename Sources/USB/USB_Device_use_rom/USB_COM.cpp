/*
 * USB_COM.cpp
 *
 *  Created on: 23.10.2017
 *      Author: Stalker1290
 */

#include "string.h"
#include "stdio.h"
#include "USB/USB_Device_use_rom/USB_COM.hpp"


const USBD_API_T* VCOM_Device_module_t::g_pUsbApi = (const USBD_API_T*) LPC_ROM_API->usbdApiBase;

VCOM_Device_module_t* VCOM_Device_module_t::VCOM_ptrs[Num_of_VCOM];
USBD_HANDLE_T VCOM_Device_module_t::hCdc[Num_of_VCOM];
uint8_t VCOM_Device_module_t::VCOM_counter = 0;

static ErrorCode_t SetCtrlLineState(USBD_HANDLE_T hCDC, uint16_t state);
static ErrorCode_t SendBreak(USBD_HANDLE_T hCDC, uint16_t mstime);
static ErrorCode_t SetLineCode(USBD_HANDLE_T hCDC, CDC_LINE_CODING* line_coding);
static ErrorCode_t CDC_ep0_override_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event);
static ErrorCode_t CDC_BulkIN_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event);
static ErrorCode_t CDC_BulkOUT_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event);


VCOM_Device_module_t::VCOM_Device_module_t(void):void_channel_t(), USB_Devices_module_t()
{
	this->USB_num = 0;
	this->UART_initialize = 0;
	this->Tx_buf_ptr = (uint8_t*)NULL;
	this->tx_buf_size = 0;
	this->num = VCOM_Device_module_t::VCOM_counter;
	VCOM_Device_module_t::VCOM_ptrs[this->num] = this;
	VCOM_Device_module_t::VCOM_counter++;
	if(VCOM_Device_module_t::VCOM_counter == Num_of_VCOM) VCOM_Device_module_t::VCOM_counter = 0;
}

ErrorCode_t VCOM_Device_module_t::USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam)
{
	USBD_CDC_INIT_PARAM_T cdc_param;
	ErrorCode_t ret = LPC_OK;
	uint32_t ep_indx;
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;

	memset((void *) &cdc_param, 0, sizeof(USBD_CDC_INIT_PARAM_T));

	this->hUSB = hUsb;

	cdc_param.mem_base = pUsbParam->mem_base;
	cdc_param.mem_size = pUsbParam->mem_size;
	if(this->USB_num == 0)
	{
		cdc_param.cif_intf_desc = (uint8_t *) VCOM_Device_module_t::find_IntfDesc(pDesc->high_speed_desc, USB0_VCOM_CIF_NUM);
		cdc_param.dif_intf_desc = (uint8_t *) VCOM_Device_module_t::find_IntfDesc(pDesc->high_speed_desc, USB0_VCOM_DIF_NUM);
	}
	if(this->USB_num == 1)
	{
		cdc_param.cif_intf_desc = (uint8_t *) VCOM_Device_module_t::find_IntfDesc(pDesc->full_speed_desc, USB1_VCOM_CIF_NUM);
		cdc_param.dif_intf_desc = (uint8_t *) VCOM_Device_module_t::find_IntfDesc(pDesc->full_speed_desc, USB1_VCOM_DIF_NUM);
	}
	cdc_param.SetCtrlLineState = SetCtrlLineState;
	cdc_param.SetLineCode =  SetLineCode;
	cdc_param.SendBreak = SendBreak;

	ret = VCOM_Device_module_t::g_pUsbApi->cdc->init(hUsb, &cdc_param, &VCOM_Device_module_t::hCdc[this->num]);
	if (ret != LPC_OK) return ret;

	/*	WORKAROUND for artf42016 ROM driver BUG:
		The default CDC class handler in initial ROM (REV A silicon) was not
		sending proper handshake after processing SET_REQUEST messages targeted
		to CDC interfaces. The workaround will send the proper handshake to host.
		Due to this bug some terminal applications such as Putty have problem
		establishing connection.
	 */
	this->g_defaultCdcHdlr = pCtrl->ep0_hdlr_cb[pCtrl->num_ep0_hdlrs - 1];
	/* store the default CDC handler and replace it with ours */
	pCtrl->ep0_hdlr_cb[pCtrl->num_ep0_hdlrs - 1] = CDC_ep0_override_hdlr;


	if(this->USB_num == 0) ep_indx = (((USB0_VCOM_IN_EP & 0x0F) << 1) + 1);
	if(this->USB_num == 1) ep_indx = (((USB1_VCOM_IN_EP & 0x0F) << 1) + 1);
	ret = VCOM_Device_module_t::g_pUsbApi->core->RegisterEpHandler(hUsb, ep_indx, CDC_BulkIN_Hdlr, this);
	if (ret != LPC_OK) return ret;

	if(this->USB_num == 0) ep_indx = ((USB0_VCOM_OUT_EP & 0x0F) << 1);
	if(this->USB_num == 1) ep_indx = ((USB1_VCOM_OUT_EP & 0x0F) << 1);
	ret = VCOM_Device_module_t::g_pUsbApi->core->RegisterEpHandler(hUsb, ep_indx, CDC_BulkOUT_Hdlr, this);
	if (ret != LPC_OK) return ret;

	/* update mem_base and size variables for cascading calls. */
	pUsbParam->mem_base = cdc_param.mem_base;
	pUsbParam->mem_size = cdc_param.mem_size;

	return ret;
}

void VCOM_Device_module_t::USB_Tasks(void)
{

}

void VCOM_Device_module_t::USB_GetParameter(uint8_t ParamName, void* ParamValue)
{

}

void VCOM_Device_module_t::USB_SetParameter(uint8_t ParamName, void* ParamValue)
{
	if(ParamName == USB_num_param)
	{
		this->USB_num = *((uint8_t*)ParamValue);
	}
}

void VCOM_Device_module_t::Initialize(void* (*mem_alloc)(size_t),uint16_t tx_buf_size, Channel_Call_Interface_t* call_interface_ptr)
{
	if(this->UART_initialize == 0)
	{
		if(tx_buf_size == 0) return;
		this->tx_buf_size = tx_buf_size;
		this->Tx_buf_ptr = (uint8_t*)mem_alloc(this->tx_buf_size);

		if(this->Tx_buf_ptr == (uint8_t*)NULL) return;
		if(call_interface_ptr == (Channel_Call_Interface_t*)NULL) return;
		this->call_interface_ptr = call_interface_ptr;
		RingBuffer_Init(&(this->Tx_ring_buf), this->Tx_buf_ptr, 1, this->tx_buf_size);

		this->UART_initialize = 1;
	}
}

void VCOM_Device_module_t::DEInitialize(void)
{
	this->UART_initialize = 0;
}

void VCOM_Device_module_t::Tx(uint8_t *mes,uint16_t m_size,void* param)
{
	uint32_t count;

	if(this->UART_initialize == 1)
	{
		if(m_size == 0) return;

		count = RingBuffer_InsertMult(&this->Tx_ring_buf, mes, m_size);
		if(count == 0)
		{
			#ifdef USE_CONSOLE
			#ifdef VCOM_console
			printf("Error in VCOM! TX ring buf is full!\r\n\r\n");
			#endif
			#endif
		}
		if(this->Connected)
		{
			if(this->Tx_busy == 0)
			{
				count = RingBuffer_PopMult(&this->Tx_ring_buf, this->tx_buf_usb, USB_HS_MAX_BULK_PACKET);
				if(count)
				{
					this->Tx_busy = 1;
					if(this->USB_num == 0)
					{
						NVIC_DisableIRQ(USB0_IRQn);
						VCOM_Device_module_t::g_pUsbApi->hw->WriteEP(this->hUSB, USB0_VCOM_IN_EP, this->tx_buf_usb, count);
						NVIC_EnableIRQ(USB0_IRQn);
					}
					if(this->USB_num == 1)
					{
						NVIC_DisableIRQ(USB1_IRQn);
						VCOM_Device_module_t::g_pUsbApi->hw->WriteEP(this->hUSB, USB1_VCOM_IN_EP, this->tx_buf_usb, count);
						NVIC_EnableIRQ(USB1_IRQn);
					}
				}
			}
		}
	}
}

void VCOM_Device_module_t::GetParameter(uint8_t ParamName, void* ParamValue)
{
	uint8_t* Param = (uint8_t*)ParamValue;
	if(ParamName == VCOM_RTS_param)
	{
		*Param = (this->CntrlLineState&RTS)>>1;
	}
	if(ParamName == VCOM_CONNECTED_param)
	{
		*Param = this->Connected;
	}
	if(ParamName == VCOM_Tx_full_param)
	{
		*Param = RingBuffer_IsFull(&this->Tx_ring_buf);
	}
}

void VCOM_Device_module_t::SetParameter(uint8_t ParamName, void* ParamValue)
{
	uint8_t* Param = (uint8_t*)ParamValue;
	if(ParamName == VCOM_CTS_param)
	{
		this->CTS = *Param;
	}
}

static ErrorCode_t CDC_BulkIN_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
	uint32_t count;
	VCOM_Device_module_t* Instance_ptr = (VCOM_Device_module_t*)data;

	#ifdef USE_CONSOLE
	#ifdef VCOM_console
		if (event == USB_EVT_IN_NAK) printf("Error in VCOM! BULK IN USB_EVT_IN_NAK\r\n\r\n\r\n");      /**< 5    IN Packet - Not Acknowledged */
		if (event == USB_EVT_IN_STALL) printf("Error in VCOM! BULK IN USB_EVT_IN_STALL\r\n\r\n\r\n");   /**< 7    IN Packet - Stalled */
		if (event == USB_EVT_IN_DMA_ERR) printf("Error in VCOM! BULK IN USB_EVT_IN_DMA_ERR\r\n\r\n\r\n"); /**< 13  DMA  IN EP - Error */
	#endif
	#endif
	if(event == USB_EVT_IN)
	{
		if(Instance_ptr->UART_initialize == 1)
		{
			count = RingBuffer_PopMult(&Instance_ptr->Tx_ring_buf, Instance_ptr->tx_buf_usb, USB_HS_MAX_BULK_PACKET);
			if(count)
			{
				if(Instance_ptr->USB_num == 0)
				{
					VCOM_Device_module_t::g_pUsbApi->hw->WriteEP(hUsb, USB0_VCOM_IN_EP, Instance_ptr->tx_buf_usb, count);
				}
				if(Instance_ptr->USB_num == 1)
				{
					VCOM_Device_module_t::g_pUsbApi->hw->WriteEP(hUsb, USB1_VCOM_IN_EP, Instance_ptr->tx_buf_usb, count);
				}
			}
			else Instance_ptr->Tx_busy = 0;
		}
		return LPC_OK;
	}
	return ERR_USBD_UNHANDLED;
}

static ErrorCode_t CDC_BulkOUT_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
	uint32_t count;
	VCOM_Device_module_t* Instance_ptr = (VCOM_Device_module_t*)data;

	#ifdef USE_CONSOLE
	#ifdef VCOM_console
		if (event == USB_EVT_OUT_STALL) printf("Error in VCOM! BULK OUT USB_EVT_OUT_STALL\r\n\r\n\r\n");   /**< 6   OUT Packet - Stalled */
		if (event == USB_EVT_OUT_DMA_ERR) printf("BError in VCOM! BULK OUT USB_EVT_OUT_DMA_ERR\r\n\r\n\r\n"); /**< 12  DMA OUT EP - Error */
	#endif
	#endif

	if (event == USB_EVT_OUT)
	{	
		if(Instance_ptr->USB_num == 0)
		{
			count = VCOM_Device_module_t::g_pUsbApi->hw->ReadEP(Instance_ptr->hUSB, USB0_VCOM_OUT_EP, Instance_ptr->rx_buf_usb);
		}
		if(Instance_ptr->USB_num == 1)
		{
			count = VCOM_Device_module_t::g_pUsbApi->hw->ReadEP(Instance_ptr->hUSB, USB1_VCOM_OUT_EP, Instance_ptr->rx_buf_usb);
		}
		Instance_ptr->RX_buf_queued = 0;
		if(Instance_ptr->UART_initialize == 1)
		{
			if(Instance_ptr->call_interface_ptr != (Channel_Call_Interface_t*)NULL)
			{
				Instance_ptr->call_interface_ptr->channel_callback(Instance_ptr->rx_buf_usb,count,Instance_ptr, NULL);
			}
		}
	}
	if(event == USB_EVT_OUT_NAK)
	{
		if(Instance_ptr->CTS == 0)
		{
			if(Instance_ptr->RX_buf_queued == 0)
			{
				if(Instance_ptr->USB_num == 0)
				{
					VCOM_Device_module_t::g_pUsbApi->hw->ReadReqEP(Instance_ptr->hUSB, USB0_VCOM_OUT_EP,
							Instance_ptr->rx_buf_usb, USB_HS_MAX_BULK_PACKET);
				}
				if(Instance_ptr->USB_num == 1)
				{
					VCOM_Device_module_t::g_pUsbApi->hw->ReadReqEP(Instance_ptr->hUSB, USB1_VCOM_OUT_EP,
							Instance_ptr->rx_buf_usb, USB_HS_MAX_BULK_PACKET);
				}
				Instance_ptr->RX_buf_queued = 1;
			}
		}
	}
	return LPC_OK;
}


/* CDC EP0_patch part of WORKAROUND for artf42016. */
static ErrorCode_t CDC_ep0_override_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	uint8_t index = 0;
	USB_CORE_CTRL_T *pCtrl = (USB_CORE_CTRL_T *) hUsb;
	USB_CDC_CTRL_T *pCdcCtrl = (USB_CDC_CTRL_T *) data;
	USB_CDC0_CTRL_T *pCdc0Ctrl = (USB_CDC0_CTRL_T *) data;
	uint8_t cif_num, dif_num;
	CIC_SetRequest_t setReq;
	ErrorCode_t ret = ERR_USBD_UNHANDLED;

	if ( (event == USB_EVT_OUT) &&
		 (pCtrl->SetupPacket.bmRequestType.BM.Type == REQUEST_CLASS) &&
		 (pCtrl->SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_INTERFACE) ) {

		/* Check which CDC control structure to use. If epin_num doesn't have BIT7 set then we are
		   at wrong offset so use the old CDC control structure. BIT7 is set for all EP_IN endpoints.

		 */
		if ((pCdcCtrl->epin_num & 0x80) == 0) {
			cif_num = pCdc0Ctrl->cif_num;
			dif_num = pCdc0Ctrl->dif_num;
			setReq = pCdc0Ctrl->CIC_SetRequest;
		}
		else {
			cif_num = pCdcCtrl->cif_num;
			dif_num = pCdcCtrl->dif_num;
			setReq = pCdcCtrl->CIC_SetRequest;
		}
		/* is the request target is our interfaces */
		if (((pCtrl->SetupPacket.wIndex.WB.L == cif_num)  ||
			 (pCtrl->SetupPacket.wIndex.WB.L == dif_num)) ) {

			pCtrl->EP0Data.pData -= pCtrl->SetupPacket.wLength;
			ret = setReq(pCdcCtrl, &pCtrl->SetupPacket, &pCtrl->EP0Data.pData,
						 pCtrl->SetupPacket.wLength);
			if ( ret == LPC_OK) {
				/* send Acknowledge */
				VCOM_Device_module_t::g_pUsbApi->core->StatusInStage(pCtrl);
			}
		}

	}
	else
	{
		for(uint8_t i = 0; i<VCOM_Device_module_t::VCOM_counter; i++)
		{
			if(data == VCOM_Device_module_t::hCdc[i]) break;
			index++;
		}
		ret = VCOM_Device_module_t::VCOM_ptrs[index]->g_defaultCdcHdlr(hUsb, data, event);
	}
	return ret;
}

static ErrorCode_t SetCtrlLineState(USBD_HANDLE_T hCDC, uint16_t state)
{
	ErrorCode_t ret = LPC_OK;
	uint8_t index = 0;

	for(uint8_t i = 0; i<VCOM_Device_module_t::VCOM_counter; i++)
	{
		if(hCDC == VCOM_Device_module_t::hCdc[i]) break;
		index++;
	}

	VCOM_Device_module_t::VCOM_ptrs[index]->CntrlLineState = state;

	if(VCOM_Device_module_t::VCOM_ptrs[index]->CntrlLineState&DTR)
	{
		VCOM_Device_module_t::VCOM_ptrs[index]->Connected = 1;
	}
	else VCOM_Device_module_t::VCOM_ptrs[index]->Connected = 0;

	#ifdef USE_CONSOLE
	#ifdef VCOM_console
	printf("VCOM SetCtrlLineState\r\n");
	if(VCOM_Device_module_t::VCOM_ptrs[index]->CntrlLineState&DTR) printf("DTR ON! VCOM CONNECTED\r\n");
	else printf("DTR OFF! VCOM DISCONNECTED\r\n");
	if(VCOM_Device_module_t::VCOM_ptrs[index]->CntrlLineState&RTS) printf("RTS ON\r\n\r\n");
	else printf("RTS OFF\r\n\r\n");
	#endif
	#endif
	return ret;
}

static ErrorCode_t SendBreak(USBD_HANDLE_T hCDC, uint16_t mstime)
{
	ErrorCode_t ret = LPC_OK;
	uint8_t index = 0;

	for(uint8_t i = 0; i<VCOM_Device_module_t::VCOM_counter; i++)
	{
		if(hCDC == VCOM_Device_module_t::hCdc[i]) break;
		index++;
	}

	#ifdef USE_CONSOLE
	#ifdef VCOM_console
	printf("VCOM SendBreak\r\n\r\n");
	#endif
	#endif
	return ret;
}

static ErrorCode_t SetLineCode(USBD_HANDLE_T hCDC, CDC_LINE_CODING* line_coding)
{
	ErrorCode_t ret = LPC_OK;
	uint8_t index = 0;

	for(uint8_t i = 0; i<VCOM_Device_module_t::VCOM_counter; i++)
	{
		if(hCDC == VCOM_Device_module_t::hCdc[i]) break;
		index++;
	}

	memcpy(&VCOM_Device_module_t::VCOM_ptrs[index]->LineCode,line_coding, sizeof(CDC_LINE_CODING));

	#ifdef USE_CONSOLE
	#ifdef VCOM_console
	printf("VCOM Set Line Codding\r\n");
	printf("Speed = %i bits/s\r\n",VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.dwDTERate);

	if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bCharFormat == Char_format_Stop_bit_1) printf("1 Stop bit\r\n");
	else if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bCharFormat == Char_format_Stop_bit_1_5) printf("1.5 Stop bit\r\n");
	else if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bCharFormat == Char_format_Stop_bit_2) printf("2 Stop bit\r\n");

	if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bParityType == Parity_None) printf("None Parity\r\n");
	else if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bParityType == Parity_Odd) printf("Odd Parity\r\n");
	else if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bParityType == Parity_Even) printf("Even Parity\r\n");
	else if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bParityType == Parity_Mark) printf("Mark Parity\r\n");
	else if(VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bParityType == Parity_Space) printf("Space Parity\r\n");
	printf("Number of databits = %i bits\r\n\r\n",VCOM_Device_module_t::VCOM_ptrs[index]->LineCode.bDataBits);

	#endif
	#endif

	return ret;
}


USB_INTERFACE_DESCRIPTOR* VCOM_Device_module_t::find_IntfDesc(const uint8_t *pDesc, uint32_t IntfNum)
{
	USB_COMMON_DESCRIPTOR* pD = (USB_COMMON_DESCRIPTOR*) pDesc;
	USB_INTERFACE_DESCRIPTOR* pIntfDesc = 0;
	uint32_t next_desc_adr;

	while(pD->bLength)
	{
		if(pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE)
		{
			pIntfDesc = (USB_INTERFACE_DESCRIPTOR*) pD;
			if (pIntfDesc->bInterfaceNumber == IntfNum) break;
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR*)next_desc_adr;
	}
	return pIntfDesc;
}

