/*
 * USB_Device.hpp
 *
 *  Created on: 14.09.2017
 *      Author: Stalker1290
 */

#include "chip.h"
#include "Defines.h"
#include "IRQ_Controller.hpp"
#include "USB_Device_module.hpp"


#ifndef USB_DEVICE_HPP_
#define USB_DEVICE_HPP_

#define num_of_usb 2

class USB_Device_t:protected IRQ_LISTENER_t,public Callback_Interface_t
{
public:
	static USB_Device_t* get_USB_Device(uint8_t num);
	void Initialize(USB_Devices_module_t** modules_buf_ptr,uint8_t num_of_modules);
	void Connect(void);
	void Disconnect(void);
	void Reset(void);
	ErrorCode_t ResetEvent(USBD_HANDLE_T hUsb);
	ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event);
	USB_Devices_module_t** modules_buf_ptr;
	uint8_t num_of_modules;
	static uint8_t USB0_DeviceDescriptor[];
	static uint8_t USB0_HsConfigDescriptor[];
	static uint8_t USB0_FsConfigDescriptor[];
	static uint8_t USB0_StringDescriptor[];
	static uint8_t USB0_DeviceQualifier[];
	static uint8_t USB1_DeviceDescriptor[];
	static uint8_t USB1_FsConfigDescriptor[];
	static uint8_t USB1_StringDescriptor[];
private:
	USB_Device_t(uint8_t num);
	virtual void IRQ(int8_t IRQ_num);
	virtual void void_callback(void* Intf_ptr, void* parameters);

	uint8_t num;
	USBD_HANDLE_T g_hUsb;
	/* Endpoint 0 patch that prevents nested NAK event processing */
	uint32_t g_ep0RxBusy;/* flag indicating whether EP0 OUT/RX buffer is busy. */
	USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */

	static USB_Device_t* USB_Device_ptrs[num_of_usb];
	static const USBD_API_T* g_pUsbApi;
};


#endif /* USB_DEVICE_HPP_ */
