/*
 * Sys_tick_GPT.c
 *
 *  Created on: 01.02.2017
 *      Author: Stalker1290
 */

#include "SYS_TICK_GPT.hpp"

SYS_TICK_t& SYS_TICK_t::getSYS_TICK(void)
{
	static SYS_TICK_t Instance;
	return Instance;
}

SYS_TICK_t::SYS_TICK_t(void):VOID_TIMER_t()
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<(-SysTick_IRQn));
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Cortex_IRQ_Listener(this);
}

void SYS_TICK_t::Initialize(uint32_t us)
{
	volatile uint32_t cmp_value;

	#ifdef CORE_M4
	uint32_t prioritygroup = 0;
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, SYSTICK_interrupt_priority, 0));
	#endif

	/* Determine aapproximate compare value based on clock rate and passed interval */
	cmp_value = (SystemCoreClock / 1000000) * us;

	SysTick->LOAD  = cmp_value - 1;                                  /* set reload register */
	SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;

}

void SYS_TICK_t::Start(void)
{
	SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk;
}

void SYS_TICK_t::Stop(void)
{
	SysTick->CTRL  &= (~SysTick_CTRL_ENABLE_Msk);
}

void SYS_TICK_t::Reset(void)
{
	if(SysTick->CTRL&SysTick_CTRL_ENABLE_Msk)
	{
		this->Stop();
		SysTick->VAL = 0;
		this->Start();
	}
	else
	{
		SysTick->VAL = 0;
	}
}

uint32_t SYS_TICK_t::GetCounter(void)
{
	return SysTick->VAL;
}

void SYS_TICK_t::SetCounter(uint32_t count)
{
	if(SysTick->CTRL&SysTick_CTRL_ENABLE_Msk)
	{
		this->Stop();
		SysTick->VAL = count;
		this->Start();
	}
	else
	{
		SysTick->VAL = count;
	}
}

void SYS_TICK_t::AddCall(VOID_CALLBACK_t IntCallback)
{
	if(this->callback == (VOID_CALLBACK_t)NULL) this->callback = IntCallback;
}

void SYS_TICK_t::AddCall(Callback_Interface_t* IntCallback)
{
	if(this->callback_intrf_ptr == (Callback_Interface_t*)NULL) this->callback_intrf_ptr = IntCallback;
}

void SYS_TICK_t::DeleteCall(void)
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}

void SYS_TICK_t::Deinitialize(void)
{
	this->Stop();
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Delete_Cortex_IRQ_Listener(this);
	this->Reset();
	this->callback = (VOID_CALLBACK_t)NULL;
}

void SYS_TICK_t::IRQ(int8_t IRQ_num)
{
	if(this->callback != (VOID_CALLBACK_t)NULL) this->callback();
	else if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
}
