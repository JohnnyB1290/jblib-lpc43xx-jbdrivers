/*
 * GROUP_PIN_INT.cpp
 *
 *  Created on: 27 θών. 2018 γ.
 *      Author: Stalker1290
 */

#include "GROUP_PIN_INT.hpp"

#ifdef CORE_M4
const IRQn_Type GroupPinInt_t::groupPinIntIRQn[] = {GINT0_IRQn, GINT1_IRQn};
const uint32_t GroupPinInt_t::interruptPrior[] = {GROUPPININT0_interrupt_priority ,GROUPPININT1_interrupt_priority};
#endif

#ifdef CORE_M0
const IRQn_Type GroupPinInt_t::groupPinIntIRQn[] = {GINT1_IRQn,GINT1_IRQn};
const uint32_t GroupPinInt_t::interruptPrior[] = {GROUPPININT1_interrupt_priority, GROUPPININT1_interrupt_priority};
#endif

GroupPinInt_t* GroupPinInt_t::groupPinIntPtrs[GROUP_PIN_INT_GROUPS_NUM] = {(GroupPinInt_t*)NULL, (GroupPinInt_t*)NULL};

GroupPinInt_t* GroupPinInt_t::getGroupPinInt(uint8_t groupNum){
#ifdef CORE_M0
	if(groupNum != 1) return (GroupPinInt_t*)NULL;
#endif
	if(groupNum < GROUP_PIN_INT_GROUPS_NUM){
		if(GroupPinInt_t::groupPinIntPtrs[groupNum] == (GroupPinInt_t*)NULL){
			GroupPinInt_t::groupPinIntPtrs[groupNum] = new GroupPinInt_t(groupNum);
		}
		return GroupPinInt_t::groupPinIntPtrs[groupNum];
	}
	else return (GroupPinInt_t*)NULL;
}

GroupPinInt_t::GroupPinInt_t(uint8_t groupNum):IRQ_LISTENER_t(){
	if(groupNum < GROUP_PIN_INT_GROUPS_NUM){
		this->groupNum = groupNum;
	}
	else{
		this->groupNum = 0;
	}
	this->callbackIfacePtr = (Callback_Interface_t*)NULL;

	this->setCode((uint64_t)1<<(GroupPinInt_t::groupPinIntIRQn[this->groupNum]));
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

void GroupPinInt_t::initialize(uint8_t mode){

	if((mode && 1) == OR_MODE){
		Chip_GPIOGP_SelectOrMode(LPC_GPIOGROUP, this->groupNum);
	}
	else{
		Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUP, this->groupNum);
	}
	if((mode && 2) == EDGE_MODE){
		Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUP, this->groupNum);
	}
	else{
		Chip_GPIOGP_SelectLevelMode(LPC_GPIOGROUP, this->groupNum);
	}

#ifdef CORE_M4
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(GroupPinInt_t::groupPinIntIRQn[this->groupNum], NVIC_EncodePriority(prioritygroup, GroupPinInt_t::interruptPrior[this->groupNum], 0));
#endif
#ifdef CORE_M0
	NVIC_SetPriority(GroupPinInt_t::groupPinIntIRQn[this->groupNum], GroupPinInt_t::interruptPrior[this->groupNum]);
#endif
}

void GroupPinInt_t::addGPIO(uint8_t port, uint8_t pin, uint8_t gpioPort, uint8_t gpioPin, uint16_t scuMode, GroupPinIntSense_t sense){

	Chip_SCU_PinMuxSet(port, pin, (SCU_MODE_INBUFF_EN |scuMode));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpio_port, gpio_pin);

	switch (sense){
	case LOW_LEVEL_GPOUP_PIN:
		Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP, this->groupNum, gpioPort, 1 << gpioPin);
		break;
	case HI_LEVEL_GPOUP_PIN:
		Chip_GPIOGP_SelectHighLevel(LPC_GPIOGROUP, this->groupNum, gpioPort, 1 << gpioPin);
		break;
	}
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP, this->groupNum, gpioPort, 1 << gpioPin);
}

void GroupPinInt_t::deleteGPIO(uint8_t gpioPort, uint8_t gpioPin){

	Chip_GPIOGP_DisableGroupPins(LPC_GPIOGROUP, this->groupNum, gpioPort, 1 << gpioPin);
}

void GroupPinInt_t::IRQ(int8_t IRQ_num){
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, this->groupNum);
	if(this->callbackIfacePtr != (Callback_Interface_t*)NULL) this->callbackIfacePtr->void_callback((void*)this, NULL);
}

void GroupPinInt_t::setCall(Callback_Interface_t* callback){
	this->callbackIfacePtr = callback;
}

void GroupPinInt_t::clrCall(void){
	this->callbackIfacePtr = (Callback_Interface_t*)NULL;
}

void GroupPinInt_t::enableInt(void){
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, this->groupNum);
	NVIC_ClearPendingIRQ(GroupPinInt_t::groupPinIntIRQn[this->groupNum]);
	NVIC_EnableIRQ(GroupPinInt_t::groupPinIntIRQn[this->groupNum]);
}

void GroupPinInt_t::disableInt(void){
	NVIC_DisableIRQ(GroupPinInt_t::groupPinIntIRQn[this->groupNum]);
}

void GroupPinInt_t::deinitialize(void){
	NVIC_DisableIRQ(GroupPinInt_t::groupPinIntIRQn[this->groupNum]);
	NVIC_ClearPendingIRQ(GroupPinInt_t::groupPinIntIRQn[this->groupNum]);
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, this->groupNum);
	this->callbackIfacePtr = (Callback_Interface_t*)NULL;
}
