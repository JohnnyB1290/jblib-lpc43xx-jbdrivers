/*
 * PCB_Defines.h
 *
 *  Created on: 03.08.2017
 *      Author: Stalker1290
 */

#ifndef __PCB_DEFINES_H_
#define __PCB_DEFINES_H_

#include "chip.h"

/*************************SSP***************************/
//SSP0
#define SCK0_CLK_NUM 0
//#define SCK0_PORT 		0
//#define SCK0_PIN 		0
#define SCK0_SCU_FUNC SCU_MODE_FUNC0
#define SSEL0_PORT 0
#define SSEL0_PIN 0
#define SSEL0_GPIOPORT 	0
#define SSEL0_GPIOPIN 	6
#define SSEL0_SCU_FUNC SCU_MODE_FUNC0
#define MOSI0_PORT 0
#define MOSI0_PIN 0
#define MOSI0_SCU_FUNC SCU_MODE_FUNC0
#define MISO0_PORT 0
#define MISO0_PIN 0
#define MISO0_SCU_FUNC SCU_MODE_FUNC0
#define SSP0_BITS_PER_FRAME SSP_BITS_8
#define SSP0_FRAMEFORMAT SSP_FRAMEFORMAT_SPI
#define SSP0_CLOCKMODE SSP_CLOCK_CPHA0_CPOL0
//SSP1
//#define SCK1_CLK_NUM 0
#define SCK1_PORT 0xF
#define SCK1_PIN 4
#define SCK1_SCU_FUNC SCU_MODE_FUNC0
#define SSEL1_PORT 1
#define SSEL1_PIN 5
#define SSEL1_GPIOPORT 	1
#define SSEL1_GPIOPIN 	8
#define SSEL1_SCU_FUNC SCU_MODE_FUNC0
#define MOSI1_PORT 1
#define MOSI1_PIN 4
#define MOSI1_SCU_FUNC SCU_MODE_FUNC5
#define MISO1_PORT 1
#define MISO1_PIN 3
#define MISO1_SCU_FUNC SCU_MODE_FUNC5
#define SSP1_BITS_PER_FRAME SSP_BITS_8
#define SSP1_FRAMEFORMAT SSP_FRAMEFORMAT_SPI
#define SSP1_CLOCKMODE SSP_CLOCK_CPHA0_CPOL0

/**********************************************************/

/*************************SPIFI***************************/
#define SPIFI_CLK_PORT 				0x03
#define SPIFI_CLK_PIN				3
#define SPIFI_CLK_SCU_MODE_FUNC		SCU_MODE_FUNC3

#define SPIFI_D3_PORT 				0x03
#define SPIFI_D3_PIN				4
#define SPIFI_D3_SCU_MODE_FUNC		SCU_MODE_FUNC3

#define SPIFI_D2_PORT 				0x03
#define SPIFI_D2_PIN				5
#define SPIFI_D2_SCU_MODE_FUNC		SCU_MODE_FUNC3

#define SPIFI_D1_PORT 				0x03
#define SPIFI_D1_PIN				6
#define SPIFI_D1_SCU_MODE_FUNC		SCU_MODE_FUNC3

#define SPIFI_D0_PORT 				0x03
#define SPIFI_D0_PIN				7
#define SPIFI_D0_SCU_MODE_FUNC		SCU_MODE_FUNC3

#define SPIFI_CS_PORT 				0x03
#define SPIFI_CS_PIN				8
#define SPIFI_CS_SCU_MODE_FUNC		SCU_MODE_FUNC3


/**********************************************************/


/*************************UART*******************************/

#define UART0_TX_PORT 6
#define UART0_TX_PIN 4
#define UART0_TX_SCU_FUNC SCU_MODE_FUNC2
#define UART0_RX_PORT 6
#define UART0_RX_PIN 5
#define UART0_RX_SCU_FUNC SCU_MODE_FUNC2

#define UART1_TX_PORT 1
#define UART1_TX_PIN 13
#define UART1_TX_SCU_FUNC SCU_MODE_FUNC1
#define UART1_RX_PORT 1
#define UART1_RX_PIN 14
#define UART1_RX_SCU_FUNC SCU_MODE_FUNC1

#define UART2_TX_PORT 0
#define UART2_TX_PIN 0
#define UART2_TX_SCU_FUNC SCU_MODE_FUNC0
#define UART2_RX_PORT 0
#define UART2_RX_PIN 0
#define UART2_RX_SCU_FUNC SCU_MODE_FUNC0

#define UART3_TX_PORT 0
#define UART3_TX_PIN 0
#define UART3_TX_SCU_FUNC SCU_MODE_FUNC0
#define UART3_RX_PORT 0
#define UART3_RX_PIN 0
#define UART3_RX_SCU_FUNC SCU_MODE_FUNC0

/**********************************************************/

/*************************GPIOS***************************/

