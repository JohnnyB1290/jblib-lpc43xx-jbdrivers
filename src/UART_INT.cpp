/*
 * UART_INT.cpp
 *
 *  Created on: 05.09.2017
 *      Author: Stalker1290
 */

#include "UART_INT.hpp"
#include "PCB_Defines.h"
#include "chip.h"
#include "stdlib.h"


UART_int_t* UART_int_t::UART_ptrs[num_of_uarts] = {(UART_int_t*)NULL , (UART_int_t*)NULL, (UART_int_t*)NULL, (UART_int_t*)NULL};
LPC_USART_T* UART_int_t::UART_LPC_ptrs[num_of_uarts] = {LPC_USART0 , LPC_UART1, LPC_USART2, LPC_USART3};
const IRQn_Type UART_int_t::UART_IRQn[num_of_uarts] = {USART0_IRQn, UART1_IRQn, USART2_IRQn, USART3_IRQn};
uint32_t UART_int_t::UART_INT_PRIOR[num_of_uarts] = {UART0_interrupt_priority, UART1_interrupt_priority,
		UART2_interrupt_priority, UART3_interrupt_priority};
const uint8_t UART_int_t::Tx_port[num_of_uarts] = {UART0_TX_PORT, UART1_TX_PORT, UART2_TX_PORT, UART3_TX_PORT};
const uint8_t UART_int_t::Tx_pin[num_of_uarts] = {UART0_TX_PIN, UART1_TX_PIN, UART2_TX_PIN, UART3_TX_PIN};
const uint8_t UART_int_t::Rx_port[num_of_uarts] = {UART0_RX_PORT, UART1_RX_PORT, UART2_RX_PORT, UART3_RX_PORT};
const uint8_t UART_int_t::Rx_pin[num_of_uarts] = {UART0_RX_PIN, UART1_RX_PIN, UART2_RX_PIN, UART3_RX_PIN};
const uint8_t UART_int_t::Tx_SCU_MODE_FUNC[num_of_uarts] = {UART0_TX_SCU_FUNC, UART1_TX_SCU_FUNC,
		UART2_TX_SCU_FUNC, UART3_TX_SCU_FUNC};
const uint8_t UART_int_t::Rx_SCU_MODE_FUNC[num_of_uarts] = {UART0_RX_SCU_FUNC, UART1_RX_SCU_FUNC, UART2_RX_SCU_FUNC, UART3_RX_SCU_FUNC};
const CHIP_RGU_RST_T UART_int_t::ResetNumber[num_of_uarts] = {RGU_UART0_RST, RGU_UART1_RST, RGU_UART2_RST, RGU_UART3_RST};


UART_int_t* UART_int_t::get_UART_int(uint8_t u_num, uint32_t baudrate)
{
	if(u_num>=num_of_uarts) return NULL;
	if(UART_int_t::UART_ptrs[u_num] == (UART_int_t*)NULL) UART_int_t::UART_ptrs[u_num] = new UART_int_t(u_num, baudrate);
	return UART_int_t::UART_ptrs[u_num];
}

UART_int_t* UART_int_t::get_RS485_int(uint8_t u_num, uint32_t baudrate, BOARD_GPIO_t* TR_ENABLE) {
	if(u_num>=num_of_uarts) return NULL;
	if(UART_int_t::UART_ptrs[u_num] == (UART_int_t*)NULL) UART_int_t::UART_ptrs[u_num] = new UART_int_t(u_num, baudrate, TR_ENABLE);
	return UART_int_t::UART_ptrs[u_num];
}

void UART_int_t::init(uint8_t u_num, uint32_t baudrate) {
	this->Tx_buf_ptr = (uint8_t*)NULL;;
	this->tx_buf_size = 0;
	this->call_interface_ptr = (Channel_Call_Interface_t*)NULL;
	this->UART_initialize = 0;
	this->u_num = u_num;
	this->baudrate = baudrate;
	this->TR_ENABLE = NULL;
	this->_rs485_dummy = 0;
	this->setCode((uint64_t)1<<UART_int_t::UART_IRQn[this->u_num]);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);
}

UART_int_t::UART_int_t(uint8_t u_num, uint32_t baudrate):void_channel_t(),IRQ_LISTENER_t()
{
	this->init(u_num, baudrate);
}

UART_int_t::UART_int_t(uint8_t u_num, uint32_t baudrate, BOARD_GPIO_t* TR_ENABLE):void_channel_t(),IRQ_LISTENER_t()
{
	this->init(u_num, baudrate);
	this->TR_ENABLE = TR_ENABLE;
	if(this->TR_ENABLE != NULL) {
		Chip_SCU_PinMuxSet(this->TR_ENABLE->port, this->TR_ENABLE->pin, this->TR_ENABLE->scu_mode);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, this->TR_ENABLE->gpio_port, this->TR_ENABLE->gpio_pin);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, this->TR_ENABLE->gpio_port, this->TR_ENABLE->gpio_pin, (bool) false);
	}
}

