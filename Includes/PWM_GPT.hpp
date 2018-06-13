/*
 * PWM_GPT.hpp
 *
 *  Created on: 28.07.2017
 *      Author: Stalker1290
 */

#ifndef PWM_GPT_HPP_
#define PWM_GPT_HPP_

#define Num_of_pwm_tmr 3

#include "VOIDTMR.hpp"

class PWM_TIMER_t:public VOID_TIMER_t
{
public:
	static PWM_TIMER_t* get_PWM_Timer(uint8_t timer_num);
	virtual void Initialize(uint32_t us);
	virtual void Start(void);
	virtual void Stop(void);
	virtual void Reset(void);
	virtual uint32_t GetCounter(void);
	virtual void SetCounter(uint32_t count);
	virtual void AddCall(VOID_CALLBACK_t IntCallback);
	virtual void AddCall(Callback_Interface_t* IntCallback);
	virtual void DeleteCall(void);
	virtual void Deinitialize(void);
private:
	static PWM_TIMER_t* PWM_Timer_ptrs[Num_of_pwm_tmr];
	PWM_TIMER_t(uint8_t timer_num);
	uint8_t timer_num;
	VOID_CALLBACK_t callback;
	Callback_Interface_t* callback_intrf_ptr;
	virtual void IRQ(int8_t IRQ_num);
};


#endif /* PWM_GPT_HPP_ */
