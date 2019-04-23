/*
 * IRQ_Handlers_M4.cpp
 *
 *  Created on: 12.07.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef CORE_M4

#include "chip.h"
#include "IRQ_Controller.hpp"
static IRQ_CONTROLLER_t& Controller = IRQ_CONTROLLER_t::getIRQController();

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(NonMaskableInt_IRQn);
	while(1);
}

void HardFault_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(HardFault_IRQn);
	while(1);
}

void MemManage_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(MemoryManagement_IRQn);
	while(1);
}

void BusFault_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(BusFault_IRQn);
	while(1);
}

void UsageFault_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(UsageFault_IRQn);
	while(1);
}

void SVC_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(SVCall_IRQn);
	while(1);
}

void DebugMon_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(DebugMonitor_IRQn);
	while(1);
}

void PendSV_Handler(void)
{
	Controller.HANDLE_Cortex_IRQ(PendSV_IRQn);
	while(1);
}

void SysTick_Handler(void)
{

	Controller.HANDLE_Cortex_IRQ(SysTick_IRQn);
}

void DAC_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(DAC_IRQn);
}

void M0APP_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(M0APP_IRQn);
}

void DMA_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(DMA_IRQn);
}

#ifdef __CODE_RED
void FLASH_EEPROM_IRQHandler(void){
#else
void FLASHEEPROM_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(RESERVED2_IRQn);
}

#ifdef __CODE_RED
void ETH_IRQHandler(void){
#else
void ETHERNET_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(ETHERNET_IRQn);
}

void SDIO_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SDIO_IRQn);
}

void LCD_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(LCD_IRQn);
}

void USB0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USB0_IRQn);
}

void USB1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(USB1_IRQn);
}

void SCT_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SCT_IRQn);
}

#ifdef __CODE_RED
void RIT_IRQHandler(void){
#else
void RITIMER_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(RITIMER_IRQn);
}

void TIMER0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(TIMER0_IRQn);
}

void TIMER1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(TIMER1_IRQn);
}

void TIMER2_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(TIMER2_IRQn);
}

void TIMER3_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(TIMER3_IRQn);
}

void MCPWM_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(MCPWM_IRQn);
}

void ADC0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ADC0_IRQn);
}

void I2C0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(I2C0_IRQn);
}

void I2C1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(I2C1_IRQn);
}

void SPI_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SPI_INT_IRQn);
}

void ADC1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ADC1_IRQn);
}

void SSP0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SSP0_IRQn);
}

void SSP1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SSP1_IRQn);
}

#ifdef __CODE_RED
void UART0_IRQHandler(void){
#else
void USART0_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(USART0_IRQn);
}

void UART1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(UART1_IRQn);
}

#ifdef __CODE_RED
void UART2_IRQHandler(void){
#else
void USART2_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(USART2_IRQn);
}

#ifdef __CODE_RED
void UART3_IRQHandler(void){
#else
void USART3_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(USART3_IRQn);
}

void I2S0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(I2S0_IRQn);
}

void I2S1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(I2S1_IRQn);
}

void SPIFI_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(RESERVED4_IRQn);
}

void SGPIO_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(SGPIO_INT_IRQn);
}

#ifdef __CODE_RED
void GPIO0_IRQHandler(void){
#else
void PIN_INT0_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT0_IRQn);
}

#ifdef __CODE_RED
void GPIO1_IRQHandler(void){
#else
void PIN_INT1_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT1_IRQn);
}

#ifdef __CODE_RED
void GPIO2_IRQHandler(void){
#else
void PIN_INT2_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT2_IRQn);
}

#ifdef __CODE_RED
void GPIO3_IRQHandler(void){
#else
void PIN_INT3_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT3_IRQn);
}

#ifdef __CODE_RED
void GPIO4_IRQHandler(void){
#else
void PIN_INT4_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT4_IRQn);
}

#ifdef __CODE_RED
void GPIO5_IRQHandler(void){
#else
void PIN_INT5_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT5_IRQn);
}

#ifdef __CODE_RED
void GPIO6_IRQHandler(void){
#else
void PIN_INT6_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT6_IRQn);
}

void PIN_INT7_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(PIN_INT7_IRQn);
}

void GINT0_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(GINT0_IRQn);
}

void GINT1_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(GINT1_IRQn);
}

#ifdef __CODE_RED
void EVRT_IRQHandler(void){
#else
void EVENTROUTER_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(EVENTROUTER_IRQn);
}

#ifdef __CODE_RED
void CAN1_IRQHandler(void){
#else
void C_CAN1_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(C_CAN1_IRQn);
}

void ADCHS_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ADCHS_IRQn);
}

void ATIMER_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(ATIMER_IRQn);
}

void RTC_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(RTC_IRQn);
}

#ifdef __CODE_RED
void WDT_IRQHandler(void){
#else
void WWDT_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(WWDT_IRQn);
}

void M0SUB_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(M0SUB_IRQn);
}

#ifdef __CODE_RED
void CAN0_IRQHandler(void){
#else
void C_CAN0_IRQHandler(void){
#endif
	Controller.HANDLE_Peripheral_IRQ(C_CAN0_IRQn);
}

void QEI_IRQHandler(void)
{
	Controller.HANDLE_Peripheral_IRQ(QEI_IRQn);
}

#ifdef __cplusplus
}
#endif

#endif

