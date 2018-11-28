/*
 * GPTimers_GPT.hpp
 *
 *  Created on: 28 но€б. 2018 г.
 *      Author: Stalker1290
 */

#ifndef GPTIMERS_GPT_HPP_
#define GPTIMERS_GPT_HPP_

#include "VOIDTMR.hpp"

#define Num_of_GP_timers 4

class GPTimersGPT_t: public VOID_TIMER_t{
public:
	static GPTimersGPT_t* getGPTimerGPT(uint8_t timerNum);
	virtual void Initialize(uint32_t us);
	virtual void Start(void);
	virtual void Stop(void);
	virtual void Reset(void);
	virtual uint32_t GetCounter(void);
	virtual void SetCounter(uint32_t count);
	virtual void AddCall(Callback_Interface_t* IntCallback);
	virtual void DeleteCall(void);
	virtual void Deinitialize(void);
private:
	static GPTimersGPT_t* gpTimersGPTPtrs[Num_of_GP_timers];
	GPTimersGPT_t(uint8_t timerNum);
	uint8_t timerNum;
	Callback_Interface_t* callback;
	virtual void IRQ(int8_t IRQ_num)__attribute__((used));
	static LPC_TIMER_T* lpcTmrPtr[];
	static const CHIP_RGU_RST_T resetNumber[];
	static const CHIP_CCU_CLK_T clkNum[];
	static const IRQn_Type gptIRQn[];
	static const uint32_t intPrior[];
};



#endif /* GPTIMERS_GPT_HPP_ */
