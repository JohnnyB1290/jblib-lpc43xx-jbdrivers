/*
 * CORE_LOAD.cpp
 *
 *  Created on: 24.10.2017
 *      Author: Stalker1290
 */

#include "CORE_LOAD.hpp"
#include "stdio.h"
#include "Defines.h"

CORE_LOAD_t* CORE_LOAD_t::Core_Load_ptr = (CORE_LOAD_t*)NULL;

CORE_LOAD_t* CORE_LOAD_t::Get_core_load_module(void)
{
	if(CORE_LOAD_t::Core_Load_ptr == (CORE_LOAD_t*)NULL) CORE_LOAD_t::Core_Load_ptr = new CORE_LOAD_t();
	return CORE_LOAD_t::Core_Load_ptr;
}

CORE_LOAD_t::CORE_LOAD_t(void):Callback_Interface_t()
{
	this->Timer_ptr = (VOID_TIMER_t*)NULL;
	this->initialized = 0;
	this->load_percents = 0;
	this->accuracy = 0;
	this->work_time_tics = 0;
	this->measure_step_ticks = 0;
	this->enter_point_counter = 0;
	this->work_mask = 0;
	this->while_enter_point_counter = 0;
	this->timer_irq_mask = 0;
	this->while_load = 0;
#ifdef USE_CONSOLE
#ifdef CORE_LOAD_mes_console
	this->printf_counter = 0;
	this->average_load = 0;
#endif
#endif
}

void CORE_LOAD_t::Initialize(VOID_TIMER_t* Base_timer_ptr)
{
	this->timer_irq_mask = Base_timer_ptr->getCode();
	this->accuracy = CORE_LOAD_accuracy_percent;
	this->measure_step_ticks = (SystemCoreClock/1000)*CORE_LOAD_measure_step_ms;
	this->work_time_tics = 0;
	this->enter_point_counter = 0;
	this->work_mask = 0;
	this->Timer_ptr = Base_timer_ptr;
	this->Timer_ptr->Initialize(CORE_LOAD_measure_step_ms*1000);
	this->Timer_ptr->AddCall(this);
	this->Timer_ptr->Start();
	this->initialized = 1;
}

void CORE_LOAD_t::Enter_IRQ(int8_t IRQ_num)
{
	if(this->initialized)
	{
		if(this->timer_irq_mask == ((uint64_t)1<<IRQ_num)) return;
		if(this->work_mask == 0)
		{
			this->enter_point_counter = this->Timer_ptr->GetCounter();
			this->while_load = this->Timer_ptr->GetCounter() - this->while_enter_point_counter;
		}
		this->work_mask |= ((uint64_t)1<<IRQ_num);
	}
}

void CORE_LOAD_t::Exit_IRQ(int8_t IRQ_num)
{
	if(this->initialized)
	{
		if(this->timer_irq_mask == ((uint64_t)1<<IRQ_num)) return;
		this->work_mask &= ~((uint64_t)1<<IRQ_num);
		if(this->work_mask == 0)
		{
			this->work_time_tics += (this->Timer_ptr->GetCounter() - this->enter_point_counter);
			this->while_enter_point_counter = this->Timer_ptr->GetCounter();
		}
	}
}

void CORE_LOAD_t::Enter_while(void)
{
	if(this->initialized)
	{
		this->while_load = 0;
		this->while_enter_point_counter = this->Timer_ptr->GetCounter();
	}
}

void CORE_LOAD_t::Exit_while(void)
{
	static uint32_t while_exit_point_counter;
	if(this->initialized)
	{
		while_exit_point_counter = this->Timer_ptr->GetCounter();
		if(while_exit_point_counter >= this->while_enter_point_counter)
		{
			this->while_load += (while_exit_point_counter - this->while_enter_point_counter);
		}
		if(((this->while_load*100)/this->measure_step_ticks)>=this->accuracy)
		{
			this->work_time_tics += this->while_load;
		}
	}
}

uint32_t CORE_LOAD_t::Get_load_percent(void)
{
	return this->load_percents;
}

void CORE_LOAD_t::void_callback(void* Intf_ptr, void* parameters)
{
	this->load_percents = (this->work_time_tics*100)/this->measure_step_ticks;
	if (this->load_percents>=100)
	{
		this->load_percents = 100;
	}
#ifdef USE_CONSOLE
#ifdef CORE_LOAD_mes_console
	this->printf_counter++;
	this->average_load += this->load_percents;
	if(this->printf_counter == CORE_LOAD_printf_divider)
	{
		this->average_load = this->average_load/CORE_LOAD_printf_divider;
		printf("CPU LOAD = %i \n\r",this->average_load);
		this->printf_counter = 0;
		this->average_load = 0;
	}
#endif
#endif
	this->enter_point_counter = 0;
	this->work_time_tics = 0;
	this->Timer_ptr->Reset();
}


