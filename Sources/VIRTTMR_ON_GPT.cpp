/*
 * VIRTTMR_ON_GPT.c
 *
 *  Created on: 23.01.2017
 *      Author: Stalker1290
 */

#include "VIRTTMR_ON_GPT.hpp"

#ifdef CORE_M4
LPC_TIMER_T* VIRT_GPT_TMR_t::TMR_ptr[] = {LPC_TIMER0, LPC_TIMER1, LPC_TIMER2, LPC_TIMER3};
const CHIP_RGU_RST_T VIRT_GPT_TMR_t::ResetNumber[] = {RGU_TIMER0_RST, RGU_TIMER1_RST, RGU_TIMER2_RST, RGU_TIMER3_RST};
const CHIP_CCU_CLK_T VIRT_GPT_TMR_t::clk_num[] = {CLK_MX_TIMER0, CLK_MX_TIMER1, CLK_MX_TIMER2, CLK_MX_TIMER3};
const IRQn_Type VIRT_GPT_TMR_t::GPT_IRQn[] = {TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn};
const uint32_t VIRT_GPT_TMR_t::INT_PRIOR[] = {TIMER0_interrupt_priority, TIMER1_interrupt_priority, TIMER2_interrupt_priority,
		TIMER3_interrupt_priority};
#endif

#ifdef CORE_M0
LPC_TIMER_T* VIRT_GPT_TMR_t::TMR_ptr[] = {LPC_TIMER0, LPC_TIMER0, LPC_TIMER3, LPC_TIMER3};
const CHIP_RGU_RST_T VIRT_GPT_TMR_t::ResetNumber[] = {RGU_TIMER0_RST, RGU_TIMER0_RST, RGU_TIMER3_RST, RGU_TIMER3_RST};
const CHIP_CCU_CLK_T VIRT_GPT_TMR_t::clk_num[] = {CLK_MX_TIMER0, CLK_MX_TIMER0, CLK_MX_TIMER3, CLK_MX_TIMER3};
const IRQn_Type VIRT_GPT_TMR_t::GPT_IRQn[] = {TIMER0_IRQn, TIMER0_IRQn, TIMER3_IRQn, TIMER3_IRQn};
const uint32_t VIRT_GPT_TMR_t::INT_PRIOR[] = {TIMER0_interrupt_priority, TIMER0_interrupt_priority, TIMER3_interrupt_priority,
		TIMER3_interrupt_priority};
#endif

