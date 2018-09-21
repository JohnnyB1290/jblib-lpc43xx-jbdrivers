/*
 * IPC.cpp
 *
 *  Created on: 07.09.2017
 *      Author: Stalker1290
 */

#include "IPC.hpp"
#include "string.h"

typedef struct ipc_gbl_val_struct
{
	int index;
	uint32_t val;
}ipc_gbl_val_t;

IPC_proto_t* IPC_proto_t::IPC_proto_ptr[3] = {(IPC_proto_t*)NULL, (IPC_proto_t*)NULL, (IPC_proto_t*)NULL};

IPC_proto_t* IPC_proto_t::get_IPC_proto(uint8_t gate)
{
	#ifdef CORE_M4
		if(gate == CORE_M4_GATE) return (IPC_proto_t*)NULL;
	#endif
	#ifdef CORE_M0APP
		if(gate == CORE_M0APP_GATE) return (IPC_proto_t*)NULL;
	#endif
	#ifdef CORE_M0SUB
		if(gate == CORE_M0SUB_GATE) return (IPC_proto_t*)NULL;
	#endif

	if(IPC_proto_t::IPC_proto_ptr[gate] == (IPC_proto_t*)NULL) IPC_proto_t::IPC_proto_ptr[gate] = new IPC_proto_t(gate);
	return IPC_proto_t::IPC_proto_ptr[gate];
}

IPC_proto_t::IPC_proto_t(uint8_t gate):IRQ_LISTENER_t()
{
	uint32_t interruptPriority = 0;
#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
#endif
	switch(gate){

#ifndef CORE_M4
	case CORE_M4_GATE:
		this->ClearTXEvent = Chip_CREG_ClearM4Event;
		this->IPC_IRQn = M4_IRQn;

		#ifdef CORE_M0APP
		this->qwr = (ipc_queue_t*)SHARED_MEM_M0APP_M4;
		this->qrd = (ipc_queue_t*)SHARED_MEM_M4_M0APP;
		#endif

		#ifdef CORE_M0SUB
		this->qwr = (ipc_queue_t*)SHARED_MEM_M0SUB_M4;
		this->qrd = (ipc_queue_t*)SHARED_MEM_M4_M0SUB;
		#endif

		interruptPriority = IPC_M4_interrupt_priority;
		break;
#endif

#ifndef CORE_M0APP
	case CORE_M0APP_GATE:
		this->ClearTXEvent = Chip_CREG_ClearM0AppEvent;
		this->IPC_IRQn = M0APP_IRQn;

		#ifdef CORE_M4
		this->qwr = (ipc_queue_t*)SHARED_MEM_M4_M0APP;
		this->qrd = (ipc_queue_t*)SHARED_MEM_M0APP_M4;
		#endif

		#ifdef CORE_M0SUB
		this->qwr = (ipc_queue_t*)SHARED_MEM_M0SUB_M0APP;
		this->qrd = (ipc_queue_t*)SHARED_MEM_M0APP_M0SUB;
		#endif

		interruptPriority = IPC_M0APP_interrupt_priority;
		break;
#endif

#ifndef CORE_M0SUB
	case CORE_M0SUB_GATE:
		this->ClearTXEvent = Chip_CREG_ClearM0SubEvent;
		this->IPC_IRQn = M0SUB_IRQn;

		#ifdef CORE_M4
		this->qwr = (ipc_queue_t*)SHARED_MEM_M4_M0SUB;
		this->qrd = (ipc_queue_t*)SHARED_MEM_M0SUB_M4;
		#endif

		#ifdef CORE_M0APP
		this->qwr = (ipc_queue_t*)SHARED_MEM_M0APP_M0SUB;
		this->qrd = (ipc_queue_t*)SHARED_MEM_M0SUB_M0APP;
		#endif

		interruptPriority = IPC_M0SUB_interrupt_priority;
		break;
#endif
	default:
		return;
	}

	for(int i=0; i<IPC_Listeners_num; i++) this->ipcex_listeners[i] = (IPC_listener_t*)NULL;
	for(int i=0; i<IPCEX_MAX_GBLVAL; i++) this->gblval[i] = 0;
	/* Check if size is a power of 2 and >0*/
	if ((IPCEX_QUEUE_SZ & (IPCEX_QUEUE_SZ - 1))|| IPCEX_QUEUE_SZ == 0) while (1); /* BUG: Size must always be power of 2 */

	memset(this->qwr, 0, sizeof(*this->qwr));
	this->qwr->count = IPCEX_QUEUE_SZ;
	this->qwr->size = sizeof(ipcex_msg_t);
	this->qwr->data = (uint8_t*)this->ipcex_queue;
	this->qwr->valid = QUEUE_MAGIC_VALID;

	this->setCode((uint64_t)1<<this->IPC_IRQn);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);

	#ifdef CORE_M0
	NVIC_SetPriority(this->IPC_IRQn,interruptPriority);
	#endif
	#ifdef CORE_M4
	NVIC_SetPriority(this->IPC_IRQn, NVIC_EncodePriority(prioritygroup, interruptPriority, 0));
	#endif
	NVIC_ClearPendingIRQ(this->IPC_IRQn);
	NVIC_EnableIRQ(this->IPC_IRQn);
}

