/*
 * CONTROLLER.cpp
 *
 *  Created on: 13.09.2017
 *      Author: Stalker1290
 */

#include "CONTROLLER.hpp"


BOARD_GPIO_t CONTROLLER_t::BOARD_GPIOs[]  = {{LED0_PORT,LED0_PIN,LED0_GPIOPORT,LED0_GPIOPIN,LED0_SCU_MODE_FUNC},
		{LED1_PORT,LED1_PIN,LED1_GPIOPORT,LED1_GPIOPIN,LED1_SCU_MODE_FUNC},};

CONTROLLER_t* CONTROLLER_t::CONTROLLER_ptr = (CONTROLLER_t*)NULL;

CONTROLLER_t* CONTROLLER_t::get_CONTROLLER(void)
{
	if(CONTROLLER_t::CONTROLLER_ptr == (CONTROLLER_t*)NULL) CONTROLLER_t::CONTROLLER_ptr =  new CONTROLLER_t();
	return CONTROLLER_t::CONTROLLER_ptr;
}


CONTROLLER_t::CONTROLLER_t(void)
{
	SystemCoreClockUpdate();
#ifdef CORE_M4
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);
#endif
	__enable_irq();
	this->Initialize_done = 0;
	for(int i = 0; i<main_proc_num; i++) this->main_procedures[i] = (Callback_Interface_t*)NULL;
}


void CONTROLLER_t::Initialize(void)
{
	if(this->Initialize_done == 0)
	{
		Chip_GPIO_Init(LPC_GPIO_PORT);
		Chip_Clock_Enable(CLK_MX_SCU);
		for(uint32_t i=0; i<( sizeof(CONTROLLER_t::BOARD_GPIOs)/sizeof(BOARD_GPIO_t) ); i++)
		{
			Chip_SCU_PinMuxSet(CONTROLLER_t::BOARD_GPIOs[i].port, CONTROLLER_t::BOARD_GPIOs[i].pin, CONTROLLER_t::BOARD_GPIOs[i].scu_mode);
			Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CONTROLLER_t::BOARD_GPIOs[i].gpio_port, CONTROLLER_t::BOARD_GPIOs[i].gpio_pin);
			Chip_GPIO_SetPinState(LPC_GPIO_PORT, CONTROLLER_t::BOARD_GPIOs[i].gpio_port, CONTROLLER_t::BOARD_GPIOs[i].gpio_pin, (bool) false);
		}
		this->LED_music();
		this->Initialize_done = 1;
	}

}

void CONTROLLER_t::GPIO_ON(uint8_t num)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, CONTROLLER_t::BOARD_GPIOs[num].gpio_port, CONTROLLER_t::BOARD_GPIOs[num].gpio_pin);
}

void CONTROLLER_t::GPIO_OFF(uint8_t num)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, CONTROLLER_t::BOARD_GPIOs[num].gpio_port, CONTROLLER_t::BOARD_GPIOs[num].gpio_pin);
}

void CONTROLLER_t::GPIO_TGL(uint8_t num)
{
	Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, CONTROLLER_t::BOARD_GPIOs[num].gpio_port, CONTROLLER_t::BOARD_GPIOs[num].gpio_pin);
}

void CONTROLLER_t::_delay_ms(uint32_t ms)
{
	for(uint32_t i=0;i<ms;i++) for(int j=0;j<40824;j++) __NOP();
}

void CONTROLLER_t::_delay_us(uint32_t us)
{
	for(uint32_t i=0;i<us*39;i++) __NOP();
}

void CONTROLLER_t::Start_M0APP(uint32_t img_addr)
{
	/* Make sure the M0 core is being held in reset via the RGU */
	Chip_RGU_TriggerReset(RGU_M0APP_RST);
	Chip_Clock_Enable(CLK_M4_M0APP);

	/* Keep in mind the M0 image must be aligned on a 4K boundary */
	Chip_CREG_SetM0AppMemMap(img_addr);
	Chip_RGU_ClearReset(RGU_M0APP_RST);

	#ifdef USE_CONSOLE
	printf("Start of ARM Cortex-M0APP on %li MHz\n\r",(Chip_Clock_GetRate(CLK_M4_M0APP)/1000000));
	#endif
}

