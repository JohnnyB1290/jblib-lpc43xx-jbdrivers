/**
 * @file
 * @brief Void Timer on PWM Timer Driver Realization
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

#include "jbdrivers/PwmVoidTimer.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

PwmVoidTimer* PwmVoidTimer::pwmVoidTimers_[PWM_TIMERS_NUM] = {
		(PwmVoidTimer*)NULL, (PwmVoidTimer*)NULL, (PwmVoidTimer*)NULL
};



PwmVoidTimer* PwmVoidTimer::getPwmVoidTimer(uint8_t number)
{
	if(number < PWM_TIMERS_NUM) {
		if(pwmVoidTimers_[number] == (PwmVoidTimer*)NULL)
			pwmVoidTimers_[number] = new PwmVoidTimer(number);
		return pwmVoidTimers_[number];
	}
	else
		return (PwmVoidTimer*)NULL;
}



PwmVoidTimer::PwmVoidTimer(uint8_t number) : IVoidTimer(), IIrqListener()
{
	this->number_ = number;
	IrqController::getIrqController()->addIrqListener(this, MCPWM_IRQn);
}



void PwmVoidTimer::irqHandler(int irqNumber)
{
	if((LPC_MCPWM->INTF) & (1 << (4 * (this->number_)))) {
		LPC_MCPWM->INTF_CLR = 1 << (4 * (this->number_));
		if(this->callback_)
			this->callback_->voidCallback((void*)this, NULL);
	}
}



void PwmVoidTimer::initialize(uint32_t us)
{
	Chip_Clock_EnableOpts(CLK_APB1_MOTOCON, true, true, 1);
	IrqController::getIrqController()->
			setPriority(MCPWM_IRQn, PWM_TIMER_INTERRUPT_PRIORITY);
	IrqController::getIrqController()->enableInterrupt(MCPWM_IRQn);
	uint32_t period = Chip_Clock_GetRate(CLK_APB1_MOTOCON) / 1000000;
	LPC_MCPWM->LIM[this->number_] = (period * us);
	LPC_MCPWM->INTEN_SET = 1 << ((this->number_) * 4);
}



void PwmVoidTimer::start(void)
{
	LPC_MCPWM->CON_SET = 1 << ((this->number_) * 8);
}



void PwmVoidTimer::stop(void)
{
	LPC_MCPWM->CON_CLR = 1 << ((this->number_) * 8);
}



void PwmVoidTimer::reset(void)
{
	if(LPC_MCPWM->CON & (1 << ((this->number_) * 8))) {
		this->stop();
		LPC_MCPWM->TC[this->number_] = 0;
		this->start();
	}
	else
		LPC_MCPWM->TC[this->number_] = 0;
}



uint32_t PwmVoidTimer::getCounter(void)
{
	return LPC_MCPWM->TC[this->number_];
}



void PwmVoidTimer::setCounter(uint32_t count)
{
	if(LPC_MCPWM->CON & (1 << ((this->number_) * 8))) {
		this->stop();
		LPC_MCPWM->TC[this->number_] = count;
		this->start();
	}
	else
		LPC_MCPWM->TC[this->number_] = count;
}



void PwmVoidTimer::addCallback(IVoidCallback* const callback)
{
	this->callback_ = callback;
}



void PwmVoidTimer::deleteCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
}



void PwmVoidTimer::deinitialize(void)
{
	this->stop();
	this->deleteCallback();

	uint32_t temp = 0;
	for(uint32_t i = 0; i < PWM_TIMERS_NUM; i++)
		temp |= LPC_MCPWM->INTEN & (1 << ((this->number_) * 4));

	if(temp == 0) {
		IrqController::getIrqController()->disableInterrupt(MCPWM_IRQn);
		Chip_RGU_TriggerReset(RGU_MOTOCONPWM_RST);
		while (Chip_RGU_InReset(RGU_MOTOCONPWM_RST)) {}
		Chip_Clock_Disable(CLK_APB1_MOTOCON);
	}
}

}
