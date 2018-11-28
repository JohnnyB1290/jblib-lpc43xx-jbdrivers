/*
 * GPTimers_GPT.cpp
 *
 *  Created on: 28 но€б. 2018 г.
 *      Author: Stalker1290
 */

#include "GPTimers_GPT.hpp"

#ifdef CORE_M4
LPC_TIMER_T* GPTimersGPT_t::lpcTmrPtr[] = {LPC_TIMER0, LPC_TIMER1, LPC_TIMER2, LPC_TIMER3};
const CHIP_RGU_RST_T GPTimersGPT_t::resetNumber[] = {RGU_TIMER0_RST, RGU_TIMER1_RST, RGU_TIMER2_RST, RGU_TIMER3_RST};
const CHIP_CCU_CLK_T GPTimersGPT_t::clkNum[] = {CLK_MX_TIMER0, CLK_MX_TIMER1, CLK_MX_TIMER2, CLK_MX_TIMER3};
const IRQn_Type GPTimersGPT_t::gptIRQn[] = {TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn};
const uint32_t GPTimersGPT_t::intPrior[] = {TIMER0_interrupt_priority, TIMER1_interrupt_priority, TIMER2_interrupt_priority,
		TIMER3_interrupt_priority};
#endif

#ifdef CORE_M0

#ifdef CORE_M0APP
LPC_TIMER_T* GPTimersGPT_t::lpcTmrPtr[] = {LPC_TIMER0, LPC_TIMER0, LPC_TIMER3, LPC_TIMER3};
const CHIP_RGU_RST_T GPTimersGPT_t::resetNumber[] = {RGU_TIMER0_RST, RGU_TIMER0_RST, RGU_TIMER3_RST, RGU_TIMER3_RST};
const CHIP_CCU_CLK_T GPTimersGPT_t::clkNum[] = {CLK_MX_TIMER0, CLK_MX_TIMER0, CLK_MX_TIMER3, CLK_MX_TIMER3};
const IRQn_Type GPTimersGPT_t::gptIRQn[] = {TIMER0_IRQn, TIMER0_IRQn, TIMER3_IRQn, TIMER3_IRQn};
const uint32_t GPTimersGPT_t::intPrior[] = {TIMER0_interrupt_priority, TIMER0_interrupt_priority, TIMER3_interrupt_priority,
		TIMER3_interrupt_priority};
#endif

#ifdef CORE_M0SUB
LPC_TIMER_T* GPTimersGPT_t::lpcTmrPtr[] = {LPC_TIMER1, LPC_TIMER1, LPC_TIMER2, LPC_TIMER2};
const CHIP_RGU_RST_T GPTimersGPT_t::resetNumber[] = {RGU_TIMER1_RST, RGU_TIMER1_RST, RGU_TIMER2_RST, RGU_TIMER2_RST};
const CHIP_CCU_CLK_T GPTimersGPT_t::clkNum[] = {CLK_MX_TIMER1, CLK_MX_TIMER1, CLK_MX_TIMER2, CLK_MX_TIMER2};
const IRQn_Type GPTimersGPT_t::gptIRQn[] = {TIMER1_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER2_IRQn};
const uint32_t GPTimersGPT_t::intPrior[] = {TIMER1_interrupt_priority, TIMER1_interrupt_priority, TIMER2_interrupt_priority,
		TIMER2_interrupt_priority};
#endif

#endif

GPTimersGPT_t* GPTimersGPT_t::gpTimersGPTPtrs[Num_of_GP_timers] = {NULL, NULL, NULL, NULL};

