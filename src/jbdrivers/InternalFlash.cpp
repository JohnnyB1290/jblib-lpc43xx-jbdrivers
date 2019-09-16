/**
 * @file
 * @brief Internal flash driver Realization
 *
 * @note
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
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

#include "string.h"
#include "jbdrivers/InternalFlash.hpp"
#include "jbdrivers/JbController.hpp"
#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
#include <stdio.h>
#endif


namespace jblib::jbdrivers
{

using namespace jbkernel;

InternalFlash::InternalFlash(FlashBankNumber_t bankNumber) : IVoidMemory()
{
	this->bankNumber_ = bankNumber;
	if(this->bankNumber_ == FLASH_BANK_A)
		this->baseAddress_ = FLASH_A_BASE;
	else
		this->baseAddress_ = FLASH_B_BASE;
}



InternalFlash::~InternalFlash(void)
{

}



void InternalFlash::initialize(void)
{
	if(!this->isInitialized_){
		#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
		uint8_t result =
		#endif
		Chip_IAP_Init();
		#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
		if(result){
			printf("Internal flash error: Chip_IAP_Init error %u", result);
		}
		#endif
		this->isInitialized_ = true;
	}
}



void InternalFlash::readMemory(uint32_t address, uint8_t* data, uint32_t size)
{
	memcpy(data, (void*)address, size);
}



void InternalFlash::writeMemory(uint32_t address, uint8_t* data, uint32_t size)
{
	uint32_t pageOffset = address % FLASH_PAGE_SIZE;
	uint32_t writeSize = FLASH_PAGE_SIZE - pageOffset;
	while(size){
		uint32_t pageAddress = address - pageOffset;
		if(pageOffset)
			writeSize = (writeSize < size) ? writeSize : size;
		else
			writeSize = (FLASH_PAGE_SIZE < size) ? FLASH_PAGE_SIZE : size;

		if(writeSize != FLASH_PAGE_SIZE){
			uint8_t* auxBuf = (uint8_t*)malloc_s(FLASH_PAGE_SIZE);
			if(!auxBuf)
				return;
			this->readMemory(pageAddress, auxBuf, FLASH_PAGE_SIZE);
			memcpy(auxBuf + pageOffset, data, writeSize);
			this->eraseProgramPage(pageAddress, auxBuf);
			free_s(auxBuf);
		}
		else
			this->eraseProgramPage(pageAddress, data);

		address += writeSize;
		size -= writeSize;
		data += writeSize;
		pageOffset = 0;
	}
}



void InternalFlash::eraseMemory(uint32_t address, uint32_t size)
{
	uint32_t pageOffset = address % FLASH_PAGE_SIZE;
	uint32_t pageAddress = address - pageOffset;
	while(size){
		uint8_t sectorNumber = this->getSectorNumber(pageAddress);
		uint8_t result =
				Chip_IAP_PreSectorForReadWrite(sectorNumber, sectorNumber, this->bankNumber_);
		if(result){
			#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
			printf("Internal flash error: eraseMemory "
					"Chip_IAP_PreSectorForReadWrite error %u", result);
			#endif
			return;
		}
		jblib::jbdrivers::JbController::delayUs(1);
		result = Chip_IAP_ErasePage(pageAddress, pageAddress);
		if(result){
			#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
			printf("Internal flash error: eraseMemory "
					"Chip_IAP_ErasePage error %u", result);
			#endif
			return;
		}
		pageAddress += FLASH_PAGE_SIZE;
		size = (size < FLASH_PAGE_SIZE) ? 0 : (size - FLASH_PAGE_SIZE);
	}
}



void InternalFlash::eraseProgramPage(uint32_t address, uint8_t* data)
{
	uint8_t sectorNumber = this->getSectorNumber(address);
	uint8_t result =
			Chip_IAP_PreSectorForReadWrite(sectorNumber, sectorNumber, this->bankNumber_);
	if(result){
		#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
		printf("Internal flash error: eraseProgramPage "
				"Chip_IAP_PreSectorForReadWrite 1 error %u", result);
		#endif
		return;
	}
	jblib::jbdrivers::JbController::delayUs(1);
	result = Chip_IAP_ErasePage(address, address);
	if(result){
		#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
		printf("Internal flash error: eraseProgramPage "
				"Chip_IAP_ErasePage error %u", result);
		#endif
		return;
	}
	result = Chip_IAP_PreSectorForReadWrite(sectorNumber, sectorNumber, this->bankNumber_);
	if(result){
		#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
		printf("Internal flash error: eraseProgramPage "
				"Chip_IAP_PreSectorForReadWrite 2 error %u", result);
		#endif
		return;
	}
	jblib::jbdrivers::JbController::delayUs(1);
	result = Chip_IAP_CopyRamToFlash(address, (uint32_t*)data, FLASH_PAGE_SIZE);
	#if (USE_CONSOLE && INTERNAL_FLASH_USE_CONSOLE)
	if(result){
		printf("Internal flash error: eraseProgramPage "
				"Chip_IAP_CopyRamToFlash error %u", result);
	}
	#endif
}



uint8_t InternalFlash::getSectorNumber(uint32_t address)
{
	address &= 0x00FFFFFF;
	if(address < 0x00002000)
		return 0;
	else if((address >= 0x00002000) && (address < 0x00004000))
		return 1;
	else if((address >= 0x00004000) && (address < 0x00006000))
		return 2;
	else if((address >= 0x00006000) && (address < 0x00008000))
		return 3;
	else if((address >= 0x00008000) && (address < 0x0000A000))
		return 4;
	else if((address >= 0x0000A000) && (address < 0x0000C000))
		return 5;
	else if((address >= 0x0000C000) && (address < 0x0000E000))
		return 6;
	else if((address >= 0x0000E000) && (address < 0x00010000))
		return 7;
	else if((address >= 0x00010000) && (address < 0x00020000))
		return 8;
	else if((address >= 0x00020000) && (address < 0x00030000))
		return 9;
	else if((address >= 0x00030000) && (address < 0x00040000))
		return 10;
	else if((address >= 0x00040000) && (address < 0x00050000))
		return 11;
	else if((address >= 0x00050000) && (address < 0x00060000))
		return 12;
	else if((address >= 0x00060000) && (address < 0x00070000))
		return 13;
	else if((address >= 0x00070000) && (address < 0x00080000))
		return 14;
	else return 0;
}


}
