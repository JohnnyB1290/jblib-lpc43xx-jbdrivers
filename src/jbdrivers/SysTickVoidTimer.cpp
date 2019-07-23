/**
 * @file
 * @brief Void Timer on SysTick Timer Driver Realization
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

#include "jbdrivers/SysTickVoidTimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

SysTickVoidTimer* SysTickVoidTimer::sysTickVoidTimer_ = NULL;



SysTickVoidTimer* SysTickVoidTimer::getSysTickVoidTimer(void)
{
	if(sysTickVoidTimer_ == NULL)
		sysTickVoidTimer_ = new SysTickVoidTimer();
	return sysTickVoidTimer_;
}



SysTickVoidTimer::SysTickVoidTimer(void) : IVoidTimer(), IIrqListener()
{
	IrqController::getIrqController()->
			addIrqListener(this, SysTick_IRQn);
}



void SysTickVoidTimer::initialize(uint32_t us)
{
	#ifdef CORE_M4
	IrqController::getIrqController()->
			setPriority(SysTick_IRQn, SYS_TICK_INTERRUPT_PRIORITY);
	#endif
	SysTick->LOAD  = ((SystemCoreClock / 1000000) * us) - 1;
	SysTick->VAL   = 0;
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
}



void SysTickVoidTimer::start(void)
{
	SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk;
}



void SysTickVoidTimer::stop(void)
{
	SysTick->CTRL  &= (~SysTick_CTRL_ENABLE_Msk);
}



void SysTickVoidTimer::reset(void)
{
	if(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) {
		this->stop();
		SysTick->VAL = 0;
		this->start();
	}
	else
		SysTick->VAL = 0;
}



uint32_t SysTickVoidTimer::getCounter(void)
{
	return SysTick->VAL;
}



void SysTickVoidTimer::setCounter(uint32_t count)
{
	if(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) {
		this->stop();
		SysTick->VAL = count;
		this->start();
	}
	else
		SysTick->VAL = count;
}



void SysTickVoidTimer::addCallback(IVoidCallback* const callback)
{
	this->callback_ = callback;
}



void SysTickVoidTimer::deleteCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
}



void SysTickVoidTimer::deinitialize(void)
{
	this->stop();
	this->reset();
}



void SysTickVoidTimer::irqHandler(int irqNumber)
{
	if(this->callback_)
		this->callback_->voidCallback((void*)this, NULL);
}

}