void CONTROLLER_t::Reset_periph(void)
{
	Chip_RGU_TriggerReset(RGU_SCU_RST);
	while(Chip_RGU_InReset(RGU_SCU_RST)) {}
	Chip_RGU_TriggerReset(RGU_LCD_RST);
	while(Chip_RGU_InReset(RGU_LCD_RST)) {}
	Chip_RGU_TriggerReset(RGU_USB0_RST);
	while(Chip_RGU_InReset(RGU_USB0_RST)) {}
	Chip_RGU_TriggerReset(RGU_USB1_RST);
	while(Chip_RGU_InReset(RGU_USB1_RST)) {}
	Chip_RGU_TriggerReset(RGU_DMA_RST);
	while(Chip_RGU_InReset(RGU_DMA_RST)) {}
	Chip_RGU_TriggerReset(RGU_SDIO_RST);
	while(Chip_RGU_InReset(RGU_SDIO_RST)) {}
	Chip_RGU_TriggerReset(RGU_EMC_RST);
	while(Chip_RGU_InReset(RGU_EMC_RST)) {}
	Chip_RGU_TriggerReset(RGU_ETHERNET_RST);
	while(Chip_RGU_InReset(RGU_ETHERNET_RST)) {}
	Chip_RGU_TriggerReset(RGU_FLASHA_RST);
	while(Chip_RGU_InReset(RGU_FLASHA_RST)) {}
	Chip_RGU_TriggerReset(RGU_EEPROM_RST);
	while(Chip_RGU_InReset(RGU_EEPROM_RST)) {}
	Chip_RGU_TriggerReset(RGU_GPIO_RST);
	while(Chip_RGU_InReset(RGU_GPIO_RST)) {}
	Chip_RGU_TriggerReset(RGU_FLASHB_RST);
	while(Chip_RGU_InReset(RGU_FLASHB_RST)) {}
	Chip_RGU_TriggerReset(RGU_TIMER0_RST);
	while(Chip_RGU_InReset(RGU_TIMER0_RST)) {}
	Chip_RGU_TriggerReset(RGU_TIMER1_RST);
	while(Chip_RGU_InReset(RGU_TIMER1_RST)) {}
	Chip_RGU_TriggerReset(RGU_TIMER2_RST);
	while(Chip_RGU_InReset(RGU_TIMER2_RST)) {}
	Chip_RGU_TriggerReset(RGU_TIMER3_RST);
	while(Chip_RGU_InReset(RGU_TIMER3_RST)) {}
	Chip_RGU_TriggerReset(RGU_RITIMER_RST);
	while(Chip_RGU_InReset(RGU_RITIMER_RST)) {}
	Chip_RGU_TriggerReset(RGU_SCT_RST);
	while(Chip_RGU_InReset(RGU_SCT_RST)) {}
	Chip_RGU_TriggerReset(RGU_MOTOCONPWM_RST);
	while(Chip_RGU_InReset(RGU_MOTOCONPWM_RST)) {}
	Chip_RGU_TriggerReset(RGU_QEI_RST);
	while(Chip_RGU_InReset(RGU_QEI_RST)) {}
	Chip_RGU_TriggerReset(RGU_ADC0_RST);
	while(Chip_RGU_InReset(RGU_ADC0_RST)) {}
	Chip_RGU_TriggerReset(RGU_ADC1_RST);
	while(Chip_RGU_InReset(RGU_ADC1_RST)) {}
	Chip_RGU_TriggerReset(RGU_DAC_RST);
	while(Chip_RGU_InReset(RGU_DAC_RST)) {}
	Chip_RGU_TriggerReset(RGU_UART0_RST);
	while(Chip_RGU_InReset(RGU_UART0_RST)) {}
	Chip_RGU_TriggerReset(RGU_UART1_RST);
	while(Chip_RGU_InReset(RGU_UART1_RST)) {}
	Chip_RGU_TriggerReset(RGU_UART2_RST);
	while(Chip_RGU_InReset(RGU_UART2_RST)) {}
	Chip_RGU_TriggerReset(RGU_UART3_RST);
	while(Chip_RGU_InReset(RGU_UART3_RST)) {}
	Chip_RGU_TriggerReset(RGU_I2C0_RST);
	while(Chip_RGU_InReset(RGU_I2C0_RST)) {}
	Chip_RGU_TriggerReset(RGU_I2C1_RST);
	while(Chip_RGU_InReset(RGU_I2C1_RST)) {}
	Chip_RGU_TriggerReset(RGU_SSP0_RST);
	while(Chip_RGU_InReset(RGU_SSP0_RST)) {}
	Chip_RGU_TriggerReset(RGU_SSP1_RST);
	while(Chip_RGU_InReset(RGU_SSP1_RST)) {}
	Chip_RGU_TriggerReset(RGU_I2S_RST);
	while(Chip_RGU_InReset(RGU_I2S_RST)) {}
	Chip_RGU_TriggerReset(RGU_SPIFI_RST);
	while(Chip_RGU_InReset(RGU_SPIFI_RST)) {}
	Chip_RGU_TriggerReset(RGU_CAN1_RST);
	while(Chip_RGU_InReset(RGU_CAN1_RST)) {}
	Chip_RGU_TriggerReset(RGU_CAN0_RST);
	while(Chip_RGU_InReset(RGU_CAN0_RST)) {}
	Chip_RGU_TriggerReset(RGU_SGPIO_RST);
	while(Chip_RGU_InReset(RGU_SGPIO_RST)) {}
	Chip_RGU_TriggerReset(RGU_SPI_RST);
	while(Chip_RGU_InReset(RGU_SPI_RST)) {}
	Chip_RGU_TriggerReset(RGU_ADCHS_RST);
	while(Chip_RGU_InReset(RGU_ADCHS_RST)) {}
}

