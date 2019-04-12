/*
 * SPIFI.cpp
 *
 *  Created on: 03.08.2017
 *      Author: Stalker1290
 */

#include "SPIFI.hpp"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


#ifdef SPIFI_console
#ifdef USE_CONSOLE
STATIC INLINE void Delay_for_printf(void) //10ms
{
	uint32_t i;
	for(i=0;i<390000;i++) __NOP();
}
#endif
#endif


const PINMUX_GRP_T SPIFI_t::spifipinmuxing[] = {
	{SPIFI_CLK_PORT, SPIFI_CLK_PIN,  SPIFI_CLK_SCU_MODE_FUNC|SCU_PINIO_FAST},	/* SPIFI CLK */
	{SPIFI_D3_PORT, SPIFI_D3_PIN,  SPIFI_D3_SCU_MODE_FUNC|SCU_PINIO_FAST},	/* SPIFI D3 */
	{SPIFI_D2_PORT, SPIFI_D2_PIN,  SPIFI_D2_SCU_MODE_FUNC|SCU_PINIO_FAST},	/* SPIFI D2 */
	{SPIFI_D1_PORT, SPIFI_D1_PIN,  SPIFI_D1_SCU_MODE_FUNC|SCU_PINIO_FAST},	/* SPIFI D1 */
	{SPIFI_D0_PORT, SPIFI_D0_PIN,  SPIFI_D0_SCU_MODE_FUNC|SCU_PINIO_FAST},	/* SPIFI D0 */
	{SPIFI_CS_PORT, SPIFI_CS_PIN,  SPIFI_CS_SCU_MODE_FUNC|SCU_PINIO_FAST}	/* SPIFI CS/SSEL */
};
SPIFI_t* SPIFI_t::SPIFI_ptr = (SPIFI_t*)NULL;
SPIFI_HANDLE_T* SPIFI_t::pSpifi = (SPIFI_HANDLE_T*)NULL;
uint32_t* SPIFI_t::lmem_ptr = (uint32_t*)NULL;

SPIFI_t* SPIFI_t::getSPIFI(void)
{
	if(SPIFI_t::SPIFI_ptr == (SPIFI_t*)NULL) SPIFI_t::SPIFI_ptr = new SPIFI_t();
	if(SPIFI_t::pSpifi == (SPIFI_HANDLE_T*)NULL) return (SPIFI_t*)NULL;
	else return SPIFI_t::SPIFI_ptr;
}

SPIFI_t::SPIFI_t(void)
{
	uint32_t spifiBaseClockRate;
	uint32_t memSize;

#ifdef SPIFI_console
#ifdef USE_CONSOLE
	uint16_t libVersion;
	/* Report the library version to start with */
	libVersion = spifiGetLibVersion();
	printf("SPIFI Lib Version %02d.%02d\r\n", ((libVersion >> 8) & 0xff), (libVersion & 0xff));
#endif
#endif

	/* Setup SPIFI FLASH pin muxing (QUAD) */
	Chip_SCU_SetPinMuxing(SPIFI_t::spifipinmuxing, sizeof(SPIFI_t::spifipinmuxing) / sizeof(PINMUX_GRP_T));

	/* SPIFI base clock will be based on the main PLL rate and a divider */
	spifiBaseClockRate = Chip_Clock_GetClockInputHz(CLKIN_MAINPLL);

	/* Setup SPIFI clock to run around 1Mhz. Use divider E for this, as it allows
		  higher divider values up to 256 maximum) */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, CalculateDivider(spifiBaseClockRate, 1000000));
	Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IDIVE, true, false);

