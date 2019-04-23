/*
 * MSC_ram.cpp
 *
 *  Created on: 14.11.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "USB/USB_Device_use_rom/MSC_IFLASH.hpp"
#include "string.h"
#include "stdio.h"
#include "CONTROLLER.hpp"


/*Mass Storage Control Structure for older ROM stack i.e version 0x01111101 */
typedef struct OLD_MSC_CTRL_T
{
  /* If it's a USB HS, the max packet is 512, if it's USB FS,
  the max packet is 64. Use 512 for both HS and FS. */
  /*ALIGNED(4)*/ uint8_t  BulkBuf[USB_HS_MAX_BULK_PACKET]; /* Bulk In/Out Buffer */
  /*ALIGNED(4)*/MSC_CBW CBW;                   /* Command Block Wrapper */
  /*ALIGNED(4)*/MSC_CSW CSW;                   /* Command Status Wrapper */

  USB_CORE_CTRL_T*  pUsbCtrl;

  uint32_t Offset;                  /* R/W Offset */
  uint32_t Length;                  /* R/W Length */
  uint32_t BulkLen;                 /* Bulk In/Out Length */
  uint8_t* rx_buf;

  uint8_t BulkStage;               /* Bulk Stage */
  uint8_t if_num;                  /* interface number */
  uint8_t epin_num;                /* BULK IN endpoint number */
  uint8_t epout_num;               /* BULK OUT endpoint number */
  uint32_t MemOK;                  /* Memory OK */

  uint8_t*  InquiryStr;
  uint32_t  BlockCount;
  uint32_t  BlockSize;
  uint32_t  MemorySize;
  /* user defined functions */
  void (*MSC_Write)( uint32_t offset, uint8_t** src, uint32_t length);
  void (*MSC_Read)( uint32_t offset, uint8_t** dst, uint32_t length);
  ErrorCode_t (*MSC_Verify)( uint32_t offset, uint8_t src[], uint32_t length);
  /* optional call back for MSC_Write optimization */
  void (*MSC_GetWriteBuf)( uint32_t offset, uint8_t** buff_adr, uint32_t length);

}USB_OLD_MSC_CTRL_T;

USB_EP_HANDLER_T	MCS_IFLASH_dev_module_t::default_bulk_out_hdlr;

const USBD_API_T* MCS_IFLASH_dev_module_t::g_pUsbApi = (const USBD_API_T*) LPC_ROM_API->usbdApiBase;

MCS_IFLASH_dev_module_t* MCS_IFLASH_dev_module_t::MSC_IFLASH_dev_ptr = (MCS_IFLASH_dev_module_t*)NULL;

uint8_t MCS_IFLASH_dev_module_t::g_InquiryStr[] = {'J', 'o', 'h', 'n', 'n', 'y', ' ', 'B', 'r', 'a', 'v', 'o', ' ',
		'I', 'F', 'L','A', 'S', 'H', ' ', 'D', 'i', 's', 'k',
		   ' ', '1', '.', '0', }; //MUST 28 BYTES

uint8_t* MCS_IFLASH_dev_module_t::memDiskArea_ptr = NULL;
uint32_t MCS_IFLASH_dev_module_t::mem_size = 0;

uint8_t MCS_IFLASH_dev_module_t::WR_buf[MSC_BUFF_SIZE];

void* MCS_IFLASH_dev_module_t::g_pMscCtrl;
uint32_t	MCS_IFLASH_dev_module_t::wrBuffIndex = 0;
uint32_t	MCS_IFLASH_dev_module_t::startAddr;
uint32_t	MCS_IFLASH_dev_module_t::xfer_buff_len;
uint32_t	MCS_IFLASH_dev_module_t::total_xfer_len;
uint8_t	MCS_IFLASH_dev_module_t::update_xfer_len;

MCS_IFLASH_dev_module_t* MCS_IFLASH_dev_module_t::get_MSC_IFLASH_dev(uint32_t mem_base, uint32_t mem_size)
{
	if(MCS_IFLASH_dev_module_t::MSC_IFLASH_dev_ptr == (MCS_IFLASH_dev_module_t*)NULL)
		MCS_IFLASH_dev_module_t::MSC_IFLASH_dev_ptr = new MCS_IFLASH_dev_module_t(mem_base,mem_size);
	return MCS_IFLASH_dev_module_t::MSC_IFLASH_dev_ptr;
}

