/*
 * SGPIO_SPI.cpp
 *
 *  Created on: 28 сент. 2018 г.
 *      Author: Stalker1290
 */

#include "SGPIO_SPI.hpp"
#include "stdlib.h"

uint8_t SGPIO_SPI_t::reverseBitsInByteTable[] = {
 0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
 0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
 0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
 0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
 0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
 0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
 0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
 0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
 0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
 0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
 0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

/******************************************************************************
* SLICE P is used to generate a NSS
******************************************************************************/
SgpioSliceCfg SGPIO_SPI_t::NSS_CFG = {P, SGPIO_OUTPUT_PIN, SGPIO_15};

/******************************************************************************
* SLICE N is used to transmit the data for MASTER 0
******************************************************************************/
SgpioSliceCfg SGPIO_SPI_t::DATA_OUT_CFG = {N, SGPIO_OUTPUT_PIN, SGPIO_11};

/******************************************************************************
* SLICE G is used to receive the data for MASTER 0
******************************************************************************/
SgpioSliceCfg SGPIO_SPI_t::DATA_IN_CFG = {G, SGPIO_INPUT_PIN, SGPIO_10};

/******************************************************************************
* SLICE D is used to generate a clock signal
* the configuration is the same for each SPI mode
******************************************************************************/
SgpioSliceCfg SGPIO_SPI_t::CLOCK_CFG = {D, SGPIO_OUTPUT_PIN, SGPIO_12};

SGPIO_SPI_t* SGPIO_SPI_t::SGPIO_SPI_ptr = (SGPIO_SPI_t*)NULL;

SGPIO_SPI_t* SGPIO_SPI_t::getSGPIOspi(void){
	if(SGPIO_SPI_ptr == (SGPIO_SPI_t*)NULL) SGPIO_SPI_ptr = new SGPIO_SPI_t();
	return SGPIO_SPI_ptr;
}

SGPIO_SPI_t::SGPIO_SPI_t(void){
	this->sgpioClockHz = 0;
	Chip_RGU_TriggerReset(RGU_SGPIO_RST);
	while (Chip_RGU_InReset(RGU_SGPIO_RST)) {}
}

void SGPIO_SPI_t::Initialize(uint32_t bitrate){

	SGPIO_SliceMuxConfig 	sliceMuxCfg;
	SGPIO_MuxConfig 		muxCfg;
	SGPIO_OutMuxConfig		outMuxCfg;

	Chip_Clock_Enable(CLK_PERIPH_SGPIO);
	this->sgpioClockHz = Chip_Clock_GetRate(CLK_PERIPH_SGPIO);

	Chip_SCU_PinMuxSet(SGPIO_SPI_MOSI_PORT, SGPIO_SPI_MOSI_PIN, (SCU_MODE_PULLUP | SGPIO_SPI_MOSI_SCU_FUNC)); /*  MOSI */
	Chip_SCU_PinMuxSet(SGPIO_SPI_MISO_PORT, SGPIO_SPI_MISO_PIN, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SGPIO_SPI_MISO_SCU_FUNC)); /* MISO */
	Chip_SCU_PinMuxSet(SGPIO_SPI_SCK_PORT, SGPIO_SPI_SCK_PIN, (SCU_MODE_PULLUP | SCU_MODE_20MA_DRIVESTR | SCU_MODE_INBUFF_EN | SGPIO_SPI_SCK_SCU_FUNC)); /* SCK */
	Chip_SCU_PinMuxSet(SGPIO_SPI_SSEL_PORT, SGPIO_SPI_SSEL_PIN, (SCU_MODE_PULLUP  | SGPIO_SPI_SSEL_SCU_FUNC)); /* NSS */


	this->masterMask = ((1U << CLOCK_CFG.sliceId)|\
					(1U << DATA_OUT_CFG.sliceId)|\
					(1U << DATA_IN_CFG.sliceId)/*|\
					(1U << NSS_CFG.sliceId)*/);

	/**********************************************************************
	* make the static configuration for the Clock, slice D
	*
	**********************************************************************/
	SGPIO_disableSlice(CLOCK_CFG.sliceId);

	sliceMuxCfg = SGPIO_makeSliceMuxConfig(
		SMC_MATCH_DATA,
		SMC_CLKCAP_DONTCARE,
		SMC_CLKGEN_COUNTER,
		SMC_INVOUT_DONTCARE,
		SMC_DATACAP_DONTCARE,
		SMC_PAR_1BPCK,
		SMC_INVQUAL_INVERTED
		);

	SGPIO_configSliceMuxReg(CLOCK_CFG.sliceId, sliceMuxCfg);

	muxCfg = SGPIO_makeMuxConfig(
		MC_CLK_DONTCARE,
		MC_CLKSRC_PIN_DONTCARE,
		MC_CLKSRC_SLICE_DONTCARE,
		MC_QUALMODE_DISABLE,
		MC_QUALPIN_DONTCARE,
		MC_QUALSLICE_DONTCARE,
		MC_CONCAT_DATA,
		MC_CONCAT_SELF_LOOP);


	SGPIO_configMuxReg(CLOCK_CFG.sliceId, muxCfg);

	outMuxCfg = SGPIO_makeOutMuxConfig(OMC_DOUTM1, OMC_GPIO_OE);
	SGPIO_configOutMuxReg(CLOCK_CFG.pinId, outMuxCfg);
	if(CLOCK_CFG.sliceFunc == SGPIO_OUTPUT_PIN)
		SGPIO_setOeReg(CLOCK_CFG.pinId, outMuxCfg);

	SGPIO_setBitCountReg(CLOCK_CFG.sliceId, (DBIT_8)*2);
	SGPIO_setCountReloadReg(CLOCK_CFG.sliceId, this->sgpioClockHz/(2*bitrate));
	SGPIO_writeDataReg(CLOCK_CFG.sliceId, 0xAAAAAAAA);

	/**********************************************************************
	* make the static configuration for the Data out pin, slice N
	*
	**********************************************************************/
	SGPIO_disableSlice(DATA_OUT_CFG.sliceId);

		sliceMuxCfg = SGPIO_makeSliceMuxConfig(
		//SMC_DONT_MATCH,
		SMC_MATCH_DATA,
		SMC_CLKCAP_FALLING,
		SMC_CLKGEN_COUNTER,
		SMC_INVOUT_DONTCARE,
		SMC_DATACAP_DONTCARE,
		SMC_PAR_1BPCK,
		SMC_INVQUAL_INVERTED
		);

	SGPIO_configSliceMuxReg(DATA_OUT_CFG.sliceId, sliceMuxCfg);

	muxCfg = SGPIO_makeMuxConfig(
		MC_CLK_INTERNAL,
		MC_CLKSRC_PIN_DONTCARE,
		MC_CLKSRC_SLICE_D,
		MC_QUALMODE_DISABLE,
		MC_QUALPIN_DONTCARE,
		MC_QUALSLICE_DONTCARE,
		MC_CONCATEN_DONTCARE,
		MC_CONCAT_ORDER_DONTCARE
		);

	SGPIO_configMuxReg(DATA_OUT_CFG.sliceId, muxCfg);

	outMuxCfg = SGPIO_makeOutMuxConfig(OMC_DOUTM1, OMC_GPIO_OE);
	//outMuxCfg = SGPIO_makeOutMuxConfig(OMC_CLKOUT, OMC_GPIO_OE);

	SGPIO_configOutMuxReg(DATA_OUT_CFG.pinId, outMuxCfg);
	if(DATA_OUT_CFG.sliceFunc == SGPIO_OUTPUT_PIN)
		SGPIO_setOeReg(DATA_OUT_CFG.pinId, outMuxCfg);

	SGPIO_setBitCountReg(DATA_OUT_CFG.sliceId, DBIT_8);
	SGPIO_setCountReloadReg(DATA_OUT_CFG.sliceId, this->sgpioClockHz/bitrate);
	SGPIO_writeDataReg(DATA_OUT_CFG.sliceId, 0x0);
	SGPIO_writeDataShadowReg(DATA_OUT_CFG.sliceId, 0x0);

	/**********************************************************************
	* make the static configuration for the Data in pin, slice G
	*
	**********************************************************************/
	SGPIO_disableSlice(DATA_IN_CFG.sliceId);

	sliceMuxCfg = SGPIO_makeSliceMuxConfig(
		//SMC_DONT_MATCH,
		SMC_MATCH_DATA,
		SMC_CLKCAP_RISING,
		SMC_CLKGEN_COUNTER,
		SMC_INVOUT_DONTCARE,
		SMC_DATACAP_RISING,
		SMC_PAR_1BPCK,
		SMC_INVQUAL_DONTCARE);

	SGPIO_configSliceMuxReg(DATA_IN_CFG.sliceId, sliceMuxCfg);

	muxCfg = SGPIO_makeMuxConfig(
		MC_CLK_INTERNAL,
		MC_CLKSRC_PIN_DONTCARE,
		MC_CLKSRC_SLICE_D,
		MC_QUALMODE_DONTCARE,
		MC_QUALPIN_DONTCARE,
		MC_QUALSLICE_DONTCARE,
		MC_CONCATEN_DONTCARE,
		MC_CONCAT_ORDER_DONTCARE);

	SGPIO_configMuxReg(DATA_IN_CFG.sliceId, muxCfg);

	outMuxCfg = SGPIO_makeOutMuxConfig(OMC_DOUTM1, OMC_GPIO_OE);
	SGPIO_configOutMuxReg(DATA_IN_CFG.pinId, outMuxCfg);
//	  can be modified to test the clk output
//	 	outMuxCfg = SGPIO_makeOutMuxConfig(OMC_CLKOUT, OMC_GPIO_OE);
//	 	SGPIO_configOutMuxReg((SGPIO_Pin)10, outMuxCfg);

	  //can be modified to test the clk output
	// SGPIO_setOeReg((SGPIO_Pin)10, outMuxCfg);

	SGPIO_setBitCountReg(DATA_IN_CFG.sliceId, DBIT_8);
	SGPIO_setCountReloadReg(DATA_IN_CFG.sliceId, this->sgpioClockHz/bitrate);
	SGPIO_writeDataReg(DATA_IN_CFG.sliceId, 0x0);
	SGPIO_writeDataShadowReg(DATA_IN_CFG.sliceId, 0x0);


	/**********************************************************************
	* make the static configuration for the NSS, slice P
	*
	**********************************************************************/
	SGPIO_disableSlice(NSS_CFG.sliceId);

	sliceMuxCfg = SGPIO_makeSliceMuxConfig(
		SMC_DONT_MATCH,
		SMC_CLKCAP_DONTCARE,
		SMC_CLKGEN_DONTCARE,
		SMC_INVOUT_DONTCARE,
		SMC_DATACAP_DONTCARE,
		SMC_PAR_DONTCARE,
		SMC_INVQUAL_INVERTED
		);

	SGPIO_configSliceMuxReg(NSS_CFG.sliceId, sliceMuxCfg);

	muxCfg = SGPIO_makeMuxConfig(
		MC_CLK_DONTCARE,
		MC_CLKSRC_PIN_DONTCARE,
		MC_CLKSRC_SLICE_DONTCARE,
		MC_QUALMODE_DISABLE,
		MC_QUALPIN_DONTCARE,
		MC_QUALSLICE_DONTCARE,
		MC_CONCATEN_DONTCARE,
		MC_CONCAT_ORDER_DONTCARE);

	SGPIO_configMuxReg(NSS_CFG.sliceId, muxCfg);

	outMuxCfg = SGPIO_makeOutMuxConfig(OMC_GPIO_OUT, OMC_GPIO_OE);
	SGPIO_configOutMuxReg(NSS_CFG.pinId, outMuxCfg);
	if(NSS_CFG.sliceFunc == SGPIO_OUTPUT_PIN)
		SGPIO_setOeReg(NSS_CFG.pinId, outMuxCfg);

	LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);

	// enable capture interrupt for slice CHIP SELECT
	LPC_SGPIO->SET_EN_1 = (1<<CLOCK_CFG.sliceId);
	LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);		// clear capture interrupt status
	while(LPC_SGPIO->STATUS_1 & (1<<CLOCK_CFG.sliceId));		// wait for status to clear
}