void UART_int_t::Initialize(void* (*mem_alloc)(size_t),uint16_t tx_buf_size, Channel_Call_Interface_t* call_interface_ptr)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
#endif
	
	if(this->UART_initialize == 0)
	{
		if(tx_buf_size == 0) return;
		this->tx_buf_size = tx_buf_size;
		this->Tx_buf_ptr = (uint8_t*)mem_alloc(this->tx_buf_size);

		if(this->Tx_buf_ptr == (uint8_t*)NULL) return;
		if(call_interface_ptr == (Channel_Call_Interface_t*)NULL) return;
		this->call_interface_ptr = call_interface_ptr;
		RingBuffer_Init(&(this->Tx_ring_buf), this->Tx_buf_ptr, 1, this->tx_buf_size);

		Chip_SCU_PinMuxSet(UART_int_t::Tx_port[this->u_num], UART_int_t::Tx_pin[this->u_num],
				(SCU_MODE_INACT | UART_int_t::Tx_SCU_MODE_FUNC[this->u_num]));/* UART_TXD */
		Chip_SCU_PinMuxSet(UART_int_t::Rx_port[this->u_num], UART_int_t::Rx_pin[this->u_num],
				(SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | UART_int_t::Rx_SCU_MODE_FUNC[this->u_num]));/* UART_RXD */

		Chip_UART_Init(UART_int_t::UART_LPC_ptrs[this->u_num]);
		Chip_UART_SetBaud(UART_int_t::UART_LPC_ptrs[this->u_num], this->baudrate);
		Chip_UART_ConfigData(UART_int_t::UART_LPC_ptrs[this->u_num], (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
		Chip_UART_SetupFIFOS(UART_int_t::UART_LPC_ptrs[this->u_num], (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV0));
		/* Enable receive data and line status interrupt */
		Chip_UART_IntEnable(UART_int_t::UART_LPC_ptrs[this->u_num], UART_IER_RBRINT);
		Chip_UART_TXEnable(UART_int_t::UART_LPC_ptrs[this->u_num]);

		/* Enable interrupt in the NVIC */
		#ifdef CORE_M4
		prioritygroup = NVIC_GetPriorityGrouping();
		NVIC_SetPriority(UART_int_t::UART_IRQn[this->u_num], NVIC_EncodePriority(prioritygroup, UART_int_t::UART_INT_PRIOR[this->u_num], 0));
		#endif
		#ifdef CORE_M0
		NVIC_SetPriority(UART_int_t::UART_IRQn[this->u_num], UART_int_t::UART_INT_PRIOR[this->u_num]);
		#endif
		NVIC_ClearPendingIRQ(UART_int_t::UART_IRQn[this->u_num]);
		NVIC_EnableIRQ(UART_int_t::UART_IRQn[this->u_num]);
		this->UART_initialize = 1;
	}
}

void UART_int_t::IRQ(int8_t IRQ_num)
{
	static uint8_t byte;

	if(this->UART_initialize == 1)
	{
		/* Handle transmit interrupt if enabled */
		if (UART_int_t::UART_LPC_ptrs[this->u_num]->IER & UART_IER_THREINT)
		{
			Chip_UART_TXIntHandlerRB(UART_int_t::UART_LPC_ptrs[this->u_num], (RINGBUFF_T*)&(this->Tx_ring_buf));

			/* Disable transmit interrupt if the ring buffer is empty */
			if (RingBuffer_IsEmpty((RINGBUFF_T*)&(this->Tx_ring_buf)))
			{
				Chip_UART_IntDisable(UART_int_t::UART_LPC_ptrs[this->u_num], UART_IER_THREINT);
				if(this->TR_ENABLE != NULL) {
					Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, this->TR_ENABLE->gpio_port, this->TR_ENABLE->gpio_pin);
//					this->call_interface_ptr->channel_callback(NULL, 0, (void*)this, (void*) RS485Event_TX_END);
				}
			}
		}

		while(Chip_UART_ReadLineStatus(UART_int_t::UART_LPC_ptrs[this->u_num]) & UART_LSR_RDR)
		{
			byte = Chip_UART_ReadByte(UART_int_t::UART_LPC_ptrs[this->u_num]);
			this->call_interface_ptr->channel_callback(&byte, 1, (void*)this, NULL);
//			this->call_interface_ptr->channel_callback(&byte, 1, (void*)this, this->TR_ENABLE == NULL ? NULL : ((void*) RS485Event_RX));
		}
	}
}

void UART_int_t::DEInitialize(void)
{
	if(this->UART_initialize == 1)
	{
		Chip_RGU_TriggerReset(UART_int_t::ResetNumber[this->u_num]);
		while (Chip_RGU_InReset(UART_int_t::ResetNumber[this->u_num])) {}
		Chip_UART_DeInit(UART_int_t::UART_LPC_ptrs[this->u_num]);
		this->UART_initialize = 0;
	}
}

UART_int_t::~UART_int_t(void)
{
	NVIC_DisableIRQ(UART_int_t::UART_IRQn[this->u_num]);
}


void UART_int_t::Tx(uint8_t *mes,uint16_t m_size,void* param)
{
	if(this->UART_initialize == 1)
	{
		if(this->TR_ENABLE != NULL) {
			Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, this->TR_ENABLE->gpio_port, this->TR_ENABLE->gpio_pin);
		}
		Chip_UART_SendRB(UART_int_t::UART_LPC_ptrs[this->u_num], (RINGBUFF_T*)&(this->Tx_ring_buf), mes, m_size);
		if(this->TR_ENABLE != NULL) {
			RingBuffer_InsertMult((RINGBUFF_T*)&(this->Tx_ring_buf), &(this->_rs485_dummy), 1);
		}
	}
}


void UART_int_t::GetParameter(uint8_t ParamName, void* ParamValue)
{
//	if(this->UART_initialize == 1)
//	{
//
//	}
}

void UART_int_t::SetParameter(uint8_t ParamName, void* ParamValue)
{
//	if(this->UART_initialize == 1)
//	{
//
//	}
}

