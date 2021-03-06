/**
 * @file
 * @brief Inter-Processor Communication Driver Description
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

#ifndef IPC_HPP_
#define IPC_HPP_

#include "jbkernel/jb_common.h"
#include "jbkernel/callback_interfaces.hpp"
#include "jbdrivers/IrqController.hpp"
#include "jbkernel/IIpc.hpp"
#include <forward_list>

namespace jblib::jbdrivers
{

using namespace jbkernel;

typedef enum
{
	GATE_CORE_M4 = 0,
	GATE_CORE_M0APP = 1,
	GATE_CORE_M0SUB = 2,
}IpcGate_t;

class Ipc : public IIpc, protected IIrqListener
{
public:
	static Ipc* getIpc(uint8_t gate);
	virtual void addIpcListener(IIpcListener* listener);
    virtual void deleteIpcListener(IIpcListener* listener);
    virtual int pushMsg(uint32_t id, uint32_t data);
    virtual int getWriteQueueMsgCount(void);
    virtual int setGlobalValue(uint32_t index, uint32_t value);
    virtual uint32_t getGlobalValue(uint32_t index);

private:
	Ipc(uint8_t gate);
	virtual void irqHandler(int irqNumber);
	void (*clearTxEvent_)(void) = NULL;

	static Ipc* ipcs_[3];
    IpcQueue_t* readQueue_ = NULL;
    IpcQueue_t* writeQueue_ = NULL;
    IpcMsg_t writeQueueData_[IPC_QUEUE_SIZE];
    uint32_t globalValues_[IPC_NUM_GLOBAL_VALUES];
    std::forward_list<IIpcListener*> listenersList_;
    std::forward_list<IIpcListener*> listenersDeleteList_;
    IRQn_Type irqN_;
};

}

#endif /* IPC_HPP_ */
