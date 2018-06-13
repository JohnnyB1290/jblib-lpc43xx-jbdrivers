/*
 * Time_Engine.hpp
 *
 *  Created on: 25.10.2017
 *      Author: Stalker1290
 */

#ifndef TIME_ENGINE_HPP_
#define TIME_ENGINE_HPP_

#include "chip.h"
#include "Common_interfaces.hpp"
#include "GPTimers.hpp"
#include "Event_timer.hpp"
#include "Kernel_Performer_TMR.hpp"
#include "stdlib.h"

#define TE_num_of_NRT 3

typedef enum
{
	Lowest_priority_delay = 0,
	Medium_priority_delay = 1,
	Kernel_performer = 2,
	RT_module_n = 128,
}TE_module_num_t;

typedef struct Time_engine_callback_param_struct
{
	TE_module_num_t TE_module_num;
	void* data;
	uint32_t RT_Timer_moment;
	uint32_t RT_match_moment;
}Time_engine_callback_param_t;

class Time_Engine_t:public Callback_Interface_t
{
public:
	static Time_Engine_t* Get_Time_Engine(void);
/**************************RT Module******************************/
	uint32_t RT_getCS(void);
	void RT_setCS(uint32_t ticks);
	void RT_setEvent(uint32_t match_ticks, Callback_Interface_t* call, void* data);
/*****************************************************************/

/*************************Delay and Performer*********************/
	void NRT_setEvent(TE_module_num_t module, uint32_t us, Callback_Interface_t* call, void* data);
/*****************************************************************/
	virtual void void_callback(void* Intf_ptr, void* parameters);
private:
	Time_Engine_t(void);
	static Time_Engine_t* Time_Engine_ptr;
	Time_engine_callback_param_t output_param;

/**************************RT Module******************************/
	GPTimer_t* RT_Timer_ptr;
	Callback_Interface_t* RT_Callbacks_ptrs[Num_of_matches];
	void* RT_Call_data[Num_of_matches];
	uint32_t RT_event_match_ticks[Num_of_matches];
/*****************************************************************/
	typedef struct TE_Even_timer_data_struct
	{
		Callback_Interface_t* Out_call_ptr;
		void* Out_data_ptr;
		bool Desc_FREE;
	}TE_Even_timer_data_t;

/**************************NRT Modules***************************/
	Event_timer_t* Event_tmr_ptrs[TE_num_of_NRT];
	TE_Even_timer_data_t Event_timer_data_instances[TE_num_of_NRT][Event_TMR_size_of_events];

/*****************************************************************/
};



#endif /* TIME_ENGINE_HPP_ */
