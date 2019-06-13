/**
 * @file
 * @brief JbController M4 Core class realization
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
#include <string.h>
#include "jbdrivers/JbControllerM4.hpp"

namespace jblib::jbdrivers
{

using namespace jbkernel;

BoardGpio_t JbController::boardGpios_[] = JBCONTROLLER_BOARD_GPIOS;
bool JbController::isInitialized = false;
IVoidCallback* JbController::mainProcedures_[JBCONTROLLER_NUM_MAIN_PROCEDURES];
void* JbController::mainProceduresParameters_[JBCONTROLLER_NUM_MAIN_PROCEDURES];



void JbController::initialize(void)
{
	if(!isInitialized) {
		for(uint32_t i = 0; i < JBCONTROLLER_NUM_MAIN_PROCEDURES; i++){
			mainProcedures_[i] = NULL;
			mainProceduresParameters_[i] = NULL;
		}
		SystemCoreClockUpdate();
		NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);
		__enable_irq();

		Chip_GPIO_Init(LPC_GPIO_PORT);
		Chip_Clock_Enable(CLK_MX_SCU);
		for(uint32_t i = 0; i < (sizeof(boardGpios_)/sizeof(BoardGpio_t)); i++) {
			Chip_SCU_PinMuxSet(boardGpios_[i].port, boardGpios_[i].pin, boardGpios_[i].scuMode);
			Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, boardGpios_[i].gpioPort, boardGpios_[i].gpioPin);
			Chip_GPIO_SetPinState(LPC_GPIO_PORT, boardGpios_[i].gpioPort, boardGpios_[i].gpioPin, false);
		}
		isInitialized = true;
	}
}



void JbController::delayMs(uint32_t ms)  //For 204MHz Clock
{
	for(uint32_t i = 0; i < ms; i++)
		for(uint32_t j = 0; j < JBCONTROLLER_NUM_NOP_DELAY_MS; j++)
			__NOP();
}



void JbController::delayUs(uint32_t us) //For 204MHz Clock
{
	for(uint32_t i = 0; i < us * JBCONTROLLER_NUM_NOP_DELAY_US; i++)
		__NOP();
}



void JbController::gpioOn(uint8_t number)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
			boardGpios_[number].gpioPort, boardGpios_[number].gpioPin);
}



void JbController::gpioOff(uint8_t number)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
			boardGpios_[number].gpioPort, boardGpios_[number].gpioPin);
}



void JbController::gpioTgl(uint8_t number)
{
	Chip_GPIO_SetPinToggle(LPC_GPIO_PORT,
			boardGpios_[number].gpioPort, boardGpios_[number].gpioPin);
}



void JbController::startM0App(uint32_t imageAddress)
{
	/* Make sure the M0 core is being held in reset via the RGU */
	Chip_RGU_TriggerReset(RGU_M0APP_RST);
	Chip_Clock_Enable(CLK_M4_M0APP);

	/* Keep in mind the M0 image must be aligned on a 4K boundary */
	Chip_CREG_SetM0AppMemMap(imageAddress);
	Chip_RGU_ClearReset(RGU_M0APP_RST);

	#if USE_CONSOLE
	printf("Start of ARM Cortex-M0APP on %li MHz\r\n",
			(Chip_Clock_GetRate(CLK_M4_M0APP)/1000000));
	#endif
}



void JbController::startM0Sub(uint32_t imageAddress)
{
	/* Make sure the M0 core is being held in reset via the RGU */
	Chip_RGU_TriggerReset(RGU_M0SUB_RST);
	Chip_Clock_Enable(CLK_PERIPH_BUS);
	Chip_Clock_Enable(CLK_PERIPH_CORE);

	/* Keep in mind the M0 image must be aligned on a 4K boundary */
	Chip_CREG_SetM0SubMemMap(imageAddress);

	Chip_RGU_ClearReset(RGU_M0SUB_RST);

	#if USE_CONSOLE
	printf("Start of ARM Cortex-M0SUB BUS on %li MHz\r\n",
			(Chip_Clock_GetRate(CLK_PERIPH_BUS)/1000000));
	printf("Start of ARM Cortex-M0SUB CORE on %li MHz\r\n",
			(Chip_Clock_GetRate(CLK_PERIPH_CORE)/1000000));
	#endif
}



