/*
 * IRQ_CONTROLLER.hpp
 *
 *  Created on: 12.07.2017
 *      Author: Stalker1290
 */

#ifndef IRQ_CONTROLLER_HPP_
#define IRQ_CONTROLLER_HPP_

#include "chip.h"
#include "Common_interfaces.hpp"
#include "Defines.h"

class IRQ_LISTENER_t
{
private:
    uint64_t CODE;
public:
    IRQ_LISTENER_t(void);
    virtual ~IRQ_LISTENER_t(void){}
    virtual void IRQ(int8_t IRQ_num) = 0;
    uint64_t getCode(void);
    void setCode(uint64_t CODE);
};

class IRQ_CONTROLLER_t
{
public:
	static IRQ_CONTROLLER_t& getIRQController(void);
    void Add_Cortex_IRQ_Listener(IRQ_LISTENER_t* listener);
    void Add_Peripheral_IRQ_Listener(IRQ_LISTENER_t* listener);
    void Delete_Cortex_IRQ_Listener(IRQ_LISTENER_t* listener);
    void Delete_Peripheral_IRQ_Listener(IRQ_LISTENER_t* listener);
	void HANDLE_Cortex_IRQ(int8_t IRQ_num);
	void HANDLE_Peripheral_IRQ(int8_t IRQ_num);
private:
    IRQ_LISTENER_t* Cortex_IRQ_LISTENERS[Cortex_Listeners_num];
    IRQ_LISTENER_t* Peripheral_IRQ_LISTENERS[Peripheral_Listeners_num];
    IRQ_CONTROLLER_t(void);
    IRQ_CONTROLLER_t(IRQ_CONTROLLER_t const&);
    IRQ_CONTROLLER_t& operator=(IRQ_CONTROLLER_t const&);
};


#endif /* IRQ_CONTROLLER_HPP_ */
