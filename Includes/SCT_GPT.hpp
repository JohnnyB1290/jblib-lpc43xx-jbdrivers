/*
 * SCT_GPT.hpp
 *
 *  Created on: 28.07.2017
 *      Author: Stalker1290
 */

#ifndef SCT_GPT_HPP_
#define SCT_GPT_HPP_

#include "VOIDTMR.hpp"

class SCT_TIMER_t:public VOID_TIMER_t
{
public:
	static SCT_TIMER_t& getSCT_TIMER(void);
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
	VOID_CALLBACK_t callback;
	Callback_Interface_t* callback_intrf_ptr;
	virtual void IRQ(int8_t IRQ_num);
	SCT_TIMER_t(void);
	SCT_TIMER_t(SCT_TIMER_t const&);
	SCT_TIMER_t& operator=(SCT_TIMER_t const&);
};


#endif /* SCT_GPT_HPP_ */
