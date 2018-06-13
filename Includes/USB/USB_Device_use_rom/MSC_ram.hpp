/*
 * MSC_ram.hpp
 *
 *  Created on: 14.11.2017
 *      Author: Stalker1290
 */

#ifndef MSC_RAM_HPP_
#define MSC_RAM_HPP_

#include "USB_Device_module.hpp"

class MCS_ram_dev_module_t: public USB_Devices_module_t
{
public:
	static MCS_ram_dev_module_t* get_MSC_RAM_dev(uint32_t mem_base, uint32_t mem_size);
	virtual ErrorCode_t USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam);
	virtual void USB_Tasks(void);
	virtual void USB_GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void USB_SetParameter(uint8_t ParamName, void* ParamValue);
private:
	MCS_ram_dev_module_t(uint32_t mem_base, uint32_t mem_size);
	static void translate_rd(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset);
	static void translate_wr(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset);
	static void translate_GetWrBuf(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset);
	static ErrorCode_t translate_verify(uint32_t offset, uint8_t *src, uint32_t length, uint32_t hi_offset);
	static USB_INTERFACE_DESCRIPTOR* find_IntfDesc(const uint8_t *pDesc, uint32_t IntfNum);

	static const USBD_API_T* g_pUsbApi;
	static MCS_ram_dev_module_t* MSC_ram_dev_ptr;
	static uint8_t g_InquiryStr[];
	static uint8_t* memDiskArea_ptr;
	static uint32_t mem_size;
	uint8_t USB_num;
};



#endif /* MSC_RAM_HPP_ */
