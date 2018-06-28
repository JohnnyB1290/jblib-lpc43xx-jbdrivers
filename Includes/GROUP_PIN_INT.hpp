/*
 * GROUP_PIN_INT.hpp
 *
 *  Created on: 27 θών. 2018 γ.
 *      Author: Stalker1290
 */

#ifndef GROUP_PIN_INT_HPP_
#define GROUP_PIN_INT_HPP_

#include "chip.h"
#include "IRQ_Controller.hpp"
#include "Defines.h"

#define GROUP_PIN_INT_GROUPS_NUM 2

typedef enum GroupPinIntSense
{
	LOW_LEVEL_GPOUP_PIN,
	HI_LEVEL_GPOUP_PIN,
}GroupPinIntSense_t;

typedef enum GroupPinIntMode
{
	OR_MODE = 0,
	AND_MODE = 1,
	EDGE_MODE = 0,
	LEVEL_MODE = 2,
}GroupPinIntMode_t;

class GroupPinInt_t:protected IRQ_LISTENER_t
{
public:
	static GroupPinInt_t* getGroupPinInt(uint8_t groupNum);
	void initialize(uint8_t mode);
	void addGPIO(uint8_t port, uint8_t pin, uint8_t gpioPort, uint8_t gpioPin, uint16_t scuMode, GroupPinIntSense_t sense);
	void deleteGPIO(uint8_t gpioPort, uint8_t gpioPin);
	void setCall(Callback_Interface_t* callback);
	void clrCall(void);
	void enableInt(void);
	void disableInt(void);
	void deinitialize(void);
private:
	static GroupPinInt_t* groupPinIntPtrs[GROUP_PIN_INT_GROUPS_NUM];
	GroupPinInt_t(uint8_t groupNum);
	virtual void IRQ(int8_t IRQ_num);
	uint8_t groupNum;
	Callback_Interface_t* callbackIfacePtr;
	static const IRQn_Type groupPinIntIRQn[];
	static const uint32_t interruptPrior[];
};


#endif /* GROUP_PIN_INT_HPP_ */