#pragma pack(push, 1)
typedef struct BOARD_GPIO_struct
{
	unsigned char port;
	unsigned char pin;
	unsigned char gpio_port;
	unsigned char gpio_pin;
	unsigned char scu_mode;
}BOARD_GPIO_t;
#pragma pack(pop)

#define LED0 	0
#define LED1 	1

#define LED0_PORT 2
#define LED0_PIN 11
#define LED0_GPIOPORT 1
#define LED0_GPIOPIN 11
#define LED0_SCU_MODE_FUNC SCU_MODE_FUNC0

#define LED1_PORT 2
#define LED1_PIN 13
#define LED1_GPIOPORT 1
#define LED1_GPIOPIN 13
#define LED1_SCU_MODE_FUNC SCU_MODE_FUNC0


/**********************************************************/


/*************************Ethernet***************************/
#define USE_RMII
//#define USE_MII
//#define USE_PHY_RESET_PIN

#define ETH_TXD0_PORT 				1
#define ETH_TXD0_PIN 				18
#define ETH_TXD0_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_TXD1_PORT 				1
#define ETH_TXD1_PIN 				20
#define ETH_TXD1_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_TXEN_PORT 				0
#define ETH_TXEN_PIN 				1
#define ETH_TXEN_MODE_FUNC			SCU_MODE_FUNC6
#define ETH_RXD0_PORT 				1
#define ETH_RXD0_PIN 				15
#define ETH_RXD0_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_RXD1_PORT 				0
#define ETH_RXD1_PIN 				0
#define ETH_RXD1_MODE_FUNC			SCU_MODE_FUNC2
#define ETH_CRS_DV_PORT 			1
#define ETH_CRS_DV_PIN 				16
#define ETH_CRS_DV_MODE_FUNC		SCU_MODE_FUNC7
#define ETH_MDIO_PORT	 			1
#define ETH_MDIO_PIN 				17
#define ETH_MDIO_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_MDC_PORT	 			2
#define ETH_MDC_PIN 				0
#define ETH_MDC_MODE_FUNC			SCU_MODE_FUNC7
#define ETH_REF_CLK_PORT	 		1
#define ETH_REF_CLK_PIN 			19
#define ETH_REF_CLK_MODE_FUNC		SCU_MODE_FUNC0
#ifdef USE_MII
#define ETH_RX_ER_PORT				0xC
#define ETH_RX_ER_PIN				9
#define ETH_RX_ER_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_RXDV_PORT				0xC
#define ETH_RXDV_PIN				8
#define ETH_RXDV_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_RXD2_PORT				0x9
#define ETH_RXD2_PIN				3
#define ETH_RXD2_MODE_FUNC			SCU_MODE_FUNC5
#define ETH_RXD3_PORT				0x9
#define ETH_RXD3_PIN				2
#define ETH_RXD3_MODE_FUNC			SCU_MODE_FUNC5
#define ETH_RXCLK_PORT				0xC
#define ETH_RXCLK_PIN				0
#define ETH_RXCLK_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_TXD2_PORT				0xC
#define ETH_TXD2_PIN				2
#define ETH_TXD2_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_TXD3_PORT				0xC
#define ETH_TXD3_PIN				3
#define ETH_TXD3_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_TX_ER_PORT				0xC
#define ETH_TX_ER_PIN				5
#define ETH_TX_ER_MODE_FUNC			SCU_MODE_FUNC3
#define ETH_COL_PORT				0x9
#define ETH_COL_PIN					6
#define ETH_COL_MODE_FUNC			SCU_MODE_FUNC5
#endif

#ifdef USE_PHY_RESET_PIN
#define ETH_PHY_RESET_PORT	 		1
#define ETH_PHY_RESET_PIN 			0
#define ETH_PHY_RESET_GPIO_PORT	 	0
#define ETH_PHY_RESET_GPIO_PIN 		4
#define ETH_PHY_RESET_MODE_FUNC		SCU_MODE_FUNC0
#endif

#ifdef ETH_ACT_LED
#define ETH_ACT_LED_PORT 			0xC
#define ETH_ACT_LED_PIN 			11
#define ETH_ACT_LED_GPIOPORT		6
#define ETH_ACT_LED_GPIOPIN 		10
#define ETH_ACT_LED_SCU_MODE_FUNC 	SCU_MODE_FUNC4
#endif
/**********************************************************/

/*************************User***************************/



/***************************************************************/

#ifdef CORE_M0

//cc1200
#define CC1200_SSP_num 1

#define CC1200_GPIO_PORT 			1
#define CC1200_GPIO_PIN 			8
#define CC1200_GPIO_GPIOPORT 		1
#define CC1200_GPIO_GPIOPIN 		1
#define CC1200_GPIO_SCU_MODE_FUNC 	SCU_MODE_FUNC0

//

#endif

#endif /* PCB_DEFINES_H_ */
