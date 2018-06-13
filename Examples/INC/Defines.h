/*
 * Defines.h
 *
 *  Created on: 20.01.2017
 *      Author: Stalker1290
 */

#ifndef DEFINES_H_
#define DEFINES_H_


/*******************************IRQ PRIORITY**************************************************************/
#ifdef CORE_M0

//Used IRQ
#define UART1_interrupt_priority		3 //Console M0
#define IPC_interrupt_priority			3 //Inter Processing Communications
#define PININT4_interrupt_priority		3 //Pin interrupt


#define PWM_TIMER_interrupt_priority	3 //PWM 1 Time Engine Lowest Prority
#define RITIMER_interrupt_priority		3 //Time Engine Medium Prority
#define TIMER0_interrupt_priority		3 //void_GPT_0_0 Time Engine Kernel Performer


//Unused IRQ
#define EVENTROUTER_interrupt_priority	3 //ATIMER
#define TIMER3_interrupt_priority		3
#define SCT_TIMER_interrupt_priority	3
#define DMA_interrupt_priority			3
#define UART0_interrupt_priority		3
#define UART2_interrupt_priority		3
#define UART3_interrupt_priority		3
#define ATIMER_Interrrupt_priority		3
#define USB0_Interrrupt_priority		3
#define USB1_Interrrupt_priority		3
#define Ethernet_Interrrupt_priority	3

#endif

#ifdef CORE_M4

//Used IRQ
#define UART0_interrupt_priority		7 //Console M4
#define IPC_interrupt_priority			7 //Inter Processing Communications
#define Ethernet_Interrrupt_priority	5 //Ethernet
#define USB0_Interrrupt_priority		5 //RNDIS + VCOM

#define TIMER1_interrupt_priority 		0 //Time Engine RT Timer
#define PWM_TIMER_interrupt_priority	7 //PWM 0 Time Engine Lowest Prority
#define SYSTICK_interrupt_priority		6 //Time Engine Medium Prority
#define SCT_TIMER_interrupt_priority	5 //Time Engine Kernel Performer

//Unused IRQ
#define EVENTROUTER_interrupt_priority	7
#define RITIMER_interrupt_priority		7
#define PININT0_interrupt_priority		7
#define TIMER0_interrupt_priority		7
#define TIMER2_interrupt_priority		7
#define TIMER3_interrupt_priority		7
#define DMA_interrupt_priority			7
#define UART1_interrupt_priority		7
#define UART2_interrupt_priority		7
#define UART3_interrupt_priority		7
#define ATIMER_Interrrupt_priority		7
#define USB1_Interrrupt_priority		7
#define PININT1_interrupt_priority		7
#define PININT2_interrupt_priority		7
#define PININT3_interrupt_priority		7
#define PININT4_interrupt_priority		7
#define PININT5_interrupt_priority		7
#define PININT6_interrupt_priority		7
#define PININT7_interrupt_priority		7

#endif
/*****************************************************************************************************/


/*************************************Library Common Defines***********************************************/
//CONTROLLER
#define main_proc_num 8
//

//IRQ_Controller
#define Cortex_Listeners_num 4
#define Peripheral_Listeners_num 16
//

//Time Engine
#define Lowest_priority_delay_timer_step_us 	100
#define Medium_priority_delay_timer_step_us		20
#define Kernel_performer_timer_step_us 			1
//Event_Timer
#define Event_TMR_size_of_events 16
//

//Void_Console
#define CONSOLE_CMD_BUF_SIZE 64
#define Console_listeners_num 8
#define Console_Rx_buf_size	64

#define Console_Tx_temp_buf_size 256

#ifdef CORE_M4
#define Console_Tx_buf_size 1024
#endif
#ifdef CORE_M0
#define Console_Tx_buf_size 1024
#endif

#define Console_iface_tx_buf Console_Tx_temp_buf_size
//

//IPC Interfaces
#define IPC_iface_console_M0_rx_buf_size 64
//

//Core_load
#define CORE_LOAD_printf_divider 	100
#define CORE_LOAD_measure_step_ms	20
#define CORE_LOAD_accuracy_percent	1
//

//TCP Server
#define TCP_Server_max_num_broadcast_connections 16
//

//Interface_checker
#define Iface_checker_max_num_ifaces	8

//

//DHCP_server
#define Domain_name "JBravo"
//
//DNS_server
#define DNS_MAX_HOST_NAME_LEN 128

#define Main_page_name_0 "KTP.JBravo"
#define Main_page_name_1 "www.KTP.JBravo"

/*****************************************************************************************************/

/*************************************ETHERNET********************************************************/

#define Eth_Rx_desc_buf_size					200

//Ethernet_phy
#define Eth_phy_Rx_Queue_Len 					32
#define Eth_phy_Tx_Queue_Len 					4
#define Eth_phy_Default_MAC 					{0x08,0x0a,0x35, 0x06, 0x05, 0x05}//{0x08,0x0a,0x35, 0x06, 0x05, 0x04}
#define Eth_phy_Default_speed 					speed_autoneg
#define Eth_phy_Check_link_event_period_us		100000

//#define Eth_phy_USE_DMA_errors_control
#define Eth_phy_DMA_errors_treshhold			128
//
//Ethernet_RNDIS
#define Eth_RNDIS_Rx_Queue_Len 				4
#define Eth_RNDIS_Tx_Queue_Len 				4
#define Eth_RNDIS_Default_MAC 				{0x08,0x0a,0x35, 0x04, 0x05, 0x06}
#define Eth_RNDIS_Default_Adapter_MAC		{0x00,0x0a,0x35, 0x03, 0x02, 0x01}
//

