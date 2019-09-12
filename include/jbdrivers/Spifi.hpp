/**
 * @file
 * @brief SPIFI Driver Description
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

#ifndef SPIFI_HPP_
#define SPIFI_HPP_

#include "jbkernel/jb_common.h"
#include "SPIFI/spifilib_api.h"
#include "jbkernel/IVoidMemory.hpp"

#ifndef SPIFLASH_BASE_ADDRESS
#define SPIFLASH_BASE_ADDRESS (0x14000000)
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;


class Spifi : public IVoidMemory
{
public:
	static Spifi* getSpifi(void);
	virtual void initialize(void);
	bool isEmpty(uint32_t address, uint32_t size);
	void deinitialize(void);

private:
	Spifi(void);
	static void setMemoryMode(void);
	static void clearMemoryMode(void);
	static void printErrorString(char* string, SPIFI_ERR_T errorNumber);
	static uint32_t calculateDivisor(uint32_t baseClock, uint32_t target);
	void writeBlock(uint32_t address,uint8_t* data, uint32_t size);
	int copyBlock(uint32_t srcBlockAddress, uint32_t dstBlockAddress,
			uint32_t offset, uint32_t size);
	void writeFast(uint32_t address, uint8_t* data, uint32_t size);
	virtual void readMemory(uint32_t address, uint8_t* data, uint32_t size);
	virtual void writeMemory(uint32_t address, uint8_t* data, uint32_t size);
	virtual void eraseMemory(uint32_t address, uint32_t size);

	static Spifi* spifi_;
	static SPIFI_HANDLE_T* spifiHandle_;
	static uint32_t* handleMemory_;
	static const PINMUX_GRP_T spifiPinMuxingGroup_[];
	uint32_t auxBlockAddress_ = 0;
};

}

#endif /* SPIFI_HPP_ */
