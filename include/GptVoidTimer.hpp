/**
 * @file
 * @brief Void Timer on GP Timer Driver Description
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

#ifndef GPTVOIDTIMER_HPP_
#define GPTVOIDTIMER_HPP_

#include "jb_common.h"
#include "IVoidTimer.hpp"
#include "callback_interfaces.hpp"
#include "IrqController.hpp"

#if !defined GPT_TIMERS_NUM
#define GPT_TIMERS_NUM 			4
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;

class GptVoidTimer: public IVoidTimer, protected IIrqListener
{
public:
	static GptVoidTimer* getGptVoidTimer(uint8_t timerNumber);
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
	GptVoidTimer(uint8_t timerNumber);
	virtual void irqHandler(int8_t irqNumber);

	static const CHIP_RGU_RST_T resetNumbers_[GPT_TIMERS_NUM];
	static const CHIP_CCU_CLK_T clockNumbers_[GPT_TIMERS_NUM];
	static const IRQn_Type irqNumbers_[GPT_TIMERS_NUM];
	static const uint32_t interruptPriorities_[GPT_TIMERS_NUM];
	static LPC_TIMER_T* lpcTimers_[GPT_TIMERS_NUM];
	static GptVoidTimer* gptVoidTimers_[GPT_TIMERS_NUM];
	uint8_t timerNumber_ = 0;
	IVoidCallback* callback_ = NULL;
};

}

#endif /* GPTVOIDTIMER_HPP_ */
