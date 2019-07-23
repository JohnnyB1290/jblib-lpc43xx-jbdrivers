/**
 * @file
 * @brief JbController M0 Core class realization
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

#ifdef CORE_M0
#include <string.h>
#include "jbdrivers/JbControllerM0.hpp"

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
		enableInterrupts();
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
    disableInterrupts();
    void* ptr = malloc(ret);
    while(ptr != NULL){
        free(ptr);
        ret += 10;
        ptr = malloc(ret);
    }
    enableInterrupts();
    return ret;
}

}


#endif