#ifdef USE_FS
MCS_IFLASH_dev_module_t::MCS_IFLASH_dev_module_t(uint32_t mem_base, uint32_t mem_size):USB_Devices_module_t(),Storage_Device_FS_t()
#else
MCS_IFLASH_dev_module_t::MCS_IFLASH_dev_module_t(uint32_t mem_base, uint32_t mem_size):USB_Devices_module_t()
#endif
{
	#ifdef USE_FS
	this->Disc_Stat = STA_NOINIT;
	#endif
	MCS_IFLASH_dev_module_t::memDiskArea_ptr = (uint8_t*) mem_base;
	MCS_IFLASH_dev_module_t::mem_size = mem_size;
	this->USB_num = 0;
	Chip_IAP_Init();
	#ifdef USE_FS
	this->Disc_Stat &= ~STA_NOINIT;
	#endif
}

void MCS_IFLASH_dev_module_t::translate_rd(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	if(MCS_IFLASH_dev_module_t::g_pUsbApi->version == 0x01111101) hi_offset = 0;
	*buff_adr = &MCS_IFLASH_dev_module_t::memDiskArea_ptr[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32))];
}


void MCS_IFLASH_dev_module_t::translate_wr(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	if(MCS_IFLASH_dev_module_t::g_pUsbApi->version == 0x01111101) hi_offset = 0;

	/* If a new write request has started then copy total transfer length from control structure
		 and calculate the data block size for write */
	if(MCS_IFLASH_dev_module_t::update_xfer_len)
	{
		/* Start Addr for the write request is updated */
		MCS_IFLASH_dev_module_t::startAddr = (uint32_t)MCS_IFLASH_dev_module_t::memDiskArea_ptr + (((uint64_t) offset) | (((uint64_t) hi_offset) << 32));

		if(MCS_IFLASH_dev_module_t::g_pUsbApi->version > 0x01111101)
		{
			/* New ROM stack version, use new control structure */
			MCS_IFLASH_dev_module_t::total_xfer_len = ((USB_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->Length;
		}
		else
		{
			/* Old ROM stack version, use old control structure */
			MCS_IFLASH_dev_module_t::total_xfer_len = ((USB_OLD_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->Length;
		}
		/* Transfer size from SD card is the minimum of buffer size and total transfer length */
		if(MCS_IFLASH_dev_module_t::total_xfer_len > MSC_BUFF_SIZE)
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MSC_BUFF_SIZE;
		}
		else
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MCS_IFLASH_dev_module_t::total_xfer_len;
		}
		MCS_IFLASH_dev_module_t::wrBuffIndex = 0;
		MCS_IFLASH_dev_module_t::update_xfer_len = 0;
	}
	/* Increment the index for the buffer */
	MCS_IFLASH_dev_module_t::wrBuffIndex += length;
	/* When entire buffer is written, write the data block to the IFlash */
	if(MCS_IFLASH_dev_module_t::wrBuffIndex == MCS_IFLASH_dev_module_t::xfer_buff_len)
	{
		MCS_IFLASH_dev_module_t::Write_flash(MCS_IFLASH_dev_module_t::startAddr,MCS_IFLASH_dev_module_t::WR_buf,MCS_IFLASH_dev_module_t::xfer_buff_len);
		/* Reset index*/
		MCS_IFLASH_dev_module_t::wrBuffIndex = 0;
		/* Update the start sector, total transfer length and data block size for SD write */
		MCS_IFLASH_dev_module_t::startAddr += MCS_IFLASH_dev_module_t::xfer_buff_len;
		MCS_IFLASH_dev_module_t::total_xfer_len -= MCS_IFLASH_dev_module_t::xfer_buff_len;
		if(MCS_IFLASH_dev_module_t::total_xfer_len > MSC_BUFF_SIZE)
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MSC_BUFF_SIZE;
		}
		else
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MCS_IFLASH_dev_module_t::total_xfer_len;
		}
	}
	/* Data pointer to the next USB bulk OUT packet */
	*buff_adr =  &MCS_IFLASH_dev_module_t::WR_buf[MCS_IFLASH_dev_module_t::wrBuffIndex];
}

