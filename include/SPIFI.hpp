/*
 * SPIFI.hpp
 *
 *  Created on: 03.08.2017
 *      Author: Stalker1290
 */

#ifndef SPIFI_HPP_
#define SPIFI_HPP_


#include "chip.h"
#include "SPIFI/spifilib_api.h"

class SPIFI_t
{
public:
	static SPIFI_t* getSPIFI(void);
	void Initialize(void);
	void Write(uint32_t Adress,uint8_t* Buf, uint32_t size);
	void Erase(uint32_t Adress, uint32_t size);
	void Deinitialize(void);
private:
	static SPIFI_t* SPIFI_ptr;
	SPIFI_t(void);
	SPIFI_t(SPIFI_t const&);
	SPIFI_t& operator=(SPIFI_t const&);
	static SPIFI_HANDLE_T* pSpifi;
	static uint32_t* lmem_ptr;
	static void SetMemMode(void);
	static void ClearMemMode(void);
	static void Error_out(char *str, SPIFI_ERR_T errNum);
	static uint32_t CalculateDivider(uint32_t baseClock, uint32_t target);
	static const PINMUX_GRP_T spifipinmuxing[];
};

#ifndef SPIFLASH_BASE_ADDRESS
#define SPIFLASH_BASE_ADDRESS (0x14000000)
#endif




#endif /* SPIFI_HPP_ */
