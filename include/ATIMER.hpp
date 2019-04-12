/*
 * ATIMER.hpp
 *
 *  Created on: 17.02.2017
 *      Author: Stalker1290
 */

#ifndef ATIMER_HPP_
#define ATIMER_HPP_

#include "chip.h"
#include "IRQ_Controller.hpp"
#include "Defines.h"

class ATIMER_t:protected IRQ_LISTENER_t
{
public:
	static ATIMER_t& getATIMER(void);
	void Initialize(uint8_t Period_s);
	void Start(void);
	void Stop(void);
	void Deinitialize(void);
	void Set_call(VOID_CALLBACK_t callback);
	void Set_call(Callback_Interface_t* callback);
	void Clr_call(void);
private:
	VOID_CALLBACK_t callback;
	Callback_Interface_t* callback_intrf_ptr;
	virtual void IRQ(int8_t IRQ_num);
	ATIMER_t(void);
	ATIMER_t(ATIMER_t const&);
	ATIMER_t& operator=(ATIMER_t const&);
};


#endif /* ATIMER_HPP_ */
