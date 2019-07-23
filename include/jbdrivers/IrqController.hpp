/**
 * @file
 * @brief IRQ Controller Driver Description
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

#ifndef IRQCONTROLLER_HPP_
#define IRQCONTROLLER_HPP_

#include <stdint.h>
#include "jbkernel/jb_common.h"
#include "jbkernel/callback_interfaces.hpp"
#include "jbutilities/LinkedList.hpp"

namespace jblib::jbdrivers
{

class IIrqListener
{
public:
	IIrqListener(void){}
	virtual ~IIrqListener(void){}
	virtual void irqHandler(int irqNumber) = 0;
};


using namespace jbutilities;

class IrqController
{
public:
	static IrqController* getIrqController(void);
	void enableInterrupt(int irqNumber);
	void disableInterrupt(int irqNumber);
	void setPriority(int irqNumber, uint32_t priority);
    void addIrqListener(IIrqListener* const listener, int irqNumber);
    void deleteIrqListener(IIrqListener* const listener);
	void handleIrq(const int irqNumber);

private:
	IrqController(void);

	static IrqController* irqController_;
	typedef struct
	{
		IIrqListener* listener = NULL;
		int irqNumber = 0;
	}ListenersListItem;
	LinkedList<ListenersListItem>* listenersList_ = NULL;
};

}


#endif /* IRQCONTROLLER_HPP_ */
