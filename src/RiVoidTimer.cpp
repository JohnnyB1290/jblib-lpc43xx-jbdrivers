/**
 * @file
 * @brief Void Timer on RI Timer Driver Realization
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

#include "RiVoidTimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

RiVoidTimer* RiVoidTimer::riVoidTimer_ = NULL;



RiVoidTimer* RiVoidTimer::getRiVoidTimer(void)
{
	if(RiVoidTimer::riVoidTimer_ == NULL)
		RiVoidTimer::riVoidTimer_ = new RiVoidTimer();
	return RiVoidTimer::riVoidTimer_;
}



RiVoidTimer::RiVoidTimer(void) : IVoidTimer(), IIrqListener()
{
	this->setCode((uint64_t)1 << RITIMER_IRQn);
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



void RiVoidTimer::irqHandler(int8_t irqNumber)
{
	if(this->callback_)
		this->callback_->voidCallback((void*)this, NULL);
	Chip_RIT_ClearInt(LPC_RITIMER);
}



void RiVoidTimer::initialize(uint32_t us)
{
	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_Disable(LPC_RITIMER);
	Chip_RIT_TimerDebugEnable(LPC_RITIMER);

	uint32_t cmpValue =
			(Chip_Clock_GetRate(CLK_MX_RITIMER) / 1000000) * us;
	Chip_RIT_SetCOMPVAL(LPC_RITIMER, cmpValue);

	Chip_RIT_EnableCTRL(LPC_RITIMER, RIT_CTRL_ENCLR);

	#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(RITIMER_IRQn,
			NVIC_EncodePriority(prioritygroup, RI_TIMER_INTERRUPT_PRIORITY, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(RITIMER_IRQn, RI_TIMER_INTERRUPT_PRIORITY);
	#endif

	NVIC_ClearPendingIRQ(RITIMER_IRQn);
	NVIC_EnableIRQ(RITIMER_IRQn);
}



void RiVoidTimer::start(void)
{
	Chip_RIT_Enable(LPC_RITIMER);
}



void RiVoidTimer::stop(void)
{
	Chip_RIT_ClearInt(LPC_RITIMER);
	NVIC_ClearPendingIRQ(RITIMER_IRQn);
	Chip_RIT_Disable(LPC_RITIMER);
}



void RiVoidTimer::reset(void)
{
	if(LPC_RITIMER->CTRL&RIT_CTRL_TEN) {
		this->stop();
		LPC_RITIMER->COUNTER = 0;
		this->start();
	}
	else
		LPC_RITIMER->COUNTER = 0;
}



uint32_t RiVoidTimer::getCounter(void) const
{
	return LPC_RITIMER->COUNTER;
}



void RiVoidTimer::setCounter(uint32_t count)
{
	if(LPC_RITIMER->CTRL & RIT_CTRL_TEN) {
		this->stop();
		LPC_RITIMER->COUNTER = count;
		this->start();
	}
	else
		LPC_RITIMER->COUNTER = count;
}



void RiVoidTimer::addCallback(IVoidCallback* const callback)
{
	this->callback_ = callback;
}



void RiVoidTimer::deleteCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
}



void RiVoidTimer::deinitialize(void)
{
	IrqController::getIrqController()->deletePeripheralIrqListener(this);
	this->stop();
	NVIC_DisableIRQ(RITIMER_IRQn);
	Chip_RGU_TriggerReset(RGU_RITIMER_RST);
	while (Chip_RGU_InReset(RGU_RITIMER_RST)) {}
	Chip_RIT_DeInit(LPC_RITIMER);
	this->callback_ = (IVoidCallback*)NULL;
}

}