void SGPIO_SPI_t::SGPIO_spiWrite(uint8_t dataWrite, uint8_t* dataRead){

	static uint32_t rxWord = 0;

	SGPIO_writeDataReg(DATA_OUT_CFG.sliceId, reverseBitsInByteTable[dataWrite]);
	SGPIO_enableSlices(true,this->masterMask);

	while(! ((LPC_SGPIO->STATUS_1) & (1<<CLOCK_CFG.sliceId)));
	LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);

	SGPIO_disableSlices(this->masterMask);
	SGPIO_readDataReg(DATA_IN_CFG.sliceId, DBIT_8, &rxWord);
	if(dataRead != NULL) *dataRead = reverseBitsInByteTable[rxWord&0xFF];

	SGPIO_writeDataReg(DATA_IN_CFG.sliceId, 0x0);
	SGPIO_setCountReg(CLOCK_CFG.sliceId, 0);
	SGPIO_setCountReg(DATA_OUT_CFG.sliceId, 0);
	SGPIO_setCountReg(DATA_IN_CFG.sliceId, 0);
}

uint8_t SGPIO_SPI_t::TxRx_frame(uint8_t data){

	static uint8_t ret = 0;

	LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << NSS_CFG.pinId);

	this->SGPIO_spiWrite(data, &ret);

	LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);

	return ret;
}


