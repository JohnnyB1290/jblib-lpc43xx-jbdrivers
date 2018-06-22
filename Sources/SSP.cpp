/*
 * SSP.c
 *
 *  Created on: 19.01.2017
 *      Author: Stalker1290
 */

#include "SSP.hpp"
#include "stdlib.h"

LPC_SSP_T* SSP_t::SSP_ptr[] = {LPC_SSP0, LPC_SSP1};
const CHIP_RGU_RST_T SSP_t::ResetNumber[] = {RGU_SSP0_RST, RGU_SSP1_RST};

SSP_t* SSP_t::SSP_ptrs[Num_of_SSP] = {(SSP_t*)NULL, (SSP_t*)NULL};

SSP_t* SSP_t::get_SSP(uint8_t SSP_num)
{
	if(SSP_num < Num_of_SSP)
	{
		if(SSP_ptrs[SSP_num] == (SSP_t*)NULL) SSP_ptrs[SSP_num] = new SSP_t(SSP_num);
		return SSP_ptrs[SSP_num];
	}
	else return (SSP_t*)NULL;
}

SSP_t::SSP_t(uint8_t SSP_num)
{
	this->cselGpioArrPtr = (IODescription_t*)NULL;
	this->cselNum = 0;
	this->SSP_num = SSP_num;
}

void SSP_t::Initialize(uint32_t bitrate){
	this->Initialize(bitrate, (IODescription_t*)NULL, 0);
}

void SSP_t::Initialize(uint32_t bitrate, IODescription_t* cselGpioArrPtr){
	this->Initialize(bitrate, cselGpioArrPtr, 1);
}

void SSP_t::Initialize(uint32_t bitrate, SSEL_type_t SSEL_type){

	if(SSEL_type == GPIO_SSEL){
		IODescription_t* tempIODescPtr = (IODescription_t*)malloc_s(sizeof(IODescription_t));
		if(tempIODescPtr != (IODescription_t*)NULL){

			if (this->SSP_num == 0) {
				#ifdef SSEL0_PORT
				tempIODescPtr->port = SSEL0_PORT;
				tempIODescPtr->pin = SSEL0_PIN;
				tempIODescPtr->gpioPort = SSEL0_GPIOPORT;
				tempIODescPtr->gpioPin = SSEL0_GPIOPIN;
				tempIODescPtr->scuModeFunc = SSEL0_SCU_FUNC;
				#endif
			}
			if (this->SSP_num == 1) {
				#ifdef SSEL1_PORT
				tempIODescPtr->port = SSEL1_PORT;
				tempIODescPtr->pin = SSEL1_PIN;
				tempIODescPtr->gpioPort = SSEL1_GPIOPORT;
				tempIODescPtr->gpioPin = SSEL1_GPIOPIN;
				tempIODescPtr->scuModeFunc = SSEL1_SCU_FUNC;
				#endif
			}

			this->Initialize(bitrate, tempIODescPtr, 1);
		}
		else return;
	}
	else{
		this->Initialize(bitrate, (IODescription_t*)NULL, 0);
	}
}

void SSP_t::Initialize(uint32_t bitrate, IODescription_t* cselGpioArrPtr, uint32_t cselNum)
{
	this->cselGpioArrPtr = cselGpioArrPtr;
	this->cselNum = cselNum;

	if(this->cselNum == 0) this->cselGpioArrPtr = (IODescription_t*) NULL;

	if (this->cselGpioArrPtr == (IODescription_t*) NULL) {
		if (this->SSP_num == 0) {
			#ifdef SSEL0_PORT
			Chip_SCU_PinMuxSet(SSEL0_PORT, SSEL0_PIN, (SCU_PINIO_FAST | SSEL0_SCU_FUNC)); /* SSEL*/
			#endif
		}
		if (this->SSP_num == 1) {
			#ifdef SSEL1_PORT
			Chip_SCU_PinMuxSet(SSEL1_PORT, SSEL1_PIN, (SCU_PINIO_FAST | SSEL1_SCU_FUNC)); /* SSEL*/
			#endif
		}
	} else {
		for (uint32_t i = 0; i < this->cselNum; i++) {
			Chip_SCU_PinMuxSet(this->cselGpioArrPtr[i].port, this->cselGpioArrPtr[i].pin,
					(SCU_PINIO_FAST | this->cselGpioArrPtr[i].scuModeFunc));
			Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, this->cselGpioArrPtr[i].gpioPort, this->cselGpioArrPtr[i].gpioPin);
			Chip_GPIO_SetPinState(LPC_GPIO_PORT, this->cselGpioArrPtr[i].gpioPort, this->cselGpioArrPtr[i].gpioPin, (bool) true);
		}
	}

	if(this->SSP_num == 0)
	{
#ifdef SCK0_CLK_NUM
		Chip_SCU_ClockPinMuxSet(SCK0_CLK_NUM, SCU_PINIO_FAST|SCK0_SCU_FUNC);  //SCK
#endif
#ifdef SCK0_PORT
		Chip_SCU_PinMuxSet(SCK0_PORT, SCK0_PIN, (SCU_PINIO_FAST | SCK0_SCU_FUNC));  //SCK
#endif
		Chip_SCU_PinMuxSet(MOSI0_PORT, MOSI0_PIN, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | MOSI0_SCU_FUNC)); /*  MOSI */
		Chip_SCU_PinMuxSet(MISO0_PORT, MISO0_PIN, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | MISO0_SCU_FUNC)); /* MISO */
		Chip_SSP_Init(this->SSP_ptr[this->SSP_num]);
		Chip_SSP_SetFormat(this->SSP_ptr[this->SSP_num], SSP0_BITS_PER_FRAME, SSP0_FRAMEFORMAT, SSP0_CLOCKMODE);
	}
	if(this->SSP_num == 1)
	{
#ifdef SCK1_CLK_NUM
		Chip_SCU_ClockPinMuxSet(SCK1_CLK_NUM, SCU_PINIO_FAST|SCK1_SCU_FUNC);  //SCK
#endif
#ifdef SCK1_PORT
		Chip_SCU_PinMuxSet(SCK1_PORT, SCK1_PIN, (SCU_PINIO_FAST | SCK1_SCU_FUNC));  //SCK
#endif
		Chip_SCU_PinMuxSet(MOSI1_PORT, MOSI1_PIN, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | MOSI1_SCU_FUNC)); /*  MOSI */
		Chip_SCU_PinMuxSet(MISO1_PORT, MISO1_PIN, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | MISO1_SCU_FUNC)); /* MISO */
		Chip_SSP_Init(this->SSP_ptr[this->SSP_num]);
		Chip_SSP_SetFormat(this->SSP_ptr[this->SSP_num], SSP1_BITS_PER_FRAME, SSP1_FRAMEFORMAT, SSP1_CLOCKMODE);
	}
	Chip_SSP_SetBitRate(this->SSP_ptr[this->SSP_num], bitrate);
}

