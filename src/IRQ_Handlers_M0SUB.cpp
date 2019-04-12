/*
 * IRQ_Handlers_M0SUB.cpp
 *
 *  Created on: 26.06.2018
 *      Author: Stalker1290
 */

#ifdef CORE_M0SUB

#include "chip.h"
#include "IRQ_Controller.hpp"
#ifdef CORE_LOAD_mesure
#include "CORE_LOAD.hpp"
#endif

static IRQ_CONTROLLER_t& Controller = IRQ_CONTROLLER_t::getIRQController();

#ifdef __cplusplus
extern "C" {
#endif

void M0S_NMI_Handler(void){
	Controller.HANDLE_Cortex_IRQ(NonMaskableInt_IRQn);
	while(1);
}

void M0S_HardFault_Handler(void){
	Controller.HANDLE_Cortex_IRQ(HardFault_IRQn);
	while(1);
}

void M0S_SVC_Handler(void){
	Controller.HANDLE_Cortex_IRQ(SVCall_IRQn);
	while(1);
}

void M0S_DebugMon_Handler(void){
	Controller.HANDLE_Cortex_IRQ(DebugMonitor_IRQn);
	while(1);
}

void M0S_PendSV_Handler(void){
	Controller.HANDLE_Cortex_IRQ(PendSV_IRQn);
	while(1);
}

void M0S_SysTick_Handler(void){
	Controller.HANDLE_Cortex_IRQ(SysTick_IRQn);
}






void M0S_RTC_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(DAC_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(DAC_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(DAC_IRQn);
#endif
}

void M0S_M4CORE_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(M4_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(M4_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(M4_IRQn);
#endif
}

void M0S_DMA_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(DMA_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(DMA_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(DMA_IRQn);
#endif
}

void M0S_SGPIO_INPUT_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SGPIO_INPUT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SGPIO_INPUT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SGPIO_INPUT_IRQn);
#endif
}

void M0S_SGPIO_MATCH_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SGPIO_MATCH_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SGPIO_MATCH_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SGPIO_MATCH_IRQn);
#endif
}

void M0S_SGPIO_SHIFT_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SGPIO_SHIFT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SGPIO_SHIFT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SGPIO_SHIFT_IRQn);
#endif
}

void M0S_SGPIO_POS_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SGPIO_POS_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SGPIO_POS_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SGPIO_POS_IRQn);
#endif
}

void M0S_USB0_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USB0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USB0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USB0_IRQn);
#endif
}

void M0S_USB1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USB1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USB1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USB1_IRQn);
#endif
}

void M0S_SCT_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SCT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SCT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SCT_IRQn);
#endif
}

void M0S_RITIMER_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(RITIMER_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(RITIMER_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(RITIMER_IRQn);
#endif
}

void M0S_GINT1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(GINT1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(GINT1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(GINT1_IRQn);
#endif
}

void M0S_TIMER1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(TIMER1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(TIMER1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(TIMER1_IRQn);
#endif
}

void M0S_TIMER2_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(TIMER2_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(TIMER2_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(TIMER2_IRQn);
#endif
}

void M0S_PIN_INT5_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(PIN_INT5_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(PIN_INT5_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(PIN_INT5_IRQn);
#endif
}

void M0S_MCPWM_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(MCPWM_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(MCPWM_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(MCPWM_IRQn);
#endif
}

void M0S_ADC0_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ADC0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ADC0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ADC0_IRQn);
#endif
}

void M0S_I2C0_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2C0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2C0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2C0_IRQn);
#endif
}

void M0S_I2C1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2C1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2C1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2C1_IRQn);
#endif
}

void M0S_SPI_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SPI_INT_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SPI_INT_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SPI_INT_IRQn);
#endif
}

void M0S_ADC1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ADC1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ADC1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ADC1_IRQn);
#endif
}

void M0S_SSP0_OR_SSP1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(SSP0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(SSP0_IRQn);
	Controller.HANDLE_Peripheral_IRQ(SSP1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(SSP0_IRQn);
#endif
}

void M0S_EVENTROUTER_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(EVENTROUTER_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(EVENTROUTER_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(EVENTROUTER_IRQn);
#endif
}

void M0S_USART0_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USART0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USART0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USART0_IRQn);
#endif
}

void M0S_UART1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(UART1_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(UART1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(UART1_IRQn);
#endif
}

void M0S_USART2_OR_C_CAN1_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USART2_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USART2_IRQn);
	Controller.HANDLE_Peripheral_IRQ(C_CAN1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USART2_IRQn);
#endif
}

void M0S_USART3_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(USART3_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(USART3_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(USART3_IRQn);
#endif
}

void M0S_I2S0_OR_I2S1_OR_QEI_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(I2S0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(I2S0_IRQn);
	Controller.HANDLE_Peripheral_IRQ(I2S1_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(I2S0_IRQn);
#endif
}

void M0S_C_CAN0_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(C_CAN0_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(C_CAN0_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(C_CAN0_IRQn);
#endif
}

void M0S_SPIFI_OR_ADCHS_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(ADCHS_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(ADCHS_IRQn);
	Controller.HANDLE_Peripheral_IRQ(SPIFI_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(ADCHS_IRQn);
#endif
}

void M0S_M0APP_IRQHandler(void){
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Enter_IRQ(M0APP_IRQn);
#endif
	Controller.HANDLE_Peripheral_IRQ(M0APP_IRQn);
#ifdef CORE_LOAD_mesure
	CORE_LOAD_t::Get_core_load_module()->Exit_IRQ(M0APP_IRQn);
#endif
}


#ifdef __cplusplus
}
#endif


#endif
