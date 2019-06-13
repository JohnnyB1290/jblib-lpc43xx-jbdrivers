/**
 * @file
 * @brief Pin interrupt Driver Realization
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

#include "jbdrivers/PinInterrupt.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

#ifdef CORE_M4
const IRQn_Type PinInterrupt::pinInterruptIrqNs_[] = {PIN_INT0_IRQn, PIN_INT1_IRQn, PIN_INT2_IRQn, PIN_INT3_IRQn,
		PIN_INT4_IRQn, PIN_INT5_IRQn, PIN_INT6_IRQn, PIN_INT7_IRQn};
#endif

#ifdef CORE_M0

#ifdef CORE_M0APP
const IRQn_Type PinInterrupt::pinInterruptIrqNs_[] = {PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn,
		PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn};
#endif

#ifdef CORE_M0SUB
const IRQn_Type PinInterrupt::pinInterruptIrqNs_[] = {PIN_INT5_IRQn, PIN_INT5_IRQn, PIN_INT5_IRQn, PIN_INT5_IRQn,
		PIN_INT5_IRQn, PIN_INT5_IRQn, PIN_INT5_IRQn, PIN_INT5_IRQn};
#endif

#endif

PinInterrupt* PinInterrupt::pinInterrupts_[PIN_INTERRUPT_NUM_CHANNELS] = {NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL};
const uint32_t PinInterrupt::interruptPriorities_[] = {
		PIN_INTERRUPT_0_INTERRUPT_PRIORITY, PIN_INTERRUPT_1_INTERRUPT_PRIORITY,
		PIN_INTERRUPT_2_INTERRUPT_PRIORITY, PIN_INTERRUPT_3_INTERRUPT_PRIORITY,
		PIN_INTERRUPT_4_INTERRUPT_PRIORITY, PIN_INTERRUPT_5_INTERRUPT_PRIORITY,
		PIN_INTERRUPT_6_INTERRUPT_PRIORITY, PIN_INTERRUPT_7_INTERRUPT_PRIORITY
};



PinInterrupt* PinInterrupt::getPinInterrupt(uint8_t number)
{
#ifdef CORE_M0
#ifdef CORE_M0APP
	if(number != 4)
		return (PinInterrupt*)NULL;
#endif
#ifdef CORE_M0SUB
	if(number != 5)
		return (PinInterrupt*)NULL;
#endif
#endif
	if(number < PIN_INTERRUPT_NUM_CHANNELS) {
		if(pinInterrupts_[number] == (PinInterrupt*)NULL)
			pinInterrupts_[number] = new PinInterrupt(number);
		return pinInterrupts_[number];
	}
	else
		return (PinInterrupt*)NULL;
}



PinInterrupt::PinInterrupt(uint8_t number) : IIrqListener()
{
	this->number_ = number;
	this->callback_ = (IVoidCallback*)NULL;
	this->setCode((uint64_t)1<<(pinInterruptIrqNs_[this->number_]));
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



void PinInterrupt::initialize(const uint8_t port, const uint8_t pin,
			const uint8_t gpioPort, const uint8_t gpioPin,
			const uint16_t scuMode, const PinInterruptSense_t sense)
{
	Chip_SCU_PinMuxSet(port, pin, SCU_MODE_INBUFF_EN | scuMode );
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpioPort, gpioPin);
	Chip_SCU_GPIOIntPinSel(this->number_, gpioPort, gpioPin);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->number_));

	switch (sense) {
		case SENSE_LOW_EDGE:
		{
			Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->number_));
		}
			break;
		case SENSE_LOW_LEVEL:
		{
			Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->number_));
		}
			break;
		case SENSE_HI_EDGE:
		{
			Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->number_));
		}
			break;
		case SENSE_HI_LEVEL:
		{
			Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->number_));
		}
			break;
		case SENSE_BOTH_LEVEL:
		{
			Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->number_));
		}
			break;
		case SENSE_BOTH_EDGE:
		{
			Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->number_));
			Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->number_));
		}
			break;
	}

	#ifdef CORE_M0
	NVIC_SetPriority(pinInterruptIrqNs_[this->number_], interruptPriorities_[this->number_]);
	#endif
	#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(pinInterruptIrqNs_[this->number_],
			NVIC_EncodePriority(prioritygroup, interruptPriorities_[this->number_], 0));
	#endif
}



void PinInterrupt::addCallback(IVoidCallback* const callback)
{
	this->callback_ = callback;
}



void PinInterrupt::deleteCallback(void)
{
	this->callback_ = (IVoidCallback*)NULL;
}



void PinInterrupt::enableInterrupt(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->number_));
	NVIC_ClearPendingIRQ(pinInterruptIrqNs_[this->number_]);
	NVIC_EnableIRQ(pinInterruptIrqNs_[this->number_]);
}



void PinInterrupt::disableInterrupt(void)
{
	NVIC_DisableIRQ(this->pinInterruptIrqNs_[this->number_]);
}



void PinInterrupt::deinitialize(void)
{
	NVIC_DisableIRQ(pinInterruptIrqNs_[this->number_]);
	NVIC_ClearPendingIRQ(pinInterruptIrqNs_[this->number_]);
	Chip_PININT_DisableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->number_));
	Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->number_));
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->number_));
	this->callback_ = (IVoidCallback*)NULL;
}



void PinInterrupt::irqHandler(int8_t irqNumber)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->number_));
	if(this->callback_ != (IVoidCallback*)NULL)
		this->callback_->voidCallback((void*)this, NULL);
}

}
