/**
 * @file
 * @brief JbController M4 Core class definition
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

#ifndef JBCONTROLLERM4_HPP_
#define JBCONTROLLERM4_HPP_

#include "jbkernel/jb_common.h"
#include "jbkernel/JbKernel.hpp"


namespace jblib::jbdrivers
{

using namespace jbkernel;

class JbController : public JbKernel
{
public:
	static void initialize(void);
	static void gpioOn(uint8_t number);
	static void gpioOff(uint8_t number);
	static void gpioTgl(uint8_t number);
	static void startM0App(uint32_t imageAddress);
	static void startM0Sub(uint32_t imageAddress);
	static void copyFwToRam(uint32_t flashAddress, uint32_t ramAddress, uint32_t size);
	static void resetPeriphery(void);
	static void softReset(void);
	static void goToApp(uint32_t applicationAddress);

protected:
	static BoardGpio_t boardGpios_[];
};

}

#endif /*  JBCONTROLLERM4_HPP_ */
