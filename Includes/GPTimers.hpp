/*
 * GPTimers.h
 *
 *  Created on: 19 Jan 2017.
 *      Author: Stalker1290
 */

#ifndef GPTIMERS_HPP_
#define GPTIMERS_HPP_

#include "chip.h"
#include "IRQ_Controller.hpp"
#include "Defines.h"

//DEFALT RESET IS "OVERFLOW"
#define Num_of_timers 4
#define Num_of_matches 4

class GPTimer_t:protected IRQ_LISTENER_t
{
public:
	static GPTimer_t* get_GPTimer(uint8_t timer_num);
	void Initialize(void);
	void Start(void);
	void Stop(void);
	void Reset(void);
	void SetMatch(uint8_t MatchNum, uint32_t us,bool Reset_on_match);
	void SetMatch_ticks(uint8_t MatchNum, uint32_t ticks,bool Reset_on_match);
	void DeleteMatch(uint8_t MatchNum);
	void Deinitialize(void);
	void SetMatch_call(uint8_t MatchNum,VOID_CALLBACK_t MatchCallback);
	void SetMatch_call(uint8_t MatchNum,Callback_Interface_t* MatchCallback);
	void DeleteMatch_call(uint8_t MatchNum);
	uint32_t GetCounter(void);
	void SetCounter(uint32_t count);
private:
	static GPTimer_t* GPTimer_ptrs[Num_of_timers];
	GPTimer_t(uint8_t timer_num);
	virtual void IRQ(int8_t IRQ_num);
	uint8_t timer_num;
	VOID_CALLBACK_t match_callbacks[Num_of_matches];
	Callback_Interface_t* match_callback_intrf_ptrs[Num_of_matches];
	static LPC_TIMER_T* TMR_ptr[];
	static const CHIP_RGU_RST_T ResetNumber[];
	static const CHIP_CCU_CLK_T clk_num[];
	static const IRQn_Type GPT_IRQn[];
	static const uint32_t INT_PRIOR[];
};


#endif /* GPTIMERS_HPP_ */