#ifdef SPIFI_console
#ifdef USE_CONSOLE
	printf("SPIFI clock rate %d\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));
#endif
#endif

	/* Initialize LPCSPIFILIB library, reset the interface */
	spifiInit(LPC_SPIFI_BASE, true);
	/* register support for the family(s) we may want to work with
	     (only 1 is required) */
    spifiRegisterFamily(spifi_REG_FAMILY_CommonCommandSet);

	/* Get required memory for detected device, this may vary per device family */
	memSize = spifiGetHandleMemSize(LPC_SPIFI_BASE);
	if (memSize == 0)
	{
		/* No device detected, error */
		Error_out((char*)"spifiGetHandleMemSize", SPIFI_ERR_GEN);
		return;
	}
	else
	{
		SPIFI_t::lmem_ptr = (uint32_t*)malloc(memSize);
		if(SPIFI_t::lmem_ptr == (uint32_t*)NULL)
		{
#ifdef USE_CONSOLE
			printf("ERROR! SPIFI malloc lmem \r\n");
#endif
			return;
		}
	}
	/* Initialize and detect a device and get device context */
	/* NOTE: Since we don't have malloc enabled we are just supplying
	     a chunk of memory that we know is large enough. It would be
	     better to use malloc if it is available. */
	SPIFI_t::pSpifi = spifiInitDevice((void*)SPIFI_t::lmem_ptr, memSize, LPC_SPIFI_BASE, SPIFLASH_BASE_ADDRESS);
	if (SPIFI_t::pSpifi == NULL) {
		Error_out((char*)"spifiInitDevice", SPIFI_ERR_GEN);
	}
}


