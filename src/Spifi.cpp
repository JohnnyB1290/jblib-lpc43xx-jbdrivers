/**
 * @file
 * @brief SPIFI Driver Realization
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

#include <string.h>
#include <stdlib.h>
#include "Spifi.hpp"
#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
#include <stdio.h>
#endif

namespace jblib::jbdrivers
{

const PINMUX_GRP_T Spifi::spifiPinMuxingGroup_[] = {
	{SPIFI_CLK_PORT, SPIFI_CLK_PIN,  SPIFI_CLK_SCU_MODE | SCU_PINIO_FAST},	/* SPIFI CLK */
	{SPIFI_D3_PORT,  SPIFI_D3_PIN,   SPIFI_D3_SCU_MODE | SCU_PINIO_FAST},	/* SPIFI D3 */
	{SPIFI_D2_PORT,  SPIFI_D2_PIN,   SPIFI_D2_SCU_MODE | SCU_PINIO_FAST},	/* SPIFI D2 */
	{SPIFI_D1_PORT,  SPIFI_D1_PIN,   SPIFI_D1_SCU_MODE | SCU_PINIO_FAST},	/* SPIFI D1 */
	{SPIFI_D0_PORT,  SPIFI_D0_PIN,   SPIFI_D0_SCU_MODE | SCU_PINIO_FAST},	/* SPIFI D0 */
	{SPIFI_CS_PORT,  SPIFI_CS_PIN,   SPIFI_CS_SCU_MODE | SCU_PINIO_FAST}	/* SPIFI CS/SSEL */
};
Spifi* Spifi::spifi_ = (Spifi*)NULL;
SPIFI_HANDLE_T* Spifi::spifiHandle_ = (SPIFI_HANDLE_T*)NULL;
uint32_t* Spifi::handleMemory_ = (uint32_t*)NULL;



Spifi* Spifi::getSpifi(void)
{
	if(Spifi::spifi_ == (Spifi*)NULL)
		Spifi::spifi_ = new Spifi();
	if(Spifi::spifiHandle_ == (SPIFI_HANDLE_T*)NULL)
		return (Spifi*)NULL;
	else
		return Spifi::spifi_;
}



Spifi::Spifi(void)
{
#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	/* Report the library version to start with */
	uint16_t libVersion = spifiGetLibVersion();
	printf("SPIFI Lib Version %02d.%02d\r\n", ((libVersion >> 8) & 0xff), (libVersion & 0xff));
#endif
	/* Setup SPIFI FLASH pin muxing (QUAD) */
	Chip_SCU_SetPinMuxing(Spifi::spifiPinMuxingGroup_,
			sizeof(Spifi::spifiPinMuxingGroup_) / sizeof(PINMUX_GRP_T));

	/* SPIFI base clock will be based on the main PLL rate and a divider */
	uint32_t spifiBaseClockRate = Chip_Clock_GetClockInputHz(CLKIN_MAINPLL);

	/* Setup SPIFI clock to run around 1Mhz. Use divider E for this, as it allows
		  higher divider values up to 256 maximum) */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL,
			calculateDivisor(spifiBaseClockRate, 1000000));
	Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IDIVE, true, false);

#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	printf("SPIFI clock rate %lu\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));
#endif

	/* initialize LPCSPIFILIB library, reset the interface */
	spifiInit(LPC_SPIFI_BASE, 1);
	/* register support for the family(s) we may want to work with
	     (only 1 is required) */
    spifiRegisterFamily(spifi_REG_FAMILY_CommonCommandSet);

	/* Get required memory for detected device, this may vary per device family */
    uint32_t memSize = spifiGetHandleMemSize(LPC_SPIFI_BASE);
	if (memSize == 0) {
		/* No device detected, error */
		printErrorString((char*)"spifiGetHandleMemSize", SPIFI_ERR_GEN);
		return;
	}
	else {
		Spifi::handleMemory_ = (uint32_t*)malloc_s(memSize);
		if(Spifi::handleMemory_ == (uint32_t*)NULL) {
#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
			printf("ERROR! SPIFI malloc memory. No mem! \r\n");
#endif
			return;
		}
	}
	/* initialize and detect a device and get device context */
	/* NOTE: Since we don't have malloc enabled we are just supplying
	     a chunk of memory that we know is large enough. It would be
	     better to use malloc if it is available. */
	Spifi::spifiHandle_ = spifiInitDevice((void*)Spifi::handleMemory_,
			memSize, LPC_SPIFI_BASE, SPIFLASH_BASE_ADDRESS);
	if (Spifi::spifiHandle_ == NULL)
		printErrorString((char*)"spifiInitDevice", SPIFI_ERR_GEN);
}



