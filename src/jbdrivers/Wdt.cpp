/**
 * @file
 * @brief Watchdog Driver Realization
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

#include "jbdrivers/Wdt.hpp"

namespace jblib::jbdrivers
{

Wdt* Wdt::wdt_ = NULL;



Wdt* Wdt::getWatchDog(void)
{
	if(Wdt::wdt_ == NULL)
		Wdt::wdt_ = new Wdt();
	return Wdt::wdt_;
}



Wdt::Wdt(void)
{

}



void Wdt::initialize(uint32_t periodMs)
{
	uint32_t timeout = (WDT_OSC/4000) * periodMs;
	Chip_WWDT_SetTimeOut(LPC_WWDT, timeout);
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET|WWDT_WDMOD_WDINT);
	Chip_WWDT_UnsetOption(LPC_WWDT, WWDT_WDMOD_WDTOF);
}



void Wdt::start(void)
{
	Chip_WWDT_Start(LPC_WWDT);
}



void Wdt::reset(void)
{
	__disable_irq();
	Chip_WWDT_Feed(LPC_WWDT);
	__enable_irq();
}


}
