/**
 * @file
 * @brief JbController class selector
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

#ifndef JBCONTROLLER_HPP_
#define JBCONTROLLER_HPP_

#ifdef CORE_M4
#include "jbdrivers/JbControllerM4.hpp"
#endif
#ifdef CORE_M0
#include "jbdrivers/JbControllerM0.hpp"
#endif


#endif /*JBCONTROLLER_HPP_ */