void Spifi::initialize(void)
{
	/* Get some info needed for the application */
	uint32_t maxSpifiClock = spifiDevGetInfo(Spifi::spifiHandle_, SPIFI_INFO_MAXCLOCK);

#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	/* Get info */
	printf("Device Identified   = %s\r\n", spifiDevGetDeviceName((SPIFI_HANDLE_T*)spifiHandle_));
	printf("Capabilities        = 0x%lx\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_CAPS));
	printf("Device size         = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_DEVSIZE));
	printf("Max Clock Rate      = %lu\r\n", maxSpifiClock);
	printf("erase blocks        = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_BLOCKS));
	printf("erase block size    = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_BLOCKSIZE));
	printf("erase sub-blocks    = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_SUBBLOCKS));
	printf("erase sub-blocksize = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_SUBBLOCKSIZE));
	printf("write page size     = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_PAGESIZE));
	printf("Max single readsize = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_MAXREADSIZE));
	printf("Current dev status  = 0x%lx\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_STATUS));
	printf("Current options     = %lu\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_OPTIONS));
	#endif

	/* SPIFI base clock will be based on the main PLL rate and a divider */
	uint32_t spifiBaseClockRate = Chip_Clock_GetClockInputHz(CLKIN_MAINPLL);

	/* Setup SPIFI clock to at the maximum interface rate the detected device
	   can use. This should be done after device init. */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL,
			calculateDivisor(spifiBaseClockRate, maxSpifiClock));

#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	printf("SPIFI final Rate    = %lu\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));
	printf("Unlocking QSPI Flash\r\n");
#endif

	/* start by unlocking the device */
	SPIFI_ERR_T errCode = spifiDevUnlockDevice(Spifi::spifiHandle_);
	if (errCode != SPIFI_ERR_NONE)
		printErrorString((char*)"unlockDevice", errCode);

	/* Enable quad.  If not supported it will be ignored */
	spifiDevSetOpts(Spifi::spifiHandle_, SPIFI_OPT_USE_QUAD, 1);
	setMemoryMode();
}



void Spifi::setMemoryMode(void)
{
	/* Enter memMode */
	spifiDevSetMemMode(Spifi::spifiHandle_, 1);
}



void Spifi::clearMemoryMode(void)
{
	/* Enter memMode */
	spifiDevSetMemMode(Spifi::spifiHandle_, 0);
}



void Spifi::write(uint32_t address,uint8_t* data, uint32_t size)
{
	Spifi::clearMemoryMode();
	uint32_t pageSize = spifiDevGetInfo(Spifi::spifiHandle_, SPIFI_INFO_PAGESIZE);
	uint32_t pageOffset = address % pageSize;
	uint32_t bw = 0;
	if(pageOffset != 0) {
		uint32_t tempSize = pageSize - pageOffset;
		if(size <= tempSize) tempSize = size;
		SPIFI_ERR_T errCode =
				spifiDevPageProgram(Spifi::spifiHandle_, address, (uint32_t*)data, tempSize);
		if (errCode != SPIFI_ERR_NONE)
			printErrorString((char*)"QSPI Flash Page write", errCode);
		size -= tempSize;
		address = address - pageOffset + pageSize;
		bw = tempSize;
	}
	if(size != 0) {
		SPIFI_ERR_T errCode =
				spifiProgram(Spifi::spifiHandle_, address, (uint32_t*)&data[bw], size);
		if (errCode != SPIFI_ERR_NONE) {
			printErrorString((char*)"QSPI Flash write", errCode);
		}
	}
	Spifi::setMemoryMode();
}



void Spifi::erase(uint32_t address, uint32_t size)
{
	Spifi::clearMemoryMode();

	#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	printf("Erasing QSPI Flash...\r\n");
	#endif

	SPIFI_ERR_T errCode = spifiEraseByAddr(Spifi::spifiHandle_, address, (address+size));
	if (errCode != SPIFI_ERR_NONE)
		printErrorString((char*)"EraseBlocks", errCode);

	Spifi::setMemoryMode();

	#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	printf("Verifying QSPI Flash erased...\r\n");
	#endif
	for (uint32_t idx = 0; idx < size; idx += sizeof(uint32_t)) {
		if ( ((uint32_t *)address)[(idx >> 2)] != 0xffffffff)
			printErrorString((char*)"EraseDevice verify", SPIFI_ERR_GEN);
	}
	#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	printf("Erase Done. Verify OK.\r\n");
	#endif
}



void Spifi::deinitialize(void)
{
	/* Done, de-init will enter memory mode */
	spifiDevDeInit(Spifi::spifiHandle_);
}



void Spifi::printErrorString(char* string, SPIFI_ERR_T errorNumber)
{
	#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
	printf("\r\n%s() Error:%d %s\r\n",
			string, errorNumber, spifiReturnErrString(errorNumber));
	#endif
}



uint32_t Spifi::calculateDivisor(uint32_t baseClock, uint32_t target)
{
	uint32_t divider = (baseClock / target);
	/* If there is a remainder then increment the dividor so that the resultant
	   clock is not over the target */
	if(baseClock % target)
		divider++;
	return divider;
}

}