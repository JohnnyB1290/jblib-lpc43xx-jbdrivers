/**
 * @file
 * @brief SSP Driver Description
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

#ifndef SSP_HPP_
#define SSP_HPP_

#include "jbkernel/jb_common.h"

#if !defined SSP_NUM_MODULES
#define SSP_NUM_MODULES			2
#endif

namespace jblib::jbdrivers
{

typedef enum
{
	SSEL_TYPE_HARDWARE = 0,
	SSEL_TYPE_GPIO = 1,
}SspSlaveSelectType_t;



class Ssp
{
public:
	static Ssp* getSsp(uint8_t number);
	void initilize(uint32_t bitrate);
	void initilize(uint32_t bitrate, BoardGpio_t* sSelGpio);
	void initilize(uint32_t bitrate, SspSlaveSelectType_t sSelType);
	void initilize(uint32_t bitrate,
			BoardGpio_t* sSelGpios, uint32_t sSelSize);
	void enable(void);
	void disable(void);
	uint16_t txRxFrame(uint16_t data);
	uint16_t txRxFrame(uint16_t data, uint32_t deviceNumber);
	uint32_t txRxFrame(void* txData, void* rxData, uint32_t length);
	uint32_t txRxFrame(void* txData, void* rxData,
			uint32_t length, uint32_t deviceNumber);
	void deinitilize(void);

private:
	Ssp(uint8_t number);

	static Ssp* ssps_[SSP_NUM_MODULES];
	static LPC_SSP_T* lpcSsps_[];
	static const CHIP_RGU_RST_T resetNumbers_[];
	uint8_t number_ = 0;
	BoardGpio_t* sSelGpios_ = NULL;
	uint32_t sSelSize_ = 0;
};

}

#endif /* SSP_HPP_ */
