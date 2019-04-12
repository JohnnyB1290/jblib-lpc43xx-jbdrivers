/*
 * DAC.cpp
 *
 *  Created on: 14.07.2017
 *      Author: Stalker1290
 */

#include "DAC.hpp"

DAC_t& DAC_t::getDAC(void)
{
	static DAC_t Instance;
	return Instance;
}

DAC_t::DAC_t(void)
{

}

void DAC_t::Initialize(void)
{
	Chip_DAC_Init(LPC_DAC);
	Chip_DAC_UpdateValue(LPC_DAC,0);
}

void DAC_t::Start(void)
{
	LPC_DAC->CTRL = 8;
}

void DAC_t::SetOut(uint32_t value)
{
	Chip_DAC_UpdateValue(LPC_DAC,value);
}

void DAC_t::DeInitialize(void)
{
	Chip_RGU_TriggerReset(RGU_DAC_RST);
	while (Chip_RGU_InReset(RGU_DAC_RST)) {}
	Chip_DAC_DeInit(LPC_DAC);
}
