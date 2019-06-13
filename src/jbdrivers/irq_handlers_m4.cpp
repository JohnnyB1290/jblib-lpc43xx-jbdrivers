/**
 * @file
 * @brief Bind IRQ Handlers with IRQ irqController (CORE_M4)
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

#ifdef CORE_M4

#include "IrqController.hpp"

namespace jblib::jbdrivers
{

static IrqController* const irqController = IrqController::getIrqController();

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void)
{
	irqController->handleCortexIrq(NonMaskableInt_IRQn);
	while(1);
}

void HardFault_Handler(void)
{
	irqController->handleCortexIrq(HardFault_IRQn);
	while(1);
}

void MemManage_Handler(void)
{
	irqController->handleCortexIrq(MemoryManagement_IRQn);
	while(1);
}

void BusFault_Handler(void)
{
	irqController->handleCortexIrq(BusFault_IRQn);
	while(1);
}

void UsageFault_Handler(void)
{
	irqController->handleCortexIrq(UsageFault_IRQn);
	while(1);
}

void SVC_Handler(void)
{
	irqController->handleCortexIrq(SVCall_IRQn);
	while(1);
}

void DebugMon_Handler(void)
{
	irqController->handleCortexIrq(DebugMonitor_IRQn);
	while(1);
}

void PendSV_Handler(void)
{
	irqController->handleCortexIrq(PendSV_IRQn);
	while(1);
}

void SysTick_Handler(void)
{

	irqController->handleCortexIrq(SysTick_IRQn);
}

void DAC_IRQHandler(void)
{
	irqController->handlePeripheralIrq(DAC_IRQn);
}

void M0APP_IRQHandler(void)
{
	irqController->handlePeripheralIrq(M0APP_IRQn);
}

void DMA_IRQHandler(void)
{
	irqController->handlePeripheralIrq(DMA_IRQn);
}

#ifdef __CODE_RED
void FLASH_EEPROM_IRQHandler(void){
#else
void FLASHEEPROM_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(RESERVED2_IRQn);
}

#ifdef __CODE_RED
void ETH_IRQHandler(void){
#else
void ETHERNET_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(ETHERNET_IRQn);
}

void SDIO_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SDIO_IRQn);
}

void LCD_IRQHandler(void)
{
	irqController->handlePeripheralIrq(LCD_IRQn);
}

void USB0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USB0_IRQn);
}

void USB1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(USB1_IRQn);
}

void SCT_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SCT_IRQn);
}

#ifdef __CODE_RED
void RIT_IRQHandler(void){
#else
void RITIMER_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(RITIMER_IRQn);
}

void TIMER0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(TIMER0_IRQn);
}

void TIMER1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(TIMER1_IRQn);
}

void TIMER2_IRQHandler(void)
{
	irqController->handlePeripheralIrq(TIMER2_IRQn);
}

void TIMER3_IRQHandler(void)
{
	irqController->handlePeripheralIrq(TIMER3_IRQn);
}

void MCPWM_IRQHandler(void)
{
	irqController->handlePeripheralIrq(MCPWM_IRQn);
}

void ADC0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ADC0_IRQn);
}

void I2C0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(I2C0_IRQn);
}

void I2C1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(I2C1_IRQn);
}

void SPI_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SPI_INT_IRQn);
}

void ADC1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ADC1_IRQn);
}

void SSP0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SSP0_IRQn);
}

void SSP1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SSP1_IRQn);
}

#ifdef __CODE_RED
void UART0_IRQHandler(void){
#else
void USART0_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(USART0_IRQn);
}

void UART1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(UART1_IRQn);
}

#ifdef __CODE_RED
void UART2_IRQHandler(void){
#else
void USART2_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(USART2_IRQn);
}

#ifdef __CODE_RED
void UART3_IRQHandler(void){
#else
void USART3_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(USART3_IRQn);
}

void I2S0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(I2S0_IRQn);
}

void I2S1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(I2S1_IRQn);
}

void SPIFI_IRQHandler(void)
{
	irqController->handlePeripheralIrq(RESERVED4_IRQn);
}

void SGPIO_IRQHandler(void)
{
	irqController->handlePeripheralIrq(SGPIO_INT_IRQn);
}

#ifdef __CODE_RED
void GPIO0_IRQHandler(void){
#else
void PIN_INT0_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT0_IRQn);
}

#ifdef __CODE_RED
void GPIO1_IRQHandler(void){
#else
void PIN_INT1_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT1_IRQn);
}

#ifdef __CODE_RED
void GPIO2_IRQHandler(void){
#else
void PIN_INT2_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT2_IRQn);
}

#ifdef __CODE_RED
void GPIO3_IRQHandler(void){
#else
void PIN_INT3_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT3_IRQn);
}

#ifdef __CODE_RED
void GPIO4_IRQHandler(void){
#else
void PIN_INT4_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT4_IRQn);
}

#ifdef __CODE_RED
void GPIO5_IRQHandler(void){
#else
void PIN_INT5_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT5_IRQn);
}

#ifdef __CODE_RED
void GPIO6_IRQHandler(void){
#else
void PIN_INT6_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(PIN_INT6_IRQn);
}

void PIN_INT7_IRQHandler(void)
{
	irqController->handlePeripheralIrq(PIN_INT7_IRQn);
}

void GINT0_IRQHandler(void)
{
	irqController->handlePeripheralIrq(GINT0_IRQn);
}

void GINT1_IRQHandler(void)
{
	irqController->handlePeripheralIrq(GINT1_IRQn);
}

#ifdef __CODE_RED
void EVRT_IRQHandler(void){
#else
void EVENTROUTER_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(EVENTROUTER_IRQn);
}

#ifdef __CODE_RED
void CAN1_IRQHandler(void){
#else
void C_CAN1_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(C_CAN1_IRQn);
}

void ADCHS_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ADCHS_IRQn);
}

void ATIMER_IRQHandler(void)
{
	irqController->handlePeripheralIrq(ATIMER_IRQn);
}

void RTC_IRQHandler(void)
{
	irqController->handlePeripheralIrq(RTC_IRQn);
}

#ifdef __CODE_RED
void WDT_IRQHandler(void){
#else
void WWDT_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(WWDT_IRQn);
}

void M0SUB_IRQHandler(void)
{
	irqController->handlePeripheralIrq(M0SUB_IRQn);
}

#ifdef __CODE_RED
void CAN0_IRQHandler(void){
#else
void C_CAN0_IRQHandler(void){
#endif
	irqController->handlePeripheralIrq(C_CAN0_IRQn);
}

void QEI_IRQHandler(void)
{
	irqController->handlePeripheralIrq(QEI_IRQn);
}

#ifdef __cplusplus
}
#endif

}

#endif
