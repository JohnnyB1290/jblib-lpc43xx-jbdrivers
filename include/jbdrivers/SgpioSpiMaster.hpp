/**
 * @file
 * @brief SGPIO SPI Master Driver Description
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

#ifndef SGPIOSPIMASTER_HPP_
#define SGPIOSPIMASTER_HPP_

#include "jbkernel/jb_common.h"

namespace jblib::jbdrivers
{

class SgpioSpiMaster
{
public:
	static SgpioSpiMaster* getSgpioSpiMaster(void);
	void initialize(uint32_t bitrate);
	uint8_t txRxFrame(uint8_t data);
	uint32_t txRxFrame(uint8_t* txData, uint8_t* rxData, uint32_t length);
	void deinitialize(void);

private:
	SgpioSpiMaster(void);

	static SgpioSpiMaster* sgpioSpiMaster_;
	static const uint8_t reverseBitsInByteTable_[];
	uint32_t masterMask_ = 0;
	uint32_t sgpioClockHz_ = 0;
};

}

#endif /* SGPIOSPIMASTER_HPP_ */