//ARP Module
#define ETX_ARP_REFRESH_RECORDS_TIME_s		((uint32_t)60UL)
#define ETX_ARP_DELETE_RECORDS_TIME_s		((uint32_t)600UL)
#define ETX_IP_TABLE_FOR_ARP_MAX_NUMBER 	((0x10U))
#define ETX_ARP_TABLE_MAX_NUMBER 			((0x20U))

//
/*****************************************************************************************************/



/***********************CORE LOAD MODULE***************************************************************/

#ifdef CORE_LOAD_MES_def
#define CORE_LOAD_MES_accuracy_percent 		1
#define CORE_LOAD_MES_step_ms				200
#endif

/******************************************************************************************************/

/**********************************IPC MODULE**********************************************************/

#define IPCEX_QUEUE_SZ        32
#define IPCEX_MAX_IDS         16
#define IPCEX_MAX_GBLVAL      8

#define IPCEX_ID_FREEMEM					1  /*!< Frees memory allocated by other core */
#define IPCEX_ID_GBLUPDATE					2  /*!< Update global variable or other core */
#define IPCEX_ID_CONSOLE_M0_IFACE			3  /*!< Update global variable or other core */

/*******************************************************************************************************/

/****************************************USB**********************************************************/

//USB_VCOM
#define Num_of_VCOM 2
//
/* Manifest constants used by USBD ROM stack. These values SHOULD NOT BE CHANGED
   for advance features which require usage of USB_CORE_CTRL_T structure.
   Since these are the values used for compiling USB stack.
 */
#define USB_MAX_IF_NUM          8		/*!< Max interface number used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_EP_NUM          6		/*!< Max number of EP used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_PACKET0         64		/*!< Max EP0 packet size used for building USBD ROM. DON'T CHANGE. */
#define USB_FS_MAX_BULK_PACKET  64		/*!< MAXP for FS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_HS_MAX_BULK_PACKET  512		/*!< MAXP for HS bulk EPs used for building USBD ROM. DON'T CHANGE. */

/**********************USB0*********************/
#ifdef CORE_M4

/* Manifest constants defining interface numbers and endpoints used by a
   particular interface in this application.
 */
#define USB0_RNDIS_CIF_NUM		0
#define USB0_RNDIS_DIF_NUM		1
#define USB0_VCOM_CIF_NUM		2
#define USB0_VCOM_DIF_NUM		3
#define USB0_MSC_IF_NUM      	4

#define USB0_RNDIS_IN_EP		0x81
#define USB0_RNDIS_OUT_EP		0x01
#define USB0_RNDIS_INT_EP		0x82
#define USB0_VCOM_IN_EP			0x83
#define USB0_VCOM_OUT_EP		0x03
#define USB0_VCOM_INT_EP		0x84
#define USB0_MSC_IN_EP          0x85
#define USB0_MSC_OUT_EP        	0x05

#endif

/**********************USB1****************************/
#ifdef CORE_M4

#define USB1_RNDIS_CIF_NUM		0
#define USB1_RNDIS_DIF_NUM		1
#define USB1_VCOM_CIF_NUM		2
#define USB1_VCOM_DIF_NUM		3
#define USB1_MSC_IF_NUM      	4

#define USB1_RNDIS_IN_EP		0x81
#define USB1_RNDIS_OUT_EP		0x01
#define USB1_RNDIS_INT_EP		0x82
#define USB1_VCOM_IN_EP			0x83
#define USB1_VCOM_OUT_EP		0x03
#define USB1_VCOM_INT_EP		0x84

#endif
/*******************************************************************************************************/

/********************************************USER***********************************************************/
#ifdef CORE_M0

//CC1200
#define cc1200_SSP_freq 7000000
#define cc1200_LED LED2
//


#endif


#ifdef CORE_M4

#define Default_IP 					{192,168,1,1}
#define Default_GW 					{192,168,1,1}
#define Default_NETMASK 			{255,255,255,0}

#define ETH_ROUTER_NUM_OF_NETWORK_INTF 	2
#define ETH_ROUTER_NUM_OF_LISTENERS		8

//UDP
#define CONSOLE_M4_MCAST_IP		{227,0,1,1}
#define CONSOLE_M4_UDP_SPORT	38094
#define CONSOLE_M4_UDP_DPORT	38095


#define CONSOLE_M0_MCAST_IP		{227,0,1,1}
#define CONSOLE_M0_UDP_SPORT	38090
#define CONSOLE_M0_UDP_DPORT	38091

#endif


//Crypto_check
#define Crypto_check_Key_length		24
#define Crypto_check_Gamma_length	64
//

//Zinger_boot
#define ZINGER_BOOT_RX_BUFSIZE (2048)

#define ZINGER_BOOT_TIMEOUT_INI_PACKET_MS (100)
#define ZINGER_BOOT_START_WAITING_TIMEOUT_MS (30000)
#define ZINGER_BOOT_TIMEOUT_FOR_1_STRING_MS (32*8/64) // 32byte * 8 bit/byte / 128 kbit/s
#define ZINGER_BOOT_TIMEOUT_BASE_MS (50)
//

//Parameters
#define PARAM_STR_MAX_SIZE		64
#define PARAM_FILE_NAME			"Bootini.txt"
#define PARAM_PATH				"0:/"

/***********************************************************************************************************/


#endif /* DEFINES_H_ */
