/**
 * @file
 * @brief UART Driver Realization
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

#include <stdlib.h>
#include "Uart.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

Uart* Uart::uarts_[UARTS_NUM] = {(Uart*)NULL , (Uart*)NULL, (Uart*)NULL, (Uart*)NULL};
LPC_USART_T* Uart::lpcUarts_[UARTS_NUM] = {LPC_USART0 , LPC_UART1, LPC_USART2, LPC_USART3};
const IRQn_Type Uart::uartIrqNs_[UARTS_NUM] = {USART0_IRQn, UART1_IRQn, USART2_IRQn, USART3_IRQn};
uint32_t Uart::uartInterruptPriorities_[UARTS_NUM] = {
		UART_0_INTERRUPT_PRIORITY, UART_1_INTERRUPT_PRIORITY,
		UART_2_INTERRUPT_PRIORITY, UART_3_INTERRUPT_PRIORITY};
const uint8_t Uart::txPorts_[UARTS_NUM] = {UART_0_TX_PORT, UART_1_TX_PORT,
		UART_2_TX_PORT, UART_3_TX_PORT};
const uint8_t Uart::txPins_[UARTS_NUM] = {UART_0_TX_PIN, UART_1_TX_PIN,
		UART_2_TX_PIN, UART_3_TX_PIN};
const uint8_t Uart::rxPorts_[UARTS_NUM] = {UART_0_RX_PORT, UART_1_RX_PORT,
		UART_2_RX_PORT, UART_3_RX_PORT};
const uint8_t Uart::rxPins_[UARTS_NUM] = {UART_0_RX_PIN, UART_1_RX_PIN,
		UART_2_RX_PIN, UART_3_RX_PIN};
const uint8_t Uart::txScuModes_[UARTS_NUM] = {UART_0_TX_SCU_MODE, UART_1_TX_SCU_MODE,
		UART_2_TX_SCU_MODE, UART_3_TX_SCU_MODE};
const uint8_t Uart::rxScuModes_[UARTS_NUM] = {UART_0_RX_SCU_MODE, UART_1_RX_SCU_MODE,
		UART_2_RX_SCU_MODE, UART_3_RX_SCU_MODE};
const CHIP_RGU_RST_T Uart::resetNumbers_[UARTS_NUM] = {
		RGU_UART0_RST, RGU_UART1_RST, RGU_UART2_RST, RGU_UART3_RST};



Uart* Uart::getUart(uint8_t number, uint32_t baudrate)
{
	if(number >= UARTS_NUM)
		return NULL;
	if(Uart::uarts_[number] == (Uart*)NULL)
		Uart::uarts_[number] = new Uart(number, baudrate);
	return Uart::uarts_[number];
}



Uart* Uart::getRs485(uint8_t number, uint32_t baudrate, BoardGpio_t* trEnableGpio)
{
	if(number >= UARTS_NUM)
		return NULL;
	if(Uart::uarts_[number] == (Uart*)NULL)
		Uart::uarts_[number] = new Uart(number, baudrate, trEnableGpio);
	return Uart::uarts_[number];
}



void Uart::constructor(uint8_t number, uint32_t baudrate)
{
	this->number_ = number;
	this->baudrate_ = baudrate;
	this->setCode((uint64_t)1 << Uart::uartIrqNs_[this->number_]);
	IrqController::getIrqController()->addPeripheralIrqListener(this);
}



Uart::Uart(uint8_t number, uint32_t baudrate) : IVoidChannel(), IIrqListener()
{
	this->constructor(number, baudrate);
}



Uart::Uart(uint8_t number, uint32_t baudrate, BoardGpio_t* trEnableGpio) : IVoidChannel(), IIrqListener()
{
	this->constructor(number, baudrate);
	this->trEnableGpio_ = trEnableGpio;
	if(this->trEnableGpio_) {
		Chip_SCU_PinMuxSet(this->trEnableGpio_->port,
				this->trEnableGpio_->pin, this->trEnableGpio_->scuMode);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,
				this->trEnableGpio_->gpioPort, this->trEnableGpio_->gpioPin);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT,
				this->trEnableGpio_->gpioPort, this->trEnableGpio_->gpioPin, (bool) false);
	}
}



void Uart::initialize(void* (* const mallocFunc)(size_t),
		const uint16_t txBufferSize, IChannelCallback* const callback)
{
	if(!this->initialized_) {
		if(txBufferSize == 0)
			return;
		this->txBufferSize_ = txBufferSize;
		this->txBuffer_ = (uint8_t*)mallocFunc(this->txBufferSize_);
		if(this->txBuffer_ == (uint8_t*)NULL)
			return;
		this->callback_ = callback;
		RingBuffer_Init(&(this->txRingBuffer_), this->txBuffer_, 1, this->txBufferSize_);

		Chip_SCU_PinMuxSet(Uart::txPorts_[this->number_], Uart::txPins_[this->number_],
				(SCU_MODE_INACT | Uart::txScuModes_[this->number_]));
		Chip_SCU_PinMuxSet(Uart::rxPorts_[this->number_], Uart::rxPins_[this->number_],
				(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS |
						Uart::rxScuModes_[this->number_]));

		Chip_UART_Init(Uart::lpcUarts_[this->number_]);
		Chip_UART_SetBaud(Uart::lpcUarts_[this->number_], this->baudrate_);
		Chip_UART_ConfigData(Uart::lpcUarts_[this->number_], (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
		Chip_UART_SetupFIFOS(Uart::lpcUarts_[this->number_],
				(UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV0));
		/* Enable receive data and line status interrupt */
		Chip_UART_IntEnable(Uart::lpcUarts_[this->number_], UART_IER_RBRINT);
		Chip_UART_TXEnable(Uart::lpcUarts_[this->number_]);

		#ifdef CORE_M4
		uint32_t prioritygroup = NVIC_GetPriorityGrouping();
		NVIC_SetPriority(Uart::uartIrqNs_[this->number_],
				NVIC_EncodePriority(prioritygroup, Uart::uartInterruptPriorities_[this->number_], 0));
		#endif
		#ifdef CORE_M0
		NVIC_SetPriority(Uart::uartIrqNs_[this->number_], Uart::uartInterruptPriorities_[this->number_]);
		#endif
		NVIC_ClearPendingIRQ(Uart::uartIrqNs_[this->number_]);
		NVIC_EnableIRQ(Uart::uartIrqNs_[this->number_]);
		this->initialized_ = true;
	}
}



