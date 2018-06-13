/*
 * EEPROM.h
 *
 *  Created on: 15.02.2017
 *      Author: Stalker1290
 */

#ifndef EEPROM_HPP_
#define EEPROM_HPP_


#include "chip.h"

class EEPROM_t
{
public:
	static EEPROM_t& getEEPROM(void);
	void Initialize(void);
	void Read(uint32_t Adress,uint8_t* Buf, uint32_t size);
	void Write(uint32_t Adress,uint8_t* Buf, uint32_t size);
	void Deinitialize(void);
private:
	EEPROM_t(void);
	EEPROM_t(EEPROM_t const&);
	EEPROM_t& operator=(EEPROM_t const&);
};

#endif /* EEPROM_HPP_ */
