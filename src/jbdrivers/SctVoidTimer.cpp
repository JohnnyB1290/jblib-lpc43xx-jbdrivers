/**
 * @file
 * @brief Void Timer on SCT Timer Driver Realization
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

#include "jbdrivers/SctVoidTimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

SctVoidTimer* SctVoidTimer::sctVoidTimer_ = NULL;



SctVoidTimer* SctVoidTimer::getSctVoidTimer(void)
{
	if(sctVoidTimer_ == NULL)
		sctVoidTimer_ = new SctVoidTimer();
	return sctVoidTimer_;
}



SctVoidTimer::SctVoidTimer(void) : IVoidTimer(), IIrqListener()
{
	IrqController::getIrqController()->addIrqListener(this, SCT_IRQn);
}



void SctVoidTimer::irqHandler(int irqNumber)
{
	if(LPC_SCT->EVFLAG & 1) {
		Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
		if(this->callback_)
			this->callback_->voidCallback((void*)this, NULL);
	}
	NVIC_ClearPendingIRQ(SCT_IRQn);
}



void SctVoidTimer::initialize(uint32_t us)
{
	Chip_SCT_Init(LPC_SCT);
	Chip_RGU_TriggerReset(RGU_SCT_RST);
	while (Chip_RGU_InReset(RGU_SCT_RST)) {}
	Chip_SCT_Config(LPC_SCT,
			SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_CLKMODE_BUSCLK |
			SCT_CONFIG_AUTOLIMIT_L | SCT_CONFIG_NORELOADL_U);
	uint32_t period = (Chip_Clock_GetRate(CLK_MX_SCT) / 1000000);
	period = period * us;
	Chip_SCT_SetMatchCount(LPC_SCT, SCT_MATCH_0, period);
	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_0);
	LPC_SCT->EVENT[0].CTRL = 1|(1 << 12); //EVENT0 == MATCH0, use match only
	LPC_SCT->EVENT[0].STATE = 1; //EVENT0 in STATE0
	IrqController::getIrqController()->
			setPriority(SCT_IRQn, SCT_TIMER_INTERRUPT_PRIORITY);
	IrqController::getIrqController()->enableInterrupt(SCT_IRQn);
}



void SctVoidTimer::start(void)
{
	Chip_SCT_ClearControl(LPC_SCT, SCT_CTRL_HALT_L);
}



void SctVoidTimer::stop(void)
{
	Chip_SCT_SetControl(LPC_SCT, SCT_CTRL_HALT_L);
}



void SctVoidTimer::reset(void)
{
	if(LPC_SCT->CTRL_U & SCT_CTRL_HALT_L)
		Chip_SCT_SetControl(LPC_SCT, SCT_CTRL_CLRCTR_L);
	else {
		this->stop();
		Chip_SCT_SetControl(LPC_SCT, SCT_CTRL_CLRCTR_L);
		this->start();
	}
}



uint32_t SctVoidTimer::getCounter(void)
{
	return LPC_SCT->COUNT_U;
}



void SctVoidTimer::setCounter(uint32_t count)
{
	if(LPC_SCT->CTRL_U & SCT_CTRL_HALT_L)
		Chip_SCT_SetCount(LPC_SCT, count);
	else {
		this->stop();
		Chip_SCT_SetCount(LPC_SCT, count);
		this->start();
	}
}



void SctVoidTimer::addCallback(IVoidCallback* const callback)
{
	this->callback_ = callback;
}



void SctVoidTimer::deleteCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
	Chip_SCT_DisableEventInt(LPC_SCT, SCT_EVT_0);
	LPC_SCT->EVENT[0].CTRL = 0;
	LPC_SCT->EVENT[0].STATE = 0;
}



void SctVoidTimer::deinitialize(void)
{
	this->stop();
	IrqController::getIrqController()->
			disableInterrupt(SCT_IRQn);
	Chip_RGU_TriggerReset(RGU_SCT_RST);
	while (Chip_RGU_InReset(RGU_SCT_RST)) {}
	Chip_SCT_DeInit(LPC_SCT);
}

}