void SSP_t::Enable(void)
{
	Chip_SSP_Enable(this->SSP_ptr[this->SSP_num]);
}

void SSP_t::Disable(void)
{
	Chip_SSP_Disable(this->SSP_ptr[this->SSP_num]);
}

uint16_t SSP_t::TxRx_frame(uint16_t data){
	return this->TxRx_frame(data, 0);
}

uint16_t SSP_t::TxRx_frame(uint16_t data, uint32_t deviceNum)
{
	uint32_t i;

	Chip_SSP_Int_FlushData(this->SSP_ptr[this->SSP_num]);

	if((this->cselGpioArrPtr != (IODescription_t*)NULL) && (deviceNum < this->cselNum)){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, this->cselGpioArrPtr[deviceNum].gpioPort, this->cselGpioArrPtr[deviceNum].gpioPin);
	}
	
	Chip_SSP_SendFrame(this->SSP_ptr[this->SSP_num], data);
	for(i=0; i<50000; i++){
		if(Chip_SSP_GetStatus(this->SSP_ptr[this->SSP_num], SSP_STAT_BSY) != SET) break;
	}

	if((this->cselGpioArrPtr != (IODescription_t*)NULL) && (deviceNum < this->cselNum)){
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, this->cselGpioArrPtr[deviceNum].gpioPort, this->cselGpioArrPtr[deviceNum].gpioPin);
	}

	return Chip_SSP_ReceiveFrame(this->SSP_ptr[this->SSP_num]);
}

uint32_t SSP_t::TxRx_frame(void* tx_data, void* rx_data, uint32_t length)
{
	return this->TxRx_frame(tx_data, rx_data, length, 0);
}

uint32_t SSP_t::TxRx_frame(void* tx_data, void* rx_data, uint32_t length, uint32_t deviceNum)
{
	Chip_SSP_DATA_SETUP_T data_setup;
	uint32_t ret;

	data_setup.tx_data = tx_data;
	data_setup.tx_cnt = 0;
	data_setup.rx_data = rx_data;
	data_setup.rx_cnt = 0;
	data_setup.length = length;

	if((this->cselGpioArrPtr != (IODescription_t*)NULL) && (deviceNum < this->cselNum)){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, this->cselGpioArrPtr[deviceNum].gpioPort, this->cselGpioArrPtr[deviceNum].gpioPin);
		ret = Chip_SSP_RWFrames_Blocking(this->SSP_ptr[this->SSP_num], &data_setup);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, this->cselGpioArrPtr[deviceNum].gpioPort, this->cselGpioArrPtr[deviceNum].gpioPin);
	}
	else{
		ret = Chip_SSP_RWFrames_Blocking(this->SSP_ptr[this->SSP_num], &data_setup);
	}

	return ret;
}

void SSP_t::Deinitialize(void)
{
	Chip_RGU_TriggerReset(this->ResetNumber[this->SSP_num]);
	while (Chip_RGU_InReset(this->ResetNumber[this->SSP_num])) {}
	Chip_SSP_DeInit(this->SSP_ptr[this->SSP_num]);
}

