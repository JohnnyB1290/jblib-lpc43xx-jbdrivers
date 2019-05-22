/**
 * @file
 * @brief Real Time Timer on GP Timer Driver Realization
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

#include "GptRtTimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

#ifdef CORE_M4
LPC_TIMER_T* GptRtTimer::lpcTimers_[GPT_TIMERS_NUM] = {
		LPC_TIMER0, LPC_TIMER1,
		LPC_TIMER2, LPC_TIMER3
};
const CHIP_RGU_RST_T GptRtTimer::resetNumbers_[GPT_TIMERS_NUM] = {
		RGU_TIMER0_RST, RGU_TIMER1_RST,
		RGU_TIMER2_RST, RGU_TIMER3_RST
};
const CHIP_CCU_CLK_T GptRtTimer::clockNumbers_[GPT_TIMERS_NUM] = {
		CLK_MX_TIMER0, CLK_MX_TIMER1,
		CLK_MX_TIMER2, CLK_MX_TIMER3
};
const IRQn_Type GptRtTimer::irqNumbers_[GPT_TIMERS_NUM] = {
		TIMER0_IRQn, TIMER1_IRQn,
		TIMER2_IRQn, TIMER3_IRQn
};
const uint32_t GptRtTimer::interruptPriorities_[GPT_TIMERS_NUM] = {
		GP_TIMER_0_INTERRUPT_PRIORITY, GP_TIMER_1_INTERRUPT_PRIORITY,
		GP_TIMER_2_INTERRUPT_PRIORITY, GP_TIMER_3_INTERRUPT_PRIORITY};
#endif

#ifdef CORE_M0

#ifdef CORE_M0APP
LPC_TIMER_T* GptRtTimer::lpcTimers_[GPT_TIMERS_NUM] = {
		LPC_TIMER0, LPC_TIMER0,
		LPC_TIMER3, LPC_TIMER3
};
const CHIP_RGU_RST_T GptRtTimer::resetNumbers_[GPT_TIMERS_NUM] = {
		RGU_TIMER0_RST, RGU_TIMER0_RST,
		RGU_TIMER3_RST, RGU_TIMER3_RST
};
const CHIP_CCU_CLK_T GptRtTimer::clockNumbers_[GPT_TIMERS_NUM] = {
		CLK_MX_TIMER0, CLK_MX_TIMER0,
		CLK_MX_TIMER3, CLK_MX_TIMER3
};
const IRQn_Type GptRtTimer::irqNumbers_[GPT_TIMERS_NUM] = {
		TIMER0_IRQn, TIMER0_IRQn,
		TIMER3_IRQn, TIMER3_IRQn
};
const uint32_t GptRtTimer::interruptPriorities_[GPT_TIMERS_NUM] = {
		GP_TIMER_0_INTERRUPT_PRIORITY, GP_TIMER_0_INTERRUPT_PRIORITY,
		GP_TIMER_3_INTERRUPT_PRIORITY, GP_TIMER_3_INTERRUPT_PRIORITY};
#endif

#ifdef CORE_M0SUB
LPC_TIMER_T* GptRtTimer::lpcTimers_[GPT_TIMERS_NUM] = {
		LPC_TIMER1, LPC_TIMER1,
		LPC_TIMER2, LPC_TIMER2
};
const CHIP_RGU_RST_T GptRtTimer::resetNumbers_[GPT_TIMERS_NUM] = {
		RGU_TIMER1_RST, RGU_TIMER1_RST,
		RGU_TIMER2_RST, RGU_TIMER2_RST
};
const CHIP_CCU_CLK_T GptRtTimer::clockNumbers_[GPT_TIMERS_NUM] = {
		CLK_MX_TIMER1, CLK_MX_TIMER1,
		CLK_MX_TIMER2, CLK_MX_TIMER2
};
const IRQn_Type GptRtTimer::irqNumbers_[GPT_TIMERS_NUM] = {
		TIMER1_IRQn, TIMER1_IRQn,
		TIMER2_IRQn, TIMER2_IRQn
};
const uint32_t GptRtTimer::interruptPriorities_[GPT_TIMERS_NUM] = {
		GP_TIMER_1_INTERRUPT_PRIORITY, GP_TIMER_1_INTERRUPT_PRIORITY,
		GP_TIMER_2_INTERRUPT_PRIORITY, GP_TIMER_2_INTERRUPT_PRIORITY
};
#endif

#endif

GptRtTimer* GptRtTimer::gptRtTimers_[GPT_TIMERS_NUM] = {
		(GptRtTimer*)NULL, (GptRtTimer*)NULL,
		(GptRtTimer*)NULL, (GptRtTimer*)NULL
};



GptRtTimer* GptRtTimer::getGptRtTimer(uint8_t timerNumber)
{
	if(timerNumber < GPT_TIMERS_NUM) {
#ifdef CORE_M0
#ifdef CORE_M0APP
		if((timerNumber != 0)&&(timerNumber != 3))
			return (GptRtTimer*)NULL;
#endif
#ifdef CORE_M0SUB
		if((timerNumber != 1)&&(timerNumber != 2))
			return (GptRtTimer*)NULL;
#endif
#endif
		if(gptRtTimers_[timerNumber] == (GptRtTimer*)NULL)
			gptRtTimers_[timerNumber] = new GptRtTimer(timerNumber);
		return gptRtTimers_[timerNumber];
	}
	else
		return (GptRtTimer*)NULL;
}




GptRtTimer::GptRtTimer(uint8_t timerNumber) : IRtTimer(), IIrqListener()
{
	this->timerNumber_ = timerNumber;
	for(uint32_t i = 0; i < GPT_TIMERS_NUM_MATCHES; i++)
		this->matchCallbacks_[i] = (IVoidCallback*)NULL;
	this->setCode((uint64_t)1 << (this->irqNumbers_[this->timerNumber_]));
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



void GptRtTimer::irqHandler(int8_t irqNumber)
{
	static uint8_t matchNumber;
	for(uint32_t i = 0; i< GPT_TIMERS_NUM_MATCHES; i++) {
		matchNumber = i;
		if (Chip_TIMER_MatchPending(this->lpcTimers_[this->timerNumber_], matchNumber)) {
			Chip_TIMER_ClearMatch(this->lpcTimers_[this->timerNumber_], matchNumber);
			if(this->matchCallbacks_[matchNumber])
				this->matchCallbacks_[matchNumber]->voidCallback((void*)this, (void*)&matchNumber);
		}
	}
}



void GptRtTimer::initialize(void)
{
	Chip_TIMER_Init(this->lpcTimers_[this->timerNumber_]);
	Chip_RGU_TriggerReset(this->resetNumbers_[this->timerNumber_]);
	while (Chip_RGU_InReset(this->resetNumbers_[this->timerNumber_])) {}
	Chip_TIMER_Reset(this->lpcTimers_[this->timerNumber_]);

	#ifdef CORE_M0
	NVIC_SetPriority(this->irqNumbers_[this->timerNumber_], this->interruptPriorities_[this->timerNumber_]);
	#endif
	#ifdef CORE_M4
	uint32_t priorityGroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(this->irqNumbers_[this->timerNumber_],
			NVIC_EncodePriority(priorityGroup, this->interruptPriorities_[this->timerNumber_], 0));
	#endif
	NVIC_ClearPendingIRQ(this->irqNumbers_[this->timerNumber_]);
	NVIC_EnableIRQ(this->irqNumbers_[this->timerNumber_]);
}



void GptRtTimer::start(void)
{
	Chip_TIMER_Enable(this->lpcTimers_[this->timerNumber_]);
}



void GptRtTimer::stop(void)
{
	Chip_TIMER_Disable(this->lpcTimers_[this->timerNumber_]);
}



void GptRtTimer::reset(void)
{
	Chip_TIMER_Reset(this->lpcTimers_[this->timerNumber_]);
}



void GptRtTimer::addMatchUs(const uint8_t matchNumber,
		uint32_t us, bool isResetOnMatch)
{
	uint32_t tics = (Chip_Clock_GetRate(this->clockNumbers_[this->timerNumber_]) / 1000000) * us;
	Chip_TIMER_SetMatch(this->lpcTimers_[this->timerNumber_], matchNumber, tics);
	if(isResetOnMatch)
		Chip_TIMER_ResetOnMatchEnable(this->lpcTimers_[this->timerNumber_], matchNumber);
	Chip_TIMER_MatchEnableInt(this->lpcTimers_[this->timerNumber_], matchNumber);
}



void GptRtTimer::addMatchTicks(const uint8_t matchNumber,
		uint32_t ticks, bool isResetOnMatch)
{
	Chip_TIMER_SetMatch(this->lpcTimers_[this->timerNumber_], matchNumber, ticks);
	if(isResetOnMatch)
		Chip_TIMER_ResetOnMatchEnable(this->lpcTimers_[this->timerNumber_], matchNumber);
	Chip_TIMER_MatchEnableInt(this->lpcTimers_[this->timerNumber_], matchNumber);
}



void GptRtTimer::deleteMatch(const uint8_t matchNumber)
{
	Chip_TIMER_MatchDisableInt(this->lpcTimers_[this->timerNumber_], matchNumber);
	Chip_TIMER_ResetOnMatchDisable(this->lpcTimers_[this->timerNumber_], matchNumber);
}



void GptRtTimer::setMatchCallback(const uint8_t matchNumber,
		IVoidCallback* const matchCallback)
{
	this->matchCallbacks_[matchNumber] = matchCallback;
}



void GptRtTimer::deleteMatchCallback(const uint8_t matchNumber)
{
	this->matchCallbacks_[matchNumber] = (IVoidCallback*)NULL;
}



void GptRtTimer::deinitialize(void)
{
	this->stop();
	NVIC_DisableIRQ(this->irqNumbers_[this->timerNumber_]);
	Chip_RGU_TriggerReset(this->resetNumbers_[this->timerNumber_]);
	while (Chip_RGU_InReset(this->resetNumbers_[this->timerNumber_])) {}
	Chip_TIMER_DeInit(this->lpcTimers_[this->timerNumber_]);
}



uint32_t GptRtTimer::getCounter(void)
{
	return this->lpcTimers_[this->timerNumber_]->TC;
}



void GptRtTimer::setCounter(uint32_t count)
{
	this->lpcTimers_[this->timerNumber_]->TC = count;
}

}
