/**
 * @file
 * @brief I2C Master Driver Description
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

#ifndef I2CMASTER_HPP_
#define I2CMASTER_HPP_

#include "jb_common.h"

#if !defined I2C_NUM
#define I2C_NUM			2
#endif

namespace jblib::jbdrivers
{

class I2cMaster{
public:
	static I2cMaster* getI2cMaster(uint8_t number);
	void initialize(void);
	void initialize(uint32_t speed);
	uint32_t write(uint8_t slaveAddress, uint8_t data);
	uint32_t write(uint8_t slaveAddress, uint8_t* data, uint16_t size);
	uint8_t read(uint8_t slaveAddress);
	void read(uint8_t slaveAddress, uint8_t* data, uint16_t size);
	uint8_t transfer(uint8_t slaveAddress, uint8_t txData);
	uint32_t transfer(uint8_t slaveAddress, uint8_t* txData, uint16_t txDataSize,
			uint8_t* rxData, uint16_t rxDataSize);
	void setOptions(uint8_t options);
	uint16_t getLastOperationStatus(void);

private:
	I2cMaster(uint8_t number);

	static I2cMaster* i2cMasters_[I2C_NUM];
	static LPC_I2C_T* lpcI2cs_[I2C_NUM];
	bool initialized_ = false;
	uint8_t number_ = 0;
	uint8_t options_ = 0;
	I2CM_XFER_T xferDescriptor_;
};

}

#endif /* I2CMASTER_HPP_ */
