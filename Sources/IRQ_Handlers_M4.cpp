/*
 * IRQ_Handlers_M4.cpp
 *
 *  Created on: 12.07.2017
 *      Author: Stalker1290
 */

#include "chip.h"
#include "IRQ_Controller.hpp"
#ifdef CORE_LOAD_mesure
#include "CORE_LOAD.hpp"
#endif

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
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(DAC_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(DAC_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(DAC_IRQn);
#endif
}

void M0APP_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(M0APP_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(M0APP_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(M0APP_IRQn);
#endif
}

void DMA_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(DMA_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(DMA_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(DMA_IRQn);
#endif
}

void FLASHEEPROM_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(RESERVED2_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(RESERVED2_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(RESERVED2_IRQn);
#endif
}

void ETHERNET_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ETHERNET_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ETHERNET_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ETHERNET_IRQn);
#endif
}

void SDIO_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SDIO_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SDIO_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SDIO_IRQn);
#endif
}

void LCD_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(LCD_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(LCD_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(LCD_IRQn);
#endif
}

void USB0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USB0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USB0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USB0_IRQn);
#endif
}

void USB1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USB1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USB1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USB1_IRQn);
#endif
}

void SCT_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SCT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SCT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SCT_IRQn);
#endif
}

void RITIMER_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(RITIMER_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(RITIMER_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(RITIMER_IRQn);
#endif
}

void TIMER0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(TIMER0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(TIMER0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(TIMER0_IRQn);
#endif
}

void TIMER1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(TIMER1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(TIMER1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(TIMER1_IRQn);
#endif
}

void TIMER2_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(TIMER2_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(TIMER2_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(TIMER2_IRQn);
#endif
}

void TIMER3_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(TIMER3_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(TIMER3_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(TIMER3_IRQn);
#endif
}

void MCPWM_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(MCPWM_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(MCPWM_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(MCPWM_IRQn);
#endif
}

void ADC0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ADC0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ADC0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ADC0_IRQn);
#endif
}

void I2C0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2C0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2C0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2C0_IRQn);
#endif
}

void I2C1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2C1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2C1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2C1_IRQn);
#endif
}

void SPI_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SPI_INT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SPI_INT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SPI_INT_IRQn);
#endif
}

void ADC1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ADC1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ADC1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ADC1_IRQn);
#endif
}

void SSP0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SSP0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SSP0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SSP0_IRQn);
#endif
}

void SSP1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SSP1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SSP1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SSP1_IRQn);
#endif
}

void USART0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USART0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USART0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USART0_IRQn);
#endif
}

void UART1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(UART1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(UART1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(UART1_IRQn);
#endif
}

void USART2_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USART2_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USART2_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USART2_IRQn);
#endif
}

void USART3_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USART3_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USART3_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USART3_IRQn);
#endif
}

void I2S0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2S0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2S0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2S0_IRQn);
#endif
}

void I2S1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2S1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2S1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2S1_IRQn);
#endif
}

void SPIFI_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(RESERVED4_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(RESERVED4_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(RESERVED4_IRQn);
#endif
}

void SGPIO_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SGPIO_INT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SGPIO_INT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SGPIO_INT_IRQn);
#endif
}

void PIN_INT0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT0_IRQn);
#endif
}

void PIN_INT1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT1_IRQn);
#endif
}

void PIN_INT2_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT2_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT2_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT2_IRQn);
#endif
}

void PIN_INT3_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT3_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT3_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT3_IRQn);
#endif
}

void PIN_INT4_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT4_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT4_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT4_IRQn);
#endif
}

void PIN_INT5_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT5_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT5_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT5_IRQn);
#endif
}

void PIN_INT6_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT6_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT6_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT6_IRQn);
#endif
}

void PIN_INT7_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT7_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT7_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT7_IRQn);
#endif
}

void GINT0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(GINT0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(GINT0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(GINT0_IRQn);
#endif
}

void GINT1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(GINT1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(GINT1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(GINT1_IRQn);
#endif
}

void EVENTROUTER_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(EVENTROUTER_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(EVENTROUTER_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(EVENTROUTER_IRQn);
#endif
}

void C_CAN1_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(C_CAN1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(C_CAN1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(C_CAN1_IRQn);
#endif
}

void ADCHS_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ADCHS_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ADCHS_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ADCHS_IRQn);
#endif
}

void ATIMER_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ATIMER_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ATIMER_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ATIMER_IRQn);
#endif
}

void RTC_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(RTC_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(RTC_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(RTC_IRQn);
#endif
}

void WWDT_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(WWDT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(WWDT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(WWDT_IRQn);
#endif
}

void M0SUB_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(M0SUB_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(M0SUB_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(M0SUB_IRQn);
#endif
}

void C_CAN0_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(C_CAN0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(C_CAN0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(C_CAN0_IRQn);
#endif
}

void QEI_IRQHandler(void)
{
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(QEI_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(QEI_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(QEI_IRQn);
#endif
}

#ifdef __cplusplus
}
#endif



