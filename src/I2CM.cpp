/*
 * I2CM.cpp
 *
 *  Created on: 29 ���. 2018 �.
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "I2CM.hpp"

I2CM_t* I2CM_t::i2cmInstancePtr[NUM_OF_I2C] = {(I2CM_t*)NULL, (I2CM_t*)NULL};
LPC_I2C_T* I2CM_t::ifacePtrs[NUM_OF_I2C] = {LPC_I2C0, LPC_I2C1};

I2CM_t* I2CM_t::getI2CM(uint8_t num){
	if(num >= NUM_OF_I2C) return (I2CM_t*)NULL;
	if(I2CM_t::i2cmInstancePtr[num] == (I2CM_t*)NULL)
		I2CM_t::i2cmInstancePtr[num] = new I2CM_t(num);
	return I2CM_t::i2cmInstancePtr[num];
}

I2CM_t::I2CM_t(uint8_t num){
	this->num = num;
	this->options = 0;
	this->initialized = false;
}

void I2CM_t::initialize(uint32_t speed){

	if(!this->initialized){
		if(this->num == 0){
			Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
		}
		else {
			Chip_SCU_PinMuxSet(SDA1_PORT, SDA1_PIN, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SDA1_SCU_FUNC));	/*I2C1_SDA */
			Chip_SCU_PinMuxSet(SCL1_PORT, SCL1_PIN, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCL1_SCU_FUNC));	/*I2C1_SCL */
		}

		Chip_I2CM_Init(I2CM_t::ifacePtrs[this->num]);
		Chip_I2CM_SetBusSpeed(I2CM_t::ifacePtrs[this->num], speed);
		this->initialized = true;
	}
}

void I2CM_t::initialize(void){
	this->initialize(100000);
}

uint32_t I2CM_t::write(uint8_t slaveAddr, uint8_t data){

	this->xferDesc.slaveAddr = slaveAddr;
	this->xferDesc.options = this->options;
	this->xferDesc.status = I2CM_STATUS_OK;
	this->xferDesc.txSz = 1;
	this->xferDesc.rxSz = 0;
	this->xferDesc.txBuff = &data;
	this->xferDesc.rxBuff = (uint8_t*)NULL;

	return Chip_I2CM_XferBlocking(I2CM_t::ifacePtrs[this->num], &this->xferDesc);
}

uint32_t I2CM_t::write(uint8_t slaveAddr, uint8_t* data, uint16_t size){

	this->xferDesc.slaveAddr = slaveAddr;
	this->xferDesc.options = this->options;
	this->xferDesc.status = I2CM_STATUS_OK;
	this->xferDesc.txSz = size;
	this->xferDesc.rxSz = 0;
	this->xferDesc.txBuff = data;
	this->xferDesc.rxBuff = (uint8_t*)NULL;

	return Chip_I2CM_XferBlocking(I2CM_t::ifacePtrs[this->num], &this->xferDesc);
}

uint8_t I2CM_t::read(uint8_t slaveAddr){

	uint8_t ret = 0;

	this->xferDesc.slaveAddr = slaveAddr;
	this->xferDesc.options = this->options;
	this->xferDesc.status = I2CM_STATUS_OK;
	this->xferDesc.txSz = 0;
	this->xferDesc.rxSz = 1;
	this->xferDesc.txBuff = (uint8_t*)NULL;
	this->xferDesc.rxBuff = &ret;

	Chip_I2CM_XferBlocking(I2CM_t::ifacePtrs[this->num], &this->xferDesc);
	return ret;
}

void I2CM_t::read(uint8_t slaveAddr, uint8_t* data, uint16_t size){

	this->xferDesc.slaveAddr = slaveAddr;
	this->xferDesc.options = this->options;
	this->xferDesc.status = I2CM_STATUS_OK;
	this->xferDesc.txSz = 0;
	this->xferDesc.rxSz = size;
	this->xferDesc.txBuff = (uint8_t*)NULL;
	this->xferDesc.rxBuff = data;

	Chip_I2CM_XferBlocking(I2CM_t::ifacePtrs[this->num], &this->xferDesc);
}

uint8_t I2CM_t::transfer(uint8_t slaveAddr, uint8_t txData){

	uint8_t ret = 0;

	this->xferDesc.slaveAddr = slaveAddr;
	this->xferDesc.options = this->options;
	this->xferDesc.status = I2CM_STATUS_OK;
	this->xferDesc.txSz = 1;
	this->xferDesc.rxSz = 1;
	this->xferDesc.txBuff = &txData;
	this->xferDesc.rxBuff = &ret;

	Chip_I2CM_XferBlocking(I2CM_t::ifacePtrs[this->num], &this->xferDesc);
	return ret;
}

uint32_t I2CM_t::transfer(uint8_t slaveAddr, uint8_t* txData, uint16_t txDataSize,
			uint8_t* rxData, uint16_t rxDataSize){

	this->xferDesc.slaveAddr = slaveAddr;
	this->xferDesc.options = this->options;
	this->xferDesc.status = I2CM_STATUS_OK;
	this->xferDesc.txSz = txDataSize;
	this->xferDesc.rxSz = rxDataSize;
	this->xferDesc.txBuff = txData;
	this->xferDesc.rxBuff = rxData;

	return Chip_I2CM_XferBlocking(I2CM_t::ifacePtrs[this->num], &this->xferDesc);
}

void I2CM_t::setOptions(uint8_t options){
	this->options = options;
}

uint16_t I2CM_t::getLastOperationStatus(void){
	return this->xferDesc.status;
}

