/**
 * @file
 * @brief ATimer Driver Realization
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

#include "jbdrivers/Atimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

Atimer* Atimer::atimer_ = NULL;



Atimer* Atimer::getAtimer(void)
{
	if(Atimer::atimer_ == NULL)
		Atimer::atimer_ = new Atimer();
	return Atimer::atimer_;
}



Atimer::Atimer(void) : IIrqListener()
{
	this->setCode((uint64_t)1<<EVENTROUTER_IRQn);
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



void Atimer::irqHandler(int8_t irqNumber)
{
	if (Chip_EVRT_IsSourceInterrupting(EVRT_SRC_ATIMER)) {
		if(this->callback_ != NULL)
			this->callback_->voidCallback((void*)this, NULL);
		Chip_ATIMER_ClearIntStatus(LPC_ATIMER);
		Chip_EVRT_ClrPendIntSrc(EVRT_SRC_ATIMER);
	}
}



void Atimer::initialize(uint8_t periodS)
{
	if(!((LPC_CREG->CREG0 & (1 << 1)) && (LPC_CREG->CREG0 & (1 << 0))))
		Chip_Clock_RTCEnable();
	Chip_ATIMER_Init(LPC_ATIMER, periodS * 1023);
	Chip_EVRT_Init();
	Chip_EVRT_ConfigIntSrcActiveType(EVRT_SRC_ATIMER, EVRT_SRC_ACTIVE_HIGH_LEVEL);
	Chip_EVRT_SetUpIntSrc(EVRT_SRC_ATIMER, ENABLE);

	#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(EVENTROUTER_IRQn,
			NVIC_EncodePriority(prioritygroup, EVENT_ROUTER_INTERRUPT_PRIORITY, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(EVENTROUTER_IRQn, EVENT_ROUTER_INTERRUPT_PRIORITY);
	#endif

	NVIC_ClearPendingIRQ(EVENTROUTER_IRQn);
	NVIC_EnableIRQ(EVENTROUTER_IRQn);
	Chip_ATIMER_ClearIntStatus(LPC_ATIMER);
	Chip_EVRT_ClrPendIntSrc(EVRT_SRC_ATIMER);
}



void Atimer::start(void)
{
	Chip_ATIMER_IntEnable(LPC_ATIMER);
}



void Atimer::stop(void)
{
	Chip_ATIMER_IntDisable(LPC_ATIMER);
}



void Atimer::deinitialize(void)
{
	IrqController::getIrqController()->deletePeripheralIrqListener(this);
	NVIC_DisableIRQ(EVENTROUTER_IRQn);
	NVIC_ClearPendingIRQ(EVENTROUTER_IRQn);
	Chip_ATIMER_DeInit(LPC_ATIMER);
	Chip_EVRT_SetUpIntSrc(EVRT_SRC_ATIMER, DISABLE);
	this->callback_ = NULL;
}



void Atimer::addCallback(IVoidCallback* callback)
{
	this->callback_ = callback;
}



void Atimer::deleteCallback(void)
{
	this->callback_ = NULL;
}

}
