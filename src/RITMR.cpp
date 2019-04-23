/*
 * RITMR.cpp
 *
 *  Created on: 13.07.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "RITMR.hpp"
#include "CONTROLLER.hpp"


RITIMER_t& RITIMER_t::getRITIMER(void)
{
	static RITIMER_t Instance;
	return Instance;
}

RITIMER_t::RITIMER_t(void):VOID_TIMER_t()
{
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<RITIMER_IRQn);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

#include "CONTROLLER.hpp"
void RITIMER_t::IRQ(int8_t IRQ_num)
{
	if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
	Chip_RIT_ClearInt(LPC_RITIMER);
}

void RITIMER_t::Initialize(uint32_t us)
{
	volatile uint32_t cmp_value;
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif

	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_Disable(LPC_RITIMER);
	Chip_RIT_TimerDebugEnable(LPC_RITIMER);

	/* Determine aapproximate compare value based on clock rate and passed interval */
	cmp_value = (Chip_Clock_GetRate(CLK_MX_RITIMER) / 1000000) * us;

	/* Set timer compare value */
	Chip_RIT_SetCOMPVAL(LPC_RITIMER, cmp_value);

	/* Set timer enable clear bit to clear timer to 0 whenever
	   counter value equals the contents of RICOMPVAL */
	Chip_RIT_EnableCTRL(LPC_RITIMER, RIT_CTRL_ENCLR);

	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(RITIMER_IRQn, NVIC_EncodePriority(prioritygroup, RITIMER_interrupt_priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(RITIMER_IRQn, RITIMER_interrupt_priority);
	#endif

	NVIC_ClearPendingIRQ(RITIMER_IRQn);
	NVIC_EnableIRQ(RITIMER_IRQn);
}

void RITIMER_t::Start(void)
{
	Chip_RIT_Enable(LPC_RITIMER);
}

void RITIMER_t::Stop(void)
{
	Chip_RIT_ClearInt(LPC_RITIMER);
	NVIC_ClearPendingIRQ(RITIMER_IRQn);
	Chip_RIT_Disable(LPC_RITIMER);
}

void RITIMER_t::Reset(void)
{
	if(LPC_RITIMER->CTRL&RIT_CTRL_TEN)
	{
		this->Stop();
		LPC_RITIMER->COUNTER = 0;
		this->Start();
	}
	else
	{
		LPC_RITIMER->COUNTER = 0;
	}
}

uint32_t RITIMER_t::GetCounter(void)
{
	return LPC_RITIMER->COUNTER;
}

void RITIMER_t::SetCounter(uint32_t count)
{
	if(LPC_RITIMER->CTRL&RIT_CTRL_TEN)
	{
		this->Stop();
		LPC_RITIMER->COUNTER = count;
		this->Start();
	}
	else
	{
		LPC_RITIMER->COUNTER = count;
	}
}

void RITIMER_t::AddCall(Callback_Interface_t* IntCallback)
{
	this->callback_intrf_ptr = IntCallback;
}

void RITIMER_t::DeleteCall(void)
{
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}

void RITIMER_t::Deinitialize(void)
{
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Delete_Peripheral_IRQ_Listener(this);
	this->Stop();
	NVIC_DisableIRQ(RITIMER_IRQn);
	Chip_RGU_TriggerReset(RGU_RITIMER_RST);
	while (Chip_RGU_InReset(RGU_RITIMER_RST)) {}
	Chip_RIT_DeInit(LPC_RITIMER);
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}
