/*
 * NVMParameters.cpp
 *
 *  Created on: 11 сент. 2018 г.
 *      Author: Stalker1290
 */

#include "NVMParameters.hpp"
#include "EEPROM.hpp"
#include "CRC.hpp"
#include "string.h"
#include "stdlib.h"

#include "stdio.h"

NVMParameters_t* NVMParameters_t::nvmParametersPtr = (NVMParameters_t*)NULL;
uint32_t NVMParameters_t::baseAddr = EEPROM_NVM_PARAMETERS_BASE;
NVMParamsHeader_t* NVMParameters_t::paramsHeaderPtr = (NVMParamsHeader_t*)EEPROM_NVM_PARAMETERS_BASE;
uint8_t NVMParameters_t::paramsHeaderSize = sizeof(NVMParamsHeader_t);
uint8_t NVMParameters_t::paramsCellSize = sizeof(NVMParamsCell_t);

NVMParameters_t* NVMParameters_t::getNVMParametersPtr(void){
	if(NVMParameters_t::nvmParametersPtr == (NVMParameters_t*)NULL)
		NVMParameters_t::nvmParametersPtr = new NVMParameters_t();
	return NVMParameters_t::nvmParametersPtr;
}

NVMParameters_t::NVMParameters_t(void){

	EEPROM_t::getEEPROM().Initialize();
	if(NVMParameters_t::paramsHeaderPtr->magic != NVM_PARAMETERS_MAGIC){
		this->eraseAllParameters();
		printf("NVM Parameters Error: invalid Magic!\r\n");
		return;
	}

	if(NVMParameters_t::paramsHeaderPtr->size == 0) return;

	uint16_t calcCrc = CRC_t::Crc16((uint8_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize),
			(NVMParameters_t::paramsHeaderPtr->size)*NVMParameters_t::paramsCellSize);

	if(NVMParameters_t::paramsHeaderPtr->crc != calcCrc){
		this->eraseAllParameters();
		printf("NVM Parameters Error: invalid CRC!\r\n");
	}
}


NVMParamsCell_t* NVMParameters_t::getParameter(char* paramDescription){

	if(NVMParameters_t::paramsHeaderPtr->size == 0) return (NVMParamsCell_t*)NULL;

	NVMParamsCell_t* cellPtr = (NVMParamsCell_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize);
	for(uint8_t i = 0; i < NVMParameters_t::paramsHeaderPtr->size; i++){
		if(strncmp(cellPtr->description, paramDescription, sizeof(cellPtr->description)) == 0){
			return cellPtr;
		}
		cellPtr++;
	}
	return (NVMParamsCell_t*)NULL;
}


NVMParamsCell_t* NVMParameters_t::getParameter(char* paramDescription, uint8_t* buf, uint8_t bufSize){

	NVMParamsCell_t* cellPtr = this->getParameter(paramDescription);

	if(cellPtr != NULL)
		memcpy(buf, cellPtr->data, (cellPtr->dataSize <= bufSize) ? cellPtr->dataSize : bufSize);

	return cellPtr;
}

void NVMParameters_t::setParameter(NVMParamsCell_t* paramsCellPtr){

	NVMParamsHeader_t tempHeader;
	memcpy(&tempHeader, NVMParameters_t::paramsHeaderPtr, NVMParameters_t::paramsHeaderSize);

	NVMParamsCell_t* cellPtr = this->getParameter(paramsCellPtr->description);

	if(cellPtr == (NVMParamsCell_t*)NULL){

		cellPtr = (NVMParamsCell_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize +
				NVMParameters_t::paramsCellSize * NVMParameters_t::paramsHeaderPtr->size);

		tempHeader.size++;
	}

	EEPROM_t::getEEPROM().Write((uint32_t)cellPtr,(uint8_t*)paramsCellPtr,NVMParameters_t::paramsCellSize);

	tempHeader.crc = CRC_t::Crc16((uint8_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize),
				tempHeader.size * NVMParameters_t::paramsCellSize);

	EEPROM_t::getEEPROM().Write((uint32_t)NVMParameters_t::paramsHeaderPtr,(uint8_t*)&tempHeader,NVMParameters_t::paramsHeaderSize);
}

void NVMParameters_t::setParameter(uint8_t type, char* description, uint8_t* data, uint8_t dataSize){

	NVMParamsCell_t tempCell;
	memset(&tempCell, 0, NVMParameters_t::paramsCellSize);

	tempCell.type = type;
	tempCell.dataSize = (sizeof(tempCell.data) <= dataSize) ? sizeof(tempCell.data) : dataSize;
	strncpy(tempCell.description, description, sizeof(tempCell.description));
	memcpy(tempCell.data, data, tempCell.dataSize);
	tempCell.descriptionSize = strlen(tempCell.description);

	this->setParameter(&tempCell);
}

