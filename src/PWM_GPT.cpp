/*
 * PWM_GPT.c
 *
 *  Created on: 24.01.2017
 *      Author: Stalker1290
 */

#include "PWM_GPT.hpp"

PWM_TIMER_t* PWM_TIMER_t::PWM_Timer_ptrs[Num_of_pwm_tmr] = {(PWM_TIMER_t*)NULL, (PWM_TIMER_t*)NULL, (PWM_TIMER_t*)NULL };

PWM_TIMER_t* PWM_TIMER_t::get_PWM_Timer(uint8_t timer_num)
{
	if(timer_num < Num_of_pwm_tmr)
	{
		if(PWM_Timer_ptrs[timer_num] == (PWM_TIMER_t*)NULL) PWM_Timer_ptrs[timer_num] = new PWM_TIMER_t(timer_num);
		return PWM_Timer_ptrs[timer_num];
	}
	else return (PWM_TIMER_t*)NULL;
}


PWM_TIMER_t::PWM_TIMER_t(uint8_t timer_num):VOID_TIMER_t()
{
	this->timer_num = timer_num;
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<MCPWM_IRQn);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void PWM_TIMER_t::IRQ(int8_t IRQ_num)
{
	if((LPC_MCPWM->INTF)&(1<<(4*(this->timer_num))))
	{
		LPC_MCPWM->INTF_CLR = 1<<(4*(this->timer_num));
		if(this->callback != (VOID_CALLBACK_t)NULL) this->callback();
		else if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
	}
}


void PWM_TIMER_t::Initialize(uint32_t us)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif
	uint32_t Period;

	Chip_Clock_EnableOpts(CLK_APB1_MOTOCON, true, true, 1);
	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(MCPWM_IRQn, NVIC_EncodePriority(prioritygroup, PWM_TIMER_interrupt_priority, 0));
	#endif
	#ifdef CORE_M0
	NVIC_SetPriority(MCPWM_IRQn, PWM_TIMER_interrupt_priority);
	#endif
	NVIC_EnableIRQ(MCPWM_IRQn);

	Period = Chip_Clock_GetRate(CLK_APB1_MOTOCON)/1000000;
	Period = Period*us;
	LPC_MCPWM->LIM[this->timer_num] = Period;
	LPC_MCPWM->INTEN_SET = 1<<((this->timer_num)*4);
}

void PWM_TIMER_t::Start(void)
{
	LPC_MCPWM->CON_SET = 1<<((this->timer_num)*8);
}

void PWM_TIMER_t::Stop(void)
{
	LPC_MCPWM->CON_CLR = 1<<((this->timer_num)*8);
}

void PWM_TIMER_t::Reset(void)
{
	if(LPC_MCPWM->CON&(1<<((this->timer_num)*8)))
	{
		this->Stop();
		LPC_MCPWM->TC[this->timer_num] = 0;
		this->Start();
	}
	else LPC_MCPWM->TC[this->timer_num] = 0;
}

uint32_t PWM_TIMER_t::GetCounter(void)
{
	return LPC_MCPWM->TC[this->timer_num];
}

void PWM_TIMER_t::SetCounter(uint32_t count)
{
	if(LPC_MCPWM->CON&(1<<((this->timer_num)*8)))
	{
		this->Stop();
		LPC_MCPWM->TC[this->timer_num] = count;
		this->Start();
	}
	else LPC_MCPWM->TC[this->timer_num] = count;
}

void PWM_TIMER_t::AddCall(VOID_CALLBACK_t IntCallback)
{
	if(this->callback_intrf_ptr == (Callback_Interface_t*)NULL) this->callback = IntCallback;
}

void PWM_TIMER_t::AddCall(Callback_Interface_t* IntCallback)
{
	if(this->callback == (VOID_CALLBACK_t)NULL) this->callback_intrf_ptr = IntCallback;
}

void PWM_TIMER_t::DeleteCall(void)
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}

void PWM_TIMER_t::Deinitialize(void)
{
	uint32_t temp = 0;

	this->Stop();
	this->DeleteCall();

	for(int i=0; i<Num_of_pwm_tmr; i++) temp|= LPC_MCPWM->INTEN&(1<<((this->timer_num)*4));
	if(temp == 0)
	{
		NVIC_DisableIRQ(MCPWM_IRQn);
		NVIC_ClearPendingIRQ(MCPWM_IRQn);
		Chip_RGU_TriggerReset(RGU_MOTOCONPWM_RST);
		while (Chip_RGU_InReset(RGU_MOTOCONPWM_RST)) {}
		Chip_Clock_Disable(CLK_APB1_MOTOCON);
	}
}

