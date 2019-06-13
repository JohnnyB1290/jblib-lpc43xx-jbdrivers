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
	irqController->handleCortexIrq(NonMaskableInt_IRQn);
	while(1);
}

void M0_HardFault_Handler(void)
{
	irqController->handleCortexIrq(HardFault_IRQn);
	while(1);
}

void M0_SVC_Handler(void)
{
	irqController->handleCortexIrq(SVCall_IRQn);
	while(1);
}

void M0_DebugMon_Handler(void)
{
	irqController->handleCortexIrq(DebugMonitor_IRQn);
	while(1);
}

void M0_PendSV_Handler(void)
{
	irqController->handleCortexIrq(PendSV_IRQn);
	while(1);
}

void M0_SysTick_Handler(void)
{
	irqController->handleCortexIrq(SysTick_IRQn);
}

void M0_RTC_IRQHandler(void)
{
	irqController->handlePeripheralIrq(RTC_IRQn);
}

void M0_M4CORE_IRQHandler(void)
{
	irqController->handlePeripheralIrq(M4_IRQn);
}

void M0_DMA_IRQHandler(void)
{
	irqController->handlePeripheralIrq(DMA_IRQn);
}

void M0_FLASHEEPROMAT_IRQHandler(void)
{
	irqController->handlePeripheralIrq(FLASHEEPROM_IRQn);
}


void M0_ETHERNET_IRQHandler(void){
	irqController->handlePeripheralIrq(ETHERNET_IRQn);
}

void M0_SDIO_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SDIO_IRQn);
}

void M0_LCD_IRQHandler(void)
{
	irqController->handlePeripheralIrq(LCD_IRQn);
}


void M0_USB0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USB0_IRQn);
}


void M0_USB1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USB1_IRQn);
}


void M0_SCT_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SCT_IRQn);
}

void M0_RITIMER_OR_WWDT_IRQHandler(void){
	irqController->handlePeripheralIrq(RITIMER_IRQn);
}


void M0_TIMER0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(TIMER0_IRQn);
}


void M0_GINT1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(GINT1_IRQn);
}


void M0_PIN_INT4_IRQHandler(void)
{
	irqController->handlePeripheralIrq(PIN_INT4_IRQn);
}


void M0_TIMER3_IRQHandler(void)
{
	irqController->handlePeripheralIrq(TIMER3_IRQn);
}


void M0_MCPWM_IRQHandler(void)
{
	irqController->handlePeripheralIrq(MCPWM_IRQn);
}

void M0_ADC0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ADC0_IRQn);
}

void M0_I2C0_OR_I2C1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(I2C0_IRQn);
}

void M0_SGPIO_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SGPIO_INT_IRQn);
}

void M0_SPI_OR_DAC_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SPI_INT_IRQn);
}

void M0_ADC1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ADC1_IRQn);
}

void M0_SSP0_OR_SSP1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SSP0_IRQn);
}

void M0_EVENTROUTER_IRQHandler(void)
{
	irqController->handlePeripheralIrq(EVENTROUTER_IRQn);
}

void M0_USART0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USART0_IRQn);
}

void M0_UART1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(UART1_IRQn);
}

void M0_USART2_OR_C_CAN1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USART2_IRQn);
}

void M0_USART3_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USART3_IRQn);
}

void M0_I2S0_OR_I2S1_QEI_IRQHandler(void)
{
	irqController->handlePeripheralIrq(I2S0_IRQn);
}

void M0_C_CAN0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(C_CAN0_IRQn);
}

void M0_SPIFI_OR_ADCHS_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ADCHS_IRQn);
}

void M0_M0SUB_IRQHandler(void)
{
	irqController->handlePeripheralIrq(M0SUB_IRQn);
}

#ifdef __cplusplus
}
#endif

}

#endif