void NVMParameters_t::deleteParameter(char* paramDescription){

	if(NVMParameters_t::paramsHeaderPtr->size == 0) return;

	NVMParamsHeader_t tempHeader;
	memcpy(&tempHeader, NVMParameters_t::paramsHeaderPtr, NVMParameters_t::paramsHeaderSize);

	NVMParamsCell_t* cellPtr = this->getParameter(paramDescription);
	if(cellPtr != (NVMParamsCell_t*)NULL){
		uint32_t tailSize = ((uint32_t)cellPtr - NVMParameters_t::baseAddr -
				NVMParameters_t::paramsHeaderSize) / NVMParameters_t::paramsCellSize;
		tailSize = NVMParameters_t::paramsHeaderPtr->size - tailSize - 1;
		tailSize = tailSize * NVMParameters_t::paramsCellSize;

		void* tempPtr = malloc_s(tailSize);
		if(tempPtr == NULL ) return;
		memcpy(tempPtr, (void*)((uint32_t)cellPtr + NVMParameters_t::paramsCellSize), tailSize);
		EEPROM_t::getEEPROM().Write((uint32_t)cellPtr,(uint8_t*)tempPtr,tailSize);
		free_s(tempPtr);
	}

	tempHeader.size--;
	tempHeader.crc = CRC_t::Crc16((uint8_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize),
					tempHeader.size * NVMParameters_t::paramsCellSize);

	EEPROM_t::getEEPROM().Write((uint32_t)NVMParameters_t::paramsHeaderPtr,(uint8_t*)&tempHeader,NVMParameters_t::paramsHeaderSize);
}

void NVMParameters_t::eraseAllParameters(void){
	NVMParamsHeader_t tempHeader;

	memset(&tempHeader, 0, NVMParameters_t::paramsHeaderSize);
	tempHeader.magic = NVM_PARAMETERS_MAGIC;
	tempHeader.size = 0;

	EEPROM_t::getEEPROM().Write((uint32_t)NVMParameters_t::paramsHeaderPtr,(uint8_t*)&tempHeader,NVMParameters_t::paramsHeaderSize);
}


NVMParamsHeader_t* NVMParameters_t::getHeaderPtr(void){
	return NVMParameters_t::paramsHeaderPtr;
}

uint32_t NVMParameters_t::getParametersSize(void){
	return (NVMParameters_t::paramsHeaderSize +
			NVMParameters_t::paramsCellSize * NVMParameters_t::paramsHeaderPtr->size);
}

void NVMParameters_t::setAllParameters(void* ptr){
	EEPROM_t::getEEPROM().Write((uint32_t)NVMParameters_t::paramsHeaderPtr,
			(uint8_t*)ptr,NVMParameters_t::paramsHeaderSize +
			NVMParameters_t::paramsCellSize * ((NVMParamsHeader_t*)ptr)->size);
}


uint32_t NVMParameters_t::getCompressedParametersSize(void){

	uint32_t retSize = NVMParameters_t::paramsHeaderSize;
	NVMParamsCell_t* cellPtr = (NVMParamsCell_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize);

	for(uint8_t i = 0; i < NVMParameters_t::paramsHeaderPtr->size; i++){
		retSize += 4 + cellPtr->descriptionSize + cellPtr->dataSize;
		cellPtr++;
	}
	return retSize;
}

uint32_t NVMParameters_t::getCompressedParameters(uint8_t* buf){

	uint32_t retSize = NVMParameters_t::paramsHeaderSize;
	uint8_t* ptr = buf;
	NVMParamsCell_t* cellPtr = (NVMParamsCell_t*)(NVMParameters_t::baseAddr + NVMParameters_t::paramsHeaderSize);

	memcpy(ptr,(uint8_t*)NVMParameters_t::paramsHeaderPtr, NVMParameters_t::paramsHeaderSize);
	ptr += NVMParameters_t::paramsHeaderSize;

	for(uint8_t i = 0; i < NVMParameters_t::paramsHeaderPtr->size; i++){

		retSize += 4 + cellPtr->descriptionSize + cellPtr->dataSize;
		memcpy(ptr,(uint8_t*)cellPtr, 4 + cellPtr->descriptionSize);
		ptr += (4 + cellPtr->descriptionSize);
		memcpy(ptr,((uint8_t*)cellPtr) + 4 + sizeof(cellPtr->description), cellPtr->dataSize);
		ptr += cellPtr->dataSize;
		cellPtr++;
	}
	return retSize;
}
