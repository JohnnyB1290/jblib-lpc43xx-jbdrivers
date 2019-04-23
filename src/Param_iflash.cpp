/*
 * Param_iflash.cpp
 *
 *  Created on: 22.11.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Param_iflash.hpp"
#include "USB/USB_Device_use_rom/MSC_IFLASH.hpp"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "Defines.h"

bool Parameters_module_t::Initialized = false;
FIL Parameters_module_t::fil;		/* File object */
TCHAR Parameters_module_t::file_name[] = PARAM_FILE_NAME;
TCHAR* Parameters_module_t::Path = PARAM_PATH;
DIR Parameters_module_t::dp;
TCHAR* Parameters_module_t::Get_param_str = NULL;
FSIZE_t	Parameters_module_t::Get_param_fptr = 0;

void Parameters_module_t::Initialize(void)
{
	FRESULT Res;

	Res = f_opendir (&Parameters_module_t::dp, Parameters_module_t::Path);
	if (Res) return;

	Parameters_module_t::Initialized = true;
}

void Parameters_module_t::Print_all_Params(void)
{
	TCHAR file_string[PARAM_STR_MAX_SIZE];
	FRESULT Res = FR_OK;

	if(!Parameters_module_t::Initialized) return;

	Res = f_open(&Parameters_module_t::fil, Parameters_module_t::file_name, FA_OPEN_EXISTING| FA_READ);
	if(Res) return;

	Res = f_lseek(&Parameters_module_t::fil, 0);
	if(Res)
	{
		f_close(&Parameters_module_t::fil);
		return;
	}

	while(f_gets(file_string, PARAM_STR_MAX_SIZE,&Parameters_module_t::fil) != NULL)
	{
		printf("%s",file_string);
	}
	f_close(&Parameters_module_t::fil);
}

uint16_t Parameters_module_t::Get_param(char* param_name, uint8_t* buf, uint16_t buf_size, bool interpretate)
{
	uint16_t readen_count = 0;
	FRESULT Res = FR_OK;
	TCHAR file_string[PARAM_STR_MAX_SIZE];
	char* lexem_ptr = NULL;
	uint32_t file_str_len = 0;

	if(!Parameters_module_t::Initialized) return 0;
	if(buf_size == 0) return 0;

	if(param_name != NULL)
	{
		Parameters_module_t::Get_param_str = param_name;
		Parameters_module_t::Get_param_fptr = 0;
	}
	
	Res = f_open(&Parameters_module_t::fil, Parameters_module_t::file_name, FA_OPEN_EXISTING| FA_READ);
	if(Res) return 0;

	Res = f_lseek(&Parameters_module_t::fil, Parameters_module_t::Get_param_fptr);
	if(Res)
	{
		f_close(&Parameters_module_t::fil);
		return 0;
	}

	while(f_gets(file_string, PARAM_STR_MAX_SIZE,&Parameters_module_t::fil) != NULL)
	{
		file_str_len = strlen(file_string);
		lexem_ptr = strtok(file_string," ,.");
		if(lexem_ptr != NULL)
		{
			if(strcmp(Parameters_module_t::Get_param_str,lexem_ptr) == 0)
			{
				if(interpretate)
				{
					int temp_int = 0;
					lexem_ptr = strtok(NULL," ,.");
					while((lexem_ptr != NULL)&&(readen_count<buf_size))
					{
						if(sscanf(lexem_ptr,"%i",&temp_int))
						{
							buf[readen_count] = temp_int;
							readen_count++;
						}
						lexem_ptr = strtok(NULL," ,.");
					}
					Parameters_module_t::Get_param_fptr = Parameters_module_t::fil.fptr;
					f_close(&Parameters_module_t::fil);
					return readen_count;
				}
				else
				{
					if(strlen(lexem_ptr) == file_str_len)
					{
						Parameters_module_t::Get_param_fptr = Parameters_module_t::fil.fptr;
						f_close(&Parameters_module_t::fil);
						return 0;
					}
					char* temp_ptr = file_string;
					for(int i = 0; i<strlen(lexem_ptr)+1; i++) temp_ptr++;
					strlcpy((char*)buf, temp_ptr, buf_size);
					for(int i = 0; i<strlen((char*)buf); i++)
					{
						if((buf[i] == 0x0a)||(buf[i] == 0x0d))
						{
							buf[i] = 0;
							break;
						}
					}
					Parameters_module_t::Get_param_fptr = Parameters_module_t::fil.fptr;
					f_close(&Parameters_module_t::fil);
					return buf_size;
				}
			}
		}
	}

	f_close(&Parameters_module_t::fil);
	return 0;
}

