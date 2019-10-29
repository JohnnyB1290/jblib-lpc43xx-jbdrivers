/**
 * @file
 * @brief SPI Driver Realization
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

#include "jbdrivers/Spi.hpp"

namespace jblib::jbdrivers
{

LPC_SPI_T* Spi::lpcSpi_ = LPC_SPI;
Spi* Spi::spi_ = NULL;



Spi* Spi::getSpi(void)
{
	if(!spi_){
		spi_ = new Spi();
	}
	return spi_;
}



Spi::Spi(void)
{

}



void Spi::initialize(uint32_t bitrate, BoardGpio_t* sSelGpio)
{
	this->initialize(bitrate, sSelGpio, 1);
}



void Spi::initialize(uint32_t bitrate,
		BoardGpio_t* sSelGpios, uint32_t sSelSize)
{
	if((sSelSize == 0) || (sSelGpios == NULL)){
		return;
	}
	this->sSelGpios_ = sSelGpios;
	this->sSelSize_ = sSelSize;

	for (uint32_t i = 0; i < this->sSelSize_; i++) {
		Chip_SCU_PinMuxSet(this->sSelGpios_[i].port, this->sSelGpios_[i].pin,
				(SCU_PINIO_FAST | this->sSelGpios_[i].scuMode));
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, this->sSelGpios_[i].gpioPort,
				this->sSelGpios_[i].gpioPin);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, this->sSelGpios_[i].gpioPort,
				this->sSelGpios_[i].gpioPin, (bool) true);
	}

	Chip_SCU_PinMuxSet(SPI_CLK_PORT, SPI_CLK_PIN,
			(SCU_PINIO_FAST | SPI_CLK_SCU_MODE));  //SCK
	Chip_SCU_PinMuxSet(SPI_MOSI_PORT, SPI_MOSI_PIN,
			(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SPI_MOSI_SCU_MODE)); /*  MOSI */
	Chip_SCU_PinMuxSet(SPI_MISO_PORT, SPI_MISO_PIN,
			(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SPI_MISO_SCU_MODE)); /* MISO */

	Chip_Clock_Enable(CLK_SPI);
	Chip_SPI_SetMode(lpcSpi_, SPI_MODE_MASTER);
	lpcSpi_->CR = (lpcSpi_->CR & (~0xF1C)) | SPI_CR_BIT_EN |
			SPI_BITS_PER_FRAME | SPI_CLOCKMODE | SPI_BIT_ORDER;
	Chip_SPI_SetBitRate(lpcSpi_, bitrate);
}



uint16_t Spi::txRxFrame(uint16_t data)
{
	return this->txRxFrame(data, 0);
}



uint16_t Spi::txRxFrame(uint16_t data, uint32_t deviceNumber)
{
	Chip_SPI_Int_FlushData(this->lpcSpi_);

	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}

	Chip_SPI_SendFrame(this->lpcSpi_, data);

	for(uint32_t i = 0; i < 50000; i++) {
		if(Chip_SPI_GetStatus(this->lpcSpi_) & (SPI_SR_SPIF | SPI_SR_ERROR)){
			break;
		}
	}

	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}

	return Chip_SPI_ReceiveFrame(this->lpcSpi_);
}



uint32_t Spi::txRxFrame(void* txData, void* rxData, uint32_t length)
{
	return this->txRxFrame(txData, rxData, length, 0);
}



uint32_t Spi::txRxFrame(void* txData, void* rxData, uint32_t length, uint32_t deviceNumber)
{
	SPI_DATA_SETUP_T dataSetup = {
		.pTxData = (uint8_t*)txData,
		.pRxData = (uint8_t*)rxData,
		.cnt = 0,
		.length = length,
		.fnBefFrame = NULL,
		.fnAftFrame = NULL,
		.fnBefTransfer = NULL,
		.fnAftTransfer = NULL
	};
	uint32_t ret = 0;

	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
		ret = Chip_SPI_RWFrames_Blocking(this->lpcSpi_, &dataSetup);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}
	else{
		ret = Chip_SPI_RWFrames_Blocking(this->lpcSpi_, &dataSetup);
	}
	return ret;
}



void Spi::txRxFrames(uint32_t framesCount, void** txDataPointers,
		void** rxDataPointers, uint32_t* lengthArray, uint32_t deviceNumber)
{
	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}
	uint32_t i = 0;
	while(i < framesCount) {
		this->txRxFrame(txDataPointers[i], rxDataPointers[i], lengthArray[i], this->sSelSize_ + 1);
		i++;
	}
	if((this->sSelGpios_ != NULL) && (deviceNumber < this->sSelSize_)){
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
				this->sSelGpios_[deviceNumber].gpioPort,
				this->sSelGpios_[deviceNumber].gpioPin);
	}
}



void Spi::deinitialize(void)
{
	Chip_RGU_TriggerReset(this->resetNumber_);
	while (Chip_RGU_InReset(this->resetNumber_)) {}
	Chip_Clock_Disable(CLK_SPI);
}


}
