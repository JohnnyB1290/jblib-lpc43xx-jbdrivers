/*
 * FRAM_CY15B104Q.hpp
 *
 *  Created on: 21 θών. 2018 γ.
 *      Author: Stalker1290
 */

#ifndef FRAM_CY15B104Q_HPP_
#define CY15B104Q_HPP_

#include "chip.h"
#include "Defines.h"
#include "SSP.hpp"

#define WREN_OPCODE 	0x06 //Set write enable latch
#define WRDI_OPCODE 	0x04 //Reset write enable latch
#define RDSR_OPCODE 	0x05 //Read Status Register
#define WRSR_OPCODE 	0x01 //Write Status Register
#define READ_OPCODE 	0x03 //Read memory data
#define FSTRD_OPCODE 	0x0B //Fast read memory data
#define WRITE_OPCODE 	0x02 //Write memory data
#define SLEEP_OPCODE 	0xB9 //Enter sleep mode
#define RDID_OPCODE 	0x9F //Read device ID

#define SREG_WPEN_bm	0x80 //Write Protect Enable bit. Used to enable the function of Write Protect Pin (WP)
#define SREG_BP1_bm		0x08 //Used for block protection
#define SREG_BP0_bm		0x04 //Used for block protection
#define SREG_WEL_bm		0x02 //Write Enable

#define DEVICE_ID_SIZE 9

#define MANUFACTURER_ID_1 0x007F7F7F 	//BYTE678
#define MANUFACTURER_ID_0 0x7F7F7FC2 	//BYTE2345

#define REVISION_bm	0x38 //BYTE0
#define SUB_CODE_bm	0xC0 //BYTE0
#define DENSITY_bm	0x1F //BYTE1
#define FAMILY_bm	0xE0 //BYTE1

#define FRAM_SIZE 512*1024

class FRAMcy15b104q_t{
public:
	FRAMcy15b104q_t(SSP_t* SSPPtr, uint32_t SSPDeviceNum);
	bool Initialize();
	void memcpy(void* destination, void* source, uint32_t size);
private:
	SSP_t* SSPPtr;
	uint32_t SSPDeviceNum;
	bool dataExchange(uint8_t opCode, uint32_t address, uint8_t* txDataPtr, uint8_t* rxDataPtr, uint32_t length);
};



#endif /* CY15B104Q_HPP_ */