VIRT_GPT_TMR_t* VIRT_GPT_TMR_t::VIRT_GPT_TMR_ptrs[Num_of_main_timers][Num_of_sub_timers] = {
		(VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL,
		(VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL,
		(VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL,
		(VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL, (VIRT_GPT_TMR_t*)NULL,};


VIRT_GPT_TMR_t* VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(uint8_t Main_tmr_num, uint8_t Sub_tmr_num)
{
	if(Main_tmr_num < Num_of_main_timers)
	{
#ifdef CORE_M0
		if((Main_tmr_num!=0)&&(Main_tmr_num!=3)) return (VIRT_GPT_TMR_t*)NULL;
#endif
		if(Sub_tmr_num < Num_of_sub_timers)
		{
			if(VIRT_GPT_TMR_ptrs[Main_tmr_num][Sub_tmr_num] == (VIRT_GPT_TMR_t*)NULL)
				VIRT_GPT_TMR_ptrs[Main_tmr_num][Sub_tmr_num] = new VIRT_GPT_TMR_t(Main_tmr_num, Sub_tmr_num);
			return VIRT_GPT_TMR_ptrs[Main_tmr_num][Sub_tmr_num];
		}
	}
	return (VIRT_GPT_TMR_t*)NULL;
}

VIRT_GPT_TMR_t::VIRT_GPT_TMR_t(uint8_t Main_tmr_num, uint8_t Sub_tmr_num):VOID_TIMER_t()
{
	this->Main_tmr_num = Main_tmr_num;
	this->Sub_tmr_num = Sub_tmr_num;
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<(this->GPT_IRQn[this->Main_tmr_num]));
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void VIRT_GPT_TMR_t::IRQ(int8_t IRQ_num)
{
	uint32_t current_counter;

	if (Chip_TIMER_MatchPending(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num))
	{
		Chip_TIMER_ClearMatch(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num);
		current_counter = this->TMR_ptr[this->Main_tmr_num]->TC;
		Chip_TIMER_SetMatch(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num, (current_counter + this->VTMR_period));
		if(this->callback != (VOID_CALLBACK_t)NULL) this->callback();
		else if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
	}
}

void VIRT_GPT_TMR_t::Initialize(uint32_t us)
{
#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
#endif

	Chip_TIMER_Init(this->TMR_ptr[this->Main_tmr_num]);
	#ifdef CORE_M0
	NVIC_SetPriority(this->GPT_IRQn[this->Main_tmr_num], this->INT_PRIOR[this->Main_tmr_num]);
	#endif
	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(this->GPT_IRQn[this->Main_tmr_num], NVIC_EncodePriority(prioritygroup, this->INT_PRIOR[this->Main_tmr_num], 0));
	#endif
	NVIC_EnableIRQ(this->GPT_IRQn[this->Main_tmr_num]);
	this->VTMR_period = (Chip_Clock_GetRate(this->clk_num[this->Main_tmr_num])/1000000)*us;
	Chip_TIMER_Enable(this->TMR_ptr[this->Main_tmr_num]);
}

void VIRT_GPT_TMR_t::Start(void)
{
	this->Reset();
	Chip_TIMER_MatchEnableInt(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num);
}

void VIRT_GPT_TMR_t::Stop(void)
{
	Chip_TIMER_MatchDisableInt(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num);
}

void VIRT_GPT_TMR_t::Reset(void)
{
	uint32_t current_counter;
	current_counter = this->TMR_ptr[this->Main_tmr_num]->TC;
	Chip_TIMER_SetMatch(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num, (current_counter + this->VTMR_period));
}

uint32_t VIRT_GPT_TMR_t::GetCounter(void)
{
	uint32_t Main_current_counter;
	uint32_t current_counter;
	Main_current_counter = this->TMR_ptr[this->Main_tmr_num]->TC;
	current_counter = this->VTMR_period - (this->TMR_ptr[this->Main_tmr_num]->MR[this->Sub_tmr_num] - Main_current_counter);
	return current_counter;
}

void VIRT_GPT_TMR_t::SetCounter(uint32_t count)
{
	uint32_t current_counter;
	current_counter = this->TMR_ptr[this->Main_tmr_num]->TC;
	Chip_TIMER_SetMatch(this->TMR_ptr[this->Main_tmr_num], this->Sub_tmr_num, (current_counter + (this->VTMR_period - count)));
}

void VIRT_GPT_TMR_t::AddCall(VOID_CALLBACK_t IntCallback)
{
	if(this->callback_intrf_ptr == (Callback_Interface_t*)NULL) this->callback = IntCallback;
}

void VIRT_GPT_TMR_t::AddCall(Callback_Interface_t* IntCallback)
{
	if(this->callback == (VOID_CALLBACK_t)NULL) this->callback_intrf_ptr = IntCallback;
}

void VIRT_GPT_TMR_t::DeleteCall(void)
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}

void VIRT_GPT_TMR_t::Deinitialize(void)
{
	uint32_t temp = 0;

	this->Stop();
	for(int i=0; i<Num_of_main_timers; i++) temp|= ((this->TMR_ptr[this->Main_tmr_num]->MCR)&TIMER_INT_ON_MATCH(this->Sub_tmr_num));
	if(temp == 0)
	{
		Chip_TIMER_Disable(this->TMR_ptr[this->Main_tmr_num]);
		NVIC_DisableIRQ(this->GPT_IRQn[this->Main_tmr_num]);
		Chip_RGU_TriggerReset(this->ResetNumber[this->Main_tmr_num]);
		while (Chip_RGU_InReset(this->ResetNumber[this->Main_tmr_num])) {}
		Chip_TIMER_DeInit(this->TMR_ptr[this->Main_tmr_num]);
	}
}



