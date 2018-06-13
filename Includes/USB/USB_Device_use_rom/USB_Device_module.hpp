/*
 * USB_Device_module.hpp
 *
 *  Created on: 14.09.2017
 *      Author: Stalker1290
 */

#ifndef USB_DEVICE_MODULE_HPP_
#define USB_DEVICE_MODULE_HPP_


#include "chip.h"
#include "Defines.h"
#include "error.h"
#include "usbd_rom/usbd_rom_api.h"

typedef enum
{
	USB_num_param = 0,
	RNDIS_Adapter_MAC_param = 1,
}USB_Device_module_param_t;

class  USB_Devices_module_t
{
public:
	USB_Devices_module_t(){}
	virtual ~USB_Devices_module_t(void){}
	virtual ErrorCode_t USB_Initialize(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* pUsbParam) = 0;
	virtual void USB_Tasks(void) = 0;
	virtual void USB_GetParameter(uint8_t ParamName, void* ParamValue) = 0;
	virtual void USB_SetParameter(uint8_t ParamName, void* ParamValue) = 0;
};

#endif /* USB_DEVICE_MODULE_HPP_ */
