/**
 * @file
 * @brief Bind IRQ Handlers with IRQ Controller (CORE_M0APP)
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

#ifdef CORE_M0APP

#include "jbdrivers/IrqController.hpp"

namespace jblib::jbdrivers
{

static IrqController* const irqController = IrqController::getIrqController();

#ifdef __cplusplus
extern "C" {
#endif

void M0_NMI_Handler(void)
{
	irqController->handleIrq(NonMaskableInt_IRQn);
	while(1);
}

void M0_HardFault_Handler(void)
{
	irqController->handleIrq(HardFault_IRQn);
	while(1);
}

void M0_SVC_Handler(void)
{
	irqController->handleIrq(SVCall_IRQn);
	while(1);
}

void M0_DebugMon_Handler(void)
{
	irqController->handleIrq(DebugMonitor_IRQn);
	while(1);
}

void M0_PendSV_Handler(void)
{
	irqController->handleIrq(PendSV_IRQn);
	while(1);
}

void M0_SysTick_Handler(void)
{
	irqController->handleIrq(SysTick_IRQn);
}

void M0_RTC_IRQHandler(void)
{
	irqController->handleIrq(RTC_IRQn);
}

void M0_M4CORE_IRQHandler(void)
{
	irqController->handleIrq(M4_IRQn);
}

void M0_DMA_IRQHandler(void)
{
	irqController->handleIrq(DMA_IRQn);
}

void M0_FLASHEEPROMAT_IRQHandler(void)
{
	irqController->handleIrq(FLASHEEPROM_IRQn);
}


void M0_ETHERNET_IRQHandler(void){
	irqController->handleIrq(ETHERNET_IRQn);
}

void M0_SDIO_IRQHandler(void)
{
	irqController->handleIrq(SDIO_IRQn);
}

void M0_LCD_IRQHandler(void)
{
	irqController->handleIrq(LCD_IRQn);
}


void M0_USB0_IRQHandler(void)
{
	irqController->handleIrq(USB0_IRQn);
}


void M0_USB1_IRQHandler(void)
{
	irqController->handleIrq(USB1_IRQn);
}


void M0_SCT_IRQHandler(void)
{
	irqController->handleIrq(SCT_IRQn);
}

void M0_RITIMER_OR_WWDT_IRQHandler(void){
	irqController->handleIrq(RITIMER_IRQn);
}


void M0_TIMER0_IRQHandler(void)
{
	irqController->handleIrq(TIMER0_IRQn);
}


void M0_GINT1_IRQHandler(void)
{
	irqController->handleIrq(GINT1_IRQn);
}


void M0_PIN_INT4_IRQHandler(void)
{
	irqController->handleIrq(PIN_INT4_IRQn);
}


void M0_TIMER3_IRQHandler(void)
{
	irqController->handleIrq(TIMER3_IRQn);
}


void M0_MCPWM_IRQHandler(void)
{
	irqController->handleIrq(MCPWM_IRQn);
}

void M0_ADC0_IRQHandler(void)
{
	irqController->handleIrq(ADC0_IRQn);
}

void M0_I2C0_OR_I2C1_IRQHandler(void)
{
	irqController->handleIrq(I2C0_IRQn);
}

void M0_SGPIO_IRQHandler(void)
{
	irqController->handleIrq(SGPIO_INT_IRQn);
}

void M0_SPI_OR_DAC_IRQHandler(void)
{
	irqController->handleIrq(SPI_INT_IRQn);
}

void M0_ADC1_IRQHandler(void)
{
	irqController->handleIrq(ADC1_IRQn);
}

void M0_SSP0_OR_SSP1_IRQHandler(void)
{
	irqController->handleIrq(SSP0_IRQn);
}

void M0_EVENTROUTER_IRQHandler(void)
{
	irqController->handleIrq(EVENTROUTER_IRQn);
}

void M0_USART0_IRQHandler(void)
{
	irqController->handleIrq(USART0_IRQn);
}

void M0_UART1_IRQHandler(void)
{
	irqController->handleIrq(UART1_IRQn);
}

void M0_USART2_OR_C_CAN1_IRQHandler(void)
{
	irqController->handleIrq(USART2_IRQn);
}

void M0_USART3_IRQHandler(void)
{
	irqController->handleIrq(USART3_IRQn);
}

void M0_I2S0_OR_I2S1_QEI_IRQHandler(void)
{
	irqController->handleIrq(I2S0_IRQn);
}

void M0_C_CAN0_IRQHandler(void)
{
	irqController->handleIrq(C_CAN0_IRQn);
}

void M0_SPIFI_OR_ADCHS_IRQHandler(void)
{
	irqController->handleIrq(ADCHS_IRQn);
}

void M0_M0SUB_IRQHandler(void)
{
	irqController->handleIrq(M0SUB_IRQn);
}

#ifdef __cplusplus
}
#endif

}

#endif