#include "CONTROLLER.hpp"
uint32_t SGPIO_SPI_t::TxRx_frame(uint8_t* tx_data, uint8_t* rx_data, uint32_t length){

//	uint16_t offset = 0;
//
//	LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << NSS_CFG.pinId);
//
//	for(uint32_t i = 0; i < length; i++)
//		tx_data[i] = reverseBitsInByteTable[tx_data[i]];
//
//
//	SGPIO_writeDataReg(DATA_OUT_CFG.sliceId, tx_data[offset++]);
//
//	while(offset < length) {
//
//		LPC_SGPIO->CTRL_ENABLED	 |= this->masterMask;
//
//		LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);
//
//		if(true == true) LPC_SGPIO->CTRL_DISABLED |= this->masterMask;
//
//		//SGPIO_enableSlices(true, this->masterMask);
//
//
//		SGPIO_writeDataShadowReg(DATA_OUT_CFG.sliceId, tx_data[offset]);
//
//		LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << NSS_CFG.pinId);
//
//		if(offset > 1) rx_data[offset - 2] = (LPC_SGPIO->REG_SS[DATA_IN_CFG.sliceId] >> 24)&0xFF;
//		offset++;
//
//
//		while(! ((LPC_SGPIO->STATUS_1) & (1<<CLOCK_CFG.sliceId)));
//		LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);
//
//		//rx_data[offset - 1] = (LPC_SGPIO->REG[DATA_IN_CFG.sliceId] >> 24)&0xFF;
//
//
//		SGPIO_disableSlices(this->masterMask);
//
//
//	}
//
//
//
//	SGPIO_enableSlices(true, this->masterMask);
//	while(! ((LPC_SGPIO->STATUS_1) & (1<<CLOCK_CFG.sliceId)));
//	LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);
//	rx_data[offset - 1] = (LPC_SGPIO->REG_SS[DATA_IN_CFG.sliceId] >> 24)&0xFF;
//	SGPIO_disableSlices(this->masterMask);
//
//
//	for(uint32_t i = 0; i < length; i++)
//		rx_data[i] = reverseBitsInByteTable[rx_data[i]];
//
//	LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);
//
//	return length;



	LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << NSS_CFG.pinId);

	for(uint32_t i = 0; i < length; i++){
		SGPIO_writeDataReg(DATA_OUT_CFG.sliceId, reverseBitsInByteTable[tx_data[i]]);
		//enable slices
		SGPIO_enableSlices(true, this->masterMask);

		while(! ((LPC_SGPIO->STATUS_1) & (1<<CLOCK_CFG.sliceId)));
		LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);

		rx_data[i] = reverseBitsInByteTable[(LPC_SGPIO->REG_SS[DATA_IN_CFG.sliceId] >> 24)&0xFF];

		SGPIO_disableSlices(this->masterMask);
	}

	LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);

	return length;



