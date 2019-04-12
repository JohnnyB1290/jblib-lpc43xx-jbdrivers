/*
 * GPTimers.c
 *
 *  Created on: 20 jul 2017.
 *      Author: Stalker1290
 */

#include "GPTimers.hpp"

#ifdef CORE_M4
LPC_TIMER_T* GPTimer_t::TMR_ptr[] = {LPC_TIMER0, LPC_TIMER1, LPC_TIMER2, LPC_TIMER3};
const CHIP_RGU_RST_T GPTimer_t::ResetNumber[] = {RGU_TIMER0_RST, RGU_TIMER1_RST, RGU_TIMER2_RST, RGU_TIMER3_RST};
const CHIP_CCU_CLK_T GPTimer_t::clk_num[] = {CLK_MX_TIMER0, CLK_MX_TIMER1, CLK_MX_TIMER2, CLK_MX_TIMER3};
const IRQn_Type GPTimer_t::GPT_IRQn[] = {TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn};
const uint32_t GPTimer_t::INT_PRIOR[] = {TIMER0_interrupt_priority, TIMER1_interrupt_priority, TIMER2_interrupt_priority,
		TIMER3_interrupt_priority};
#endif

#ifdef CORE_M0

#ifdef CORE_M0APP
LPC_TIMER_T* GPTimer_t::TMR_ptr[] = {LPC_TIMER0, LPC_TIMER0, LPC_TIMER3, LPC_TIMER3};
const CHIP_RGU_RST_T GPTimer_t::ResetNumber[] = {RGU_TIMER0_RST, RGU_TIMER0_RST, RGU_TIMER3_RST, RGU_TIMER3_RST};
const CHIP_CCU_CLK_T GPTimer_t::clk_num[] = {CLK_MX_TIMER0, CLK_MX_TIMER0, CLK_MX_TIMER3, CLK_MX_TIMER3};
const IRQn_Type GPTimer_t::GPT_IRQn[] = {TIMER0_IRQn, TIMER0_IRQn, TIMER3_IRQn, TIMER3_IRQn};
const uint32_t GPTimer_t::INT_PRIOR[] = {TIMER0_interrupt_priority, TIMER0_interrupt_priority, TIMER3_interrupt_priority,
		TIMER3_interrupt_priority};
#endif

#ifdef CORE_M0SUB
LPC_TIMER_T* GPTimer_t::TMR_ptr[] = {LPC_TIMER1, LPC_TIMER1, LPC_TIMER2, LPC_TIMER2};
const CHIP_RGU_RST_T GPTimer_t::ResetNumber[] = {RGU_TIMER1_RST, RGU_TIMER1_RST, RGU_TIMER2_RST, RGU_TIMER2_RST};
const CHIP_CCU_CLK_T GPTimer_t::clk_num[] = {CLK_MX_TIMER1, CLK_MX_TIMER1, CLK_MX_TIMER2, CLK_MX_TIMER2};
const IRQn_Type GPTimer_t::GPT_IRQn[] = {TIMER1_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER2_IRQn};
const uint32_t GPTimer_t::INT_PRIOR[] = {TIMER1_interrupt_priority, TIMER1_interrupt_priority, TIMER2_interrupt_priority,
		TIMER2_interrupt_priority};
#endif

#endif

GPTimer_t* GPTimer_t::GPTimer_ptrs[Num_of_timers] = {(GPTimer_t*)NULL, (GPTimer_t*)NULL, (GPTimer_t*)NULL, (GPTimer_t*)NULL};

GPTimer_t* GPTimer_t::get_GPTimer(uint8_t timer_num)
{
	if(timer_num < Num_of_timers)
	{
#ifdef CORE_M0

#ifdef CORE_M0APP
		if((timer_num!=0)&&(timer_num!=3)) return (GPTimer_t*)NULL;
#endif

#ifdef CORE_M0SUB
		if((timer_num!=1)&&(timer_num!=2)) return (GPTimer_t*)NULL;
#endif

#endif
		if(GPTimer_ptrs[timer_num] == (GPTimer_t*)NULL) GPTimer_ptrs[timer_num] = new GPTimer_t(timer_num);
		return GPTimer_ptrs[timer_num];
	}
	else return (GPTimer_t*)NULL;
}

