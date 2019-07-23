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

using namespace jbutilities;

IrqController* IrqController::irqController_ = NULL;


IrqController* IrqController::getIrqController(void)
{
	if(irqController_ == NULL)
		irqController_ = new IrqController();
	return irqController_;
}



IrqController::IrqController(void)
{
	this->listenersList_ = new LinkedList<ListenersListItem>();
}



void IrqController::enableInterrupt(int irqNumber)
{
	NVIC_ClearPendingIRQ((IRQn_Type)irqNumber);
	NVIC_EnableIRQ((IRQn_Type)irqNumber);
}



void IrqController::disableInterrupt(int irqNumber)
{
	NVIC_DisableIRQ((IRQn_Type)irqNumber);
	NVIC_ClearPendingIRQ((IRQn_Type)irqNumber);
}



void IrqController::setPriority(int irqNumber, uint32_t priority)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(irqNumber,
			NVIC_EncodePriority(prioritygroup, priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority((IRQn_Type)irqNumber, priority);
	#endif
}



void IrqController::addIrqListener(IIrqListener* const listener, int irqNumber)
{
	ListenersListItem* newItem = new ListenersListItem();
	newItem->listener = listener;
	newItem->irqNumber = irqNumber;
	this->listenersList_->insertLast(newItem);
}



void IrqController::deleteIrqListener(IIrqListener* const listener)
{
	if(!this->listenersList_->isEmpty()){
		LinkedList<ListenersListItem>::LinkIterator* iterator =
				this->listenersList_->getIterator();
		iterator->reset();
		while(1){
			if(iterator->getCurrent()->listener == listener)
				free_s(iterator->deleteCurrent());
			if(!iterator->atEnd())
				iterator->nextLink();
			else
				break;
		}
	}
}



void IrqController::handleIrq(const int irqNumber)
{
	if(!this->listenersList_->isEmpty()){
		LinkedList<ListenersListItem>::LinkIterator* iterator =
				this->listenersList_->getIterator();
		iterator->reset();
		while(1){
			if(iterator->getCurrent()->irqNumber == irqNumber)
				iterator->getCurrent()->listener->irqHandler(irqNumber);
			if(!iterator->atEnd())
				iterator->nextLink();
			else
				break;
		}
	}
}

}