//	uint16_t offset = 0;
//
//	LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << NSS_CFG.pinId);
//
//	//write first byte to slice
//	SGPIO_writeDataReg(DATA_OUT_CFG.sliceId, reverseBitsInByteTable[tx_data[offset++]]);
//	//enable slices
//	SGPIO_enableSlices(length == 1, this->masterMask);
//
//	while(offset < length) {
//		//LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);
//		//push next byte to shadow register
//		SGPIO_writeDataShadowReg(DATA_OUT_CFG.sliceId, reverseBitsInByteTable[tx_data[offset]]);
//		//wait for previous slice done
//		while(! ((LPC_SGPIO->STATUS_1) & (1<<CLOCK_CFG.sliceId)));
//		LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);
//		//disable cyclic mode when last byte transmitted
//		if(offset == (length - 1)) {
//			LPC_SGPIO->CTRL_DISABLED |= this->masterMask;
//		}
//		//read from shadow
//		rx_data[offset - 1] = reverseBitsInByteTable[(LPC_SGPIO->REG_SS[DATA_IN_CFG.sliceId] >> 24)&0xFF];
//		offset++;
//		//LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << NSS_CFG.pinId);
//	}
//	//wait for previous slice done
//
//	//LPC_SGPIO->CTRL_DISABLED |= this->masterMask;
//
//	while(! ((LPC_SGPIO->STATUS_1) & (1<<CLOCK_CFG.sliceId)));
//	LPC_SGPIO->CTR_STATUS_1 = (1<<CLOCK_CFG.sliceId);
//
//
//
//	rx_data[offset - 1] = reverseBitsInByteTable[(LPC_SGPIO->REG_SS[DATA_IN_CFG.sliceId] >> 24)&0xFF];
//
//	SGPIO_disableSlices(this->masterMask); //140ns
//
//	LPC_SGPIO->GPIO_OUTREG |= (0x1 << NSS_CFG.pinId);
//
//	return length;
}

void SGPIO_SPI_t::Deinitialize(void){

	this->sgpioClockHz = 0;
	Chip_RGU_TriggerReset(RGU_SGPIO_RST);
	while (Chip_RGU_InReset(RGU_SGPIO_RST)) {}
}



