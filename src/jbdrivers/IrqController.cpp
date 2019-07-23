/**
 * @file
 * @brief IRQ Controller Driver Realization
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

#include "jbdrivers/IrqController.hpp"

namespace jblib::jbdrivers
{

IrqController* IrqController::irqController_ = NULL;


IrqController* IrqController::getIrqController(void)
{
	if(IrqController::irqController_ == NULL)
		IrqController::irqController_ = new IrqController();
	return IrqController::irqController_;
}



IrqController::IrqController()
{
	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_CORTEX_LISTENERS; i++)
		this->cortexIrqListeners_[i] = (IIrqListener*)NULL;
	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS; i++)
		this->peripheralIrqListeners_[i] = (IIrqListener*)NULL;
	enableInterrupts();
}



void IrqController::addCortexIrqListener(IIrqListener* const listener)
{
	disableInterrupts();
	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_CORTEX_LISTENERS; i++) {
		if(this->cortexIrqListeners_[i] == listener)
			break;
		if(this->cortexIrqListeners_[i] == (IIrqListener*)NULL) {
			this->cortexIrqListeners_[i] = listener;
			break;
		}
	}
	enableInterrupts();
}



void IrqController::addPeripheralIrqListener(IIrqListener* const listener)
{
	disableInterrupts();
	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS; i++) {
		if(this->peripheralIrqListeners_[i] == listener)
			break;
		if(this->peripheralIrqListeners_[i] == (IIrqListener*)NULL) {
			this->peripheralIrqListeners_[i] = listener;
			break;
		}
	}
	enableInterrupts();
}



void IrqController::deleteCortexIrqListener(IIrqListener* const listener)
{
	uint32_t index = 0;
	disableInterrupts();
	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_CORTEX_LISTENERS; i++) {
		if(this->cortexIrqListeners_[i] == listener)
			break;
		else
			index++;
	}
	if(index == (IRQ_CONTROLLER_NUM_CORTEX_LISTENERS-1)){
		if(this->cortexIrqListeners_[index] == listener)
			this->cortexIrqListeners_[index] = (IIrqListener*)NULL;
	}
	else {
		for(uint32_t i = index; i < (IRQ_CONTROLLER_NUM_CORTEX_LISTENERS-1); i++) {
			this->cortexIrqListeners_[i] = this->cortexIrqListeners_[i+1];
			if(this->cortexIrqListeners_[i+1] == (IIrqListener*)NULL)
				break;
		}
	}
	enableInterrupts();
}



void IrqController::deletePeripheralIrqListener(IIrqListener* const listener)
{
	uint32_t index = 0;
	disableInterrupts();
	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS; i++) {
		if(this->peripheralIrqListeners_[i] == listener)
			break;
		else
			index++;
	}
	if(index == (IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS-1)) {
		if(this->peripheralIrqListeners_[index] == listener)
			this->peripheralIrqListeners_[index] = (IIrqListener*)NULL;
	}
	else {
		for(uint32_t i = index; i < (IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS-1); i++) {
			this->peripheralIrqListeners_[i] = this->peripheralIrqListeners_[i+1];
			if(this->peripheralIrqListeners_[i+1] == (IIrqListener*)NULL)
				break;
		}
	}
	enableInterrupts();
}



void IrqController::handleCortexIrq(const int8_t irqNumber)
{
	int8_t absIrqNumber = (irqNumber < 0) ? (-irqNumber) : irqNumber;

	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_CORTEX_LISTENERS; i++){
		if(this->cortexIrqListeners_[i]) {
			if( ((this->cortexIrqListeners_[i]->getCode()) >> absIrqNumber) & 1 )
				this->cortexIrqListeners_[i]->irqHandler(irqNumber);
		}
		else
			break;
	}
}



void IrqController::handlePeripheralIrq(const int8_t irqNumber)
{
	int8_t absIrqNumber = (irqNumber < 0) ? (-irqNumber) : irqNumber;

	for(uint32_t i = 0; i < IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS; i++) {
		if(this->peripheralIrqListeners_[i]) {
			if( ((this->peripheralIrqListeners_[i]->getCode()) >> absIrqNumber) & 1 )
				this->peripheralIrqListeners_[i]->irqHandler(irqNumber);
		}
		else
			break;
	}
}

}
