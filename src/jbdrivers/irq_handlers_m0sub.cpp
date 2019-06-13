/**
 * @file
 * @brief Bind IRQ Handlers with IRQ Controller (CORE_M0SUB)
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

#ifdef CORE_M0SUB

#include "jbdrivers/IrqController.hpp"

namespace jblib::jbdrivers
{

static IrqController* const irqController = IrqController::getIrqController();

#ifdef __cplusplus
extern "C" {
#endif

void M0S_NMI_Handler(void){
	irqController->handleCortexIrq(NonMaskableInt_IRQn);
	while(1);
}

void M0S_HardFault_Handler(void){
	irqController->handleCortexIrq(HardFault_IRQn);
	while(1);
}

void M0S_SVC_Handler(void){
	irqController->handleCortexIrq(SVCall_IRQn);
	while(1);
}

void M0S_DebugMon_Handler(void){
	irqController->handleCortexIrq(DebugMonitor_IRQn);
	while(1);
}

void M0S_PendSV_Handler(void){
	irqController->handleCortexIrq(PendSV_IRQn);
	while(1);
}

void M0S_SysTick_Handler(void){
	irqController->handleCortexIrq(SysTick_IRQn);
}

#ifdef __CODE_RED
void M0S_DAC_IRQHandler(void){
#else
void M0S_RTC_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(DAC_IRQn);
}

void M0S_M4CORE_IRQHandler(void){
	irqController->handlePeripheralIrq(M4_IRQn);
}

void M0S_DMA_IRQHandler(void){
	irqController->handlePeripheralIrq(DMA_IRQn);
}

void M0S_SGPIO_INPUT_IRQHandler(void){
	irqController->handlePeripheralIrq(SGPIO_INPUT_IRQn);
}

void M0S_SGPIO_MATCH_IRQHandler(void){
	irqController->handlePeripheralIrq(SGPIO_MATCH_IRQn);
}

void M0S_SGPIO_SHIFT_IRQHandler(void){
	irqController->handlePeripheralIrq(SGPIO_SHIFT_IRQn);
}

void M0S_SGPIO_POS_IRQHandler(void){
	irqController->handlePeripheralIrq(SGPIO_POS_IRQn);
}

void M0S_USB0_IRQHandler(void){
	irqController->handlePeripheralIrq(USB0_IRQn);
}

void M0S_USB1_IRQHandler(void){
	irqController->handlePeripheralIrq(USB1_IRQn);
}

void M0S_SCT_IRQHandler(void){
	irqController->handlePeripheralIrq(SCT_IRQn);
}

void M0S_RITIMER_IRQHandler(void){
	irqController->handlePeripheralIrq(RITIMER_IRQn);
}

void M0S_GINT1_IRQHandler(void){
	irqController->handlePeripheralIrq(GINT1_IRQn);
}

void M0S_TIMER1_IRQHandler(void){
	irqController->handlePeripheralIrq(TIMER1_IRQn);
}

void M0S_TIMER2_IRQHandler(void){
	irqController->handlePeripheralIrq(TIMER2_IRQn);
}

void M0S_PIN_INT5_IRQHandler(void){
	irqController->handlePeripheralIrq(PIN_INT5_IRQn);
}

void M0S_MCPWM_IRQHandler(void){
	irqController->handlePeripheralIrq(MCPWM_IRQn);
}

void M0S_ADC0_IRQHandler(void){
	irqController->handlePeripheralIrq(ADC0_IRQn);
}

void M0S_I2C0_IRQHandler(void){
	irqController->handlePeripheralIrq(I2C0_IRQn);
}

void M0S_I2C1_IRQHandler(void){
	irqController->handlePeripheralIrq(I2C1_IRQn);
}

void M0S_SPI_IRQHandler(void){
	irqController->handlePeripheralIrq(SPI_INT_IRQn);
}

void M0S_ADC1_IRQHandler(void){
	irqController->handlePeripheralIrq(ADC1_IRQn);
}

void M0S_SSP0_OR_SSP1_IRQHandler(void){
	irqController->handlePeripheralIrq(SSP0_IRQn);
	irqController->handlePeripheralIrq(SSP1_IRQn);
}

void M0S_EVENTROUTER_IRQHandler(void){
	irqController->handlePeripheralIrq(EVENTROUTER_IRQn);
}

void M0S_USART0_IRQHandler(void){
	irqController->handlePeripheralIrq(USART0_IRQn);
}

void M0S_UART1_IRQHandler(void){
	irqController->handlePeripheralIrq(UART1_IRQn);
}

void M0S_USART2_OR_C_CAN1_IRQHandler(void){
	irqController->handlePeripheralIrq(USART2_IRQn);
	irqController->handlePeripheralIrq(C_CAN1_IRQn);
}

void M0S_USART3_IRQHandler(void){
	irqController->handlePeripheralIrq(USART3_IRQn);
}

#ifdef __CODE_RED
void M0S_I2C0_OR_I2C1_OR_I2S1_OR_QEI_IRQHandler(void){
#else
void M0S_I2S0_OR_I2S1_OR_QEI_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(I2S0_IRQn);
	irqController->handlePeripheralIrq(I2S1_IRQn);
}

void M0S_C_CAN0_IRQHandler(void){
	irqController->handlePeripheralIrq(C_CAN0_IRQn);
}

#ifdef __CODE_RED
void M0S_SPIFI_OR_VADC_IRQHandler(void){
#else
void M0S_SPIFI_OR_ADCHS_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(ADCHS_IRQn);
	irqController->handlePeripheralIrq(SPIFI_IRQn);
}

void M0S_M0APP_IRQHandler(void){
	irqController->handlePeripheralIrq(M0APP_IRQn);
}

#ifdef __cplusplus
}
#endif

}

#endif
