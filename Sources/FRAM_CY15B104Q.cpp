/*
 * FRAM_CY15B104Q.cpp
 *
 *  Created on: 21 θών. 2018 γ.
 *      Author: Stalker1290
 */

#include "FRAM_CY15B104Q.hpp"
#include "stdlib.h"



FRAMcy15b104q_t::FRAMcy15b104q_t(SSP_t* SSPPtr, uint32_t SSPDeviceNum){
	this->SSPDeviceNum = SSPDeviceNum;
	this->SSPPtr = SSPPtr;
}

bool FRAMcy15b104q_t::Initialize(){

	uint8_t* deviceIdPtr = (uint8_t*)NULL;
	if(this->SSPPtr == (SSP_t*)NULL){
		return false;
	}

	deviceIdPtr = (uint8_t*)malloc_s(DEVICE_ID_SIZE);
	if(deviceIdPtr == (uint8_t*)NULL){
		return false;
	}

	this->dataExchange(RDID_OPCODE,0,NULL,deviceIdPtr,DEVICE_ID_SIZE);

	#ifdef FRAM_CY15B104Q_console
		printf("Device ID: %X%X%X%X%X%X%X%X%X", deviceIdPtr[0],deviceIdPtr[1],deviceIdPtr[2],
				deviceIdPtr[3],deviceIdPtr[4],deviceIdPtr[5],deviceIdPtr[6],deviceIdPtr[7],
				deviceIdPtr[8]);
	#endif

	free_s(deviceIdPtr);

	return true;
}

bool FRAMcy15b104q_t::dataExchange(uint8_t opCode, uint32_t address, uint8_t* txDataPtr, uint8_t* rxDataPtr, uint32_t length){

	uint8_t* txArrPtr = (uint8_t*)NULL;
	uint8_t* rxArrPtr = (uint8_t*)NULL;

	switch(opCode){
	case WREN_OPCODE:
	case WRDI_OPCODE:
	case SLEEP_OPCODE:
		this->SSPPtr->TxRx_frame(opCode,this->SSPDeviceNum);
		break;

	case RDSR_OPCODE:
	case WRSR_OPCODE:
	{
		volatile uint8_t txArr[2] = {opCode,0};
		volatile uint8_t rxArr[2] = {0,0};

		if (txData_ptr != (uint8_t*) NULL){
			txArr[1] = *txDataPtr;
		}

		this->SSPPtr->TxRx_frame(txArr, rxArr, 2, this->SSPDeviceNum);

		if (rxDataPtr != (uint8_t*) NULL) {
			*rxDataPtr = rxArr[1];
		}

		break;
	}
	case FSTRD_OPCODE:
	case READ_OPCODE:
	case WRITE_OPCODE:
	case RDID_OPCODE:
	{
		uint32_t finalLen = length;
		uint32_t dataShift = 0;
		if(opCode != RDID_OPCODE){
			finalLen += 4;
		}
		else{
			if(length != DEVICE_ID_SIZE){
				return false;
			}
			finalLen = DEVICE_ID_SIZE + 1;
		}

		txArrPtr = (uint8_t*) malloc_s(finalLen);
		if (txArrPtr == (uint8_t*) NULL) {
			return false;
		}
		txArrPtr[0] = opCode;
		dataShift++;
		if(opCode != RDID_OPCODE){
			txArrPtr[1] = (address >> 16) & 0xff;
			txArrPtr[2] = (address >> 8) & 0xff;
			txArrPtr[3] = address & 0xff;
			dataShift += 3;
		}

		if (txDataPtr != (uint8_t*) NULL) {
			memcpy(&txArrPtr[dataShift], txDataPtr, length);
		}

		if (rxDataPtr != (uint8_t*) NULL) {
			rxArrPtr = (uint8_t*) malloc_s(finalLen);
			if ((rxArrPtr == (uint8_t*) NULL)) {
				free_s(txArrPtr);
				return false;
			}
		}

		this->SSPPtr->TxRx_frame(txArrPtr, rxArrPtr, finalLen,this->SSPDeviceNum);

		if (rxDataPtr != (uint8_t*) NULL) {
			memcpy(rxDataPtr, &rxArrPtr[dataShift], length);
		}

		free_s(txArrPtr);
		if (rxArrPtr != (uint8_t*) NULL) {
			free_s(rxArrPtr);
		}
		break;
	}

	default:
		return false;
	}

	return true;
}

void FRAMcy15b104q_t::memcpy(void* destination, void* source, uint32_t size){


}




