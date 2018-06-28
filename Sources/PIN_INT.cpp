/*
 * PIN_INT.c
 *
 *  Created on: 20.01.2017
 *      Author: Stalker1290
 */

#include "PIN_INT.hpp"

#ifdef CORE_M4
const IRQn_Type PININT_t::PININT_IRQn[] = {PIN_INT0_IRQn, PIN_INT1_IRQn, PIN_INT2_IRQn, PIN_INT3_IRQn,
		PIN_INT4_IRQn, PIN_INT5_IRQn, PIN_INT6_IRQn, PIN_INT7_IRQn};
const uint32_t PININT_t::INT_PRIOR[] = {PININT0_interrupt_priority, PININT1_interrupt_priority, PININT2_interrupt_priority,
		PININT3_interrupt_priority, PININT4_interrupt_priority, PININT5_interrupt_priority, PININT6_interrupt_priority,
		PININT7_interrupt_priority};
#endif

#ifdef CORE_M0
const IRQn_Type PININT_t::PININT_IRQn[] = {PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn,
		PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn, PIN_INT4_IRQn};
const uint32_t PININT_t::INT_PRIOR[] = {PININT4_interrupt_priority, PININT4_interrupt_priority, PININT4_interrupt_priority,
		PININT4_interrupt_priority, PININT4_interrupt_priority, PININT4_interrupt_priority, PININT4_interrupt_priority,
		PININT4_interrupt_priority};

#endif


PININT_t* PININT_t::PINInt_ptrs[Num_of_pint_channels] = {(PININT_t*)NULL, (PININT_t*)NULL, (PININT_t*)NULL,
		(PININT_t*)NULL, (PININT_t*)NULL, (PININT_t*)NULL, (PININT_t*)NULL, (PININT_t*)NULL};


PININT_t* PININT_t::get_PINInt(uint8_t pinint_num)
{
#ifdef CORE_M0
	if(pinint_num != 4) return (PININT_t*)NULL;
#endif
	if(pinint_num < Num_of_pint_channels)
	{
		if(PINInt_ptrs[pinint_num] == (PININT_t*)NULL) PINInt_ptrs[pinint_num] = new PININT_t(pinint_num);
		return PINInt_ptrs[pinint_num];
	}
	else return (PININT_t*)NULL;


}

PININT_t::PININT_t(uint8_t pinint_num):IRQ_LISTENER_t()
{
	this->pinint_num = pinint_num;
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
	this->setCode((uint64_t)1<<(this->PININT_IRQn[this->pinint_num]));
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void PININT_t::Initialize(uint8_t port, uint8_t pin, uint8_t gpio_port, uint8_t gpio_pin, uint16_t SCU_MODE, PININT_SENSE_t sense)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif

	Chip_SCU_PinMuxSet(port, pin, (SCU_MODE_INBUFF_EN | SCU_MODE) );
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpio_port, gpio_pin);
	Chip_SCU_GPIOIntPinSel(this->pinint_num, gpio_port, gpio_pin);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));

	switch (sense)
	{
	case LOW_EDGE:
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		break;
	case LOW_LEVEL:
		Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		break;
	case HI_EDGE:
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		break;
	case HI_LEVEL:
		Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		break;
	case BOTH_LEVEL:
		Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		break;
	case BOTH_EDGE:
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
		break;
	}

	#ifdef CORE_M0
	NVIC_SetPriority(this->PININT_IRQn[this->pinint_num], this->INT_PRIOR[this->pinint_num]);
	#endif
	#ifdef CORE_M4
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(this->PININT_IRQn[this->pinint_num], NVIC_EncodePriority(prioritygroup, this->INT_PRIOR[this->pinint_num], 0));
	#endif
}

void PININT_t::Set_call(VOID_CALLBACK_t callback)
{
	if(this->callback_intrf_ptr == (Callback_Interface_t*)NULL) this->callback = callback;
}

void PININT_t::Set_call(Callback_Interface_t* callback)
{
	if(this->callback == (VOID_CALLBACK_t)NULL) this->callback_intrf_ptr = callback;
}

void PININT_t::Clr_call(void)
{
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}

void PININT_t::Enable_int(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
	NVIC_ClearPendingIRQ(this->PININT_IRQn[this->pinint_num]);
	NVIC_EnableIRQ(this->PININT_IRQn[this->pinint_num]);
}

void PININT_t::Disable_int(void)
{
	NVIC_DisableIRQ(this->PININT_IRQn[this->pinint_num]);
}

void PININT_t::Deinitialize(void)
{
	NVIC_DisableIRQ(this->PININT_IRQn[this->pinint_num]);
	NVIC_ClearPendingIRQ(this->PININT_IRQn[this->pinint_num]);
	Chip_PININT_DisableIntLow(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
	Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
	this->callback = (VOID_CALLBACK_t)NULL;
	this->callback_intrf_ptr = (Callback_Interface_t*)NULL;
}

void PININT_t::IRQ(int8_t IRQ_num)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(this->pinint_num));
	if(this->callback != (VOID_CALLBACK_t)NULL) this->callback();
	else if(this->callback_intrf_ptr != (Callback_Interface_t*)NULL) this->callback_intrf_ptr->void_callback((void*)this, NULL);
}