GPTimer_t::GPTimer_t(uint8_t timer_num):IRQ_LISTENER_t()
{
	this->timer_num = timer_num;
	for(int i=0; i<Num_of_matches; i++)
	{
		this->match_callbacks[i] = 	(VOID_CALLBACK_t)NULL;
		this->match_callback_intrf_ptrs[i] = (Callback_Interface_t*)NULL;
	}
	this->setCode((uint64_t)1<<(this->GPT_IRQn[this->timer_num]));
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void GPTimer_t::IRQ(int8_t IRQ_num)
{
	static uint8_t match_num;

	for(uint8_t i = 0; i< Num_of_matches; i++)
	{
		match_num = i;
		if (Chip_TIMER_MatchPending(this->TMR_ptr[this->timer_num], match_num))
		{
			Chip_TIMER_ClearMatch(this->TMR_ptr[this->timer_num], match_num);
			if(this->match_callbacks[match_num] != (VOID_CALLBACK_t)NULL) this->match_callbacks[match_num]();
			else if(this->match_callback_intrf_ptrs[match_num] != (Callback_Interface_t*)NULL)
				this->match_callback_intrf_ptrs[match_num]->void_callback((void*)this, (void*)&match_num);
		}
	}
}

void GPTimer_t::Initialize(void)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif

	Chip_TIMER_Init(this->TMR_ptr[this->timer_num]);
	Chip_RGU_TriggerReset(this->ResetNumber[this->timer_num]);
	while (Chip_RGU_InReset(this->ResetNumber[this->timer_num])) {}
	Chip_TIMER_Reset(this->TMR_ptr[this->timer_num]);

	#ifdef CORE_M0
	NVIC_SetPriority(this->GPT_IRQn[this->timer_num], this->INT_PRIOR[this->timer_num]);
	#endif
	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(this->GPT_IRQn[this->timer_num], NVIC_EncodePriority(prioritygroup, this->INT_PRIOR[this->timer_num], 0));
	#endif

	NVIC_ClearPendingIRQ(this->GPT_IRQn[this->timer_num]);
	NVIC_EnableIRQ(this->GPT_IRQn[this->timer_num]);
}

void GPTimer_t::Start(void)
{
	Chip_TIMER_Enable(this->TMR_ptr[this->timer_num]);
}

void GPTimer_t::Stop(void)
{
	Chip_TIMER_Disable(this->TMR_ptr[this->timer_num]);
}

void GPTimer_t::Reset(void)
{
	Chip_TIMER_Reset(this->TMR_ptr[this->timer_num]);
}

void GPTimer_t::SetMatch(uint8_t MatchNum, uint32_t us,bool Reset_on_match)
{
	uint32_t GPT_tics;

	GPT_tics = (Chip_Clock_GetRate(this->clk_num[this->timer_num]) / 1000000) * us;
	Chip_TIMER_SetMatch(this->TMR_ptr[this->timer_num], MatchNum, GPT_tics);
	if(Reset_on_match) Chip_TIMER_ResetOnMatchEnable(this->TMR_ptr[this->timer_num], MatchNum);
	Chip_TIMER_MatchEnableInt(this->TMR_ptr[this->timer_num], MatchNum);
}

void GPTimer_t::SetMatch_ticks(uint8_t MatchNum, uint32_t ticks,bool Reset_on_match)
{
	Chip_TIMER_SetMatch(this->TMR_ptr[this->timer_num], MatchNum, ticks);
	if(Reset_on_match) Chip_TIMER_ResetOnMatchEnable(this->TMR_ptr[this->timer_num], MatchNum);
	Chip_TIMER_MatchEnableInt(this->TMR_ptr[this->timer_num], MatchNum);
}

void GPTimer_t::DeleteMatch(uint8_t MatchNum)
{
	Chip_TIMER_MatchDisableInt(this->TMR_ptr[this->timer_num], MatchNum);
	Chip_TIMER_ResetOnMatchDisable(this->TMR_ptr[this->timer_num], MatchNum);
}

void GPTimer_t::SetMatch_call(uint8_t MatchNum,VOID_CALLBACK_t MatchCallback)
{
	this->match_callbacks[MatchNum] = MatchCallback;
}

void GPTimer_t::SetMatch_call(uint8_t MatchNum,Callback_Interface_t* MatchCallback)
{
	this->match_callback_intrf_ptrs[MatchNum] = MatchCallback;
}

void GPTimer_t::DeleteMatch_call(uint8_t MatchNum)
{
	this->match_callbacks[MatchNum] = (VOID_CALLBACK_t)NULL;
	this->match_callback_intrf_ptrs[MatchNum] = (Callback_Interface_t*)NULL;
}

void GPTimer_t::Deinitialize(void)
{
	this->Stop();
	NVIC_DisableIRQ(this->GPT_IRQn[this->timer_num]);
	Chip_RGU_TriggerReset(this->ResetNumber[this->timer_num]);
	while (Chip_RGU_InReset(this->ResetNumber[this->timer_num])) {}
	Chip_TIMER_DeInit(this->TMR_ptr[this->timer_num]);
}

uint32_t GPTimer_t::GetCounter(void)
{
	return this->TMR_ptr[this->timer_num]->TC;
}

void GPTimer_t::SetCounter(uint32_t count)
{
	this->TMR_ptr[this->timer_num]->TC = count;
}

