/*
 * RNDIS_Device.hpp
 *
 *  Created on: 20.10.2017
 *      Author: Stalker1290
 */

#ifndef RNDIS_DEVICE_HPP_
#define RNDIS_DEVICE_HPP_

#include "chip.h"
#include "RNDIS_Device_DEFINES.h"
#include "USB_Device_module.hpp"
#include "Void_Ethernet.hpp"



class RNDIS_device_module_t:public Ethernet_t, public USB_Devices_module_t
{
public:
	RNDIS_device_module_t(void);
	virtual ErrorCode_t USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam);
	virtual void USB_Tasks(void);
	virtual void USB_GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void USB_SetParameter(uint8_t ParamName, void* ParamValue);

	virtual void Initialize(void);
	virtual void Start(void);
	virtual void ResetDevice(void);
	virtual void GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void SetParameter(uint8_t ParamName, void* ParamValue);
	virtual uint8_t Check_if_TX_queue_not_full(void);
	virtual void Add_to_TX_queue(EthernetFrame* mes,uint16_t m_size);
#ifdef USE_LWIP
	virtual void Add_to_TX_queue(struct pbuf* p);
#endif
	virtual uint16_t Get_num_frames_in_RX_queue(void);
	virtual uint16_t Pull_out_RX_Frame(EthernetFrame* Frame);
	ErrorCode_t RNDIS_EP0_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event);
	ErrorCode_t RNDIS_BulkIN_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event);
	ErrorCode_t RNDIS_BulkOUT_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event);
private:
	static const USBD_API_T* g_pUsbApi;
	static uint32_t AdapterSupportedOIDList[28];
	static char AdapterVendorDescription[];

	uint8_t Adapter_name[9];
	RNDIS_RX_Queue_t* Rx_queue_ptr;
	RNDIS_TX_Queue_t* Tx_queue_ptr;
	uint8_t USB_num;
	uint8_t MAC[6];
	MAC_Address_t AdapterMACAddress;
	uint8_t RNDISMessageBuffer[sizeof(AdapterSupportedOIDList) + sizeof(RNDIS_Query_Complete_t)];
	RNDIS_Message_Header_t* MessageHeader;
	bool ResponseReady;
	uint8_t CurrRNDISState;
	uint32_t CurrPacketFilter;
	uint8_t EP0_data_count;
	uint16_t EP0_data_index;
	USBD_HANDLE_T l_hUsb;
	uint32_t Rx_packet_counter;
	uint32_t Tx_packet_counter;
	uint8_t Tx_Unlocked;

	void ProcessRNDISControlMessage(void);
	bool ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize, void* ResponseData, uint16_t* ResponseSize);
	bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize);
};





#endif /* RNDIS_DEVICE_HPP_ */
