/*
 * Compile_Defines.h
 *
 *  Created on: 23.05.2017
 *      Author: Stalker1290
 */

#ifndef __COMPILE_DEFINES_H_
#define __COMPILE_DEFINES_H_


#ifdef CORE_M0
#define USE_CONSOLE
//#define EthM_console
//#define Arp_console
//#define SPIFI_console
//#define USB_console
//#define RNDIS_console
//#define VCOM_console
//#define CORE_LOAD_mes_console

//#define CORE_LOAD_mesure
#endif


#ifdef CORE_M4
#define USE_CONSOLE
//#define EthM_console
//#define Arp_console
//#define USB_console
//#define RNDIS_console
//#define VCOM_console
//#define SPIFI_console
//#define TCP_server_CONSOLE
//#define CORE_LOAD_mes_console

//#define CORE_LOAD_mesure

//#define USE_Crypt_boot

#define USE_LWIP
#endif

#ifdef __CODE_RED

#define FLASHEEPROM_IRQHandler 	FLASH_EEPROM_IRQHandler
#define ETHERNET_IRQHandler 	ETH_IRQHandler
#define RITIMER_IRQHandler 		RIT_IRQHandler
#define USART0_IRQHandler 		UART0_IRQHandler
#define USART2_IRQHandler 		UART2_IRQHandler
#define USART3_IRQHandler		UART3_IRQHandler
#define PIN_INT0_IRQHandler		GPIO0_IRQHandler
#define PIN_INT1_IRQHandler		GPIO1_IRQHandler
#define PIN_INT2_IRQHandler		GPIO2_IRQHandler
#define PIN_INT3_IRQHandler		GPIO3_IRQHandler
#define PIN_INT4_IRQHandler		GPIO4_IRQHandler
#define PIN_INT5_IRQHandler		GPIO5_IRQHandler
#define PIN_INT6_IRQHandler		GPIO6_IRQHandler
#define PIN_INT7_IRQHandler		GPIO7_IRQHandler
#define EVENTROUTER_IRQHandler 	EVRT_IRQHandler
#define C_CAN1_IRQHandler		CAN1_IRQHandler
#define WWDT_IRQHandler			WDT_IRQHandler
#define C_CAN0_IRQHandler		CAN0_IRQHandler

#define M0_NMI_Handler						NMI_Handler
#define M0_HardFault_Handler 				HardFault_Handler
#define M0_SVC_Handler 						SVC_Handler
#define M0_PendSV_Handler 					PendSV_Handler
#define M0_RTC_IRQHandler					RTC_IRQHandler
#define M0_M4CORE_IRQHandler 				M4_IRQHandler
#define M0_DMA_IRQHandler 					DMA_IRQHandler
#define M0_FLASHEEPROMAT_IRQHandler 		FLASH_EEPROM_ATIMER_IRQHandler
#define M0_ETHERNET_IRQHandler				ETH_IRQHandler
#define M0_SDIO_IRQHandler 					SDIO_IRQHandler
#define M0_LCD_IRQHandler 					LCD_IRQHandler
#define M0_USB0_IRQHandler					USB0_IRQHandler
#define M0_USB1_IRQHandler					USB1_IRQHandler
#define M0_SCT_IRQHandler					SCT_IRQHandler
#define M0_RITIMER_OR_WWDT_IRQHandler		RIT_WDT_IRQHandler
#define M0_TIMER0_IRQHandler				TIMER0_IRQHandler
#define M0_GINT1_IRQHandler					GINT1_IRQHandler
#define M0_PIN_INT4_IRQHandler				GPIO4_IRQHandler
#define M0_TIMER3_IRQHandler				TIMER3_IRQHandler
#define M0_MCPWM_IRQHandler					MCPWM_IRQHandler
#define M0_ADC0_IRQHandler					ADC0_IRQHandler
#define M0_I2C0_OR_I2C1_IRQHandler			I2C0_IRQHandler
#define M0_SGPIO_IRQHandler					SGPIO_IRQHandler
#define M0_SPI_OR_DAC_IRQHandler			SPI_DAC_IRQHandler
#define M0_ADC1_IRQHandler					ADC1_IRQHandler
#define M0_SSP0_OR_SSP1_IRQHandler			SSP0_SSP1_IRQHandler
#define M0_EVENTROUTER_IRQHandler			EVRT_IRQHandler
#define M0_USART0_IRQHandler				UART0_IRQHandler
#define M0_UART1_IRQHandler					UART1_IRQHandler
#define M0_USART2_OR_C_CAN1_IRQHandler		UART2_IRQHandler
#define M0_USART3_IRQHandler				UART3_IRQHandler
#define M0_I2S0_OR_I2S1_QEI_IRQHandler		I2S0_I2S1_QEI_IRQHandler
#define M0_C_CAN0_IRQHandler				CAN0_IRQHandler
#define M0_SPIFI_OR_ADCHS_IRQHandler		SPIFI_ADCHS_IRQHandler
#define M0_M0SUB_IRQHandler					M0SUB_IRQHandler

#endif

#endif /* COMPILE_DEFINES_H_ */
