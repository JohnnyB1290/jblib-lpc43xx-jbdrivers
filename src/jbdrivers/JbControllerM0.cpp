/**
 * @file
 * @brief JbController M0 Core class realization
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

#ifdef CORE_M0
#include <string.h>
#include "jbdrivers/JbControllerM0.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

BoardGpio_t JbController::boardOutputGpios_[] = JBCONTROLLER_BOARD_OUTPUT_GPIOS;
BoardGpio_t JbController::boardInputGpios_[] = JBCONTROLLER_BOARD_INPUT_GPIOS;



void JbController::initialize(void)
{
	static bool isInitialized = false;
	if(!isInitialized) {
		SystemCoreClockUpdate();
		enableInterrupts();
		isInitialized = true;
	}
}



void JbController::gpioOn(uint8_t number)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
			boardOutputGpios_[number].gpioPort, boardOutputGpios_[number].gpioPin);
}



void JbController::gpioOff(uint8_t number)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
			boardOutputGpios_[number].gpioPort, boardOutputGpios_[number].gpioPin);
}



void JbController::gpioTgl(uint8_t number)
{
	Chip_GPIO_SetPinToggle(LPC_GPIO_PORT,
			boardOutputGpios_[number].gpioPort, boardOutputGpios_[number].gpioPin);
}



bool JbController::getGpio(uint8_t number)
{
	return Chip_GPIO_GetPinState(LPC_GPIO_PORT,
			boardInputGpios_[number].gpioPort, boardInputGpios_[number].gpioPin);
}

}

#endif
