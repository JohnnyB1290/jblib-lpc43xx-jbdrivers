/**
 * @file
 * @brief Void Timer on SysTick Timer Driver Description
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

#ifndef SYSTICKVOIDTIMER_HPP_
#define SYSTICKVOIDTIMER_HPP_

#include "jb_common.h"
#include "IVoidTimer.hpp"
#include "callback_interfaces.hpp"
#include "IrqController.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

class SysTickVoidTimer : public IVoidTimer, protected IIrqListener
{
public:
	static SysTickVoidTimer* getSysTickVoidTimer(void);
	virtual void initialize(uint32_t us);
	virtual void start(void);
	virtual void stop(void);
	virtual void reset(void);
	virtual uint32_t getCounter(void);
	virtual void setCounter(uint32_t count);
	virtual void addCallback(IVoidCallback* const callback);
	virtual void deleteCallback(void);
	virtual void deinitialize(void);

private:
	SysTickVoidTimer(void);
	virtual void irqHandler(int8_t irqNumber);

	static SysTickVoidTimer* sysTickVoidTimer_;
	IVoidCallback* callback_ = NULL;
};

}

#endif /* SYSTICKVOIDTIMER_HPP_ */
