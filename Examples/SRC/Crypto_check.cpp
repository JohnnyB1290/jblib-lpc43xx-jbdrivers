/*
 * Crypto_check.cpp
 *
 * Created: 03.11.2017 13:51:34
 *  Author: Stalker1290
 */ 

#include "Crypto_check.hpp"

uint8_t Crypto_checker_t::Signature_buf[2*(Crypto_check_Key_length+Crypto_check_Gamma_length)];
uint8_t* Crypto_checker_t::baseKey_ptr;
uint8_t* Crypto_checker_t::baseGamma_ptr;
uint8_t Crypto_checker_t::Serial_num[16];
uint8_t Crypto_checker_t::DES3_UNIQ_key[Crypto_check_Key_length];
uint8_t Crypto_checker_t::DES3_SN_key[Crypto_check_Key_length];
bool Crypto_checker_t::Signature_is_valid = false;


void Crypto_checker_t::Initialize(void)
{
	des_ctx dc;

	memcpy((uint8_t*)Crypto_checker_t::Signature_buf,(uint8_t*)Signature_base,
			2*(Crypto_check_Gamma_length+Crypto_check_Key_length));

	if( memcmp((uint8_t*)&Crypto_checker_t::Signature_buf[0],
			(uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length + Crypto_check_Key_length],
			Crypto_check_Gamma_length + Crypto_check_Key_length) == 0) return;

	Chip_IAP_ReadUID((uint32_t*)Crypto_checker_t::Serial_num);

	memcpy((uint8_t*)&Crypto_checker_t::DES3_SN_key[0],(uint8_t*)&Crypto_checker_t::Serial_num[8],8);
	memcpy((uint8_t*)&Crypto_checker_t::DES3_SN_key[8],(uint8_t*)&Crypto_checker_t::Serial_num[4],8);
	memcpy((uint8_t*)&Crypto_checker_t::DES3_SN_key[16],(uint8_t*)&Crypto_checker_t::Serial_num[0],8);

	memcpy((uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0],(uint8_t*)&Crypto_checker_t::Serial_num[4],8);
	TripleDES_DEC(&dc,(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0], 1,
			(uint8_t*)&Crypto_checker_t::DES3_SN_key[16], (uint8_t*)&Crypto_checker_t::DES3_SN_key[8], (uint8_t*)&Crypto_checker_t::DES3_SN_key[0]);

	memcpy((uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8],(uint8_t*)&Crypto_checker_t::Serial_num[0],8);
	TripleDES_DEC(&dc,(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8], 1,
			(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0], (uint8_t*)&Crypto_checker_t::DES3_SN_key[0], (uint8_t*)&Crypto_checker_t::DES3_SN_key[8]);

	memcpy((uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[16],(uint8_t*)&Crypto_checker_t::Serial_num[8],8);
	TripleDES_DEC(&dc,(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[16], 1,
			(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0], (uint8_t*)&Crypto_checker_t::DES3_SN_key[8]);

	TripleDES_ENC(&dc,(uint8_t*)&Crypto_checker_t::Signature_buf[0], Crypto_check_Gamma_length/8,
			(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[16], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0]);
	TripleDES_ENC(&dc,(uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length], Crypto_check_Key_length/8,
			(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[16]);
	TripleDES_DEC(&dc,(uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length+Crypto_check_Key_length], Crypto_check_Key_length/8,
			(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[16], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0]);
	TripleDES_DEC(&dc,(uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length+2*Crypto_check_Key_length], Crypto_check_Gamma_length/8,
			(uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[0], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[8], (uint8_t*)&Crypto_checker_t::DES3_UNIQ_key[16]);

	Crypto_checker_t::baseGamma_ptr = (uint8_t*)&Crypto_checker_t::Signature_buf[0];
	Crypto_checker_t::baseKey_ptr = (uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length];

	if( memcmp((uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length],
			(uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length + Crypto_check_Key_length],Crypto_check_Key_length) != 0) return;

	if( memcmp((uint8_t*)&Crypto_checker_t::Signature_buf[0],
			(uint8_t*)&Crypto_checker_t::Signature_buf[Crypto_check_Gamma_length + 2*Crypto_check_Key_length],Crypto_check_Gamma_length) != 0) return;

	Crypto_checker_t::Signature_is_valid = true;

}

uint8_t* Crypto_checker_t::Get_Base_Key_ptr(void)
{
	return (uint8_t*)Crypto_checker_t::baseKey_ptr;
}

uint8_t* Crypto_checker_t::Get_Base_Gamma_ptr(void)
{
	return (uint8_t*)Crypto_checker_t::baseGamma_ptr;
}

bool Crypto_checker_t::Check(void)
{
	return Crypto_checker_t::Signature_is_valid;
}

uint8_t* Crypto_checker_t::Get_Serial_ptr(void)
{
	return (uint8_t*)Crypto_checker_t::Serial_num;
}
