/*
 * WDT.hpp
 *
 *  Created on: 14.07.2017
 *      Author: Stalker1290
 */

#ifndef WDT_HPP_
#define WDT_HPP_

#include "chip.h"


class WatchDog_t
{
public:
	static WatchDog_t& getWatch_Dog(void);
	void Initialize(uint32_t ms_period); //max = 5.59s
	void Start(void);
	void Reset(void);
	void Deinitialize(void);
private:
	WatchDog_t(void);
	WatchDog_t(WatchDog_t const&);
	WatchDog_t& operator=(WatchDog_t const&);
};

#endif /* WDT_HPP_ */
