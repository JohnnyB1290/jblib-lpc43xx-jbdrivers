/*
 * SCT_GPT.c
 *
 *  Created on: 24.01.2017
 *      Author: Stalker1290
 */


#include "SCT_GPT.hpp"


SCT_TIMER_t& SCT_TIMER_t::getSCT_TIMER(void)
{
	static SCT_TIMER_t Instance;
	return Instance;
}

SCT_TIMER_t::SCT_TIMER_t(void):VOID_TIMER_t()
{
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<SCT_IRQn);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void SCT_TIMER_t::IRQ(int8_t IRQ_num)
{
	if(LPC_SCT->EVFLAG&1)
	{
		Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
		if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
	}
	NVIC_ClearPendingIRQ(SCT_IRQn);
}

void SCT_TIMER_t::Initialize(uint32_t us)
{
	uint32_t period;

	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif
	Chip_SCT_Init(LPC_SCT);
	Chip_RGU_TriggerReset(RGU_SCT_RST);
	while (Chip_RGU_InReset(RGU_SCT_RST)) {}
	Chip_SCT_Config(LPC_SCT,SCT_CONFIG_32BIT_COUNTER|SCT_CONFIG_CLKMODE_BUSCLK|SCT_CONFIG_AUTOLIMIT_L|SCT_CONFIG_NORELOADL_U);

	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(SCT_IRQn, NVIC_EncodePriority(prioritygroup, SCT_TIMER_interrupt_priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(SCT_IRQn, SCT_TIMER_interrupt_priority);
	#endif

	period = (Chip_Clock_GetRate(CLK_MX_SCT) / 1000000);
	period = period*us;

	Chip_SCT_SetMatchCount(LPC_SCT, SCT_MATCH_0, period);
	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_0);
	LPC_SCT->EVENT[0].CTRL = 1|(1<<12); //EVENT0 == MATCH0, use match only
	LPC_SCT->EVENT[0].STATE = 1; //EVENT0 in STATE0

	NVIC_ClearPendingIRQ(SCT_IRQn);
	NVIC_EnableIRQ(SCT_IRQn);
}

void SCT_TIMER_t::Start(void)
{
	Chip_SCT_ClearControl(LPC_SCT, SCT_CTRL_HALT_L);
}

void SCT_TIMER_t::Stop(void)
{
	Chip_SCT_SetControl(LPC_SCT, SCT_CTRL_HALT_L);
}

void SCT_TIMER_t::Reset(void)
{
	if(LPC_SCT->CTRL_U&SCT_CTRL_HALT_L)
	{
		Chip_SCT_SetControl(LPC_SCT, SCT_CTRL_CLRCTR_L);
	}
	else
	{
		this->Stop();
		Chip_SCT_SetControl(LPC_SCT, SCT_CTRL_CLRCTR_L);
		this->Start();
	}
}

uint32_t SCT_TIMER_t::GetCounter(void)
{
	return LPC_SCT->COUNT_U;
}

void SCT_TIMER_t::SetCounter(uint32_t count)
{
	if(LPC_SCT->CTRL_U&SCT_CTRL_HALT_L)
	{
		Chip_SCT_SetCount(LPC_SCT, count);
	}
	else
	{
		this->Stop();
		Chip_SCT_SetCount(LPC_SCT, count);
		this->Start();
	}
}

void SCT_TIMER_t::AddCall(Callback_Interface_t* IntCallback)
{
	this->callback_intrf_ptr = IntCallback;
}

void SCT_TIMER_t::DeleteCall(void)
{
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	Chip_SCT_DisableEventInt(LPC_SCT, SCT_EVT_0);
	LPC_SCT->EVENT[0].CTRL = 0;
	LPC_SCT->EVENT[0].STATE = 0;
	NVIC_DisableIRQ(SCT_IRQn);
	NVIC_ClearPendingIRQ(SCT_IRQn);
}

void SCT_TIMER_t::Deinitialize(void)
{
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Delete_Peripheral_IRQ_Listener(this);
	this->Stop();
	NVIC_DisableIRQ(SCT_IRQn);
	Chip_RGU_TriggerReset(RGU_SCT_RST);
	while (Chip_RGU_InReset(RGU_SCT_RST)) {}
	Chip_SCT_DeInit(LPC_SCT);
}