void MCS_IFLASH_dev_module_t::translate_GetWrBuf(uint32_t offset, uint8_t * *buff_adr, uint32_t length, uint32_t hi_offset)
{
	*buff_adr = MCS_IFLASH_dev_module_t::WR_buf;
}

ErrorCode_t MCS_IFLASH_dev_module_t::translate_verify(uint32_t offset, uint8_t *src, uint32_t length, uint32_t hi_offset)
{
	if(MCS_IFLASH_dev_module_t::g_pUsbApi->version == 0x01111101) hi_offset = 0;

	/* If a new RW request has started then copy total transfer length from control structure
		 and read a new block of data from IFLASH */
	if(MCS_IFLASH_dev_module_t::update_xfer_len)
	{
		/* Start sector for the read request is updated */
		MCS_IFLASH_dev_module_t::startAddr = (uint32_t)MCS_IFLASH_dev_module_t::memDiskArea_ptr + (((uint64_t) offset) | (((uint64_t) hi_offset) << 32));

		if(MCS_IFLASH_dev_module_t::g_pUsbApi->version > 0x01111101)
		{
			/* New ROM stack version, use new control structure */
			MCS_IFLASH_dev_module_t::total_xfer_len = ((USB_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->Length;
		}
		else {
			/* Old ROM stack version, use old control structure */
			MCS_IFLASH_dev_module_t::total_xfer_len = ((USB_OLD_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->Length;
		}
		/* Transfer size from SD card is the minimum of buffer size and total transfer length */
		if(MCS_IFLASH_dev_module_t::total_xfer_len > MSC_BUFF_SIZE)
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MSC_BUFF_SIZE;
		}
		else
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MCS_IFLASH_dev_module_t::total_xfer_len;
		}
		MCS_IFLASH_dev_module_t::update_xfer_len = 0;

		for(uint32_t i = 0; i<MCS_IFLASH_dev_module_t::xfer_buff_len; i++)
			MCS_IFLASH_dev_module_t::WR_buf[i] = ((uint8_t*)MCS_IFLASH_dev_module_t::startAddr)[i];
	}
	/* For a previous request when the buffered data is read out completely then refill the buffer with next set of data */
	else if(((((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) -
			(MCS_IFLASH_dev_module_t::startAddr - (uint32_t)MCS_IFLASH_dev_module_t::memDiskArea_ptr)) == MCS_IFLASH_dev_module_t::xfer_buff_len)
	{
		/* Update the start sector, total transfer length and data length to be read from SD card */
		MCS_IFLASH_dev_module_t::startAddr += MCS_IFLASH_dev_module_t::xfer_buff_len;
		MCS_IFLASH_dev_module_t::total_xfer_len -= MCS_IFLASH_dev_module_t::xfer_buff_len;
		if(MCS_IFLASH_dev_module_t::total_xfer_len > MSC_BUFF_SIZE)
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MSC_BUFF_SIZE;
		}
		else
		{
			MCS_IFLASH_dev_module_t::xfer_buff_len = MCS_IFLASH_dev_module_t::total_xfer_len;
		}

		for(uint32_t i = 0; i<MCS_IFLASH_dev_module_t::xfer_buff_len; i++)
			MCS_IFLASH_dev_module_t::WR_buf[i] = ((uint8_t*)MCS_IFLASH_dev_module_t::startAddr)[i];
	}
	/* Compare data return accordingly*/
	if (memcmp((void *) &MCS_IFLASH_dev_module_t::WR_buf[(((uint64_t) offset) | (((uint64_t) hi_offset) << 32)) -
	         (MCS_IFLASH_dev_module_t::startAddr - (uint32_t)MCS_IFLASH_dev_module_t::memDiskArea_ptr)], src, length)) {
		return ERR_FAILED;
	}

	return LPC_OK;
}

/* Override for Bulk out handler */
ErrorCode_t MCS_IFLASH_dev_module_t::app_bulk_out_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
	void *pMscCtrl = data;
	if(MCS_IFLASH_dev_module_t::g_pUsbApi->version > 0x01111101) {
		/* If a new RW request is received then set flag for updating total transfer length */
		if( (event == USB_EVT_OUT) && (((USB_MSC_CTRL_T*)pMscCtrl)->BulkStage == MSC_BS_CBW) ) {
			MCS_IFLASH_dev_module_t::update_xfer_len = 1;
		}
	}
	else {
		/* If a new RW request is received then set flag for updating total transfer length */
		if( (event == USB_EVT_OUT) && (((USB_OLD_MSC_CTRL_T*)pMscCtrl)->BulkStage == MSC_BS_CBW) ) {
			MCS_IFLASH_dev_module_t::update_xfer_len = 1;
		}
	}
	/* Call the default handler */
  return MCS_IFLASH_dev_module_t::default_bulk_out_hdlr(hUsb, data, event);
}

ErrorCode_t MCS_IFLASH_dev_module_t::USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam)
{
	USBD_MSC_INIT_PARAM_T msc_param;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T* pCtrl = (USB_CORE_CTRL_T*)hUsb;

	memset((void *) &msc_param, 0, sizeof(USBD_MSC_INIT_PARAM_T));
	msc_param.mem_base = pUsbParam->mem_base;
	msc_param.mem_size = pUsbParam->mem_size;
	MCS_IFLASH_dev_module_t::g_pMscCtrl = (void *)msc_param.mem_base;
	/* mass storage paramas */
	msc_param.InquiryStr = (uint8_t *)MCS_IFLASH_dev_module_t::g_InquiryStr;
	msc_param.BlockCount = MSC_IFLASH_DISK_BLOCK_COUNT;
	msc_param.BlockSize = MSC_IFLASH_DISK_BLOCK_SIZE;
	/*Update memory size based on the stack version */
	if(MCS_IFLASH_dev_module_t::g_pUsbApi->version > 0x01111101) {
		/* New ROM stack version */
		msc_param.MemorySize64 = MCS_IFLASH_dev_module_t::mem_size;
		msc_param.MemorySize = 0;
	}
	else {
		/* Old ROM stack version - cannot support more than 4GB of memory card */
		msc_param.MemorySize = MCS_IFLASH_dev_module_t::mem_size;
	}
	/* Install memory storage callback routines */
	msc_param.MSC_Write = MCS_IFLASH_dev_module_t::translate_wr;
	msc_param.MSC_Read = MCS_IFLASH_dev_module_t::translate_rd;
	msc_param.MSC_Verify = MCS_IFLASH_dev_module_t::translate_verify;
	msc_param.MSC_GetWriteBuf = MCS_IFLASH_dev_module_t::translate_GetWrBuf;

	if(this->USB_num == 0)
	{
		msc_param.intf_desc = (uint8_t *) MCS_IFLASH_dev_module_t::find_IntfDesc(pDesc->high_speed_desc, USB0_MSC_IF_NUM);
	}
	if(this->USB_num == 1)
	{
		msc_param.intf_desc = (uint8_t *) MCS_IFLASH_dev_module_t::find_IntfDesc(pDesc->full_speed_desc, USB1_MSC_IF_NUM);
	}

	ret = MCS_IFLASH_dev_module_t::g_pUsbApi->msc->init(hUsb, &msc_param);

	MCS_IFLASH_dev_module_t::update_xfer_len = 0;

	if(MCS_IFLASH_dev_module_t::g_pUsbApi->version > 0x01111101)
	{
		/* New ROM stack version */
		MCS_IFLASH_dev_module_t::default_bulk_out_hdlr = pCtrl->ep_event_hdlr[(((USB_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->epout_num & 0x0F) << 1];
		MCS_IFLASH_dev_module_t::g_pUsbApi->core->RegisterEpHandler(hUsb,
				(((USB_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->epout_num & 0x0F) << 1, MCS_IFLASH_dev_module_t::app_bulk_out_hdlr,
				MCS_IFLASH_dev_module_t::g_pMscCtrl);
	}
	else
	{
		/* Old ROM stack version */
		MCS_IFLASH_dev_module_t::default_bulk_out_hdlr = pCtrl->ep_event_hdlr[(((USB_OLD_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->epout_num & 0x0F) << 1];
		MCS_IFLASH_dev_module_t::g_pUsbApi->core->RegisterEpHandler(hUsb,
				(((USB_OLD_MSC_CTRL_T *)MCS_IFLASH_dev_module_t::g_pMscCtrl)->epout_num & 0x0F) << 1,
				MCS_IFLASH_dev_module_t::app_bulk_out_hdlr, MCS_IFLASH_dev_module_t::g_pMscCtrl);
	}
	/* update memory variables */
	pUsbParam->mem_base = msc_param.mem_base;
	pUsbParam->mem_size = msc_param.mem_size;

	return ret;
}


void MCS_IFLASH_dev_module_t::USB_Tasks(void)
{

}

void MCS_IFLASH_dev_module_t::USB_GetParameter(uint8_t ParamName, void* ParamValue)
{

}

void MCS_IFLASH_dev_module_t::USB_SetParameter(uint8_t ParamName, void* ParamValue)
{
	if(ParamName == USB_num_param)
	{
		this->USB_num = *((uint8_t*)ParamValue);
	}
}

USB_INTERFACE_DESCRIPTOR* MCS_IFLASH_dev_module_t::find_IntfDesc(const uint8_t *pDesc, uint32_t IntfNum)
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

void MCS_IFLASH_dev_module_t::Get_sect_bank_num(uint32_t addr_in_sect, uint8_t* Sect_num, uint8_t* Bank_num)
{
	if((addr_in_sect&0xFF000000) == FLASH_A_BASE) *Bank_num = IAP_FLASH_BANK_A;
	if((addr_in_sect&0xFF000000) == FLASH_B_BASE) *Bank_num = IAP_FLASH_BANK_B;

	addr_in_sect &= 0x00FFFFFF;

	if(addr_in_sect<0x00002000) *Sect_num = 0;
	if((addr_in_sect>=0x00002000)&&(addr_in_sect<0x00004000)) *Sect_num = 1;
	if((addr_in_sect>=0x00004000)&&(addr_in_sect<0x00006000)) *Sect_num = 2;
	if((addr_in_sect>=0x00006000)&&(addr_in_sect<0x00008000)) *Sect_num = 3;
	if((addr_in_sect>=0x00008000)&&(addr_in_sect<0x0000A000)) *Sect_num = 4;
	if((addr_in_sect>=0x0000A000)&&(addr_in_sect<0x0000C000)) *Sect_num = 5;
	if((addr_in_sect>=0x0000C000)&&(addr_in_sect<0x0000E000)) *Sect_num = 6;
	if((addr_in_sect>=0x0000E000)&&(addr_in_sect<0x00010000)) *Sect_num = 7;
	if((addr_in_sect>=0x00010000)&&(addr_in_sect<0x00020000)) *Sect_num = 8;
	if((addr_in_sect>=0x00020000)&&(addr_in_sect<0x00030000)) *Sect_num = 9;
	if((addr_in_sect>=0x00030000)&&(addr_in_sect<0x00040000)) *Sect_num = 10;
	if((addr_in_sect>=0x00040000)&&(addr_in_sect<0x00050000)) *Sect_num = 11;
	if((addr_in_sect>=0x00050000)&&(addr_in_sect<0x00060000)) *Sect_num = 12;
	if((addr_in_sect>=0x00060000)&&(addr_in_sect<0x00070000)) *Sect_num = 13;
	if((addr_in_sect>=0x00070000)&&(addr_in_sect<0x00080000)) *Sect_num = 14;
}

void MCS_IFLASH_dev_module_t::Flash_page_write(uint32_t Adress, uint8_t* Buf)
{
	uint8_t Sect_num = 0;
	uint8_t flash_bank_num = 0;

	MCS_IFLASH_dev_module_t::Get_sect_bank_num(Adress,&Sect_num, &flash_bank_num);
	__disable_irq();
	Chip_IAP_PreSectorForReadWrite(Sect_num, Sect_num, flash_bank_num);
	CONTROLLER_t::get_CONTROLLER()->_delay_us(1);
	Chip_IAP_ErasePage(Adress, Adress);
	Chip_IAP_PreSectorForReadWrite(Sect_num, Sect_num, flash_bank_num);
	CONTROLLER_t::get_CONTROLLER()->_delay_us(1);
	Chip_IAP_CopyRamToFlash(Adress, (uint32_t*)Buf, FLASH_PAGE_SIZE);
	__enable_irq();
}

void MCS_IFLASH_dev_module_t::Write_page_part(uint32_t Adress,uint8_t* Buf, uint32_t size)
{
	static uint8_t Page_buf[FLASH_PAGE_SIZE]__attribute__ ((aligned(4)));
	uint32_t Page_offset;
	uint32_t Page_addr;

	if(size <= FLASH_PAGE_SIZE)
	{
		Page_offset = Adress%(FLASH_PAGE_SIZE);
		Page_addr = Adress - Page_offset;

		for(int i = 0; i<FLASH_PAGE_SIZE; i++)
		{
			Page_buf[i] = ((uint8_t*)Page_addr)[i];
		}
		memcpy(&Page_buf[Page_offset],Buf,size);
		MCS_IFLASH_dev_module_t::Flash_page_write(Page_addr, Page_buf);
	}

}

void MCS_IFLASH_dev_module_t::Write_flash(uint32_t Adress,uint8_t* Buf, uint32_t size)
{
	uint32_t Page;
	uint32_t Page_count;
	uint32_t bytes_bw;
	uint32_t bytes_br;
	uint32_t Page_offset;
	uint32_t temp_size = 0;

	Page_offset = Adress%(FLASH_PAGE_SIZE);

	if(Page_offset!=0)
	{
		temp_size = FLASH_PAGE_SIZE - Page_offset;
		if(size<=temp_size) temp_size = size;
		MCS_IFLASH_dev_module_t::Write_page_part(Adress,Buf,temp_size);
		size = size - temp_size;
		Adress = Adress - Page_offset + FLASH_PAGE_SIZE;
		bytes_bw = temp_size;
	}
	else
	{
		bytes_bw = 0;
	}

	bytes_br = size;

	if(size)
	{
		Page_count = size/FLASH_PAGE_SIZE + 1;
		for (Page = 0; Page < Page_count; Page++)
		{
			if(bytes_br > FLASH_PAGE_SIZE)
			{
				MCS_IFLASH_dev_module_t::Flash_page_write( Adress, &Buf[bytes_bw]);
				bytes_br = bytes_br - FLASH_PAGE_SIZE;
				bytes_bw = bytes_bw + FLASH_PAGE_SIZE;
				Adress = Adress + FLASH_PAGE_SIZE;
			}
			else
			{
				MCS_IFLASH_dev_module_t::Write_page_part(Adress,&Buf[bytes_bw], bytes_br);
			}
		}
	}
}

#ifdef USE_FS

DSTATUS MCS_IFLASH_dev_module_t::disk_status(void)
{
	return this->Disc_Stat;
}

DSTATUS MCS_IFLASH_dev_module_t::disk_initialize(void)
{
	return this->Disc_Stat;
}

DRESULT MCS_IFLASH_dev_module_t::disk_read(BYTE* buff, DWORD sector, UINT count)
{
	uint32_t start_addr;

	if(this->Disc_Stat & STA_NOINIT) return RES_NOTRDY;

	start_addr = MSC_IFLASH_DISK_BASE + sector*MSC_IFLASH_DISK_BLOCK_SIZE;

	for(uint32_t i = 0; i < MSC_IFLASH_DISK_BLOCK_SIZE*count; i++) buff[i] = *((uint8_t*)(start_addr+i));

	return RES_OK;
}

DRESULT MCS_IFLASH_dev_module_t::disk_write(const BYTE* buff, DWORD sector, UINT count)
{
	uint32_t start_addr;

	if (this->Disc_Stat & STA_NOINIT) return RES_NOTRDY;

	start_addr = MSC_IFLASH_DISK_BASE + sector*MSC_IFLASH_DISK_BLOCK_SIZE;

	MCS_IFLASH_dev_module_t::Write_flash(start_addr,(uint8_t*)buff, MSC_IFLASH_DISK_BLOCK_SIZE*count);
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT MCS_IFLASH_dev_module_t::disk_ioctl(BYTE cmd, void* buff)
{
	DRESULT res;

	if (this->Disc_Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC:	/* Make sure that no pending write process */

		res = RES_OK;
		break;

	case GET_SECTOR_COUNT:	/* Get number of sectors on the disk (DWORD) */
		*(DWORD *) buff = MSC_IFLASH_DISK_BLOCK_COUNT;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:	/* Get R/W sector size (WORD) */
		*(WORD *) buff = MSC_IFLASH_DISK_BLOCK_SIZE;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:/* Get erase block size in unit of sector (DWORD) */
		*(DWORD *) buff = 1;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
		break;
	}

	return res;
}

#endif
