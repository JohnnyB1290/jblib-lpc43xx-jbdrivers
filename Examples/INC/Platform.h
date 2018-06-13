/*
 * Platform.h
 *
 *  Created on: 9 ���. 2017 �.
 *      Author: Stalker1290
 */

#ifndef INC_PLATFORM_H_
#define INC_PLATFORM_H_

#ifdef CODE_RED
#include <cr_section_macros.h>
#endif
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "chip.h"
#include "Defines.h"
#include "CONTROLLER.hpp"
#include "Eth_utilities.hpp"
#include "USB/USB_Device_use_rom/RNDIS_Device.hpp"
#include "USB/USB_Device_use_rom/USB_COM.hpp"
#include "UDP_channel.hpp"
#include "TCP_server.hpp"
#include "Void_iface_IPC_master.hpp"
#include "Crypto_check.hpp"
#include "Zinger_boot.hpp"
#include "Interface_checker.hpp"
#include "dhserver.hpp"
#include "dnserver.hpp"
#include "USB/USB_Device_use_rom/MSC_IFLASH.hpp"
#include "Param_iflash.hpp"
#include "Kernel_control.hpp"
#include "Web_interface.hpp"

#ifdef CORE_LOAD_mesure
#include "CORE_LOAD.hpp"
#endif

#endif /* INC_PLATFORM_H_ */
