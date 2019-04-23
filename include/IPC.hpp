/*
 * IPC.hpp
 *
 *  Created on: 07.09.2017
 *      Author: Stalker1290
 */

#ifndef IPC_HPP_
#define IPC_HPP_

#include "chip.h"
#include "IRQ_Controller.hpp"
#include "Defines.h"
#include "Common_interfaces.hpp"

#if ((IPCEX_QUEUE_SZ & (IPCEX_QUEUE_SZ - 1))|| IPCEX_QUEUE_SZ == 0)
#error "Size must always be power of 2"
#endif

#define IPC_Listeners_num IPCEX_MAX_IDS

typedef struct ipc_queue_struct {
	int32_t size;				/*!< Size of a single item in queue */
	uint32_t count;				/*!< Toal number of elements that can be stored in the queue */
	volatile uint32_t head;		/*!< Head index of the queue */
	volatile uint32_t tail;		/*!< Tail index of the queue */
	uint8_t *data;				/*!< Pointer to the data */
	uint32_t valid;             /*!< Queue is valid only if this is #QUEUE_MAGIC_VALID */
	uint32_t reserved[2];		/*!< Reserved entry to keep the structure aligned */
}ipc_queue_t;

#define QUEUE_VALID      1
#define QUEUE_INSERT     0
#define QUEUE_FULL      -1
#define QUEUE_EMPTY     -2
#define QUEUE_ERROR     -3
#define QUEUE_TIMEOUT   -4
#define QUEUE_MAGIC_VALID   0xCAB51053

#define QUEUE_DATA_COUNT(q) ((uint32_t) ((q)->head - (q)->tail))
#define QUEUE_IS_FULL(q)    (QUEUE_DATA_COUNT(q) >= (q)->count)
#define QUEUE_IS_EMPTY(q)   ((q)->head == (q)->tail)
#define QUEUE_IS_VALID(q)   ((q)->valid == QUEUE_MAGIC_VALID)

#pragma pack(push, 1)

typedef struct __ipcex_msg {
	uint8_t sender;
	uint32_t id;
	uint32_t data;
} ipcex_msg_t;

#pragma pack(pop)

class IPC_listener_t
{
public:
	IPC_listener_t(void){this->CODE = 0;}
    virtual ~IPC_listener_t(void){}
    virtual void IPC_MSG_HANDLER(ipcex_msg_t* msg) = 0;
    uint64_t getCode(void){return this->CODE;}
    void setCode(uint64_t CODE){this->CODE = CODE;}
private:
    uint64_t CODE;
};

class C_IPC_listener_t:public IPC_listener_t
{
public:
	C_IPC_listener_t(uint64_t CODE,void (*func)(ipcex_msg_t*)):IPC_listener_t()
	{
		this->setCode(CODE);
		this->callback = func;
	}
private:
	virtual void IPC_MSG_HANDLER(ipcex_msg_t* msg){ if(this->callback != NULL) this->callback(msg);}
	void (*callback)(ipcex_msg_t*);
};

typedef enum{
	CORE_M4_GATE = 0,
	CORE_M0APP_GATE = 1,
	CORE_M0SUB_GATE = 2,
}ipcGate_t;

class IPC_proto_t:protected IRQ_LISTENER_t
{
public:
	static IPC_proto_t* get_IPC_proto(uint8_t gate);
    void Add_IPC_Listener(IPC_listener_t* listener);
    void Delete_IPC_Listener(IPC_listener_t* listener);
    int Qwr_msg_count(void);
    int MsgPush(uint32_t id, uint32_t data);
    int SetGblVal(int index, uint32_t val);
    uint32_t GetGblVal(int index);
private:
	IPC_proto_t(uint8_t gate);
	static IPC_proto_t* IPC_proto_ptr[3];
    virtual void IRQ(int8_t IRQ_num);
    ipc_queue_t* qrd;
    ipc_queue_t* qwr;
    ipcex_msg_t ipcex_queue[IPCEX_QUEUE_SZ];
    uint32_t gblval[IPCEX_MAX_GBLVAL];
    IPC_listener_t* ipcex_listeners[IPC_Listeners_num];
    void (*ClearTXEvent)(void);
    IRQn_Type IPC_IRQn;
};

#endif /* IPC_HPP_ */
