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

/* supported transmission sizes for the data */
typedef enum wordSize_t {
	DBIT_4 = 4,
	DBIT_8 = 8,
	DBIT_10 = 10,
	DBIT_12 = 12,
	DBIT_14 = 14,
	DBIT_16 = 16,
	DBIT_18 = 18,
	DBIT_20 = 20,
	DBIT_22 = 22,
	DBIT_24 = 24,
	DBIT_26 = 26,
	DBIT_28 = 28,
	DBIT_30 = 30,
	DBIT_32 = 32,
} wordSize_t;

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
	void SGPIO_spiWrite(uint8_t dataWrite, uint8_t* dataRead);

	static SgpioSliceCfg DATA_OUT_CFG;
	static SgpioSliceCfg DATA_IN_CFG;
	static SgpioSliceCfg CLOCK_CFG;
	static SgpioSliceCfg NSS_CFG;

	uint32_t masterMask;
	uint32_t sgpioClockHz;
};

#endif /* SGPIO_SPI_HPP_ */
