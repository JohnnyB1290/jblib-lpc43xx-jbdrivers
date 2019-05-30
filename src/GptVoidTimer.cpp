/**
 * @file
 * @brief Void Timer on GP Timer Driver Realization
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

#include "GptVoidTimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

#ifdef CORE_M4
LPC_TIMER_T* GptVoidTimer::lpcTimers_[GPT_TIMERS_NUM] = {
		LPC_TIMER0, LPC_TIMER1,
		LPC_TIMER2, LPC_TIMER3
};
const CHIP_RGU_RST_T GptVoidTimer::resetNumbers_[GPT_TIMERS_NUM] = {
		RGU_TIMER0_RST, RGU_TIMER1_RST,
		RGU_TIMER2_RST, RGU_TIMER3_RST
};
const CHIP_CCU_CLK_T GptVoidTimer::clockNumbers_[GPT_TIMERS_NUM] = {
		CLK_MX_TIMER0, CLK_MX_TIMER1,
		CLK_MX_TIMER2, CLK_MX_TIMER3
};
const IRQn_Type GptVoidTimer::irqNumbers_[GPT_TIMERS_NUM] = {
		TIMER0_IRQn, TIMER1_IRQn,
		TIMER2_IRQn, TIMER3_IRQn
};
const uint32_t GptVoidTimer::interruptPriorities_[GPT_TIMERS_NUM] = {
		GP_TIMER_0_INTERRUPT_PRIORITY, GP_TIMER_1_INTERRUPT_PRIORITY,
		GP_TIMER_2_INTERRUPT_PRIORITY, GP_TIMER_3_INTERRUPT_PRIORITY};
#endif

#ifdef CORE_M0

#ifdef CORE_M0APP
LPC_TIMER_T* GptVoidTimer::lpcTimers_[GPT_TIMERS_NUM] = {
		LPC_TIMER0, LPC_TIMER0,
		LPC_TIMER3, LPC_TIMER3
};
const CHIP_RGU_RST_T GptVoidTimer::resetNumbers_[GPT_TIMERS_NUM] = {
		RGU_TIMER0_RST, RGU_TIMER0_RST,
		RGU_TIMER3_RST, RGU_TIMER3_RST
};
const CHIP_CCU_CLK_T GptVoidTimer::clockNumbers_[GPT_TIMERS_NUM] = {
		CLK_MX_TIMER0, CLK_MX_TIMER0,
		CLK_MX_TIMER3, CLK_MX_TIMER3
};
const IRQn_Type GptVoidTimer::irqNumbers_[GPT_TIMERS_NUM] = {
		TIMER0_IRQn, TIMER0_IRQn,
		TIMER3_IRQn, TIMER3_IRQn
};
const uint32_t GptVoidTimer::interruptPriorities_[GPT_TIMERS_NUM] = {
		GP_TIMER_0_INTERRUPT_PRIORITY, GP_TIMER_0_INTERRUPT_PRIORITY,
		GP_TIMER_3_INTERRUPT_PRIORITY, GP_TIMER_3_INTERRUPT_PRIORITY
};
#endif

#ifdef CORE_M0SUB
LPC_TIMER_T* GptVoidTimer::lpcTimers_[GPT_TIMERS_NUM] = {
		LPC_TIMER1, LPC_TIMER1,
		LPC_TIMER2, LPC_TIMER2
};
const CHIP_RGU_RST_T GptVoidTimer::resetNumbers_[GPT_TIMERS_NUM] = {
		RGU_TIMER1_RST, RGU_TIMER1_RST,
		RGU_TIMER2_RST, RGU_TIMER2_RST
};
const CHIP_CCU_CLK_T GptVoidTimer::clockNumbers_[GPT_TIMERS_NUM] = {
		CLK_MX_TIMER1, CLK_MX_TIMER1,
		CLK_MX_TIMER2, CLK_MX_TIMER2
};
const IRQn_Type GptVoidTimer::irqNumbers_[GPT_TIMERS_NUM] = {
		TIMER1_IRQn, TIMER1_IRQn,
		TIMER2_IRQn, TIMER2_IRQn
};
const uint32_t GptVoidTimer::interruptPriorities_[GPT_TIMERS_NUM] = {
		GP_TIMER_1_INTERRUPT_PRIORITY, GP_TIMER_1_INTERRUPT_PRIORITY,
		GP_TIMER_2_INTERRUPT_PRIORITY, GP_TIMER_2_INTERRUPT_PRIORITY
};
#endif

#endif

GptVoidTimer* GptVoidTimer::gptVoidTimers_[GPT_TIMERS_NUM] = {NULL, NULL, NULL, NULL};



GptVoidTimer* GptVoidTimer::getGptVoidTimer(uint8_t timerNumber)
{
	if(timerNumber < GPT_TIMERS_NUM) {
#ifdef CORE_M0
#ifdef CORE_M0APP
		if((timerNumber !=0 )&&(timerNumber != 3))
			return (GptVoidTimer*)NULL;
#endif
#ifdef CORE_M0SUB
		if((timerNumber != 1)&&(timerNumber != 2))
			return (GptVoidTimer*)NULL;
#endif
#endif
		if(gptVoidTimers_[timerNumber] == (GptVoidTimer*)NULL)
			gptVoidTimers_[timerNumber] = new GptVoidTimer(timerNumber);
		return gptVoidTimers_[timerNumber];
	}
	return (GptVoidTimer*)NULL;
}



GptVoidTimer::GptVoidTimer(uint8_t timerNumber) : IVoidTimer(), IIrqListener()
{
	this->timerNumber_ = timerNumber;
	this->setCode((uint64_t)1 << (irqNumbers_[this->timerNumber_]));
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



void GptVoidTimer::irqHandler(int8_t irqNumber)
{
	if (Chip_TIMER_MatchPending(lpcTimers_[this->timerNumber_], 0)) {
		Chip_TIMER_ClearMatch(lpcTimers_[this->timerNumber_], 0);
		if(this->callback_)
			this->callback_->voidCallback((void*)this, NULL);
	}
}



void GptVoidTimer::initialize(uint32_t us)
{
	Chip_TIMER_Init(lpcTimers_[this->timerNumber_]);
	Chip_RGU_TriggerReset(resetNumbers_[this->timerNumber_]);
	while (Chip_RGU_InReset(resetNumbers_[this->timerNumber_])) {}
	Chip_TIMER_Reset(lpcTimers_[this->timerNumber_]);

	#ifdef CORE_M0
	NVIC_SetPriority(irqNumbers_[this->timerNumber_], interruptPriorities_[this->timerNumber_]);
	#endif
	#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(irqNumbers_[this->timerNumber_],
			NVIC_EncodePriority(prioritygroup, interruptPriorities_[this->timerNumber_], 0));
	#endif
	uint32_t tics = (Chip_Clock_GetRate(clockNumbers_[this->timerNumber_]) / 1000000) * us;
	Chip_TIMER_SetMatch(lpcTimers_[this->timerNumber_], 0, tics);
	Chip_TIMER_ResetOnMatchEnable(lpcTimers_[this->timerNumber_], 0);
	Chip_TIMER_MatchEnableInt(lpcTimers_[this->timerNumber_], 0);

	NVIC_ClearPendingIRQ(irqNumbers_[this->timerNumber_]);
	NVIC_EnableIRQ(irqNumbers_[this->timerNumber_]);
}



void GptVoidTimer::start(void)
{
	Chip_TIMER_Enable(lpcTimers_[this->timerNumber_]);
}



void GptVoidTimer::stop(void)
{
	Chip_TIMER_Disable(lpcTimers_[this->timerNumber_]);
}



void GptVoidTimer::reset(void)
{
	Chip_TIMER_Reset(lpcTimers_[this->timerNumber_]);
}



uint32_t GptVoidTimer::getCounter(void)
{
	return lpcTimers_[this->timerNumber_]->TC;
}



void GptVoidTimer::setCounter(uint32_t count)
{
	lpcTimers_[this->timerNumber_]->TC = count;
}



void GptVoidTimer::addCallback(IVoidCallback* const callback)
{
	this->callback_ = callback;
}



void GptVoidTimer::deleteCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
}



void GptVoidTimer::deinitialize(void)
{
	this->stop();
	NVIC_DisableIRQ(irqNumbers_[this->timerNumber_]);
	Chip_RGU_TriggerReset(resetNumbers_[this->timerNumber_]);
	while (Chip_RGU_InReset(resetNumbers_[this->timerNumber_])) {}
	Chip_TIMER_DeInit(lpcTimers_[this->timerNumber_]);
}

}
