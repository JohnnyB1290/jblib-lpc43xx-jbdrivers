/**
 * @file
 * @brief Console class realization
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

// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "jb_common.h"

#if USE_CONSOLE
#include <string.h>
#include <stdio.h>
#include "Console.hpp"
#include "JbController.hpp"

namespace jblib::jbdrivers
{
using namespace jbkernel;
using namespace jbutilities;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CODE_RED
int _write(int iFileHandle, char *pcBuffer, int iLength)
{
	Console::getConsole()->getTxRingBuffer()->insertMult((uint8_t*)pcBuffer, iLength);
	return 0;
}

#else

struct __FILE {
	int handle;
};

int fputc(int c, FILE *f)
{
	Console::getConsole()->getTxRingBuffer()->insert((void*)&c);
	return 0;
}

int fgetc(FILE *f)
{
	return 0;
}

int ferror(FILE *f)
{
	return EOF;
}

#endif

#ifdef __cplusplus
}
#endif

Console* Console::console_ = (Console*)NULL;

Console* Console::getConsole(void)
{
	if(Console::console_ == (Console*)NULL)
		Console::console_ = new Console();
	return Console::console_;
}



Console::Console(void) : IChannelCallback(), IVoidCallback()
{
	this->rxRinfBuffer_ = new RingBuffer(NULL, 1, CONSOLE_RX_BUF_SIZE);
	this->rxRinfBuffer_->flush();
	this->txRingBuffer_ = new RingBuffer(NULL, 1, CONSOLE_TX_BUF_SIZE);
	for(uint32_t i = 0; i < CONSOLE_NUM_LISTENERS; i++)
		consoleListeners_[i] = NULL;
	memset(this->commandBuffer_, 0, CONSOLE_COMMAND_BUF_SIZE);
}



RingBuffer* Console::getTxRingBuffer(void) const{
	return this->txRingBuffer_;
}



void Console::setOutputChannel(IVoidChannel* const outputChannel)
{
	this->outputChannel_ = outputChannel;
}



IVoidChannel* Console::getOutputChannel(void) const
{
	return this->outputChannel_;
}



void Console::addListener(IConsoleListener* listener)
{
	for(uint32_t i = 0; i < CONSOLE_NUM_LISTENERS; i++) {
		if(this->consoleListeners_[i] == listener)
			break;
		if(this->consoleListeners_[i] == (IConsoleListener*)NULL) {
			this->consoleListeners_[i] = listener;
			break;
		}
	}
}



void Console::deleteListener(IConsoleListener* listener)
{
	uint32_t index = 0;
	for(uint32_t i = 0; i < CONSOLE_NUM_LISTENERS; i++) {
		if(this->consoleListeners_[i] == listener)
			break;
		else
			index++;
	}
	if(index == (CONSOLE_NUM_LISTENERS - 1)) {
		if(this->consoleListeners_[index] == listener)
			this->consoleListeners_[index] = (IConsoleListener*)NULL;
	}
	else {
		for(uint32_t i = index; i < (CONSOLE_NUM_LISTENERS - 1); i++) {
			this->consoleListeners_[i] = this->consoleListeners_[i+1];
			if(this->consoleListeners_[i+1] == (IConsoleListener*)NULL)
				break;
		}
	}
}



void Console::channelCallback(uint8_t* const buffer, const uint16_t size,
		void* const source, void* parameter)
{
	this->rxRinfBuffer_->insertMult(buffer, size);
}



void Console::voidCallback(void* const source, void* parameter)
{
	uint8_t tempBuffer[CONSOLE_TX_MESAGE_MAX_SIZE];
	uint16_t tempSize = 0;
	if(this->outputChannel_) {
		tempSize = this->txRingBuffer_->popMult(tempBuffer, CONSOLE_TX_MESAGE_MAX_SIZE);
		if(tempSize)
			this->outputChannel_->tx(tempBuffer, tempSize, NULL);
	}

	uint8_t tempChar = 0;
	tempSize = this->rxRinfBuffer_->pop(&tempChar);
	while(tempSize) {
		if(tempChar == 0x0A) {
			if(this->commandIndex_) {
				if(this->commandBuffer_[this->commandIndex_ - 1] == 0x0D)
					this->commandBuffer_[this->commandIndex_ - 1] = 0;
			}
			this->commandBuffer_[this->commandIndex_] = 0;
			#if USE_CONSOLE
			printf("#JBravo::%s\n\r",(char*)this->commandBuffer_);
			#endif
			for(uint32_t i = 0; i < CONSOLE_NUM_LISTENERS; i++) {
				if(this->consoleListeners_[i])
					this->consoleListeners_[i]->parseConsoleMes(this->commandBuffer_, this->commandIndex_);
				else
					break;
			}
			this->commandIndex_ = 0;
		}
		else {
			this->commandBuffer_[this->commandIndex_] = tempChar;
			this->commandIndex_++;
			if(this->commandIndex_ == CONSOLE_COMMAND_BUF_SIZE)
				this->commandIndex_ = 0;
		}
		tempSize = this->rxRinfBuffer_->pop(&tempChar);
	}
}

}

#endif
