/*
 * ATIMER.cpp
 *
 *  Created on: 12.07.2017
 *      Author: Stalker1290
 */

#include "ATIMER.hpp"

ATIMER_t& ATIMER_t::getATIMER(void)
{
	static ATIMER_t Instance;
	return Instance;
}

ATIMER_t::ATIMER_t(void):IRQ_LISTENER_t()
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<EVENTROUTER_IRQn);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void ATIMER_t::IRQ(int8_t IRQ_num)
{
	/* Check if source interrupt is ATIMER_t */
	if (Chip_EVRT_IsSourceInterrupting(EVRT_SRC_ATIMER))
	{
		if(this->callback != (VOID_CALLBACK_t)NULL) this->callback();
		else if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
		/* Clear Alarm Timer interrupt status */
		Chip_ATIMER_ClearIntStatus(LPC_ATIMER);
		/* Clear Alarm Timer interrupt flag */
		Chip_EVRT_ClrPendIntSrc(EVRT_SRC_ATIMER);
	}
}

void ATIMER_t::Initialize(uint8_t Period_s)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif

	if(!((LPC_CREG->CREG0 & (1 << 1))&&(LPC_CREG->CREG0 & (1 << 0)))) Chip_Clock_RTCEnable();
	Chip_ATIMER_Init(LPC_ATIMER, Period_s*1023);
	Chip_EVRT_Init();
	/* Enable EVRT in order to be able to read the ATIMER_t interrupt */
	Chip_EVRT_ConfigIntSrcActiveType(EVRT_SRC_ATIMER, EVRT_SRC_ACTIVE_HIGH_LEVEL);
	/* Enable Alarm Timer Source */
	Chip_EVRT_SetUpIntSrc(EVRT_SRC_ATIMER, ENABLE);

	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(EVENTROUTER_IRQn, NVIC_EncodePriority(prioritygroup, EVENTROUTER_interrupt_priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(EVENTROUTER_IRQn, EVENTROUTER_interrupt_priority);
	#endif

	NVIC_ClearPendingIRQ(EVENTROUTER_IRQn);
	NVIC_EnableIRQ(EVENTROUTER_IRQn);
	/* Clear Alarm Timer interrupt status */
	Chip_ATIMER_ClearIntStatus(LPC_ATIMER);
	/* Clear Alarm Timer interrupt flag */
	Chip_EVRT_ClrPendIntSrc(EVRT_SRC_ATIMER);
}


void ATIMER_t::Start(void)
{
	/* Enable Alarm Timer */
	Chip_ATIMER_IntEnable(LPC_ATIMER);
}

void ATIMER_t::Stop(void)
{
	Chip_ATIMER_IntDisable(LPC_ATIMER);
}

void ATIMER_t::Deinitialize(void)
{
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Delete_Peripheral_IRQ_Listener(this);
	NVIC_DisableIRQ(EVENTROUTER_IRQn);
	NVIC_ClearPendingIRQ(EVENTROUTER_IRQn);
	Chip_ATIMER_DeInit(LPC_ATIMER);
	Chip_EVRT_SetUpIntSrc(EVRT_SRC_ATIMER, DISABLE);
	this->callback = (VOID_CALLBACK_t)NULL;
}

void ATIMER_t::Set_call(VOID_CALLBACK_t callback)
{
	if(this->callback_intrf_ptr == (Callback_Interface_t*)NULL) this->callback = callback;
}

void ATIMER_t::Set_call(Callback_Interface_t* callback)
{
	if(this->callback == (VOID_CALLBACK_t)NULL) this->callback_intrf_ptr = callback;
}

void ATIMER_t::Clr_call(void)
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}
