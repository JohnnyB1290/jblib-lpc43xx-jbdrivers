/**
 * @file
 * @brief Console class definition
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

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <stdint.h>
#include "callback_interfaces.hpp"
#include "IVoidChannel.hpp"
#include "RingBuffer.hpp"

namespace jblib::jbdrivers
{
using namespace jbkernel;
using namespace jbutilities;

class IConsoleListener
{
public:
	IConsoleListener(void){ }
    virtual ~IConsoleListener(void){ }
    virtual void parseConsoleMes(uint8_t* const message, const uint16_t size) = 0;
};



class Console : public IChannelCallback, IVoidCallback
{
public:
	static Console* getConsole(void);
	void setOutputChannel(IVoidChannel* const outputChannel);
	IVoidChannel* getOutputChannel(void) const;
	void addListener(IConsoleListener* listener);
	void deleteListener(IConsoleListener* listener);
	virtual void channelCallback(uint8_t* const buffer, const uint16_t size,
				void* const source, void* parameter);
	virtual void voidCallback(void* const source, void* parameter);
	RingBuffer* getTxRingBuffer(void) const;

private:
	Console(void);

	static Console* console_;
	RingBuffer* txRingBuffer_ = NULL;
	IVoidChannel* outputChannel_ = NULL;
	IConsoleListener* consoleListeners_[CONSOLE_NUM_LISTENERS];
	RingBuffer* rxRinfBuffer_ = NULL;
	uint8_t commandBuffer_[CONSOLE_COMMAND_BUF_SIZE];
	uint16_t commandIndex_ = 0;
};

}

#endif /* CONSOLE_HPP_ */