GPTimersGPT_t* GPTimersGPT_t::getGPTimerGPT(uint8_t timerNum){

	if(timerNum < Num_of_GP_timers)
	{
#ifdef CORE_M0
#ifdef CORE_M0APP
		if((timerNum!=0)&&(timerNum!=3)) return (GPTimersGPT_t*)NULL;
#endif

#ifdef CORE_M0SUB
		if((timerNum!=1)&&(timerNum!=2)) return (GPTimersGPT_t*)NULL;
#endif

#endif

		if(GPTimersGPT_t::gpTimersGPTPtrs[timerNum] == (GPTimersGPT_t*)NULL)
			GPTimersGPT_t::gpTimersGPTPtrs[timerNum] = new GPTimersGPT_t(timerNum);
		return GPTimersGPT_t::gpTimersGPTPtrs[timerNum];
	}
	return (GPTimersGPT_t*)NULL;
}

GPTimersGPT_t::GPTimersGPT_t(uint8_t timerNum):VOID_TIMER_t(){

	this->timerNum = timerNum;
	this->callback = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<(GPTimersGPT_t::gptIRQn[this->timerNum]));
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void GPTimersGPT_t::IRQ(int8_t IRQ_num)
{
	if (Chip_TIMER_MatchPending(GPTimersGPT_t::lpcTmrPtr[this->timerNum], 0))
	{
		Chip_TIMER_ClearMatch(GPTimersGPT_t::lpcTmrPtr[this->timerNum], 0);
		if(this->callback != (Callback_Interface_t*)NULL)
			this->callback->void_callback((void*)this, NULL);
	}
}
void GPTimersGPT_t::Initialize(uint32_t us){
	uint32_t GPT_tics;

	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif

	Chip_TIMER_Init(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
	Chip_RGU_TriggerReset(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
	while (Chip_RGU_InReset(GPTimersGPT_t::resetNumber[this->timerNum])) {}
	Chip_TIMER_Reset(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);

	#ifdef CORE_M0
	NVIC_SetPriority(GPTimersGPT_t::gptIRQn[this->timerNum], GPTimersGPT_t::intPrior[this->timerNum]);
	#endif
	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(GPTimersGPT_t::gptIRQn[this->timerNum], NVIC_EncodePriority(prioritygroup, GPTimersGPT_t::intPrior[this->timerNum], 0));
	#endif

	GPT_tics = (Chip_Clock_GetRate(GPTimersGPT_t::clkNum[this->timerNum]) / 1000000) * us;
	Chip_TIMER_SetMatch(GPTimersGPT_t::lpcTmrPtr[this->timerNum], 0, GPT_tics);
	Chip_TIMER_ResetOnMatchEnable(GPTimersGPT_t::lpcTmrPtr[this->timerNum], 0);
	Chip_TIMER_MatchEnableInt(GPTimersGPT_t::lpcTmrPtr[this->timerNum], 0);

	NVIC_ClearPendingIRQ(GPTimersGPT_t::gptIRQn[this->timerNum]);
	NVIC_EnableIRQ(GPTimersGPT_t::gptIRQn[this->timerNum]);
}

void GPTimersGPT_t::Start(void){
	Chip_TIMER_Enable(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
}

void GPTimersGPT_t::Stop(void){
	Chip_TIMER_Disable(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
}

void GPTimersGPT_t::Reset(void){
	Chip_TIMER_Reset(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
}

uint32_t GPTimersGPT_t::GetCounter(void){
	return GPTimersGPT_t::lpcTmrPtr[this->timerNum]->TC;
}

void GPTimersGPT_t::SetCounter(uint32_t count){
	GPTimersGPT_t::lpcTmrPtr[this->timerNum]->TC = count;
}

void GPTimersGPT_t::AddCall(Callback_Interface_t* IntCallback){
	this->callback = IntCallback;
}

void GPTimersGPT_t::DeleteCall(void){
	this->callback = (Callback_Interface_t*)NULL;
}

void GPTimersGPT_t::Deinitialize(void){
	this->Stop();
	NVIC_DisableIRQ(GPTimersGPT_t::gptIRQn[this->timerNum]);
	Chip_RGU_TriggerReset(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
	while (Chip_RGU_InReset(GPTimersGPT_t::resetNumber[this->timerNum])) {}
	Chip_TIMER_DeInit(GPTimersGPT_t::lpcTmrPtr[this->timerNum]);
}

