/*
 * Web_interface.hpp
 *
 *  Created on: 15.12.2017
 *      Author: Stalker1290
 */

#ifndef WEB_INTERFACE_HPP_
#define WEB_INTERFACE_HPP_

#include "httpd.h"
#include "Param_iflash.hpp"

class Web_interface_t
{
public:
	static void Initialize(void);
private:
	static char* set_params_cgi_handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);

	static const tCGI cgi_uri_table[];
};


#endif /* WEB_INTERFACE_HPP_ */
