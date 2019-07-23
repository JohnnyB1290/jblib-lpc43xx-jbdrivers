/**
 * @file
 * @brief Inter-Processor Communication Driver Realization
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

#include <string.h>
#include "jbdrivers/Ipc.hpp"

#define HEAD_INDEX(q)                ((q)->head & ((q)->size - 1))
#define TAIL_INDEX(q)               ((q)->tail & ((q)->size - 1))

namespace jblib::jbdrivers
{

using namespace jbkernel;

typedef struct
{
	uint32_t index = 0;
	uint32_t value = 0;
}IpcGlobalValue_t;


Ipc* Ipc::ipcs_[3] = {(Ipc*)NULL, (Ipc*)NULL, (Ipc*)NULL};



Ipc* Ipc::getIpc(uint8_t gate)
{
	#ifdef CORE_M4
		if(gate == GATE_CORE_M4)
			return (Ipc*)NULL;
	#endif
	#ifdef CORE_M0APP
		if(gate == GATE_CORE_M0APP)
			return (Ipc*)NULL;
	#endif
	#ifdef CORE_M0SUB
		if(gate == GATE_CORE_M0SUB)
			return (Ipc*)NULL;
	#endif

	if(ipcs_[gate] == (Ipc*)NULL)
		ipcs_[gate] = new Ipc(gate);
	return ipcs_[gate];
}



Ipc::Ipc(uint8_t gate) : IIpc(), IIrqListener()
{
	uint32_t interruptPriority = 0;
	switch(gate)
	{
		#ifndef CORE_M4
		case GATE_CORE_M4:
		{
			this->clearTxEvent_ = Chip_CREG_ClearM4Event;
			this->irqN_ = M4_IRQn;

			#ifdef CORE_M0APP
			this->writeQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0APP_GATE_M4;
			this->readQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M4_GATE_M0APP;
			#endif

			#ifdef CORE_M0SUB
			this->writeQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0SUB_GATE_M4;
			this->readQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M4_GATE_M0SUB;
			#endif

			interruptPriority = IPC_M4_INTERRUPT_PRIORITY;
		}
			break;
		#endif

		#ifndef CORE_M0APP
		case GATE_CORE_M0APP:
		{
			this->clearTxEvent_ = Chip_CREG_ClearM0AppEvent;
			this->irqN_ = M0APP_IRQn;

			#ifdef CORE_M4
			this->writeQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M4_GATE_M0APP;
			this->readQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0APP_GATE_M4;
			#endif

			#ifdef CORE_M0SUB
			this->writeQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0SUB_GATE_M0APP;
			this->readQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0APP_GATE_M0SUB;
			#endif

			interruptPriority = IPC_M0APP_INTERRUPT_PRIORITY;
		}
			break;
		#endif

		#ifndef CORE_M0SUB
		case GATE_CORE_M0SUB:
		{
			this->clearTxEvent_ = Chip_CREG_ClearM0SubEvent;
			this->irqN_ = M0SUB_IRQn;

			#ifdef CORE_M4
			this->writeQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M4_GATE_M0SUB;
			this->readQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0SUB_GATE_M4;
			#endif

			#ifdef CORE_M0APP
			this->writeQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0APP_GATE_M0SUB;
			this->readQueue_ = (IpcQueue_t*)IPC_SHARED_MEMORY_M0SUB_GATE_M0APP;
			#endif

			interruptPriority = IPC_M0SUB_INTERRUPT_PRIORITY;
		}
			break;
		#endif
		default:
			return;
	}

	for(uint32_t i = 0; i < IPC_NUM_LISTENERS; i++)
		this->listeners_[i] = (IIpcListener*)NULL;
	for(uint32_t i = 0; i < IPC_NUM_GLOBAL_VALUES; i++)
		this->globalValues_[i] = 0;

	this->writeQueue_->size = IPC_QUEUE_SIZE;
	this->writeQueue_->itemSize = sizeof(IpcMsg_t);
	this->writeQueue_->data = (uint8_t*)this->writeQueueData_;
	this->writeQueue_->valid = IPC_QUEUE_MAGIC_VALID;
	this->writeQueue_->head = 0;
	this->writeQueue_->tail = 0;

	IrqController::getIrqController()->addIrqListener(this, this->irqN_);
	IrqController::getIrqController()->
			setPriority(this->irqN_, interruptPriority);
	IrqController::getIrqController()->enableInterrupt(this->irqN_);
}




void Ipc::addIpcListener(IIpcListener* listener)
{
	for(uint32_t i = 0; i < IPC_NUM_LISTENERS; i++) {
		if(this->listeners_[i] == listener)
			break;
		if(this->listeners_[i] == (IIpcListener*)NULL) {
			this->listeners_[i] = listener;
			break;
		}
	}
}




void Ipc::deleteIpcListener(IIpcListener* listener)
{
	uint32_t index = 0;
	for(uint32_t i = 0; i < IPC_NUM_LISTENERS; i++) {
		if(this->listeners_[i] == listener)
			break;
		else
			index++;
	}
	if(index == (IPC_NUM_LISTENERS-1)) {
		if(this->listeners_[index] == listener)
			this->listeners_[index] = (IIpcListener*)NULL;
	}
	else {
		for(uint32_t i = index; i < (IPC_NUM_LISTENERS-1); i++) {
			this->listeners_[i] = this->listeners_[i+1];
			if(this->listeners_[i+1] == (IIpcListener*)NULL)
				break;
		}
	}
}




void Ipc::irqHandler(int irqNumber)
{
	this->clearTxEvent_();
	if (!IPC_QUEUE_IS_VALID(this->readQueue_))
		return;
	for(uint32_t j = 0; j < IPC_QUEUE_SIZE; j++) {
		if (IPC_QUEUE_IS_EMPTY(this->readQueue_))
			return;
		IpcMsg_t msg;
		memcpy(&msg, this->readQueue_->data +
				(TAIL_INDEX(this->readQueue_) * this->readQueue_->itemSize),
				this->readQueue_->itemSize);
		this->readQueue_->tail++;

		if (msg.id < IPC_MSG_ID_MAX) {
			if(msg.id == IPC_MSG_ID_GLOBAL_VALUE_UPDATE) {
				IpcGlobalValue_t* gv = (IpcGlobalValue_t*)msg.data;
				if (gv->index < IPC_NUM_GLOBAL_VALUES)
					this->globalValues_[gv->index] = gv->value;
				this->pushMsg(IPC_MSG_ID_FREE_MEMORY, msg.data);
			}
			else if(msg.id == IPC_MSG_ID_FREE_MEMORY)
				free_s((void *)msg.data);
			for(uint32_t i = 0; i < IPC_NUM_LISTENERS; i++) {
				if(this->listeners_[i] != (IIpcListener*)NULL) {
					if(((this->listeners_[i]->getCode()) >> msg.id) & 1)
						this->listeners_[i]->handleIpcMsg(&msg);
				}
				else break;
			}
		}
	}
}



int Ipc::pushMsg(uint32_t id, uint32_t data)
{
	IpcMsg_t msg;
	#ifdef CORE_M4
	msg.sender = GATE_CORE_M4;
	#endif
	#ifdef CORE_M0APP
	msg.sender = GATE_CORE_M0APP;
	#endif
	#ifdef CORE_M0SUB
	msg.sender = GATE_CORE_M0SUB;
	#endif
	msg.id = id;
	msg.data = data;

	if (!IPC_QUEUE_IS_VALID(this->writeQueue_))
		return IPC_QUEUE_ERROR;
	if (IPC_QUEUE_IS_FULL(this->writeQueue_))
		return IPC_QUEUE_FULL;
	memcpy(this->writeQueue_->data + (HEAD_INDEX(this->writeQueue_) * this->writeQueue_->itemSize),
			&msg, this->writeQueue_->itemSize);
	this->writeQueue_->head++;
	__DSB(); //Send signal
	__SEV();

	return IPC_QUEUE_INSERT;
}



int Ipc::getWriteQueueMsgCount(void)
{
	if (!IPC_QUEUE_IS_VALID(this->writeQueue_))
		return IPC_QUEUE_ERROR;
	else
		return IPC_QUEUE_DATA_COUNT(this->writeQueue_);
}



int Ipc::setGlobalValue(uint32_t index, uint32_t value)
{
	if (index >= IPC_NUM_GLOBAL_VALUES)
		return 1;
	IpcGlobalValue_t* gv =
			(IpcGlobalValue_t*)malloc_s(sizeof(IpcGlobalValue_t));
	if (gv == NULL)
		return 1;
	gv->value = this->globalValues_[index] = value;
	gv->index = index;
	if(this->pushMsg(IPC_MSG_ID_GLOBAL_VALUE_UPDATE,
			(uint32_t) gv) != IPC_QUEUE_INSERT) {
		free_s(gv);
		return 1;
	}
	return 0;
}



uint32_t Ipc::getGlobalValue(uint32_t index)
{
	if (index < IPC_NUM_GLOBAL_VALUES)
		return this->globalValues_[index];
	else
		return 0;
}

}