void IPC_proto_t::Add_IPC_Listener(IPC_listener_t* listener)
{
	for(int i = 0; i < IPC_Listeners_num; i++)
	{
		if(this->ipcex_listeners[i] == listener) break;
		if(this->ipcex_listeners[i] == (IPC_listener_t*)NULL)
		{
			this->ipcex_listeners[i] = listener;
			break;
		}
	}
}

void IPC_proto_t::Delete_IPC_Listener(IPC_listener_t* listener)
{
	uint32_t index = 0;
	for(int i = 0; i < IPC_Listeners_num; i++)
	{
		if(this->ipcex_listeners[i] == listener) break;
		else index++;
	}
	if(index == (IPC_Listeners_num-1))
	{
		if(this->ipcex_listeners[index] == listener) this->ipcex_listeners[index] = (IPC_listener_t*)NULL;
	}
	else
	{
		for(int i = index; i < (IPC_Listeners_num-1); i++)
		{
			this->ipcex_listeners[i] = this->ipcex_listeners[i+1];
			if(this->ipcex_listeners[i+1] == (IPC_listener_t*)NULL) break;
		}
	}
}

void IPC_proto_t::IRQ(int8_t IRQ_num)
{
	ipcex_msg_t msg;
	ipc_gbl_val_t* gv;

	this->ClearTXEvent();
	if (!QUEUE_IS_VALID(this->qrd)) return;

	for(int j=0; j< IPCEX_QUEUE_SZ; j++)
	{
		if (QUEUE_IS_EMPTY(this->qrd)) return;

		/* Pop the queue Item */
		memcpy(&msg, this->qrd->data + ((this->qrd->tail & (this->qrd->count - 1)) * this->qrd->size),
				this->qrd->size);
		this->qrd->tail++;

		if (msg.id < IPCEX_MAX_IDS)
		{
			if(msg.id == IPCEX_ID_GBLUPDATE)
			{
				gv = (ipc_gbl_val_t*)msg.data;
				if (gv->index < IPCEX_MAX_GBLVAL) this->gblval[gv->index] = gv->val;
				this->MsgPush(IPCEX_ID_FREEMEM, msg.data);
			}
			if(msg.id == IPCEX_ID_FREEMEM) free_s((void *)msg.data);

			for(int i = 0; i < IPC_Listeners_num; i++)
			{
				if(this->ipcex_listeners[i] != (IPC_listener_t*)NULL)
				{
					if(((this->ipcex_listeners[i]->getCode())>>msg.id) & 1) this->ipcex_listeners[i]->IPC_MSG_HANDLER(&msg);
				}
				else break;
			}
		}
	}
}

/* Get number of pending items in queue */
int IPC_proto_t::Qwr_msg_count(void)
{
	if (!QUEUE_IS_VALID(this->qwr)) return QUEUE_ERROR;
	return QUEUE_DATA_COUNT(this->qwr);
}

int IPC_proto_t::MsgPush(uint32_t id, uint32_t data)
{
	ipcex_msg_t msg;
#ifdef CORE_M4
	msg.sender = CORE_M4_GATE;
#endif
#ifdef CORE_M0APP
	msg.sender = CORE_M0APP_GATE;
#endif
#ifdef CORE_M0SUB
	msg.sender = CORE_M0SUB_GATE;
#endif

	msg.id = id;
	msg.data = data;

	if (!QUEUE_IS_VALID(this->qwr)) return QUEUE_ERROR;
	if (QUEUE_IS_FULL(this->qwr)) return QUEUE_FULL;

	memcpy(IPC_proto_t::qwr->data + ((this->qwr->head & (this->qwr->count - 1)) * this->qwr->size), &msg,
			this->qwr->size);
	this->qwr->head++;

	__DSB(); //Send signal
	__SEV();

	return QUEUE_INSERT;
}

int IPC_proto_t::SetGblVal(int index, uint32_t val)
{
	ipc_gbl_val_t* gv;

	if (index >= IPCEX_MAX_GBLVAL) return 1;

	gv = (ipc_gbl_val_t*)malloc_s(sizeof(ipc_gbl_val_t));
	if (gv == NULL) return 1; /* Something wrong */

	gv->val = this->gblval[index] = val;
	gv->index = index;
	if(this->MsgPush(IPCEX_ID_GBLUPDATE, (uint32_t) gv) != QUEUE_INSERT) {
		free_s(gv);
		return 1;
	}
	return 0;
}


uint32_t IPC_proto_t::GetGblVal(int index)
{
	if (index < IPCEX_MAX_GBLVAL) 
	{
		return this->gblval[index];
	}
	return 0;
}

