/**
 * @file
 * @brief Real Time Timer on GP Timer Driver Description
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

#ifndef GPTRTTIMER_HPP_
#define GPTRTTIMER_HPP_

#include "jbkernel/jb_common.h"
#include "jbkernel/IRtTimer.hpp"
#include "jbdrivers/IrqController.hpp"

#if !defined GPT_TIMERS_NUM
#define GPT_TIMERS_NUM 					4
#endif
#if !defined GPT_TIMERS_NUM_MATCHES
#define GPT_TIMERS_NUM_MATCHES 			4
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;

class GptRtTimer : public IRtTimer, protected IIrqListener
{
public:
	static GptRtTimer* getGptRtTimer(uint8_t timerNumber);
	virtual void initialize(void);
	virtual void start(void);
	virtual void stop(void);
	virtual void reset(void);
	virtual void addMatchUs(const uint8_t matchNumber,
			uint32_t us, bool isResetOnMatch);
	virtual void addMatchTicks(const uint8_t matchNumber,
			uint32_t ticks, bool isResetOnMatch);
	virtual void deleteMatch(const uint8_t matchNumber);
	virtual void deinitialize(void);
	virtual void setMatchCallback(const uint8_t matchNumber,
			IVoidCallback* const matchCallback);
	virtual void deleteMatchCallback(const uint8_t matchNumber);
	virtual uint32_t getCounter(void);
	virtual void setCounter(uint32_t count);

private:
	GptRtTimer(uint8_t timerNumber);
	virtual void irqHandler(int8_t irqNumber);

	static const CHIP_RGU_RST_T resetNumbers_[GPT_TIMERS_NUM];
	static const CHIP_CCU_CLK_T clockNumbers_[GPT_TIMERS_NUM];
	static const IRQn_Type irqNumbers_[GPT_TIMERS_NUM];
	static const uint32_t interruptPriorities_[GPT_TIMERS_NUM];
	static LPC_TIMER_T* lpcTimers_[GPT_TIMERS_NUM];
	static GptRtTimer* gptRtTimers_[GPT_TIMERS_NUM];
	uint8_t timerNumber_ = 0;
	IVoidCallback* matchCallbacks_[GPT_TIMERS_NUM_MATCHES];
};

}

#endif /* GPTRTTIMER_HPP_ */
