/*
 * CONTROLLER.hpp
 *
 *  Created on: 31.07.2017
 *      Author: Stalker1290
 */

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include <stdio.h>
#include "chip.h"
#include "Defines.h"
#include "Common_interfaces.hpp"
#include "ATIMER.hpp"
#include "DAC.hpp"
#include "EEPROM.hpp"
#include "GPTimers.hpp"
#include "IRQ_Controller.hpp"
#include "PIN_INT.hpp"
#include "PWM_GPT.hpp"
#include "RITMR.hpp"
#include "SCT_GPT.hpp"
#include "SSP.hpp"
#include "SYS_TICK_GPT.hpp"
#include "WDT.hpp"
#include "SPIFI.hpp"
#include "VIRTTMR_ON_GPT.hpp"
#include "DMA_Controller.hpp"
#include "UART_INT.hpp"
#include "IPC.hpp"
#include "Void_Ethernet.hpp"
#include "Ethernet/EthM_LPC.hpp"
#include "Ethernet_router.hpp"
#include "USB/USB_Device_use_rom/USB_Device.hpp"
#include "Time_Engine.hpp"
#ifdef USE_CONSOLE
#include "Void_Console.hpp"
#endif

class CONTROLLER_t
{
public:
	static CONTROLLER_t* get_CONTROLLER(void);
	void Initialize(void);
	void GPIO_ON(uint8_t num);
	void GPIO_OFF(uint8_t num);
	void GPIO_TGL(uint8_t num);
	void _delay_ms(uint32_t ms);
	void _delay_us(uint32_t us);
	void Start_M0APP(uint32_t img_addr);
	void Reset_periph(void);
	void Soft_reset(void);
	void Go_to_app(uint32_t App_addr);
	void Do_main(void);
	void Add_main_procedure(Callback_Interface_t* main_proc);
	void Delete_main_procedure(Callback_Interface_t* main_proc);
	static ATIMER_t* 	get_Timer_ATIMER(void){ return &(ATIMER_t::getATIMER()); }
	static GPTimer_t* 	get_Timer_GPTimer_0(void){ return GPTimer_t::get_GPTimer(0); }
#ifdef CORE_M4
	static GPTimer_t* 	get_Timer_GPTimer_1(void){ return GPTimer_t::get_GPTimer(1); }
	static GPTimer_t* 	get_Timer_GPTimer_2(void){ return GPTimer_t::get_GPTimer(2); }
#endif
	static GPTimer_t* 	get_Timer_GPTimer_3(void){ return GPTimer_t::get_GPTimer(3); }
	static VOID_TIMER_t* get_Timer_void_PWMTimer_0(void){ return PWM_TIMER_t::get_PWM_Timer(0); }
	static VOID_TIMER_t* get_Timer_void_PWMTimer_1(void){ return PWM_TIMER_t::get_PWM_Timer(1); }
	static VOID_TIMER_t* get_Timer_void_PWMTimer_2(void){ return PWM_TIMER_t::get_PWM_Timer(2); }
	static VOID_TIMER_t* get_Timer_void_RITimer(void){ return &(RITIMER_t::getRITIMER()); }
	static VOID_TIMER_t* get_Timer_void_SCTTimer(void){ return &(SCT_TIMER_t::getSCT_TIMER()); }
#ifdef CORE_M4
	static VOID_TIMER_t* get_Timer_void_SYS_TICK(void){ return &(SYS_TICK_t::getSYS_TICK()); }
#endif
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_0_0(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(0,0); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_0_1(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(0,1); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_0_2(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(0,2); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_0_3(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(0,3); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_3_0(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(3,0); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_3_1(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(3,1); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_3_2(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(3,2); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_3_3(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(3,3); }
#ifdef CORE_M4
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_1_0(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(1,0); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_1_1(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(1,1); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_1_2(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(1,2); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_1_3(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(1,3); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_2_0(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(2,0); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_2_1(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(2,1); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_2_2(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(2,2); }
	static VOID_TIMER_t* get_Timer_void_VIRT_GPT_2_3(void){ return VIRT_GPT_TMR_t::get_VIRT_GPT_Timer(2,3); }
#endif
#ifdef CORE_M4
	static PININT_t* get_PINInt_0(void){ return PININT_t::get_PINInt(0); }
	static PININT_t* get_PINInt_1(void){ return PININT_t::get_PINInt(1); }
	static PININT_t* get_PINInt_2(void){ return PININT_t::get_PINInt(2); }
	static PININT_t* get_PINInt_3(void){ return PININT_t::get_PINInt(3); }
	static PININT_t* get_PINInt_5(void){ return PININT_t::get_PINInt(5); }
	static PININT_t* get_PINInt_6(void){ return PININT_t::get_PINInt(6); }
	static PININT_t* get_PINInt_7(void){ return PININT_t::get_PINInt(7); }
#endif
	static PININT_t* get_PINInt_4(void){ return PININT_t::get_PINInt(4); }
	static UART_int_t* get_UART0(uint32_t baudrate){ return UART_int_t::get_UART_int(0, baudrate); }
	static UART_int_t* get_UART1(uint32_t baudrate){ return UART_int_t::get_UART_int(1, baudrate); }
	static UART_int_t* get_UART2(uint32_t baudrate){ return UART_int_t::get_UART_int(2, baudrate); }
	static UART_int_t* get_UART3(uint32_t baudrate){ return UART_int_t::get_UART_int(3, baudrate); }
	static DAC_t* get_DAC(void){ return &(DAC_t::getDAC()); }
	static EEPROM_t* get_EEPROM(void){ return &(EEPROM_t::getEEPROM()); }
	static SSP_t* get_SSP_0(void){ return SSP_t::get_SSP(0); }
	static SSP_t* get_SSP_1(void){ return SSP_t::get_SSP(1); }
	static SPIFI_t* get_SPIFI(void){ return SPIFI_t::getSPIFI(); }
	static DMA_Controller_t* get_DMA(void){return DMA_Controller_t::get_DMA_Controller(); }
	static IRQ_CONTROLLER_t* get_IRQ_Controller(void){ return &(IRQ_CONTROLLER_t::getIRQController()); }
	static IPC_proto_t* get_IPC_proto(void) { return IPC_proto_t::get_IPC_proto(); }
	static USB_Device_t* get_USB0(void){ return USB_Device_t::get_USB_Device(0); }
	static USB_Device_t* get_USB1(void){ return USB_Device_t::get_USB_Device(1); }
	static Ethernet_t* get_Ethernet_phy(void) {return Eth_phy_t::get_Ethernet_phy(); }
#ifdef CORE_M4
	static WatchDog_t* get_Watch_Dog(void){ return &(WatchDog_t::getWatch_Dog()); }
#endif
#ifdef USE_CONSOLE
	static Void_Console_t* get_Console(uint16_t tx_buf_size){ return Void_Console_t::Get_Console(tx_buf_size); }
#endif
	static Time_Engine_t* get_Time_Engine(void){ return Time_Engine_t::Get_Time_Engine(); }
	static Ethernet_router_t* get_Ethernet_router(void){ return Ethernet_router_t::get_Ethernet_router(); }
private:
	static CONTROLLER_t* CONTROLLER_ptr;
	static BOARD_GPIO_t BOARD_GPIOs[];
	uint8_t Initialize_done;
	Callback_Interface_t* main_procedures[main_proc_num];
	CONTROLLER_t(void);
	void LED_music(void);
};


#endif /* CONTROLLER_HPP_ */
