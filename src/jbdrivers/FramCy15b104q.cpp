/**
 * @file
 * @brief FRAM CY15B104Q Driver Realization
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

#include <stdlib.h>
#include <string.h>
#include <cstring>
#include "jbdrivers/FramCy15b104q.hpp"
#if (USE_CONSOLE && FRAM_CY15B104Q_USE_CONSOLE)
#include <stdio.h>
#endif

#define WREN_OPCODE 		0x06 //Set write enable latch
#define WRDI_OPCODE 		0x04 //Reset write enable latch
#define RDSR_OPCODE 		0x05 //Read Status Register
#define WRSR_OPCODE 		0x01 //Write Status Register
#define READ_OPCODE 		0x03 //Read memory data
#define FSTRD_OPCODE 		0x0B //Fast read memory data
#define WRITE_OPCODE 		0x02 //Write memory data
#define SLEEP_OPCODE 		0xB9 //Enter sleep mode
#define RDID_OPCODE 		0x9F //Read device ID

#define SREG_WPEN_bm		0x80 //Write Protect Enable bit. Used to enable the function of Write Protect Pin (WP)
#define SREG_BP1_bm			0x08 //Used for block protection
#define SREG_BP0_bm			0x04 //Used for block protection
#define SREG_WEL_bm			0x02 //Write Enable

#define DEVICE_ID_SIZE 		9

#define MANUFACTURER_ID_1 	0x007F7F7F 	//BYTE678
#define MANUFACTURER_ID_0 	0x7F7F7FC2 	//BYTE2345

#define REVISION_bm			0x38 //BYTE0
#define SUB_CODE_bm			0xC0 //BYTE0
#define DENSITY_bm			0x1F //BYTE1
#define FAMILY_bm			0xE0 //BYTE1

#define FRAM_SIZE 			(512*1024)

namespace jblib::jbdrivers
{

FramCy15b104q::FramCy15b104q(Ssp* ssp, uint32_t sspDeviceNumber)
{
	this->sspDeviceNumber_ = sspDeviceNumber;
	this->ssp_ = ssp;
}



bool FramCy15b104q::initialize(void)
{
	if(this->ssp_ == (Ssp*)NULL)
		return false;

	uint8_t* deviceId = (uint8_t*)malloc_s(DEVICE_ID_SIZE);
	if(deviceId == (uint8_t*)NULL)
		return false;

	this->exchangeData(RDID_OPCODE, 0, NULL, deviceId, DEVICE_ID_SIZE);

	uint32_t manufactureId1 = (deviceId[0] << 16) | (deviceId[1] << 8) | deviceId[2];
	uint32_t manufactureId0 = (deviceId[3] << 24) | (deviceId[4] << 16) |
			(deviceId[5] << 8) | deviceId[6];

	if((manufactureId0 != MANUFACTURER_ID_0) ||
			(manufactureId1 != MANUFACTURER_ID_1)) {
		#if (USE_CONSOLE && FRAM_CY15B104Q_USE_CONSOLE)
		printf("Error initialize FRAM! Unknown manufacture ID! \n\r");
		#endif
		free_s(deviceId);
		return false;
	}
	#if (USE_CONSOLE && FRAM_CY15B104Q_USE_CONSOLE)
	else{
		printf("\n\rSSP Device num: %lu\n\r"
				"Found Cypress FRAM!\n\r"
				"Family: %i\n\r"
				"Density: %i\n\r"
				"Sub code: %i\n\r"
				"Revision: %i\n\r\n\r",
				this->sspDeviceNumber_,
				(deviceId[7] & FAMILY_bm) >> 5,
				(deviceId[7] & DENSITY_bm),
				(deviceId[8] & SUB_CODE_bm) >> 6,
				(deviceId[8] & REVISION_bm) >> 3);
	}
	#endif
	free_s(deviceId);
	return true;
}



bool FramCy15b104q::exchangeData(uint8_t opCode, uint32_t address,
		uint8_t* txData, uint8_t* rxData, uint32_t length){
	
	switch(opCode)
	{
		case WREN_OPCODE:
		case WRDI_OPCODE:
		case SLEEP_OPCODE:
		{
			this->ssp_->txRxFrame(opCode, this->sspDeviceNumber_);
		}
		break;

		case RDSR_OPCODE:
		case WRSR_OPCODE:
		{
			uint8_t txArr[2] = {opCode, 0};
			uint8_t rxArr[2] = {0, 0};

			if (txData)
				txArr[1] = *txData;
			this->ssp_->txRxFrame(txArr, rxArr, 2, this->sspDeviceNumber_);
			if (rxData)
				*rxData = rxArr[1];
		}
		break;

		case FSTRD_OPCODE:
		case READ_OPCODE:
		case WRITE_OPCODE:
		case RDID_OPCODE:
		{
			uint32_t headerLength = 4;
			uint8_t header[4];
			if(opCode == RDID_OPCODE) {
				if(length != DEVICE_ID_SIZE) {
					return false;
				}
				headerLength = 1;
			}
			header[0] = opCode;
			if(opCode != RDID_OPCODE){
				header[1] = (address >> 16) & 0xff;
				header[2] = (address >> 8) & 0xff;
				header[3] = address & 0xff;
			}
			void* txFrames[] = {(void*)header, (void*)txData};
			void* rxFrames[] = {(void*)NULL, (void*)rxData};
			uint32_t lengthArray[] = {headerLength, length};
			this->ssp_->txRxFrames(2, txFrames, rxFrames, lengthArray, this->sspDeviceNumber_);
		}
		break;

		default:
			return false;
	}
	return true;
}



void FramCy15b104q::memcpy(void* destination, void* source, uint32_t size)
{
	if((uint32_t)destination < FRAM_SIZE) {
		this->exchangeData(WREN_OPCODE, 0, NULL, NULL, 0);
		this->exchangeData(WRITE_OPCODE, (uint32_t)destination, (uint8_t*)source, NULL, size);
	}
	else if((uint32_t)source < FRAM_SIZE) {
		this->exchangeData(READ_OPCODE, (uint32_t)source, NULL, (uint8_t*)destination, size);
	}
}

}
