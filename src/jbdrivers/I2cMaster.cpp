/**
 * @file
 * @brief I2C Master Driver Realization
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

#include "jbdrivers/I2cMaster.hpp"

namespace jblib::jbdrivers
{

I2cMaster* I2cMaster::i2cMasters_[I2C_NUM] = { (I2cMaster*)NULL, (I2cMaster*)NULL };
LPC_I2C_T* I2cMaster::lpcI2cs_[I2C_NUM] = { LPC_I2C0, LPC_I2C1 };



I2cMaster* I2cMaster::getI2cMaster(uint8_t number)
{
	if(number >= I2C_NUM)
		return (I2cMaster*)NULL;
	if(i2cMasters_[number] == (I2cMaster*)NULL)
		i2cMasters_[number] = new I2cMaster(number);
	return i2cMasters_[number];
}



I2cMaster::I2cMaster(uint8_t number)
{
	this->number_ = number;
}



void I2cMaster::initialize(uint32_t speed)
{
	if(!this->initialized_) {
		if(this->number_ == 0)
			Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
		else {
			Chip_SCU_PinMuxSet(I2C_1_SDA_PORT, I2C_1_SDA_PIN,
					(SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | I2C_1_SDA_SCU_MODE));	/*I2C1_SDA */
			Chip_SCU_PinMuxSet(I2C_1_SCL_PORT, I2C_1_SCL_PIN,
					(SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | I2C_1_SCL_SCU_MODE));	/*I2C1_SCL */
		}
		Chip_I2CM_Init(lpcI2cs_[this->number_]);
		Chip_I2CM_SetBusSpeed(lpcI2cs_[this->number_], speed);
		this->initialized_ = true;
	}
}



void I2cMaster::initialize(void)
{
	this->initialize(100000);
}



uint32_t I2cMaster::write(uint8_t slaveAddress, uint8_t data)
{
	this->xferDescriptor_.slaveAddr = slaveAddress;
	this->xferDescriptor_.options = this->options_;
	this->xferDescriptor_.status = I2CM_STATUS_OK;
	this->xferDescriptor_.txSz = 1;
	this->xferDescriptor_.rxSz = 0;
	this->xferDescriptor_.txBuff = &data;
	this->xferDescriptor_.rxBuff = (uint8_t*)NULL;
	return Chip_I2CM_XferBlocking(I2cMaster::lpcI2cs_[this->number_], &this->xferDescriptor_);
}



uint32_t I2cMaster::write(uint8_t slaveAddress, uint8_t* data, uint16_t size)
{
	this->xferDescriptor_.slaveAddr = slaveAddress;
	this->xferDescriptor_.options = this->options_;
	this->xferDescriptor_.status = I2CM_STATUS_OK;
	this->xferDescriptor_.txSz = size;
	this->xferDescriptor_.rxSz = 0;
	this->xferDescriptor_.txBuff = data;
	this->xferDescriptor_.rxBuff = (uint8_t*)NULL;
	return Chip_I2CM_XferBlocking(I2cMaster::lpcI2cs_[this->number_], &this->xferDescriptor_);
}



uint8_t I2cMaster::read(uint8_t slaveAddress)
{
	uint8_t ret = 0;
	this->xferDescriptor_.slaveAddr = slaveAddress;
	this->xferDescriptor_.options = this->options_;
	this->xferDescriptor_.status = I2CM_STATUS_OK;
	this->xferDescriptor_.txSz = 0;
	this->xferDescriptor_.rxSz = 1;
	this->xferDescriptor_.txBuff = (uint8_t*)NULL;
	this->xferDescriptor_.rxBuff = &ret;
	Chip_I2CM_XferBlocking(I2cMaster::lpcI2cs_[this->number_], &this->xferDescriptor_);
	return ret;
}



void I2cMaster::read(uint8_t slaveAddress, uint8_t* data, uint16_t size)
{
	this->xferDescriptor_.slaveAddr = slaveAddress;
	this->xferDescriptor_.options = this->options_;
	this->xferDescriptor_.status = I2CM_STATUS_OK;
	this->xferDescriptor_.txSz = 0;
	this->xferDescriptor_.rxSz = size;
	this->xferDescriptor_.txBuff = (uint8_t*)NULL;
	this->xferDescriptor_.rxBuff = data;
	Chip_I2CM_XferBlocking(I2cMaster::lpcI2cs_[this->number_], &this->xferDescriptor_);
}



uint8_t I2cMaster::transfer(uint8_t slaveAddress, uint8_t txData)
{
	uint8_t ret = 0;
	this->xferDescriptor_.slaveAddr = slaveAddress;
	this->xferDescriptor_.options = this->options_;
	this->xferDescriptor_.status = I2CM_STATUS_OK;
	this->xferDescriptor_.txSz = 1;
	this->xferDescriptor_.rxSz = 1;
	this->xferDescriptor_.txBuff = &txData;
	this->xferDescriptor_.rxBuff = &ret;
	Chip_I2CM_XferBlocking(I2cMaster::lpcI2cs_[this->number_], &this->xferDescriptor_);
	return ret;
}



uint32_t I2cMaster::transfer(uint8_t slaveAddress, uint8_t* txData, uint16_t txDataSize,
			uint8_t* rxData, uint16_t rxDataSize)
{
	this->xferDescriptor_.slaveAddr = slaveAddress;
	this->xferDescriptor_.options = this->options_;
	this->xferDescriptor_.status = I2CM_STATUS_OK;
	this->xferDescriptor_.txSz = txDataSize;
	this->xferDescriptor_.rxSz = rxDataSize;
	this->xferDescriptor_.txBuff = txData;
	this->xferDescriptor_.rxBuff = rxData;
	return Chip_I2CM_XferBlocking(I2cMaster::lpcI2cs_[this->number_], &this->xferDescriptor_);
}



void I2cMaster::setOptions(uint8_t options)
{
	this->options_ = options;
}



uint16_t I2cMaster::getLastOperationStatus(void)
{
	return this->xferDescriptor_.status;
}

}
