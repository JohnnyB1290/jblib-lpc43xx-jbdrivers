/*
 * Crypto_check.hpp
 *
 * Created: 03.11.2017 13:51:07
 *  Author: Stalker1290
 */ 


#ifndef CRYPTO_CHECK_HPP_
#define CRYPTO_CHECK_HPP_

#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "Defines.h"
#include "DES.h"
#include "chip.h"

class Crypto_checker_t
{
public:
	static void Initialize(void);
	static uint8_t* Get_Base_Key_ptr(void);
	static uint8_t* Get_Base_Gamma_ptr(void);
	static bool Check(void);
	static uint8_t* Get_Serial_ptr(void);
private:
	static uint8_t Signature_buf[2*(Crypto_check_Key_length+Crypto_check_Gamma_length)];
	static uint8_t* baseKey_ptr;
	static uint8_t* baseGamma_ptr;
	static uint8_t Serial_num[16];
	static uint8_t DES3_UNIQ_key[Crypto_check_Key_length];
	static uint8_t DES3_SN_key[Crypto_check_Key_length];
	static bool Signature_is_valid;
};


#endif /* CRYPTO_CHECK_HPP_ */