void JbController::copyFwToRam(uint32_t flashAddress, uint32_t ramAddress, uint32_t size)
{
	__disable_irq();
	memcpy((void*)ramAddress, (void*)flashAddress, size);
	__enable_irq();
}



void JbController::resetPeriphery(void)
{
	for(uint32_t i = 0; i < 52; i++) {
		NVIC_DisableIRQ((IRQn_Type)i);
		NVIC_ClearPendingIRQ((IRQn_Type)i);
	}
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
	Chip_RGU_TriggerReset(RGU_EEPROM_RST);
	while(Chip_RGU_InReset(RGU_EEPROM_RST)) {}
	Chip_RGU_TriggerReset(RGU_GPIO_RST);
	while(Chip_RGU_InReset(RGU_GPIO_RST)) {}
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



void JbController::softReset(void)
{
	Chip_RGU_TriggerReset(RGU_M3_RST);
	while (Chip_RGU_InReset(RGU_M3_RST)) {}
}



void JbController::goToApp(uint32_t applicationAddress)
{
	__disable_irq();
	LPC_CREG->MXMEMMAP = applicationAddress & ~0xFFF;
	JbController::delayUs(100);
	JbController::softReset();
}



void JbController::doMain(void)
{
	for(uint32_t i = 0; i < JBCONTROLLER_NUM_MAIN_PROCEDURES; i++) {
		if(mainProcedures_[i])
			mainProcedures_[i]->voidCallback(NULL,
					mainProceduresParameters_[i]);
		else
			break;
	}
}



void JbController::addMainProcedure(IVoidCallback* callback, void* parameter)
{
	for(uint32_t i = 0; i < JBCONTROLLER_NUM_MAIN_PROCEDURES; i++) {
		if((mainProcedures_[i] == callback) &&
				mainProceduresParameters_[i] == parameter){
			break;
		}
		if(mainProcedures_[i] == NULL) {
			mainProcedures_[i] = callback;
			mainProceduresParameters_[i] = parameter;
			break;
		}
	}
}



void JbController::deleteMainProcedure(IVoidCallback* callback, void* parameter)
{
	uint32_t index = 0;
	for(uint32_t i = 0; i < JBCONTROLLER_NUM_MAIN_PROCEDURES; i++) {
		if((mainProcedures_[i] == callback) &&
				mainProceduresParameters_[i] == parameter){
			break;
		}
		else
			index++;
	}
	if(index == (JBCONTROLLER_NUM_MAIN_PROCEDURES-1)) {
		if((mainProcedures_[index] == callback) &&
				mainProceduresParameters_[index] == parameter){
			mainProcedures_[index] = NULL;
			mainProceduresParameters_[index] = NULL;
		}
	}
	else {
		for(uint32_t i = index; i < (JBCONTROLLER_NUM_MAIN_PROCEDURES - 1); i++) {
			mainProcedures_[i] = mainProcedures_[i+1];
			mainProceduresParameters_[i] = mainProceduresParameters_[i+1];
			if(mainProcedures_[i+1] == NULL)
				break;
		}
	}
}



void JbController::addMainProcedure(IVoidCallback* callback)
{
	addMainProcedure(callback, NULL);
}



void JbController::deleteMainProcedure(IVoidCallback* callback)
{
	deleteMainProcedure(callback, NULL);
}



uint32_t JbController::getHeapFree(void)
{
    uint32_t ret = 10;
    __disable_irq();
    void* ptr = malloc(ret);
    while(ptr != NULL){
        free(ptr);
        ret += 10;
        ptr = malloc(ret);
    }
    __enable_irq();
    return ret;
}

}
#endif
