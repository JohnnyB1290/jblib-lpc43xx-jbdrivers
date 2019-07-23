/**
 * @file
 * @brief Pin Interrupt Driver Description
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

#ifndef PININTERRUPT_HPP_
#define PININTERRUPT_HPP_

#include "jbkernel/jb_common.h"
#include "jbdrivers/IrqController.hpp"

#if !defined PIN_INTERRUPT_NUM_CHANNELS
#define PIN_INTERRUPT_NUM_CHANNELS 		8
#endif


namespace jblib::jbdrivers
{

using namespace jbkernel;

typedef enum
{
	SENSE_LOW_EDGE,
	SENSE_LOW_LEVEL,
	SENSE_HI_EDGE,
	SENSE_HI_LEVEL,
	SENSE_BOTH_EDGE,
	SENSE_BOTH_LEVEL
}PinInterruptSense_t;

class PinInterrupt : protected IIrqListener
{
public:
	static PinInterrupt* getPinInterrupt(uint8_t number);
	void initialize(const uint8_t port, const uint8_t pin,
			const uint8_t gpioPort, const uint8_t gpioPin,
			const uint16_t scuMode, const PinInterruptSense_t sense);
	void addCallback(IVoidCallback* const callback);
	void deleteCallback(void);
	void enableInterrupt(void);
	void disableInterrupt(void);
	void deinitialize(void);

private:
	PinInterrupt(uint8_t number);
	virtual void irqHandler(int irqNumber);

	static const IRQn_Type pinInterruptIrqNs_[];
	static const uint32_t interruptPriorities_[];
	static PinInterrupt* pinInterrupts_[PIN_INTERRUPT_NUM_CHANNELS];
	IVoidCallback* callback_ = NULL;
	uint8_t number_ = 0;
};

}

#endif /* PININTERRUPT_HPP_ */
