/*
 * USB_COM.hpp
 *
 *  Created on: 23.10.2017
 *      Author: Stalker1290
 */

#include "chip.h"
#include "USB_Device_module.hpp"
#include "Void_Channel.hpp"

#ifndef USB_COM_HPP_
#define USB_COM_HPP_


class VCOM_Device_module_t:public void_channel_t, public USB_Devices_module_t
{
public:
	VCOM_Device_module_t(void);

	virtual ErrorCode_t USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam);
	virtual void USB_Tasks(void);
	virtual void USB_GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void USB_SetParameter(uint8_t ParamName, void* ParamValue);

	virtual void Initialize(void* (*mem_alloc)(size_t),uint16_t tx_buf_size, Channel_Call_Interface_t* call_interface_ptr);
	virtual void DEInitialize(void);
	virtual void Tx(uint8_t *mes,uint16_t m_size,void* param);
	virtual void GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void SetParameter(uint8_t ParamName, void* ParamValue);
	static VCOM_Device_module_t* VCOM_ptrs[Num_of_VCOM];
	static USBD_HANDLE_T hCdc[Num_of_VCOM];
	uint8_t USB_num;
	Channel_Call_Interface_t* call_interface_ptr;
	USBD_HANDLE_T hUSB;
	USB_EP_HANDLER_T g_defaultCdcHdlr;
	uint8_t Tx_busy;
	uint8_t UART_initialize;
	uint16_t CntrlLineState;
	CDC_LINE_CODING LineCode;
	uint8_t Connected;
	uint8_t CTS;
	RINGBUFF_T Tx_ring_buf;
	uint8_t tx_buf_usb[USB_HS_MAX_BULK_PACKET];
	uint8_t rx_buf_usb[USB_HS_MAX_BULK_PACKET];
	uint8_t RX_buf_queued;
	static const USBD_API_T* g_pUsbApi;
	static uint8_t VCOM_counter;

private:
	static USB_INTERFACE_DESCRIPTOR* find_IntfDesc(const uint8_t *pDesc, uint32_t IntfNum);

	uint8_t num;
	uint8_t* Tx_buf_ptr;
	uint16_t tx_buf_size;
};

//Defines for CtrlLineState
#define DTR (1<<0)
#define RTS (1<<1)

//Defines for LineCoding
#define Char_format_Stop_bit_1		0
#define Char_format_Stop_bit_1_5	1
#define Char_format_Stop_bit_2		2

#define Parity_None		0
#define Parity_Odd		1
#define Parity_Even		2
#define Parity_Mark		3
#define Parity_Space	4

#endif /* USB_COM_HPP_ */
