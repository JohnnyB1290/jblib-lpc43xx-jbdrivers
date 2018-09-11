/*
 * Param_iflash.hpp
 *
 *  Created on: 22.11.2017
 *      Author: Stalker1290
 */

#ifndef PARAM_IFLASH_HPP_
#define PARAM_IFLASH_HPP_

#include "chip.h"
#include "ff.h"

class Parameters_module_t
{
public:
	static void Initialize(void);
	static uint16_t Get_param(char* param_name, uint8_t* buf, uint16_t buf_size, bool interpretate);
	static uint16_t Set_param(char* param_name, uint8_t* buf, uint16_t buf_size);
	static uint16_t Set_param(char* param_name, char* buf);
	static void Print_all_Params(void);
private:
	static bool Initialized;
	static FIL fil;		/* File object */
	static TCHAR file_name[];
	static TCHAR* Path;
	static DIR dp;
	static TCHAR* Get_param_str;
	static FSIZE_t	Get_param_fptr;

};


#endif /* PARAM_IFLASH_HPP_ */
