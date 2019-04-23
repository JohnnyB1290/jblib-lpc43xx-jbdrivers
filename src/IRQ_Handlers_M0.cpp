/*
 * IRQ_Handlers_M0.cpp
 *
 *  Created on: 12.07.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef CORE_M0APP

#include "chip.h"
#include "IRQ_Controller.hpp"

static IRQ_CONTROLLER_t& Controller = IRQ_CONTROLLER_t::getIRQController();

#ifdef __cplusplus
extern "C" {
#endif

void M0_NMI_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(NonMaskableInt_IRQn);
	while(1);
}

void M0_HardFault_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(HardFault_IRQn);
	while(1);
}

void M0_SVC_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(SVCall_IRQn);
	while(1);
}

void M0_DebugMon_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(DebugMonitor_IRQn);
	while(1);
}

void M0_PendSV_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(PendSV_IRQn);
	while(1);
}

void M0_SysTick_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(SysTick_IRQn);
}

void M0_RTC_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(RTC_IRQn);
}

void M0_M4CORE_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(M4_IRQn);
}

void M0_DMA_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(DMA_IRQn);
}

void M0_FLASHEEPROMAT_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(FLASHEEPROM_IRQn);
}


void M0_ETHERNET_IRQHandler(void){
	Controller.HANDLE_Peripheral_IRQ(ETHERNET_IRQn);
}

void M0_SDIO_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SDIO_IRQn);
}

void M0_LCD_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(LCD_IRQn);
}


void M0_USB0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USB0_IRQn);
}


void M0_USB1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USB1_IRQn);
}


void M0_SCT_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SCT_IRQn);
}

void M0_RITIMER_OR_WWDT_IRQHandler(void){
	Controller.HANDLE_Peripheral_IRQ(RITIMER_IRQn);
}


void M0_TIMER0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(TIMER0_IRQn);
}


void M0_GINT1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(GINT1_IRQn);
}


void M0_PIN_INT4_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(PIN_INT4_IRQn);
}


void M0_TIMER3_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(TIMER3_IRQn);
}


void M0_MCPWM_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(MCPWM_IRQn);
}

void M0_ADC0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ADC0_IRQn);
}

void M0_I2C0_OR_I2C1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(I2C0_IRQn);
}

void M0_SGPIO_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SGPIO_INT_IRQn);
}

void M0_SPI_OR_DAC_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SPI_INT_IRQn);
}

void M0_ADC1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ADC1_IRQn);
}

void M0_SSP0_OR_SSP1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SSP0_IRQn);
}

void M0_EVENTROUTER_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(EVENTROUTER_IRQn);
}

void M0_USART0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USART0_IRQn);
}

void M0_UART1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(UART1_IRQn);
}

void M0_USART2_OR_C_CAN1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USART2_IRQn);
}

void M0_USART3_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USART3_IRQn);
}

void M0_I2S0_OR_I2S1_QEI_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(I2S0_IRQn);
}

void M0_C_CAN0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(C_CAN0_IRQn);
}

void M0_SPIFI_OR_ADCHS_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ADCHS_IRQn);
}

void M0_M0SUB_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(M0SUB_IRQn);
}

#ifdef __cplusplus
}
#endif

#endif
