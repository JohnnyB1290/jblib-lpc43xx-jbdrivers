/*
 * NVMParameters.hpp
 *
 *  Created on: 11 сент. 2018 г.
 *      Author: Stalker1290
 */

#ifndef NVMPARAMETERS_HPP_
#define NVMPARAMETERS_HPP_

#include "chip.h"

#define NVM_PARAMETERS_MAGIC 0xAFDE

#pragma pack(push, 1)

typedef struct NVMParamsHeader_struct{
	uint16_t magic;
	uint16_t crc;
	uint8_t size; //number of parameters
	uint8_t reserved[3];
}NVMParamsHeader_t;


typedef struct NVMParamsCell_struct{
	uint8_t type;
	uint8_t descriptionSize;
	uint8_t dataSize;
	uint8_t reserved;
	char description[32];
	uint8_t data[32];
}NVMParamsCell_t;

#pragma pack(pop)

#define PARAMS_CELL_TYPE_ARRAY_bm 	(1<<7) //1 - data is array
#define PARAMS_CELL_TYPE_HEX_bm 	(1<<6) //1 - show data as hex, 0 - show data in dec


typedef enum{
	nvmParamTypeU8 = 0,
	nvmParamTypeU16 = 1,
	nvmParamTypeU32 = 2,
	nvmParamTypeString = 3,
	nvmParamTypeDouble = 4,
	nvmParamTypeI8 = 5,
	nvmParamTypeI16 = 6,
	nvmParamTypeI32 = 7,
}nmvParamsCellType_t;


class NVMParameters_t{
public:
	static NVMParameters_t* getNVMParametersPtr(void);
	NVMParamsCell_t* getParameter(char* paramDescription, uint8_t* buf, uint8_t bufSize); //return type
	NVMParamsCell_t* getParameter(char* paramDescription);
	void setParameter(NVMParamsCell_t* paramsCellPtr);
	void setParameter(uint8_t type, char* description, uint8_t* data, uint8_t dataSize);
	void deleteParameter(char* paramDescription);
	void eraseAllParameters(void);
	NVMParamsHeader_t* getHeaderPtr(void);
	uint32_t getParametersSize(void);
	void setAllParameters(void* ptr);
	uint32_t getCompressedParametersSize(void);
	uint32_t getCompressedParameters(uint8_t* buf);
private:
	NVMParameters_t(void);

	static NVMParameters_t* nvmParametersPtr;
	static uint32_t baseAddr;
	static NVMParamsHeader_t* paramsHeaderPtr;
	static uint8_t paramsHeaderSize;
	static uint8_t paramsCellSize;
};



#endif /* NVMPARAMETERS_HPP_ */
