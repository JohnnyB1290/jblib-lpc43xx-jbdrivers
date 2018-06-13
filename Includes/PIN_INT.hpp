/*
 * PIN_INT.h
 *
 *  Created on: 20.01.2017
 *      Author: Stalker1290
 */

#ifndef PIN_INT_HPP_
#define PIN_INT_HPP_

#include "chip.h"
#include "IRQ_Controller.hpp"
#include "Defines.h"

#define Num_of_pint_channels 8

typedef enum PININT_SENSE
{
	LOW_EDGE,
	LOW_LEVEL,
	HI_EDGE,
	HI_LEVEL,
	BOTH_EDGE,
	BOTH_LEVEL
}PININT_SENSE_t;

class PININT_t:protected IRQ_LISTENER_t
{
public:
	static PININT_t* get_PINInt(uint8_t pinint_num);
	void Initialize(uint8_t port, uint8_t pin, uint8_t gpio_port, uint8_t gpio_pin, uint16_t SCU_MODE, PININT_SENSE_t sense);
	void Set_call(VOID_CALLBACK_t callback);
	void Set_call(Callback_Interface_t* callback);
	void Clr_call(void);
	void Enable_int(void);
	void Disable_int(void);
	void Deinitialize(void);
private:
	static PININT_t* PINInt_ptrs[Num_of_pint_channels];
	PININT_t(uint8_t pinint_num);
	virtual void IRQ(int8_t IRQ_num);
	VOID_CALLBACK_t callback;
	Callback_Interface_t* callback_intrf_ptr;
	uint8_t pinint_num;
	static const IRQn_Type PININT_IRQn[];
	static const uint32_t INT_PRIOR[];
};

#endif /* PIN_INT_HPP_ */
