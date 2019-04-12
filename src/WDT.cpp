/*
 * WDT.c
 *
 *  Created on: 14.02.2017
 *      Author: Stalker1290
 */

#include "WDT.hpp"

WatchDog_t& WatchDog_t::getWatch_Dog(void)
{
	static WatchDog_t Instance;
	return Instance;
}

WatchDog_t::WatchDog_t(void)
{

}

void WatchDog_t::Initialize(uint32_t ms_period)
{
	uint32_t timeout;
	timeout = (WDT_OSC/4000)*ms_period;
	Chip_WWDT_SetTimeOut(LPC_WWDT, timeout);
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET|WWDT_WDMOD_WDINT);
	Chip_WWDT_UnsetOption(LPC_WWDT, WWDT_WDMOD_WDTOF);
}

void WatchDog_t::Start(void)
{
	Chip_WWDT_Start(LPC_WWDT);
}

void WatchDog_t::Reset(void)
{
	__disable_irq();
	Chip_WWDT_Feed(LPC_WWDT);
	__enable_irq();
}

void WatchDog_t::Deinitialize(void)
{

}
