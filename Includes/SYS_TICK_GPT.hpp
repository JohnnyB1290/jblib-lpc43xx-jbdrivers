/*
 * SYS_TICK_GPT.hpp
 *
 *  Created on: 14.07.2017
 *      Author: Stalker1290
 */

#ifndef SYS_TICK_GPT_HPP_
#define SYS_TICK_GPT_HPP_

#include "VOIDTMR.hpp"

class SYS_TICK_t:public VOID_TIMER_t
{
public:
	static SYS_TICK_t& getSYS_TICK(void);
	virtual void Initialize(uint32_t us);
	virtual void Start(void);
	virtual void Stop(void);
	virtual void Reset(void);
	virtual uint32_t GetCounter(void);
	virtual void SetCounter(uint32_t count);
	virtual void AddCall(Callback_Interface_t* IntCallback);
	virtual void DeleteCall(void);
	virtual void Deinitialize(void);
private:
	Callback_Interface_t* callback_intrf_ptr;
	virtual void IRQ(int8_t IRQ_num);
	SYS_TICK_t(void);
	SYS_TICK_t(SYS_TICK_t const&);
	SYS_TICK_t& operator=(SYS_TICK_t const&);
};


#endif /* SYS_TICK_GPT_HPP_ */
