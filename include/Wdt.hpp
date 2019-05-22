/**
 * @file
 * @brief Watchdog Driver Description
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
#ifndef WDT_HPP_
#define WDT_HPP_

#include "jb_common.h"

namespace jblib::jbdrivers
{

class Wdt
{
public:
	static Wdt* getWatchDog(void);
	void initialize(uint32_t periodMs); //max = 5.59s
	void start(void);
	void reset(void);

private:
	Wdt(void);

	static Wdt* wdt_;
};

}

#endif /* WDT_HPP_ */
