/**
 * @file
 * @brief IRQ Controller Driver Description
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

#ifndef IRQCONTROLLER_HPP_
#define IRQCONTROLLER_HPP_

#include <stdint.h>
#include "jb_common.h"
#include "callback_interfaces.hpp"

namespace jblib::jbdrivers
{

class IIrqListener
{
public:
	IIrqListener(void){ }
    virtual ~IIrqListener(void){ }
    uint64_t getCode(void) const
    {
    	return this->code_;
    }
    void setCode(uint64_t code)
    {
    	this->code_ = code;
    }
    virtual void irqHandler(int8_t irqNumber) = 0;

private:
    uint64_t code_ = 0;
};

class IrqController
{
public:
	static IrqController* getIrqController(void);
    void addCortexIrqListener(IIrqListener* const listener);
    void addPeripheralIrqListener(IIrqListener* const listener);
    void deleteCortexIrqListener(IIrqListener* const listener);
    void deletePeripheralIrqListener(IIrqListener* const listener);
	void handleCortexIrq(const int8_t irqNumber);
	void handlePeripheralIrq(const int8_t irqNumber);

private:
	IrqController(void);

	static IrqController* irqController_;
    IIrqListener* cortexIrqListeners_[IRQ_CONTROLLER_NUM_CORTEX_LISTENERS];
    IIrqListener* peripheralIrqListeners_[IRQ_CONTROLLER_NUM_PERIPHERAL_LISTENERS];
};

}

#endif /* IRQCONTROLLER_HPP_ */
