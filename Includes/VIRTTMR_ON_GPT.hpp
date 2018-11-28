/*
 * VIRTTMR_ON_GPT.hpp
 *
 *  Created on: 28.07.2017
 *      Author: Stalker1290
 */

#ifndef VIRTTMR_ON_GPT_HPP_
#define VIRTTMR_ON_GPT_HPP_

#define Num_of_main_timers 4
#define Num_of_sub_timers 4


#include "VOIDTMR.hpp"

class VIRT_GPT_TMR_t:public VOID_TIMER_t
{
public:
	static VIRT_GPT_TMR_t* get_VIRT_GPT_Timer(uint8_t Main_tmr_num, uint8_t Sub_tmr_num);
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
	static VIRT_GPT_TMR_t* VIRT_GPT_TMR_ptrs[Num_of_main_timers][Num_of_sub_timers];
	VIRT_GPT_TMR_t(uint8_t Main_tmr_num, uint8_t Sub_tmr_num);
	uint8_t Main_tmr_num;
	uint8_t Sub_tmr_num;
	uint32_t VTMR_period;
	Callback_Interface_t* callback_intrf_ptr;
	virtual void IRQ(int8_t IRQ_num)__attribute__((used));
	static LPC_TIMER_T* TMR_ptr[];
	static const CHIP_RGU_RST_T ResetNumber[];
	static const CHIP_CCU_CLK_T clk_num[];
	static const IRQn_Type GPT_IRQn[];
	static const uint32_t INT_PRIOR[];
};


#endif /* VIRTTMR_ON_GPT_HPP_ */
