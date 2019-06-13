/**
 * @file
 * @brief SSP Driver Realization
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

#include "Ssp.hpp"

namespace jblib::jbdrivers
{

LPC_SSP_T* Ssp::lpcSsps_[] = {LPC_SSP0, LPC_SSP1};
const CHIP_RGU_RST_T Ssp::resetNumbers_[] = {RGU_SSP0_RST, RGU_SSP1_RST};
Ssp* Ssp::ssps_[SSP_NUM_MODULES] = {NULL, NULL};



Ssp* Ssp::getSsp(uint8_t number)
{
	if(number < SSP_NUM_MODULES) {
		if(ssps_[number] == NULL)
			ssps_[number] = new Ssp(number);
		return ssps_[number];
	}
	else
		return (Ssp*)NULL;
}



Ssp::Ssp(uint8_t number)
{
	this->number_ = number;
}



void Ssp::initilize(uint32_t bitrate)
{
	this->initilize(bitrate, (BoardGpio_t*)NULL, 0);
}



void Ssp::initilize(uint32_t bitrate, BoardGpio_t* sSelGpio)
{
	this->initilize(bitrate, sSelGpio, 1);
}



void Ssp::initilize(uint32_t bitrate, SspSlaveSelectType_t sSelType)
{
	if(sSelType == SSEL_TYPE_GPIO) {
		BoardGpio_t* tempIODescPtr =
				(BoardGpio_t*)malloc_s(sizeof(BoardGpio_t));
		if(tempIODescPtr) {
			if (this->number_ == 0) {
				tempIODescPtr->port = 		SSP_0_SSEL_PORT;
				tempIODescPtr->pin = 		SSP_0_SSEL_PIN;
				tempIODescPtr->gpioPort = 	SSP_0_SSEL_GPIO_PORT;
				tempIODescPtr->gpioPin = 	SSP_0_SSEL_GPIO_PIN;
				tempIODescPtr->scuMode = 	SSP_0_SSEL_SCU_MODE;
			}
			if (this->number_ == 1) {
				tempIODescPtr->port = 		SSP_1_SSEL_PORT;
				tempIODescPtr->pin = 		SSP_1_SSEL_PIN;
				tempIODescPtr->gpioPort = 	SSP_1_SSEL_GPIO_PORT;
				tempIODescPtr->gpioPin = 	SSP_1_SSEL_GPIO_PIN;
				tempIODescPtr->scuMode = 	SSP_1_SSEL_SCU_MODE;
			}
			this->initilize(bitrate, tempIODescPtr, 1);
		}
		else
			return;
	}
	else
		this->initilize(bitrate, (BoardGpio_t*)NULL, 0);
}



void Ssp::initilize(uint32_t bitrate,
		BoardGpio_t* sSelGpios, uint32_t sSelSize)
{
	this->sSelGpios_ = sSelGpios;
	this->sSelSize_ = sSelSize;
	if(this->sSelSize_ == 0)
		this->sSelGpios_ = (BoardGpio_t*) NULL;

	if (this->sSelGpios_ == (BoardGpio_t*) NULL) {
		if (this->number_ == 0)
			Chip_SCU_PinMuxSet(SSP_0_SSEL_PORT, SSP_0_SSEL_PIN,
					(SCU_PINIO_FAST | SSP_0_SSEL_SCU_MODE));
		if (this->number_ == 1)
			Chip_SCU_PinMuxSet(SSP_1_SSEL_PORT, SSP_1_SSEL_PIN,
					(SCU_PINIO_FAST | SSP_1_SSEL_SCU_MODE));
	} else {
		for (uint32_t i = 0; i < this->sSelSize_; i++) {
			Chip_SCU_PinMuxSet(this->sSelGpios_[i].port, this->sSelGpios_[i].pin,
					(SCU_PINIO_FAST | this->sSelGpios_[i].scuMode));
			Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, this->sSelGpios_[i].gpioPort,
					this->sSelGpios_[i].gpioPin);
			Chip_GPIO_SetPinState(LPC_GPIO_PORT, this->sSelGpios_[i].gpioPort,
					this->sSelGpios_[i].gpioPin, (bool) true);
		}
	}

	if(this->number_ == 0) {
		#ifdef SSP_0_CLK_NUM
		Chip_SCU_ClockPinMuxSet(SSP_0_CLK_NUM, SCU_PINIO_FAST | SCK0_SCU_FUNC);  //SCK
		#else
		Chip_SCU_PinMuxSet(SSP_0_CLK_PORT, SSP_0_CLK_PIN,
				(SCU_PINIO_FAST | SSP_0_CLK_SCU_MODE));  //SCK
		#endif
		Chip_SCU_PinMuxSet(SSP_0_MOSI_PORT, SSP_0_MOSI_PIN,
				(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SSP_0_MOSI_SCU_MODE)); /*  MOSI */
		Chip_SCU_PinMuxSet(SSP_0_MOSI_PORT, SSP_0_MOSI_PIN,
				(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SSP_0_MOSI_SCU_MODE)); /* MISO */
		Chip_SSP_Init(this->lpcSsps_[this->number_]);
		Chip_SSP_SetFormat(this->lpcSsps_[this->number_], SSP_0_BITS_PER_FRAME,
				SSP_0_FRAMEFORMAT, SSP_0_CLOCKMODE);
	}
	if(this->number_ == 1) {
		#ifdef SSP_1_CLK_NUM
		Chip_SCU_ClockPinMuxSet(SSP_1_CLK_NUM, SCU_PINIO_FAST | SCK0_SCU_FUNC);  //SCK
		#else
		Chip_SCU_PinMuxSet(SSP_1_CLK_PORT, SSP_1_CLK_PIN,
				(SCU_PINIO_FAST | SSP_1_CLK_SCU_MODE));  //SCK
		#endif
		Chip_SCU_PinMuxSet(SSP_1_MOSI_PORT, SSP_1_MOSI_PIN,
				(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SSP_1_MOSI_SCU_MODE)); /*  MOSI */
		Chip_SCU_PinMuxSet(SSP_1_MOSI_PORT, SSP_1_MOSI_PIN,
				(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SSP_1_MOSI_SCU_MODE)); /* MISO */
		Chip_SSP_Init(this->lpcSsps_[this->number_]);
		Chip_SSP_SetFormat(this->lpcSsps_[this->number_], SSP_1_BITS_PER_FRAME,
				SSP_1_FRAMEFORMAT, SSP_1_CLOCKMODE);
	}

	Chip_SSP_SetBitRate(this->lpcSsps_[this->number_], bitrate);
}



