/*
 * UART_INT.hpp
 *
 *  Created on: 05.09.2017
 *      Author: Stalker1290
 */

#ifndef UART_INT_HPP_
#define UART_INT_HPP_

#include "Void_Channel.hpp"
#include "IRQ_Controller.hpp"

#define num_of_uarts 4

class UART_int_t:public void_channel_t,protected IRQ_LISTENER_t
{
public:
	static UART_int_t* get_UART_int(uint8_t u_num, uint32_t baudrate);
	virtual void Initialize(void* (*mem_alloc)(size_t),uint16_t tx_buf_size, Channel_Call_Interface_t* call_interface_ptr);
	virtual void DEInitialize(void);
	virtual void Tx(uint8_t *mes,uint16_t m_size,void* param);
	virtual void GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void SetParameter(uint8_t ParamName, void* ParamValue);
private:
	UART_int_t(uint8_t u_num, uint32_t baudrate);
	virtual ~UART_int_t(void);
	virtual void IRQ(int8_t IRQ_num);
	uint8_t u_num;
	uint32_t baudrate;
	RINGBUFF_T Tx_ring_buf;
	uint8_t* Tx_buf_ptr;
	uint16_t tx_buf_size;
	Channel_Call_Interface_t* call_interface_ptr;
	uint8_t UART_initialize;
	static UART_int_t* UART_ptrs[num_of_uarts];

	static LPC_USART_T* UART_LPC_ptrs[num_of_uarts];
	static const IRQn_Type UART_IRQn[num_of_uarts];
	static uint32_t UART_INT_PRIOR[num_of_uarts];
	static const uint8_t Tx_port[num_of_uarts];
	static const uint8_t Tx_pin[num_of_uarts];
	static const uint8_t Rx_port[num_of_uarts];
	static const uint8_t Rx_pin[num_of_uarts];
	static const uint8_t Tx_SCU_MODE_FUNC[num_of_uarts];
	static const uint8_t Rx_SCU_MODE_FUNC[num_of_uarts];
	static const CHIP_RGU_RST_T ResetNumber[num_of_uarts];
};

#endif /* UART_INT_HPP_ */
