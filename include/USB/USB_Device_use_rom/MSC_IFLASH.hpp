/*
 * MSC_IFLASH.hpp
 *
 *  Created on: 16.11.2017
 *      Author: Stalker1290
 */

#ifndef MSC_IFLASH_HPP_
#define MSC_IFLASH_HPP_

#include "USB_Device_module.hpp"
#include "diskio.h"		/* FatFs lower layer API */

#ifdef USE_FS
#include "FileSystem.hpp"
#endif

#ifdef USE_FS
	class MCS_IFLASH_dev_module_t: public USB_Devices_module_t, public Storage_Device_FS_t
#else
	class MCS_IFLASH_dev_module_t: public USB_Devices_module_t
#endif
{
public:
	static MCS_IFLASH_dev_module_t* get_MSC_IFLASH_dev(uint32_t mem_base, uint32_t mem_size);
	virtual ErrorCode_t USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam);
	virtual void USB_Tasks(void);
	virtual void USB_GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void USB_SetParameter(uint8_t ParamName, void* ParamValue);
	static void Write_flash(uint32_t Adress,uint8_t* Buf, uint32_t size);

	#ifdef USE_FS
	virtual DSTATUS disk_initialize(void);
	virtual DSTATUS disk_status(void);
	virtual DRESULT disk_read(BYTE* buff, DWORD sector, UINT count);
	virtual DRESULT disk_write(const BYTE* buff, DWORD sector, UINT count);
	virtual DRESULT disk_ioctl(BYTE cmd, void* buff);
	#endif
private:
	MCS_IFLASH_dev_module_t(uint32_t mem_base, uint32_t mem_size);
	static void translate_rd(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset);
	static void translate_wr(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset);
	static void translate_GetWrBuf(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset);
	static ErrorCode_t translate_verify(uint32_t offset, uint8_t *src, uint32_t length, uint32_t hi_offset);
	static USB_INTERFACE_DESCRIPTOR* find_IntfDesc(const uint8_t *pDesc, uint32_t IntfNum);
	static void Get_sect_bank_num(uint32_t addr_in_sect, uint8_t* Sect_num, uint8_t* Bank_num);
	static void Write_page_part(uint32_t Adress,uint8_t* Buf, uint32_t size);
	static void Flash_page_write(uint32_t Adress, uint8_t* Buf);
	static ErrorCode_t app_bulk_out_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event);

	static USB_EP_HANDLER_T	default_bulk_out_hdlr;
	static const USBD_API_T* g_pUsbApi;
	static MCS_IFLASH_dev_module_t* MSC_IFLASH_dev_ptr;
	static uint8_t g_InquiryStr[];
	static uint8_t* memDiskArea_ptr;
	static uint32_t mem_size;
	uint8_t USB_num;
	static uint8_t WR_buf[MSC_BUFF_SIZE]__attribute__ ((aligned(512)));
	static void* g_pMscCtrl;
	static uint32_t	wrBuffIndex;// = 0;
	static uint32_t	startAddr;
	static uint32_t	xfer_buff_len;
	static uint32_t	total_xfer_len;
	static uint8_t	update_xfer_len;

	#ifdef USE_FS
	DSTATUS Disc_Stat;
	#endif
};



#endif /* MSC_RAM_HPP_ */
