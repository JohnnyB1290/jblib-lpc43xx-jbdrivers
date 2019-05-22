/**
 * @file
 * @brief FRAM CY15B104Q Driver Description
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

#ifndef FRAMCY15B104Q_HPP_
#define FRAMCY15B104Q_HPP_

#include "jb_common.h"
#include "Ssp.hpp"

namespace jblib::jbdrivers
{

class FramCy15b104q
{
public:
	FramCy15b104q(Ssp* ssp, uint32_t sspDeviceNumber);
	bool initialize();
	void memcpy(void* destination, void* source, uint32_t size);

private:
	bool exchangeData(uint8_t opCode, uint32_t address,
			uint8_t* txData, uint8_t* rxData, uint32_t length);

	Ssp* ssp_ = NULL;
	uint32_t sspDeviceNumber_ = 0;
};

}

#endif /* FRAMCY15B104Q_HPP_ */
