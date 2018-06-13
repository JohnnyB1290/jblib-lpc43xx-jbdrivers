/*
 * Web_interface.cpp
 *
 *  Created on: 15.12.2017
 *      Author: Stalker1290
 */

#include "Web_interface.hpp"
#include "stdio.h"
#include "Defines.h"
#include "string.h"

const tCGI Web_interface_t::cgi_uri_table[] =
{
    { "/set_params.cgi", (tCGIHandler)Web_interface_t::set_params_cgi_handler },
};


void Web_interface_t::Initialize(void)
{
	http_set_cgi_handlers(Web_interface_t::cgi_uri_table, sizeof(Web_interface_t::cgi_uri_table) / sizeof(tCGI));
	httpd_init();
}

char* Web_interface_t::set_params_cgi_handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[])
{
	char Temp_str[PARAM_STR_MAX_SIZE];
	for(int i=0; i<iNumParams; i++)
	{
		if(strcmp(pcParam[i],"new_name") == 0)
		{
			if((i+1)<iNumParams)
			{
				if(strcmp(pcParam[i+1],"new_value") == 0)
				{
					Parameters_module_t::Set_param(pcValue[i],pcValue[i+1]);
				}
				i++;
				continue;
			}
		}

		if(Parameters_module_t::Get_param(pcParam[i], (uint8_t*)Temp_str, PARAM_STR_MAX_SIZE, false))
		{
			if(strcmp(pcValue[i],Temp_str) != 0)
			{
				Parameters_module_t::Set_param(pcParam[i],pcValue[i]);
			}
		}
		else Parameters_module_t::Set_param(pcParam[i],pcValue[i]);
	}


	return "/Bootini.txt";
}