uint16_t Parameters_module_t::Set_param(char* param_name, char* buf)
{
	FRESULT Res = FR_OK;
	TCHAR file_string[PARAM_STR_MAX_SIZE];
	char* lexem_ptr = NULL;
	uint8_t* ptr = NULL;
	FSIZE_t part_size = 0;
	UINT temp_uint = 0;
	TCHAR Out_str[PARAM_STR_MAX_SIZE];
	TCHAR temp_str[8];
	uint32_t file_string_len = 0;

	if(!Parameters_module_t::Initialized) return 0;

	if(param_name == NULL) return 0;

	Res = f_open(&Parameters_module_t::fil, Parameters_module_t::file_name, FA_OPEN_ALWAYS| FA_WRITE | FA_READ);
	if(Res) return 0;

	Res = f_lseek(&Parameters_module_t::fil, 0);
	if(Res)
	{
		f_close(&Parameters_module_t::fil);
		return 0;
	}

	while(f_gets(file_string, PARAM_STR_MAX_SIZE,&Parameters_module_t::fil) != NULL)
	{
		file_string_len = strlen(file_string);
		lexem_ptr = strtok(file_string," ,.");
		if(lexem_ptr != NULL)
		{
			if(strcmp(param_name,lexem_ptr) == 0)
			{
				FSIZE_t temp_fptr;
				
				if(Parameters_module_t::fil.fptr >= file_string_len)
				{
					temp_fptr = Parameters_module_t::fil.fptr - file_string_len + strlen(param_name) + 1;
				}
				else
				{
					f_close(&Parameters_module_t::fil);
					return 0;
				}
			
				part_size = f_size(&Parameters_module_t::fil) - Parameters_module_t::fil.fptr;
				ptr = (uint8_t*)malloc(part_size);
				if(ptr == NULL)
				{
					f_close(&Parameters_module_t::fil);
					return 0;
				}
				Res = f_read(&Parameters_module_t::fil, (void*)ptr, part_size,&temp_uint);
				if(Res)
				{
					f_close(&Parameters_module_t::fil);
					free(ptr);
					return 0;
				}

				Res = f_lseek(&Parameters_module_t::fil, temp_fptr);
				if(Res)
				{
					f_close(&Parameters_module_t::fil);
					free(ptr);
					return 0;
				}

				
				f_puts((char*)buf,&Parameters_module_t::fil);
				if(part_size != 0)
				{
					snprintf(Out_str,PARAM_STR_MAX_SIZE,"\r\n");
					f_puts(Out_str,&Parameters_module_t::fil);
				}
					
				Res = f_write (&Parameters_module_t::fil, ptr,part_size,&temp_uint);
				f_truncate (&Parameters_module_t::fil);
				f_close(&Parameters_module_t::fil);
				free(ptr);
				return 1;
			}
		}
	}

	snprintf(Out_str,PARAM_STR_MAX_SIZE,"\r\n");
	strcat(Out_str,param_name);

	snprintf(temp_str,8," ");
	strcat(Out_str, temp_str);
	f_puts(Out_str,&Parameters_module_t::fil);
	f_puts((char*)buf,&Parameters_module_t::fil);
	f_close(&Parameters_module_t::fil);
	return 1;

}

