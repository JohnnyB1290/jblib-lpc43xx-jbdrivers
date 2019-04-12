/*
 * CORE_LOAD.hpp
 *
 *  Created on: 24.10.2017
 *      Author: Stalker1290
 */

#include "chip.h"
#include "VOIDTMR.hpp"
#include "Common_interfaces.hpp"

#ifndef CORE_LOAD_HPP_
#define CORE_LOAD_HPP_


class CORE_LOAD_t:public Callback_Interface_t
{
public:
	static CORE_LOAD_t* Get_core_load_module(void);
	void Initialize(VOID_TIMER_t* Base_timer_ptr);
	void Enter_IRQ(int8_t IRQ_num);
	void Exit_IRQ(int8_t IRQ_num);
	void Enter_while(void);
	void Exit_while(void);
	uint32_t Get_load_percent(void);
	virtual void void_callback(void* Intf_ptr, void* parameters);
private:
	static CORE_LOAD_t* Core_Load_ptr;
	CORE_LOAD_t(void);
	VOID_TIMER_t* Timer_ptr;
	uint8_t initialized; //= 0;
	uint32_t load_percents;// = 0;
	uint8_t accuracy; //= 0;
	uint32_t work_time_tics;// = 0;
	uint32_t measure_step_ticks;// = 0;
	uint32_t enter_point_counter;// = 0;
	uint64_t work_mask;// = 0;
	uint32_t while_enter_point_counter;// = 0;
	uint64_t timer_irq_mask;// = 0;
	uint32_t while_load;
	#ifdef USE_CONSOLE
	#ifdef CORE_LOAD_mes_console
	uint8_t printf_counter;
	uint32_t average_load;
	#endif
	#endif
};


#endif /* CORE_LOAD_HPP_ */