void CONTROLLER_t::Soft_reset(void)
{
#ifdef CORE_M4
	Chip_RGU_TriggerReset(RGU_M3_RST);
	while (Chip_RGU_InReset(RGU_M3_RST)) {}
#endif
}

void CONTROLLER_t::Go_to_app(uint32_t App_addr)
{
//	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 3, 5); //sx reset
//	this->_delay_us(200);
//	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 3, 5);
//	this->_delay_ms(20);

	this->GPIO_ON(LED0);
	this->GPIO_ON(LED1);

	for(uint8_t h=0; h<=5; h++)
	{
		this->GPIO_TGL(LED0);
		this->_delay_ms(100);
	}

	__disable_irq();

	LPC_CREG->MXMEMMAP = App_addr & ~0xFFF;
	this->_delay_us(100);
	this->Soft_reset();
}

void CONTROLLER_t::LED_music(void)
{
	uint8_t i;

	for(i=0; i<2; i++)
	{
		this->GPIO_ON(i);
		this->_delay_ms(100);
	}
	for(i=0; i<2; i++)
	{
		this->GPIO_OFF(i);
		this->_delay_ms(100);
	}
}

void CONTROLLER_t::Do_main(void)
{
	for(int i = 0; i<main_proc_num; i++)
	{
		if(this->main_procedures[i] != (Callback_Interface_t*)NULL) this->main_procedures[i]->void_callback((void*)this, NULL);
		else break;
	}
}

void CONTROLLER_t::Add_main_procedure(Callback_Interface_t* main_proc)
{
	for(int i = 0; i < main_proc_num; i++)
	{
		if(this->main_procedures[i] == main_proc) break;
		if(this->main_procedures[i] == (Callback_Interface_t*)NULL)
		{
			this->main_procedures[i] = main_proc;
			break;
		}
	}
}

void CONTROLLER_t::Delete_main_procedure(Callback_Interface_t* main_proc)
{
	uint32_t index = 0;
	for(int i = 0; i < main_proc_num; i++)
	{
		if(this->main_procedures[i] == main_proc) break;
		else index++;
	}
	if(index == (main_proc_num-1))
	{
		if(this->main_procedures[index] == main_proc) this->main_procedures[index] = (Callback_Interface_t*)NULL;
	}
	else
	{
		for(int i = index; i < (main_proc_num-1); i++)
		{
			this->main_procedures[i] = this->main_procedures[i+1];
			if(this->main_procedures[i+1] == (Callback_Interface_t*)NULL) break;
		}
	}
}


