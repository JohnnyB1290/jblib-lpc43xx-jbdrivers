/**
 * @file
 * @brief DAC Driver Description
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

#ifndef DAC_HPP_
#define DAC_HPP_

#include "jbkernel/jb_common.h"

namespace jblib::jbdrivers
{

class Dac
{
public:
	static Dac* getDac(void);
	void initialize(void);
	void start(void);
	void setOutput(uint32_t value);
	void deinitialize(void);

private:
	Dac(void);

	static Dac* dac_;
};

}

#endif /* DAC_HPP_ */
