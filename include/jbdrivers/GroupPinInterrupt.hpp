/**
 * @file
 * @brief Group Pin Interrupt Driver Description
 *
 *
 * @note
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
 * All rights reserved.
 * @note
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 * @note
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note
 * This file is a part of JB_Lib.
 */

#ifndef GROUPPININTERRUPT_HPP_
#define GROUPPININTERRUPT_HPP_

#include "jbkernel/jb_common.h"
#include "jbdrivers/IrqController.hpp"

#if !defined GROUP_PIN_INTERRUPT_NUM_GROUPS
#define GROUP_PIN_INTERRUPT_NUM_GROUPS 		2
#endif

namespace jblib::jbdrivers
{

using namespace jbkernel;

typedef enum
{
	GPOUP_PIN_SENSE_LOW_LEVEL,
	GPOUP_PIN_SENSE_HI_LEVEL,
}GroupPinInterruptSense_t;

typedef enum
{
	GROUP_PIN_MODE_OR = 0,
	GROUP_PIN_MODE_AND = 1,
	GROUP_PIN_MODE_EDGE = 0,
	GROUP_PIN_MODE_LEVEL = 2,
}GroupPinInterruptMode_t;

class GroupPinInterrupt : protected IIrqListener
{
public:
	static GroupPinInterrupt* getGroupPinInterrupt(uint8_t groupNumber);
	void initialize(uint8_t mode);
	void addGpio(uint8_t port, uint8_t pin,
			uint8_t gpioPort, uint8_t gpioPin,
			uint16_t scuMode, GroupPinInterruptSense_t sense);
	void deleteGpio(uint8_t gpioPort, uint8_t gpioPin);
	void setCallback(IVoidCallback* callback);
	void clearCallback(void);
	void enableInterrupt(void);
	void disableInterrupt(void);
	void deinitialize(void);

private:
	GroupPinInterrupt(uint8_t groupNumber);
	virtual void irqHandler(int irqNumber);

	static GroupPinInterrupt* groupPinInterrupts_[GROUP_PIN_INTERRUPT_NUM_GROUPS];
	static const IRQn_Type groupPinInterruptIrqNs_[];
	static const uint32_t interruptPriorities_[];
	uint8_t groupNumber_ = 0;
	IVoidCallback* callback_ = NULL;
};

}

#endif /* GROUPPININTERRUPT_HPP_ */
