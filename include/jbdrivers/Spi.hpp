/**
 * @file
 * @brief SPI Driver Description
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

#ifndef JBDRIVERS_SPI_HPP_
#define JBDRIVERS_SPI_HPP_

#include "jbkernel/jb_common.h"

namespace jblib::jbdrivers
{

class Spi
{
public:
	static Spi* getSpi(void);
	void initialize(uint32_t bitrate, BoardGpio_t* sSelGpio);
	void initialize(uint32_t bitrate,
			BoardGpio_t* sSelGpios, uint32_t sSelSize);
	uint16_t txRxFrame(uint16_t data);
	uint16_t txRxFrame(uint16_t data, uint32_t deviceNumber);
	uint32_t txRxFrame(void* txData, void* rxData, uint32_t length);
	uint32_t txRxFrame(void* txData, void* rxData, uint32_t length, uint32_t deviceNumber);
	void txRxFrames(uint32_t framesCount, void** txDataPointers, void** rxDataPointers, uint32_t* lengthArray, uint32_t deviceNumber);
	void deinitialize(void);

private:
	Spi(void);

	static Spi* spi_;
	static LPC_SPI_T* lpcSpi_;
	static constexpr CHIP_RGU_RST_T resetNumber_ = RGU_SPI_RST;
	BoardGpio_t* sSelGpios_ = NULL;
	uint32_t sSelSize_ = 0;
};

}

#endif /* JBDRIVERS_SPI_HPP_ */
