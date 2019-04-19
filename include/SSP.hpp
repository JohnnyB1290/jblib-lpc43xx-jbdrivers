/*
 * SSP.h
 *
 *  Created on: 19.01.2017
 *      Author: Stalker1290
 */

#ifndef SSP_HPP_
#define SSP_HPP_

#include "chip.h"
#include "Defines.h"

#define Num_of_SSP 2

typedef enum
{
	Hardware_SSEL = 0,
	GPIO_SSEL = 1,
}SSEL_type_t;

class SSP_t
{
public:
	static SSP_t* get_SSP(uint8_t SSP_num);
	void Initialize(uint32_t bitrate);
	void Initialize(uint32_t bitrate, IODescription_t* cselGpioArrPtr);
	void Initialize(uint32_t bitrate, SSEL_type_t SSEL_type);
	void Initialize(uint32_t bitrate, IODescription_t* cselGpioArrPtr, uint32_t cselNum);
	void Enable(void);
	void Disable(void);
	uint16_t TxRx_frame(uint16_t data);
	uint16_t TxRx_frame(uint16_t data, uint32_t deviceNum);
	uint32_t TxRx_frame(void* tx_data, void* rx_data, uint32_t length);
	uint32_t TxRx_frame(void* tx_data, void* rx_data, uint32_t length, uint32_t deviceNum);
	void Deinitialize(void);
private:
	static SSP_t* SSP_ptrs[Num_of_SSP];
	SSP_t(uint8_t SSP_num);
	uint8_t SSP_num;
	static LPC_SSP_T* SSP_ptr[];
	static const CHIP_RGU_RST_T ResetNumber[];
	IODescription_t* cselGpioArrPtr;
	uint32_t cselNum;
};


#endif /* LPC_DRIVERS_SSP_H_ */