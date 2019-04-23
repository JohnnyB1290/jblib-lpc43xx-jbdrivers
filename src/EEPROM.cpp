/*
 * EEPROM.c
 *
 *  Created on: 15.02.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "EEPROM.hpp"
#include "string.h"


EEPROM_t& EEPROM_t::getEEPROM(void)
{
	static EEPROM_t Instance;
	return Instance;
}

EEPROM_t::EEPROM_t(void)
{

}

void EEPROM_t::Initialize(void)
{
	uint32_t i;
	Chip_Clock_EnableOpts(CLK_MX_EEPROM, true, true, 1);
	Chip_RGU_TriggerReset(RGU_EEPROM_RST);
	while (Chip_RGU_InReset(RGU_EEPROM_RST)) {}
	Chip_EEPROM_Init(LPC_EEPROM);
	for(i=0;i<100*39;i++) __NOP(); //100us
}

void EEPROM_t::Read(uint32_t Adress,uint8_t* Buf, uint32_t size)
{
	memcpy((void*)Buf,(void*)Adress,size);
}

void EEPROM_t::Write(uint32_t Adress,uint8_t* Buf, uint32_t size)
{
	uint32_t Page;
	uint32_t Page_count;
	uint32_t bytes_bw;
	uint32_t bytes_br;
	uint32_t Page_offset;
	uint32_t temp_size = 0;

	if(size%4 != 0) size = (size&(~3)) + 4;
	
	Page_offset = Adress%EEPROM_PAGE_SIZE;

	if(Page_offset!=0)
	{
		temp_size = EEPROM_PAGE_SIZE - Page_offset;
		if(size<=temp_size) temp_size = size;
		memcpy((void*)Adress,(void*)Buf,temp_size);
		Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
		size = size - temp_size;
		Adress = Adress - Page_offset + EEPROM_PAGE_SIZE;
		bytes_bw = temp_size;
	}
	else
	{
		bytes_bw = 0;
	}
	bytes_br = size;
	if(size)
	{
		Page_count = size/EEPROM_PAGE_SIZE + 1;
		for (Page = 0; Page < Page_count; Page++)
		{
			if(bytes_br > EEPROM_PAGE_SIZE)
			{
				memcpy((void*)Adress,(void*)&Buf[bytes_bw],EEPROM_PAGE_SIZE);
				Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
				bytes_br = bytes_br - EEPROM_PAGE_SIZE;
				bytes_bw = bytes_bw + EEPROM_PAGE_SIZE;
				Adress = Adress + EEPROM_PAGE_SIZE;
			}
			else
			{
				memcpy((void*)Adress,(void*)&Buf[bytes_bw],bytes_br);
				Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
			}
		}
	}
}

void EEPROM_t::Deinitialize(void)
{
	Chip_RGU_TriggerReset(RGU_EEPROM_RST);
	while (Chip_RGU_InReset(RGU_EEPROM_RST)) {}
	Chip_Clock_Disable(CLK_MX_EEPROM);
}