uint16_t Parameters_module_t::Set_param(char* param_name, uint8_t* buf, uint16_t buf_size)
{
	uint16_t written_count = 0;
	FRESULT Res = FR_OK;
	TCHAR file_string[PARAM_STR_MAX_SIZE];
	char* lexem_ptr = NULL;
	uint32_t file_string_len = 0;
	uint8_t* ptr = NULL;
	FSIZE_t part_size = 0;
	UINT temp_uint = 0;
	TCHAR Out_str[PARAM_STR_MAX_SIZE];
	TCHAR temp_str[8];

	if(!Parameters_module_t::Initialized) return 0;
	if(buf_size == 0) return 0;

	if(param_name == NULL) return 0;

	Res = f_open(&Parameters_module_t::fil, Parameters_module_t::file_name, FA_OPEN_ALWAYS| FA_WRITE | FA_READ);
	if(Res) return 0;

	Res = f_lseek(&Parameters_module_t::fil, 0);
	if(Res)
	{
		f_close(&Parameters_module_t::fil);
		return 0;
	}

	while(f_gets(file_string, PARAM_STR_MAX_SIZE,&Parameters_module_t::fil) != NULL)
	{
		file_string_len = strlen(file_string);
		lexem_ptr = strtok(file_string," ,.");
		if(lexem_ptr != NULL)
		{
			if(strcmp(param_name,lexem_ptr) == 0)
			{
				FSIZE_t temp_fptr = Parameters_module_t::fil.fptr;
				part_size = f_size(&Parameters_module_t::fil) - Parameters_module_t::fil.fptr;
				ptr = (uint8_t*)malloc(part_size);
				if(ptr == NULL)
				{
					f_close(&Parameters_module_t::fil);
					return 0;
				}
				Res = f_read(&Parameters_module_t::fil, (void*)ptr, part_size,&temp_uint);
				if(Res)
				{
					f_close(&Parameters_module_t::fil);
					free(ptr);
					return 0;
				}

				Res = f_lseek(&Parameters_module_t::fil, temp_fptr);
				if(Res)
				{
					f_close(&Parameters_module_t::fil);
					free(ptr);
					return 0;
				}

				strcpy(Out_str,lexem_ptr);
				if(Parameters_module_t::fil.fptr >= file_string_len) Res = f_lseek(&Parameters_module_t::fil,Parameters_module_t::fil.fptr - file_string_len);
				else
				{
					f_close(&Parameters_module_t::fil);
					free(ptr);
					return 0;
				}

				if(Res)
				{
					f_close(&Parameters_module_t::fil);
					free(ptr);
					return 0;
				}

				snprintf(temp_str,8," %i", buf[written_count]);
				written_count++;
				strcat(Out_str, temp_str);

				for(int j = 0; j<buf_size-1; j++)
				{
					snprintf(temp_str,8,".%i", buf[written_count]);
					written_count++;
					strcat(Out_str, temp_str);
				}
				snprintf(temp_str,8,"\r\n");
				strcat(Out_str, temp_str);

				f_puts(Out_str,&Parameters_module_t::fil);

				Res = f_write (&Parameters_module_t::fil, ptr,part_size,&temp_uint);
				f_truncate (&Parameters_module_t::fil);
				f_close(&Parameters_module_t::fil);
				free(ptr);
				return written_count;
			}
		}
	}

	snprintf(Out_str,PARAM_STR_MAX_SIZE,"\r\n");
	strcat(Out_str,param_name);

	snprintf(temp_str,8," %i", buf[written_count]);
	written_count++;
	strcat(Out_str, temp_str);

	for(int j = 0; j<buf_size-1; j++)
	{
		snprintf(temp_str,8,".%i", buf[written_count]);
		written_count++;
		strcat(Out_str, temp_str);
	}

	f_puts(Out_str,&Parameters_module_t::fil);
	f_close(&Parameters_module_t::fil);
	return written_count;

}