void Ssp::enable(void)
{
	Chip_SSP_Enable(this->lpcSsps_[this->number_]);
}



void Ssp::disable(void)
{
	Chip_SSP_Disable(this->lpcSsps_[this->number_]);
}



uint16_t Ssp::txRxFrame(uint16_t data){
	return this->txRxFrame(data, 0);
}



uint16_t Ssp::txRxFrame(uint16_t data, uint32_t deviceNumber)
{
	Chip_SSP_Int_FlushData(this->lpcSsps_[this->number_]);

	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_))
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	
	Chip_SSP_SendFrame(this->lpcSsps_[this->number_], data);

	for(uint32_t i = 0; i < 50000; i++) {
		if(Chip_SSP_GetStatus(this->lpcSsps_[this->number_], SSP_STAT_BSY) != SET)
			break;
	}

	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}

	return Chip_SSP_ReceiveFrame(this->lpcSsps_[this->number_]);
}



uint32_t Ssp::txRxFrame(void* txData, void* rxData, uint32_t length)
{
	return this->txRxFrame(txData, rxData, length, 0);
}



uint32_t Ssp::txRxFrame(void* txData, void* rxData,
		uint32_t length, uint32_t deviceNumber)
{
	Chip_SSP_DATA_SETUP_T dataSetup;
	uint32_t ret = 0;

	dataSetup.tx_data = txData;
	dataSetup.tx_cnt = 0;
	dataSetup.rx_data = rxData;
	dataSetup.rx_cnt = 0;
	dataSetup.length = length;

	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
		ret = Chip_SSP_RWFrames_Blocking(this->lpcSsps_[this->number_], &dataSetup);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}
	else
		ret = Chip_SSP_RWFrames_Blocking(this->lpcSsps_[this->number_], &dataSetup);
	return ret;
}



void Ssp::deinitilize(void)
{
	Chip_RGU_TriggerReset(this->resetNumbers_[this->number_]);
	while (Chip_RGU_InReset(this->resetNumbers_[this->number_])) {}
	Chip_SSP_DeInit(this->lpcSsps_[this->number_]);
}

}
