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
	if(irqController_ == NULL)
		irqController_ = new IrqController();
	return irqController_;
}



IrqController::IrqController(void)
{

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
	NVIC_SetPriority((IRQn_Type)irqNumber,
			NVIC_EncodePriority(prioritygroup, priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority((IRQn_Type)irqNumber, priority);
	#endif
}



void IrqController::addIrqListener(IIrqListener* const listener, int irqNumber)
{
	ListenersListItem newItem;
	newItem.listener = listener;
	newItem.irqNumber = irqNumber;
	this->listenersList_.push_front(newItem);
}



void IrqController::deleteIrqListener(IIrqListener* const listener)
{
	ListenersListItem newItem;
	newItem.listener = listener;
	this->listenersDeleteList_.push_front(newItem);
}



void IrqController::deleteIrqListener(ListenersListItem& listenerItem)
{
	this->listenersList_.remove_if([listenerItem](ListenersListItem item){
		if(listenerItem.listener == item.listener)
			return true;
		else
			 return false;
	});
}



void IrqController::handleIrq(const int irqNumber)
{
	for(std::forward_list<ListenersListItem>::iterator it = this->listenersList_.begin();
			it != this->listenersList_.end(); ++it){
		if(it->irqNumber == irqNumber)
			it->listener->irqHandler(irqNumber);
	}
	if(!this->listenersDeleteList_.empty()){
		for(std::forward_list<ListenersListItem>::iterator it = this->listenersDeleteList_.begin();
				it != this->listenersDeleteList_.end(); ++it){
			this->deleteIrqListener(*it);
		}
		this->listenersDeleteList_.clear();
	}
}

}
