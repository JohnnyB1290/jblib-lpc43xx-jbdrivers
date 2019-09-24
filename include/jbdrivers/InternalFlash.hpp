/**
 * @file
 * @brief Internal flash driver Description
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


#ifndef JBDRIVERS_INTERNALFLASH_HPP_
#define JBDRIVERS_INTERNALFLASH_HPP_

#include "jbkernel/jb_common.h"
#include "jbkernel/IVoidMemory.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

class InternalFlash : public IVoidMemory
{
public:
	InternalFlash(void);
	~InternalFlash(void);
	virtual void initialize(void);
	virtual bool isEmpty(uint32_t address, uint32_t size);

private:
	virtual void readMemory(uint32_t address, uint8_t* data, uint32_t size);
	virtual void writeMemory(uint32_t address, uint8_t* data, uint32_t size);
	virtual void eraseMemory(uint32_t address, uint32_t size);
	uint8_t getSectorNumber(uint32_t address);
	void eraseProgramPage(uint32_t address, uint8_t* data);
	int getBankNumber(uint32_t address);
};

}

#endif /* LIBS_LPC43XX_JB_DRIVERS_INCLUDE_JBDRIVERS_INTERNALFLASH_HPP_ */
