/*
 * RITMR.hpp
 *
 *  Created on: 13.07.2017
 *      Author: Stalker1290
 */

#ifndef RITMR_HPP_
#define RITMR_HPP_

#include "VOIDTMR.hpp"

class RITIMER_t:public VOID_TIMER_t
{
public:
	static RITIMER_t& getRITIMER(void);
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
	RITIMER_t(void);
	RITIMER_t(RITIMER_t const&);
	RITIMER_t& operator=(RITIMER_t const&);
};

#endif /* RITMR_HPP_ */
