/*
 * MSC_ram.cpp
 *
 *  Created on: 14.11.2017
 *      Author: Stalker1290
 */

#include "USB/USB_Device_use_rom/MSC_ram.hpp"
#include "string.h"
#include "stdio.h"

const USBD_API_T* MCS_ram_dev_module_t::g_pUsbApi = (const USBD_API_T*) LPC_ROM_API->usbdApiBase;

MCS_ram_dev_module_t* MCS_ram_dev_module_t::MSC_ram_dev_ptr = (MCS_ram_dev_module_t*)NULL;

uint8_t MCS_ram_dev_module_t::g_InquiryStr[] = {'J', 'o', 'h', 'n', 'n', 'y', ' ', 'B', 'r', 'a', 'v', 'o', ' ',
		'R', 'a', 'm',' ', 'D', 'i', 's', 'k', ' ', ' ', ' ',
		   '1', '.', '0', ' ', }; //MUST 28 BYTES

uint8_t* MCS_ram_dev_module_t::memDiskArea_ptr = NULL;
uint32_t MCS_ram_dev_module_t::mem_size = 0;


MCS_ram_dev_module_t* MCS_ram_dev_module_t::get_MSC_RAM_dev(uint32_t mem_base, uint32_t mem_size)
{
	if(MCS_ram_dev_module_t::MSC_ram_dev_ptr == (MCS_ram_dev_module_t*)NULL)
		MCS_ram_dev_module_t::MSC_ram_dev_ptr = new MCS_ram_dev_module_t(mem_base,mem_size);
	return MCS_ram_dev_module_t::MSC_ram_dev_ptr;
}

MCS_ram_dev_module_t::MCS_ram_dev_module_t(uint32_t mem_base, uint32_t mem_size):USB_Devices_module_t()
{
	MCS_ram_dev_module_t::memDiskArea_ptr = (uint8_t*) mem_base;
	MCS_ram_dev_module_t::mem_size = mem_size;
	this->USB_num = 0;
}

void MCS_ram_dev_module_t::translate_rd(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	*buff_adr =  &MCS_ram_dev_module_t::memDiskArea_ptr[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32))];
}

void MCS_ram_dev_module_t::translate_wr(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	*buff_adr =  &MCS_ram_dev_module_t::memDiskArea_ptr[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) + length];
}

void MCS_ram_dev_module_t::translate_GetWrBuf(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	*buff_adr =  &MCS_ram_dev_module_t::memDiskArea_ptr[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32))];
}

ErrorCode_t MCS_ram_dev_module_t::translate_verify(uint32_t offset, uint8_t *src, uint32_t length, uint32_t hi_offset)
{
	if (memcmp((void *) &MCS_ram_dev_module_t::memDiskArea_ptr[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32))], src, length)) 
	{
		return ERR_FAILED;
	}
	return LPC_OK;
}

ErrorCode_t MCS_ram_dev_module_t::USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam)
{
	USBD_MSC_INIT_PARAM_T msc_param;
	ErrorCode_t ret = LPC_OK;

	memset((void *) &msc_param, 0, sizeof(USBD_MSC_INIT_PARAM_T));
	msc_param.mem_base = pUsbParam->mem_base;
	msc_param.mem_size = pUsbParam->mem_size;
	/* mass storage paramas */
	msc_param.InquiryStr = (uint8_t *)MCS_ram_dev_module_t::g_InquiryStr;
	msc_param.BlockCount = MSC_RAM_DISK_BLOCK_COUNT;
	msc_param.BlockSize = MSC_RAM_DISK_BLOCK_SIZE;
	msc_param.MemorySize = MCS_ram_dev_module_t::mem_size;
	/* Install memory storage callback routines */
	msc_param.MSC_Write = MCS_ram_dev_module_t::translate_wr;
	msc_param.MSC_Read = MCS_ram_dev_module_t::translate_rd;
	msc_param.MSC_Verify = MCS_ram_dev_module_t::translate_verify;
	msc_param.MSC_GetWriteBuf = MCS_ram_dev_module_t::translate_GetWrBuf;

	if(this->USB_num == 0)
	{
		msc_param.intf_desc = (uint8_t *) MCS_ram_dev_module_t::find_IntfDesc(pDesc->high_speed_desc, USB0_MSC_IF_NUM);
	}
	if(this->USB_num == 1)
	{
		msc_param.intf_desc = (uint8_t *) MCS_ram_dev_module_t::find_IntfDesc(pDesc->full_speed_desc, USB1_MSC_IF_NUM);
	}

	ret = MCS_ram_dev_module_t::g_pUsbApi->msc->init(hUsb, &msc_param);
	/* update memory variables */
	pUsbParam->mem_base = msc_param.mem_base;
	pUsbParam->mem_size = msc_param.mem_size;

	return ret;
}


void MCS_ram_dev_module_t::USB_Tasks(void)
{

}

void MCS_ram_dev_module_t::USB_GetParameter(uint8_t ParamName, void* ParamValue)
{

}

void MCS_ram_dev_module_t::USB_SetParameter(uint8_t ParamName, void* ParamValue)
{
	if(ParamName == USB_num_param)
	{
		this->USB_num = *((uint8_t*)ParamValue);
	}
}

USB_INTERFACE_DESCRIPTOR* MCS_ram_dev_module_t::find_IntfDesc(const uint8_t *pDesc, uint32_t IntfNum)
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
