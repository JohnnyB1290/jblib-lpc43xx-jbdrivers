/**
 * @file
 * @brief DAC Driver Realization
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

#include "jbdrivers/Dac.hpp"

namespace jblib::jbdrivers
{

Dac* Dac::dac_ = NULL;



Dac* Dac::getDac(void)
{
	if(Dac::dac_ == NULL)
		Dac::dac_ = new Dac();
	return Dac::dac_;
}



Dac::Dac(void)
{

}



void Dac::initialize(void)
{
	Chip_DAC_Init(LPC_DAC);
	Chip_DAC_UpdateValue(LPC_DAC,0);
}



void Dac::start(void)
{
	LPC_DAC->CTRL = 8;
}



void Dac::setOutput(uint32_t value)
{
	Chip_DAC_UpdateValue(LPC_DAC,value);
}



void Dac::deinitialize(void)
{
	Chip_RGU_TriggerReset(RGU_DAC_RST);
	while (Chip_RGU_InReset(RGU_DAC_RST)) {}
	Chip_DAC_DeInit(LPC_DAC);
}

}
