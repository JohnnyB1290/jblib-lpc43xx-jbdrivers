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
#include "jbdrivers/Spifi.hpp"
#include "jbdrivers/JbController.hpp"
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
	if(!spifi_)
		spifi_ = new Spifi();
	if(!spifiHandle_){
		free_s(handleMemory_);
		handleMemory_ = NULL;
		delete spifi_;
		spifi_ = NULL;
	}
	return spifi_;
}



void Spifi::deleteSpifi(void)
{
	if(spifi_){
		spifiDevDeInit(spifiHandle_);
		free_s(handleMemory_);
		handleMemory_ = NULL;
		delete spifi_;
		spifi_ = NULL;
	}
}



Spifi::Spifi(void) : IVoidMemory()
{
	this->baseAddress_ = SPIFLASH_BASE_ADDRESS;
#if USE_FAT_FS
	this->diskBaseAddress_ = SPIFI_FLASH_DISK_BASE_ADDRESS;
	this->diskSize_ = SPIFI_FLASH_DISK_SIZE;
	this->diskBlockSize_ = 1;
#endif
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
			memSize, LPC_SPIFI_BASE, this->baseAddress_);
	if (Spifi::spifiHandle_ == NULL){
		printErrorString((char*)"spifiInitDevice", SPIFI_ERR_GEN);
		free_s(Spifi::handleMemory_);
	}
}



void Spifi::initialize(void)
{
	if(this->isInitialized_)
		return;
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

	this->auxBlockAddress_ = spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_DEVSIZE) -
			spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_BLOCKSIZE) +
			this->baseAddress_;

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
	this->isInitialized_ = true;
	#if USE_FAT_FS
		this->diskStatus_ &= ~STA_NOINIT;
	#endif
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



void Spifi::writeFast(uint32_t address,uint8_t* data, uint32_t size)
{
	Spifi::clearMemoryMode();
	uint32_t pageSize = spifiDevGetInfo(Spifi::spifiHandle_, SPIFI_INFO_PAGESIZE);
	uint32_t pageOffset = address % pageSize;
	uint32_t bw = 0;
	if(pageOffset != 0) {
		uint32_t tempSize = pageSize - pageOffset;
		if(size <= tempSize)
			tempSize = size;
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



int Spifi::copyBlock(uint32_t srcBlockAddress, uint32_t dstBlockAddress,
		uint32_t offset, uint32_t size)
{
	uint32_t pageSize = spifiDevGetInfo(Spifi::spifiHandle_, SPIFI_INFO_PAGESIZE);
	uint32_t srcAddress = srcBlockAddress + offset;
	uint32_t dstAddress = dstBlockAddress + offset;
	uint8_t* cache = (uint8_t*)malloc_s(pageSize);
	if(!cache){
		#if (USE_CONSOLE && SPIFI_USE_CONSOLE)
		printf("SPIFI Error: No memory for copyBlock!\n");
		#endif
		return -1;
	}
	uint32_t pageOffset = offset % pageSize;
	uint32_t writeSize = pageSize - pageOffset;
	uint32_t count = size / pageSize;
	count += (count * pageSize) == size ? 0 : 1;
	for(uint32_t i = 0; i < count; i++){
		if(pageOffset){
			writeSize = (writeSize < size) ? writeSize : size;
			pageOffset = 0;
		}
		else
			writeSize = (pageSize < size) ? pageSize : size;
		if(!this->isEmpty(srcAddress, writeSize)){
			memcpy(cache, (void*)srcAddress, writeSize);
			Spifi::clearMemoryMode();
			SPIFI_ERR_T errCode = spifiDevPageProgram(Spifi::spifiHandle_,
					dstAddress, (uint32_t*)cache, writeSize);
			if (errCode != SPIFI_ERR_NONE)
				printErrorString((char*)"QSPI Flash Page write", errCode);
			Spifi::setMemoryMode();
		}
		srcAddress += writeSize;
		dstAddress += writeSize;
		size -= writeSize;
	}

	free_s(cache);
	return 0;
}



void Spifi::writeBlock(uint32_t address,uint8_t* data, uint32_t size)
{
	uint32_t blockSize = spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_BLOCKSIZE);
	uint32_t blockOffset = address % blockSize;
	uint32_t blockAddress = address - blockOffset;
	uint32_t copySecondPartOffset = blockOffset + size;
	uint32_t copySecondPartSize = 0;

	this->eraseMemory(this->auxBlockAddress_, blockSize);
	if(blockOffset){
		if(this->copyBlock(blockAddress, this->auxBlockAddress_, 0, blockOffset))
			return;
	}
	if(copySecondPartOffset < blockSize){
		copySecondPartSize = blockSize - copySecondPartOffset;
		if(this->copyBlock(blockAddress, this->auxBlockAddress_,
				copySecondPartOffset, copySecondPartSize)){
			return;
		}
	}
	this->eraseMemory(blockAddress, blockSize);
	this->writeFast(address, data, size);
	if(blockOffset)
		this->copyBlock(this->auxBlockAddress_, blockAddress, 0, blockOffset);
	if(copySecondPartOffset < blockSize){
		copySecondPartSize = blockSize - copySecondPartOffset;
		this->copyBlock(this->auxBlockAddress_, blockAddress,
				copySecondPartOffset, copySecondPartSize);
	}
}



bool Spifi::isEmpty(uint32_t address, uint32_t size)
{
	for (uint32_t idx = 0; idx < size; idx += sizeof(uint32_t)) {
		if ( ((uint32_t *)address)[(idx >> 2)] != 0xffffffff)
			return false;
	}
	return true;
}



void Spifi::writeMemory(uint32_t address, uint8_t* data, uint32_t size)
{
	if(this->isEmpty(address, size)){
		this->writeFast(address, data, size);
		return;
	}
	uint32_t writeIndex = 0;
	uint32_t blockSize = spifiDevGetInfo((SPIFI_HANDLE_T*)spifiHandle_, SPIFI_INFO_ERASE_BLOCKSIZE);
	uint32_t blockOffset = address % blockSize;
	uint32_t writeSize = blockSize - blockOffset;
	uint32_t blocksCount = (blockOffset + size) / blockSize;
	blocksCount += ((blocksCount * blockSize) == (blockOffset + size)) ? 0 : 1;
	for(uint32_t i = 0; i < blocksCount; i++){
		if(blockOffset){
			writeSize = (writeSize < size) ? writeSize : size;
			blockOffset = 0;
		}
		else
			writeSize = (blockSize < size) ? blockSize : size;
		this->writeBlock(address, &data[writeIndex], writeSize);
		size -= writeSize;
		address += writeSize;
		writeIndex += writeSize;
	}
}



void Spifi::readMemory(uint32_t address, uint8_t* data, uint32_t size)
{
	memcpy(data, (void*)address, size);
}



void Spifi::eraseMemory(uint32_t address, uint32_t size)
{
	if(!size)
		return;
	Spifi::clearMemoryMode();
	SPIFI_ERR_T errCode = spifiEraseByAddr(Spifi::spifiHandle_, address, (address + size - 1));
	if (errCode != SPIFI_ERR_NONE)
		printErrorString((char*)"EraseBlocks", errCode);
	Spifi::setMemoryMode();
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