void Uart::irqHandler(int8_t irqNumber)
{
	static uint8_t byte = 0;
	if(this->initialized_) {
		if (Uart::lpcUarts_[this->number_]->IER & UART_IER_THREINT) {
			Chip_UART_TXIntHandlerRB(Uart::lpcUarts_[this->number_], (RINGBUFF_T*)&(this->txRingBuffer_));

			/* Disable transmit interrupt if the ring buffer is empty */
			if (RingBuffer_IsEmpty((RINGBUFF_T*)&(this->txRingBuffer_))) {
				Chip_UART_IntDisable(Uart::lpcUarts_[this->number_], UART_IER_THREINT);
				if(this->trEnableGpio_)
					Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
							this->trEnableGpio_->gpioPort, this->trEnableGpio_->gpioPin);
			}
		}
		while(Chip_UART_ReadLineStatus(Uart::lpcUarts_[this->number_]) & UART_LSR_RDR) {
			byte = Chip_UART_ReadByte(Uart::lpcUarts_[this->number_]);
			if(this->callback_)
				this->callback_->channelCallback(&byte, 1, (void*)this, NULL);
		}
	}
}



void Uart::deinitialize(void)
{
	if(this->initialized_) {
		Chip_RGU_TriggerReset(Uart::resetNumbers_[this->number_]);
		while (Chip_RGU_InReset(Uart::resetNumbers_[this->number_])) {}
		Chip_UART_DeInit(Uart::lpcUarts_[this->number_]);
		this->initialized_ = false;
	}
}



Uart::~Uart(void)
{
	NVIC_DisableIRQ(Uart::uartIrqNs_[this->number_]);
}



void Uart::tx(uint8_t* const buffer, const uint16_t size, void* parameter)
{
	if(this->initialized_) {
		if(this->trEnableGpio_)
			Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
					this->trEnableGpio_->gpioPort, this->trEnableGpio_->gpioPin);
		Chip_UART_SendRB(Uart::lpcUarts_[this->number_],
				(RINGBUFF_T*)&(this->txRingBuffer_), buffer, size);
		if(this->trEnableGpio_)
			RingBuffer_InsertMult((RINGBUFF_T*)&(this->txRingBuffer_), &(this->rs485Dummy_), 1);
	}
}



void Uart::getParameter(const uint8_t number, void* const value)
{

}



void Uart::setParameter(const uint8_t number, void* const value)
{

}

}