void SPIFI_t::Initialize(void)
{
	uint32_t maxSpifiClock;
	SPIFI_ERR_T errCode;
	uint32_t spifiBaseClockRate;

	/* Get some info needed for the application */
	maxSpifiClock = spifiDevGetInfo(SPIFI_t::pSpifi, SPIFI_INFO_MAXCLOCK);

	#ifdef SPIFI_console
	#ifdef USE_CONSOLE
	Delay_for_printf();
	/* Get info */
	printf("Device Identified   = %s\r\n", spifiDevGetDeviceName((SPIFI_HANDLE_T*)pSpifi));
	Delay_for_printf();
	printf("Capabilities        = 0x%x\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_CAPS));
	Delay_for_printf();
	printf("Device size         = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_DEVSIZE));
	Delay_for_printf();
	printf("Max Clock Rate      = %d\r\n", maxSpifiClock);
	Delay_for_printf();
	printf("Erase blocks        = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_ERASE_BLOCKS));
	Delay_for_printf();
	printf("Erase block size    = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_ERASE_BLOCKSIZE));
	Delay_for_printf();
	printf("Erase sub-blocks    = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_ERASE_SUBBLOCKS));
	Delay_for_printf();
	printf("Erase sub-blocksize = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_ERASE_SUBBLOCKSIZE));
	Delay_for_printf();
	printf("Write page size     = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_PAGESIZE));
	Delay_for_printf();
	printf("Max single readsize = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_MAXREADSIZE));
	Delay_for_printf();
	printf("Current dev status  = 0x%x\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_STATUS));
	Delay_for_printf();
	printf("Current options     = %d\r\n", spifiDevGetInfo((SPIFI_HANDLE_T*)pSpifi, SPIFI_INFO_OPTIONS));
	Delay_for_printf();
	#endif
	#endif

	/* SPIFI base clock will be based on the main PLL rate and a divider */
	spifiBaseClockRate = Chip_Clock_GetClockInputHz(CLKIN_MAINPLL);

	/* Setup SPIFI clock to at the maximum interface rate the detected device
	   can use. This should be done after device init. */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, CalculateDivider(spifiBaseClockRate, maxSpifiClock));

	#ifdef SPIFI_console
	#ifdef USE_CONSOLE
	printf("SPIFI final Rate    = %d\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));
	Delay_for_printf();
	printf("Unlocking QSPI Flash\r\n");
	#endif
	#endif

	/* start by unlocking the device */
	errCode = spifiDevUnlockDevice(SPIFI_t::pSpifi);
	if (errCode != SPIFI_ERR_NONE) {
		Error_out((char*)"unlockDevice", errCode);
	}

	/* Enable quad.  If not supported it will be ignored */
	spifiDevSetOpts(SPIFI_t::pSpifi, SPIFI_OPT_USE_QUAD, true);
	SetMemMode();
}

void SPIFI_t::SetMemMode(void)
{
	/* Enter memMode */
	spifiDevSetMemMode(SPIFI_t::pSpifi, true);
}

void SPIFI_t::ClearMemMode(void)
{
	/* Enter memMode */
	spifiDevSetMemMode(SPIFI_t::pSpifi, false);
}

void SPIFI_t::Write(uint32_t Adress,uint8_t* Buf, uint32_t size)
{
	uint32_t bytes_bw;
	uint32_t Page_offset;
	uint32_t temp_size = 0;
	uint32_t Page_size;
	SPIFI_ERR_T errCode;

	SPIFI_t::ClearMemMode();

	Page_size = spifiDevGetInfo(SPIFI_t::pSpifi, SPIFI_INFO_PAGESIZE);

	Page_offset = Adress%Page_size;

	if(Page_offset!=0)
	{
		temp_size = Page_size - Page_offset;
		if(size<=temp_size) temp_size = size;
		errCode = spifiDevPageProgram(SPIFI_t::pSpifi,Adress, (uint32_t*)Buf,temp_size);
		if (errCode != SPIFI_ERR_NONE)
		{
			Error_out((char*)"QSPI Flash Page Write", errCode);
		}
		size = size - temp_size;
		Adress = Adress - Page_offset + Page_size;
		bytes_bw = temp_size;
	}
	else
	{
		bytes_bw = 0;
	}
	if(size != 0)
	{
		errCode = spifiProgram(SPIFI_t::pSpifi, Adress, (uint32_t*)&Buf[bytes_bw], size);
		if (errCode != SPIFI_ERR_NONE) {
			Error_out((char*)"QSPI Flash Write", errCode);
		}
	}
	SPIFI_t::SetMemMode();
}

void SPIFI_t::Erase(uint32_t Adress, uint32_t size)
{
	uint32_t idx;
	SPIFI_ERR_T errCode;

	SPIFI_t::ClearMemMode();

	#ifdef SPIFI_console
	#ifdef USE_CONSOLE
	printf("Erasing QSPI Flash...\r\n");
	#endif
	#endif
	errCode = spifiEraseByAddr(SPIFI_t::pSpifi, Adress, (Adress+size) );
	if (errCode != SPIFI_ERR_NONE)
	{
		Error_out((char*)"EraseBlocks", errCode);
	}

	/* Enter memMode */
	SPIFI_t::SetMemMode();

	#ifdef SPIFI_console
	#ifdef USE_CONSOLE
	printf("Verifying QSPI Flash erased...\r\n");
	#endif
	#endif
	for (idx = 0; idx < size; idx += sizeof(uint32_t))
	{
		if ( ((uint32_t *) Adress)[(idx >> 2)] != 0xffffffff)
		{
			Error_out((char*)"EraseDevice verify", SPIFI_ERR_GEN);
		}
	}
	#ifdef SPIFI_console
	#ifdef USE_CONSOLE
	printf("Erase Done. Verify OK.\r\n");
	#endif
	#endif
}

void SPIFI_t::Deinitialize(void)
{
	/* Done, de-init will enter memory mode */
	spifiDevDeInit(SPIFI_t::pSpifi);
}

void SPIFI_t::Error_out(char *str, SPIFI_ERR_T errNum)
{
	#ifdef SPIFI_console
	#ifdef USE_CONSOLE
	printf("\r\n%s() Error:%d %s\r\n", str, errNum, spifiReturnErrString(errNum));
	#endif
	#endif
}

uint32_t SPIFI_t::CalculateDivider(uint32_t baseClock, uint32_t target)
{
	uint32_t divider = (baseClock / target);
	/* If there is a remainder then increment the dividor so that the resultant
	   clock is not over the target */
	if(baseClock % target) {
		++divider;
	}
	return divider;
}
