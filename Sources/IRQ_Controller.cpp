/*
 * IRQ_CONTROLLER.cpp
 *
 *  Created on: 12.07.2017
 *      Author: Stalker1290
 */

#include "IRQ_Controller.hpp"

IRQ_CONTROLLER_t& IRQ_CONTROLLER_t::getIRQController(void)
{
	static IRQ_CONTROLLER_t Instance;
	return Instance;
}

IRQ_LISTENER_t::IRQ_LISTENER_t(void)
{
	this->CODE = 0;
}

uint64_t IRQ_LISTENER_t::getCode(void)
{
	return this->CODE;
}

void IRQ_LISTENER_t::setCode(uint64_t CODE)
{
	this->CODE = CODE;
}

void IRQ_CONTROLLER_t::Add_Cortex_IRQ_Listener(IRQ_LISTENER_t* listener)
{
	__disable_irq();
	for(int i = 0; i < Cortex_Listeners_num; i++)
	{
		if(this->Cortex_IRQ_LISTENERS[i] == listener) break;
		if(this->Cortex_IRQ_LISTENERS[i] == (IRQ_LISTENER_t*)NULL)
		{
			this->Cortex_IRQ_LISTENERS[i] = listener;
			break;
		}
	}
	__enable_irq();
}

void IRQ_CONTROLLER_t::Add_Peripheral_IRQ_Listener(IRQ_LISTENER_t* listener)
{
	__disable_irq();
	for(int i = 0; i < Peripheral_Listeners_num; i++)
	{
		if(this->Peripheral_IRQ_LISTENERS[i] == listener) break;
		if(this->Peripheral_IRQ_LISTENERS[i] == (IRQ_LISTENER_t*)NULL)
		{
			this->Peripheral_IRQ_LISTENERS[i] = listener;
			break;
		}
	}
	__enable_irq();
}

void IRQ_CONTROLLER_t::Delete_Cortex_IRQ_Listener(IRQ_LISTENER_t* listener)
{
	uint32_t index = 0;
	__disable_irq();
	for(int i = 0; i < Cortex_Listeners_num; i++)
	{
		if(this->Cortex_IRQ_LISTENERS[i] == listener) break;
		else index++;
	}
	if(index == (Cortex_Listeners_num-1))
	{
		if(this->Cortex_IRQ_LISTENERS[index] == listener) this->Cortex_IRQ_LISTENERS[index] = (IRQ_LISTENER_t*)NULL;
	}
	else
	{
		for(int i = index; i < (Cortex_Listeners_num-1); i++)
		{
			this->Cortex_IRQ_LISTENERS[i] = this->Cortex_IRQ_LISTENERS[i+1];
			if(this->Cortex_IRQ_LISTENERS[i+1] == (IRQ_LISTENER_t*)NULL) break;
		}
	}
	__enable_irq();
}

void IRQ_CONTROLLER_t::Delete_Peripheral_IRQ_Listener(IRQ_LISTENER_t* listener)
{
	uint32_t index = 0;
	__disable_irq();

	for(int i = 0; i < Peripheral_Listeners_num; i++)
	{
		if(this->Peripheral_IRQ_LISTENERS[i] == listener) break;
		else index++;
	}
	if(index == (Peripheral_Listeners_num-1))
	{
		if(this->Peripheral_IRQ_LISTENERS[index] == listener) this->Peripheral_IRQ_LISTENERS[index] = (IRQ_LISTENER_t*)NULL;
	}
	else
	{
		for(int i = index; i < (Peripheral_Listeners_num-1); i++)
		{
			this->Peripheral_IRQ_LISTENERS[i] = this->Peripheral_IRQ_LISTENERS[i+1];
			if(this->Peripheral_IRQ_LISTENERS[i+1] == (IRQ_LISTENER_t*)NULL) break;
		}
	}
	__enable_irq();
}

void IRQ_CONTROLLER_t::HANDLE_Cortex_IRQ(int8_t IRQ_num)
{
	int8_t abs_IRQ_num;
	abs_IRQ_num = (IRQ_num<0)? (-IRQ_num):IRQ_num;

	for(int i = 0; i < Cortex_Listeners_num; i++)
	{
		if(this->Cortex_IRQ_LISTENERS[i] != (IRQ_LISTENER_t*)NULL)
		{
			if( ((this->Cortex_IRQ_LISTENERS[i]->getCode())>>abs_IRQ_num) & 1 )
			{
				this->Cortex_IRQ_LISTENERS[i]->IRQ(IRQ_num);
			}
		}
		else break;
	}
}

void IRQ_CONTROLLER_t::HANDLE_Peripheral_IRQ(int8_t IRQ_num)
{
	int8_t abs_IRQ_num;
	abs_IRQ_num = (IRQ_num<0)? (-IRQ_num):IRQ_num;

	for(int i = 0; i < Peripheral_Listeners_num; i++)
	{
		if(this->Peripheral_IRQ_LISTENERS[i] != (IRQ_LISTENER_t*)NULL)
		{
			if( ((this->Peripheral_IRQ_LISTENERS[i]->getCode())>>abs_IRQ_num) & 1 )
			{
				this->Peripheral_IRQ_LISTENERS[i]->IRQ(IRQ_num);
			}
		}
		else break;
	}
}

IRQ_CONTROLLER_t::IRQ_CONTROLLER_t()
{
	for(int i = 0; i < Cortex_Listeners_num; i++) this->Cortex_IRQ_LISTENERS[i] = (IRQ_LISTENER_t*)NULL;
	for(int i = 0; i < Peripheral_Listeners_num; i++) this->Peripheral_IRQ_LISTENERS[i] = (IRQ_LISTENER_t*)NULL;
	__enable_irq();
}
