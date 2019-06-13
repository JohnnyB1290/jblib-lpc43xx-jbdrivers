/**
 * @file
 * @brief EEPROM Driver Realization
 *
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

#include <string.h>
#include "Eeprom.hpp"
#include "JbController.hpp"

namespace jblib::jbdrivers
{

Eeprom* Eeprom::eeprom_ = NULL;



Eeprom* Eeprom::getEeprom(void)
{
	if( Eeprom::eeprom_ == NULL)
		Eeprom::eeprom_ = new Eeprom();
	return Eeprom::eeprom_;
}



Eeprom::Eeprom(void)
{

}



void Eeprom::initialize(void)
{
	Chip_Clock_EnableOpts(CLK_MX_EEPROM, true, true, 1);
	Chip_RGU_TriggerReset(RGU_EEPROM_RST);
	while (Chip_RGU_InReset(RGU_EEPROM_RST)) {}
	Chip_EEPROM_Init(LPC_EEPROM);
	JbController::delayUs(100);
}



void Eeprom::read(uint32_t address, uint8_t* const data, uint32_t size)
{
	memcpy((void*)data, (void*)address, size);
}

void Eeprom::write(uint32_t address, const uint8_t* data, uint32_t size)
{
	if(size % 4 != 0)
		size = (size & 0xFC) + 4;
	uint32_t pageOffset = address % EEPROM_PAGE_SIZE;
	uint32_t bw = 0;
	if(pageOffset) {
		uint32_t tempSize = EEPROM_PAGE_SIZE - pageOffset;
		if(size <= tempSize)
			tempSize = size;
		uint32_t tempAddress = address - pageOffset;
		memcpy((void*)this->pageBuffer_, (void*)tempAddress, EEPROM_PAGE_SIZE);
		memcpy((void*)&this->pageBuffer_[pageOffset],(void*)data, tempSize);
		memcpy((void*)tempAddress, (void*)this->pageBuffer_, EEPROM_PAGE_SIZE);
//		memcpy((void*)address, (void*)data, tempSize);
		Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
		size -= tempSize;
		address = address - pageOffset + EEPROM_PAGE_SIZE;
		bw = tempSize;
	}
	if(size) {
		uint32_t br = size;
		uint32_t pageCount = size/EEPROM_PAGE_SIZE;
		pageCount += (pageCount * EEPROM_PAGE_SIZE) == size ? 0 : 1;
		for (uint32_t page = 0; page < pageCount; page++) {
			if(br > EEPROM_PAGE_SIZE) {
				memcpy((void*)address, (void*)&data[bw], EEPROM_PAGE_SIZE);
				Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
				br += EEPROM_PAGE_SIZE;
				bw += EEPROM_PAGE_SIZE;
				address += EEPROM_PAGE_SIZE;
			}
			else {
				memcpy((void*)this->pageBuffer_, (void*)address, EEPROM_PAGE_SIZE);
				memcpy((void*)this->pageBuffer_, (void*)&data[bw], br);
				memcpy((void*)address, (void*)this->pageBuffer_, EEPROM_PAGE_SIZE);
				//memcpy((void*)address, (void*)&data[bw], br);
				Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
			}
		}
	}
}



void Eeprom::deinitialize(void)
{
	Chip_RGU_TriggerReset(RGU_EEPROM_RST);
	while (Chip_RGU_InReset(RGU_EEPROM_RST)) {}
	Chip_Clock_Disable(CLK_MX_EEPROM);
}

}

