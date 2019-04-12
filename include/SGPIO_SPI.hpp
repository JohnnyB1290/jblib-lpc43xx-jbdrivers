/*
 * SGPIO_SPI.hpp
 *
 *  Created on: 28 сент. 2018 г.
 *      Author: Stalker1290
 */

#ifndef SGPIO_SPI_HPP_
#define SGPIO_SPI_HPP_

#include "chip.h"
#include "Defines.h"

class SGPIO_SPI_t
{
public:
	static SGPIO_SPI_t* getSGPIOspi(void);
	void Initialize(uint32_t bitrate);
	uint8_t TxRx_frame(uint8_t data);
	uint32_t TxRx_frame(uint8_t* tx_data, uint8_t* rx_data, uint32_t length);
	void Deinitialize(void);
private:
	static SGPIO_SPI_t* SGPIO_SPI_ptr;
	static uint8_t reverseBitsInByteTable[];
	SGPIO_SPI_t(void);

	uint32_t masterMask;
	uint32_t sgpioClockHz;
};

#endif /* SGPIO_SPI_HPP_ */
