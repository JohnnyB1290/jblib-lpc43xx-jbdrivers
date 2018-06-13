/*
 * IPC.cpp
 *
 *  Created on: 07.09.2017
 *      Author: Stalker1290
 */

#include "IPC.hpp"
#include "string.h"

#ifdef CORE_M4
ipc_queue_t* IPC_proto_t::qrd = (ipc_queue_t*)SHARED_MEM_M0;
ipc_queue_t* IPC_proto_t::qwr = (ipc_queue_t*)SHARED_MEM_M4;
#define ClearTXEvent   Chip_CREG_ClearM0AppEvent
#define IPC_IRQn       M0APP_IRQn
#endif

#ifdef CORE_M0
ipc_queue_t* IPC_proto_t::qrd = (ipc_queue_t*)SHARED_MEM_M4;
ipc_queue_t* IPC_proto_t::qwr = (ipc_queue_t*)SHARED_MEM_M0;
#define ClearTXEvent   Chip_CREG_ClearM4Event
#define IPC_IRQn       M4_IRQn
#endif

typedef struct ipc_gbl_val_struct
{
	int index;
	uint32_t val;
}ipc_gbl_val_t;

IPC_proto_t* IPC_proto_t::IPC_proto_ptr = (IPC_proto_t*)NULL;

IPC_proto_t* IPC_proto_t::get_IPC_proto(void)
{
	if(IPC_proto_t::IPC_proto_ptr == (IPC_proto_t*)NULL) IPC_proto_t::IPC_proto_ptr = new IPC_proto_t();
	return IPC_proto_t::IPC_proto_ptr;
}

IPC_proto_t::IPC_proto_t(void):IRQ_LISTENER_t()
{
#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
#endif

	for(int i=0; i<IPC_Listeners_num; i++) this->ipcex_listeners[i] = (IPC_listener_t*)NULL;
	for(int i=0; i<IPCEX_MAX_GBLVAL; i++) this->gblval[i] = 0;
	/* Check if size is a power of 2 and >0*/
	if ((IPCEX_QUEUE_SZ & (IPCEX_QUEUE_SZ - 1))|| IPCEX_QUEUE_SZ == 0) while (1); /* BUG: Size must always be power of 2 */

	memset(IPC_proto_t::qwr, 0, sizeof(*IPC_proto_t::qwr));
	IPC_proto_t::qwr->count = IPCEX_QUEUE_SZ;
	IPC_proto_t::qwr->size = sizeof(ipcex_msg_t);
	IPC_proto_t::qwr->data = (uint8_t*)this->ipcex_queue;
	IPC_proto_t::qwr->valid = QUEUE_MAGIC_VALID;

	this->setCode((uint64_t)1<<IPC_IRQn);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);

	#ifdef CORE_M0
	NVIC_SetPriority(IPC_IRQn,IPC_interrupt_priority);
	#endif
	#ifdef CORE_M4
	NVIC_SetPriority(IPC_IRQn, NVIC_EncodePriority(prioritygroup, IPC_interrupt_priority, 0));
	#endif
	NVIC_EnableIRQ(IPC_IRQn);
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

	ClearTXEvent();
	if (!QUEUE_IS_VALID(IPC_proto_t::qrd)) return;

	for(int j=0; j< IPCEX_QUEUE_SZ; j++)
	{
		if (QUEUE_IS_EMPTY(IPC_proto_t::qrd)) return;

		/* Pop the queue Item */
		memcpy(&msg, IPC_proto_t::qrd->data + ((IPC_proto_t::qrd->tail & (IPC_proto_t::qrd->count - 1)) * IPC_proto_t::qrd->size),
				IPC_proto_t::qrd->size);
		IPC_proto_t::qrd->tail++;

		if (msg.id < IPCEX_MAX_IDS)
		{
			if(msg.id == IPCEX_ID_GBLUPDATE)
			{
				gv = (ipc_gbl_val_t*)msg.data;
				if (gv->index < IPCEX_MAX_GBLVAL) this->gblval[gv->index] = gv->val;
				this->MsgPush(IPCEX_ID_FREEMEM, msg.data);
			}
			if(msg.id == IPCEX_ID_FREEMEM) free((void *)msg.data);

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
	if (!QUEUE_IS_VALID(IPC_proto_t::qwr)) return QUEUE_ERROR;
	return QUEUE_DATA_COUNT(IPC_proto_t::qwr);
}

int IPC_proto_t::MsgPush(uint32_t id, uint32_t data)
{
	ipcex_msg_t msg;
	msg.id = id;
	msg.data = data;

	if (!QUEUE_IS_VALID(IPC_proto_t::qwr)) return QUEUE_ERROR;
	if (QUEUE_IS_FULL(IPC_proto_t::qwr)) return QUEUE_FULL;

	memcpy(IPC_proto_t::qwr->data + ((IPC_proto_t::qwr->head & (IPC_proto_t::qwr->count - 1)) * IPC_proto_t::qwr->size), &msg,
			IPC_proto_t::qwr->size);
	IPC_proto_t::qwr->head++;

	__DSB(); //Send signal
	__SEV();

	return QUEUE_INSERT;
}

int IPC_proto_t::SetGblVal(int index, uint32_t val)
{
	ipc_gbl_val_t* gv;

	if (index >= IPCEX_MAX_GBLVAL) return 1;

	gv = (ipc_gbl_val_t*)malloc(sizeof(ipc_gbl_val_t));
	if (gv == NULL) return 1; /* Something wrong */

	gv->val = this->gblval[index] = val;
	gv->index = index;
	if(this->MsgPush(IPCEX_ID_GBLUPDATE, (uint32_t) gv) != QUEUE_INSERT) {
		free(gv);
		return 1;
	}
	return 0;
}


uint32_t IPC_proto_t::GetGblVal(int index)
{
	static uint32_t ret = 0;
	if (index < IPCEX_MAX_GBLVAL) 
	{
		ret = this->gblval[index];
		//return this->gblval[index];
		return ret;
	}
	return 0;
}

