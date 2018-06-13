/*
 * Time_Engine.cpp
 *
 *  Created on: 25.10.2017
 *      Author: Stalker1290
 */

#include "Time_Engine.hpp"
#include "CONTROLLER.hpp"

Time_Engine_t* Time_Engine_t::Time_Engine_ptr = (Time_Engine_t*)NULL;


Time_Engine_t* Time_Engine_t::Get_Time_Engine(void)
{
	if(Time_Engine_t::Time_Engine_ptr == (Time_Engine_t*)NULL) Time_Engine_t::Time_Engine_ptr = new Time_Engine_t();
	return Time_Engine_t::Time_Engine_ptr;
}

Time_Engine_t::Time_Engine_t(void):Callback_Interface_t()
{
	for(uint8_t i = 0; i<Num_of_matches; i++)
	{
		this->RT_Callbacks_ptrs[i] = (Callback_Interface_t*)NULL;
		this->RT_event_match_ticks[i] = 0;
		this->RT_Call_data[i] = NULL;
	}

	for(uint8_t i = 0; i<TE_num_of_NRT; i++)
	{
		for(uint8_t j = 0; j<Event_TMR_size_of_events; j++)
		{
			this->Event_timer_data_instances[i][j].Out_data_ptr = NULL;
			this->Event_timer_data_instances[i][j].Out_call_ptr = (Callback_Interface_t*)NULL;
			this->Event_timer_data_instances[i][j].Desc_FREE = true;
		}
	}

	this->RT_Timer_ptr = CONTROLLER_t::get_Timer_GPTimer_1();
	this->RT_Timer_ptr->Initialize();
	this->RT_Timer_ptr->Start();

	this->Event_tmr_ptrs[Lowest_priority_delay] = new Event_timer_t(CONTROLLER_t::get_Timer_void_PWMTimer_0());
	this->Event_tmr_ptrs[Lowest_priority_delay]->Initialize(Lowest_priority_delay_timer_step_us);
	this->Event_tmr_ptrs[Lowest_priority_delay]->Start();

	this->Event_tmr_ptrs[Medium_priority_delay] = new Event_timer_t(CONTROLLER_t::get_Timer_void_SYS_TICK());
	this->Event_tmr_ptrs[Medium_priority_delay]->Initialize(Medium_priority_delay_timer_step_us);
	this->Event_tmr_ptrs[Medium_priority_delay]->Start();

	this->Event_tmr_ptrs[Kernel_performer] = new Kernel_Performer_tmr_t(CONTROLLER_t::get_Timer_void_SCTTimer());
	this->Event_tmr_ptrs[Kernel_performer]->Initialize(Kernel_performer_timer_step_us);
	this->Event_tmr_ptrs[Kernel_performer]->Start();
}

uint32_t Time_Engine_t::RT_getCS(void)
{
	if(this->RT_Timer_ptr == (GPTimer_t*)NULL) return 0;
	return this->RT_Timer_ptr->GetCounter();
}

void Time_Engine_t::RT_setCS(uint32_t ticks)
{
	if(this->RT_Timer_ptr == (GPTimer_t*)NULL) return;

	this->RT_Timer_ptr->SetCounter(ticks);
}

void Time_Engine_t::RT_setEvent(uint32_t match_ticks, Callback_Interface_t* call, void* data)
{
	if(this->RT_Timer_ptr == (GPTimer_t*)NULL) return;

	for(uint8_t i = 0; i<Num_of_matches; i++)
	{
		if(this->RT_Callbacks_ptrs[i] == (Callback_Interface_t*)NULL)
		{
			this->RT_Call_data[i] = data;
			this->RT_event_match_ticks[i] = match_ticks;
			this->RT_Callbacks_ptrs[i] = call;
			this->RT_Timer_ptr->SetMatch_call(i, this);
			this->RT_Timer_ptr->SetMatch_ticks(i, match_ticks, false);
			break;
		}
	}
}

void Time_Engine_t::NRT_setEvent(TE_module_num_t module, uint32_t us, Callback_Interface_t* call, void* data)
{
	TE_Even_timer_data_t* Event_timer_data_ptr = (TE_Even_timer_data_t*)NULL;

	for(uint8_t i = 0; i<Event_TMR_size_of_events; i++)
	{
		if(this->Event_timer_data_instances[module][i].Desc_FREE == true)
		{
			this->Event_timer_data_instances[module][i].Desc_FREE = false;
			Event_timer_data_ptr = &this->Event_timer_data_instances[module][i];
			break;
		}
	}
	if(Event_timer_data_ptr == (TE_Even_timer_data_t*)NULL)	return;

	Event_timer_data_ptr->Out_call_ptr = call;
	Event_timer_data_ptr->Out_data_ptr = data;
	this->Event_tmr_ptrs[module]->Add_OneTime_Event(us, this, (void*)Event_timer_data_ptr);
}


void Time_Engine_t::void_callback(void* Intf_ptr, void* parameters)
{
	this->output_param.data = NULL;
	this->output_param.RT_Timer_moment = 0;
	this->output_param.RT_match_moment = 0;

	if(this->RT_Timer_ptr == (GPTimer_t*)Intf_ptr)
	{
		if(this->RT_Timer_ptr == (GPTimer_t*)NULL) return;

		uint8_t Event_num = *((uint8_t*)parameters);
		Callback_Interface_t* Temp_callback;

		this->RT_Timer_ptr->DeleteMatch(Event_num);
		this->RT_Timer_ptr->DeleteMatch_call(Event_num);

		if(this->RT_Callbacks_ptrs[Event_num] != (Callback_Interface_t*)NULL)
		{
			Temp_callback = this->RT_Callbacks_ptrs[Event_num];
			this->output_param.TE_module_num = RT_module_n;
			this->output_param.data = this->RT_Call_data[Event_num];
			this->output_param.RT_match_moment = this->RT_event_match_ticks[Event_num];
			this->output_param.RT_Timer_moment = this->RT_Timer_ptr->GetCounter();

			this->RT_Callbacks_ptrs[Event_num] = (Callback_Interface_t*)NULL;
			this->RT_Call_data[Event_num] = NULL;
			this->RT_event_match_ticks[Event_num] = 0;

			Temp_callback->void_callback(this,(void*)&this->output_param);
		}
	}

	for(uint8_t i = 0; i<TE_num_of_NRT; i++)
	{
		if(this->Event_tmr_ptrs[i] == (Event_timer_t*)Intf_ptr)
		{
			TE_Even_timer_data_t* Event_timer_data_ptr = (TE_Even_timer_data_t*)parameters;
			Callback_Interface_t* temp_call = Event_timer_data_ptr->Out_call_ptr;

			this->output_param.TE_module_num = (TE_module_num_t)i;
			this->output_param.data = Event_timer_data_ptr->Out_data_ptr;
			this->output_param.RT_Timer_moment = this->RT_Timer_ptr->GetCounter();
			for(uint8_t j = 0; j<Event_TMR_size_of_events; j++)
			{
				if(Event_timer_data_ptr == &this->Event_timer_data_instances[i][j])
				{
					this->Event_timer_data_instances[i][j].Out_data_ptr = NULL;
					this->Event_timer_data_instances[i][j].Out_call_ptr = (Callback_Interface_t*)NULL;
					this->Event_timer_data_instances[i][j].Desc_FREE = true;
					break;
				}
			}
			if(temp_call != (Callback_Interface_t*)NULL) temp_call->void_callback(this, (void*)&this->output_param);
			break;
		}
	}
}
