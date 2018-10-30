/*
 * I2CM.hpp
 *
 *  Created on: 29 окт. 2018 г.
 *      Author: Stalker1290
 */

#ifndef I2CM_HPP_
#define I2CM_HPP_

#include "chip.h"

#define NUM_OF_I2C 2

class I2CM_t{
public:
	static I2CM_t* getI2CM(uint8_t num);
	void initialize(void);
	void initialize(uint32_t speed);
	uint32_t write(uint8_t slaveAddr, uint8_t data);
	uint32_t write(uint8_t slaveAddr, uint8_t* data, uint16_t size);
	uint8_t read(uint8_t slaveAddr);
	void read(uint8_t slaveAddr, uint8_t* data, uint16_t size);
	uint8_t transfer(uint8_t slaveAddr, uint8_t txData);
	uint32_t transfer(uint8_t slaveAddr, uint8_t* txData, uint16_t txDataSize,
			uint8_t* rxData, uint16_t rxDataSize);
	void setOptions(uint8_t options);
	uint16_t getLastOperationStatus(void);
private:
	I2CM_t(uint8_t num);

	static I2CM_t* i2cmInstancePtr[NUM_OF_I2C];
	static LPC_I2C_T* ifacePtrs[NUM_OF_I2C];
	uint8_t num;
	uint8_t options;
	I2CM_XFER_T xferDesc;
	bool initialized;
};

#endif /* I2CM_HPP_ */
