/**
 * @file
 * @brief Group Pin Interrupt Driver Realization
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

#include "jbdrivers/GroupPinInterrupt.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

#ifdef CORE_M4
const IRQn_Type GroupPinInterrupt::groupPinInterruptIrqNs_[] = {GINT0_IRQn, GINT1_IRQn};
const uint32_t GroupPinInterrupt::interruptPriorities_[] = {
		GROUP_PIN_INTERRUPT_0_INTERRUPT_PRIORITY,
		GROUP_PIN_INTERRUPT_1_INTERRUPT_PRIORITY
};
#endif

#ifdef CORE_M0
const IRQn_Type GroupPinInterrupt::groupPinInterruptIrqNs_[] = {GINT1_IRQn,GINT1_IRQn};
const uint32_t GroupPinInterrupt::interruptPriorities_[] = {
		GROUP_PIN_INTERRUPT_1_INTERRUPT_PRIORITY,
		GROUP_PIN_INTERRUPT_1_INTERRUPT_PRIORITY
};
#endif

GroupPinInterrupt* GroupPinInterrupt::groupPinInterrupts_[GROUP_PIN_INTERRUPT_NUM_GROUPS] = {
		(GroupPinInterrupt*)NULL, (GroupPinInterrupt*)NULL
};



GroupPinInterrupt* GroupPinInterrupt::getGroupPinInterrupt(uint8_t groupNumber)
{
	#ifdef CORE_M0
	if(groupNumber != 1)
		return (GroupPinInterrupt*)NULL;
	#endif
	if(groupNumber < GROUP_PIN_INTERRUPT_NUM_GROUPS) {
		if(groupPinInterrupts_[groupNumber] == (GroupPinInterrupt*)NULL)
			groupPinInterrupts_[groupNumber] = new GroupPinInterrupt(groupNumber);
		return groupPinInterrupts_[groupNumber];
	}
	else
		return (GroupPinInterrupt*)NULL;
}



GroupPinInterrupt::GroupPinInterrupt(uint8_t groupNumber) : IIrqListener()
{
	this->groupNumber_ = groupNumber;
	this->setCode((uint64_t)1 << (groupPinInterruptIrqNs_[this->groupNumber_]));
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



void GroupPinInterrupt::initialize(uint8_t mode)
{
	if((mode & 1) == GROUP_PIN_MODE_OR)
		Chip_GPIOGP_SelectOrMode(LPC_GPIOGROUP, this->groupNumber_);
	else
		Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUP, this->groupNumber_);
	if((mode & 2) == GROUP_PIN_MODE_EDGE)
		Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUP, this->groupNumber_);
	else
		Chip_GPIOGP_SelectLevelMode(LPC_GPIOGROUP, this->groupNumber_);

#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(groupPinInterruptIrqNs_[this->groupNumber_],
			NVIC_EncodePriority(prioritygroup, interruptPriorities_[this->groupNumber_], 0));
#endif
#ifdef CORE_M0
	NVIC_SetPriority(groupPinInterruptIrqNs_[this->groupNumber_],
			GroupPinInterrupt::interruptPriorities_[this->groupNumber_]);
#endif
}



void GroupPinInterrupt::addGpio(uint8_t port, uint8_t pin,
		uint8_t gpioPort, uint8_t gpioPin,
		uint16_t scuMode, GroupPinInterruptSense_t sense)
{
	Chip_SCU_PinMuxSet(port, pin, (SCU_MODE_INBUFF_EN |scuMode));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpioPort, gpioPin);
	switch (sense)
	{
		case GPOUP_PIN_SENSE_LOW_LEVEL:
			Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP, this->groupNumber_, gpioPort, 1 << gpioPin);
			break;
		case GPOUP_PIN_SENSE_HI_LEVEL:
			Chip_GPIOGP_SelectHighLevel(LPC_GPIOGROUP, this->groupNumber_, gpioPort, 1 << gpioPin);
			break;
	}
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP, this->groupNumber_, gpioPort, 1 << gpioPin);
}



void GroupPinInterrupt::deleteGpio(uint8_t gpioPort, uint8_t gpioPin)
{
	Chip_GPIOGP_DisableGroupPins(LPC_GPIOGROUP,
			this->groupNumber_, gpioPort, 1 << gpioPin);
}



void GroupPinInterrupt::irqHandler(int8_t irqNumber)
{
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, this->groupNumber_);
	if(this->callback_)
		this->callback_->voidCallback((void*)this, NULL);
}



void GroupPinInterrupt::setCallback(IVoidCallback* callback)
{
	this->callback_ = callback;
}



void GroupPinInterrupt::clearCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
}



void GroupPinInterrupt::enableInterrupt(void)
{
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, this->groupNumber_);
	NVIC_ClearPendingIRQ(groupPinInterruptIrqNs_[this->groupNumber_]);
	NVIC_EnableIRQ(groupPinInterruptIrqNs_[this->groupNumber_]);
}



void GroupPinInterrupt::disableInterrupt(void)
{
	NVIC_DisableIRQ(groupPinInterruptIrqNs_[this->groupNumber_]);
}



void GroupPinInterrupt::deinitialize(void)
{
	NVIC_DisableIRQ(groupPinInterruptIrqNs_[this->groupNumber_]);
	NVIC_ClearPendingIRQ(groupPinInterruptIrqNs_[this->groupNumber_]);
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, this->groupNumber_);
	this->callback_ = (IVoidCallback*)NULL;
}

}
