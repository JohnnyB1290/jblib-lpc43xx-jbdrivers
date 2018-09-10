/*
 * USB_Device.c
 *
 *  Created on: 13.03.2017
 *      Author: Stalker1290
 */


#include "USB/USB_Device_use_rom/USB_Device.hpp"
#include "string.h"
#include "stdio.h"
#include "CONTROLLER.hpp"

static ErrorCode_t Reset_event_USB0(USBD_HANDLE_T hUsb);
static ErrorCode_t Reset_event_USB1(USBD_HANDLE_T hUsb);
static ErrorCode_t USB0_EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event);
static ErrorCode_t USB1_EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event);

USB_Device_t* USB_Device_t::USB_Device_ptrs[num_of_usb] = {(USB_Device_t*)NULL, (USB_Device_t*)NULL};
const USBD_API_T* USB_Device_t::g_pUsbApi = (const USBD_API_T*) LPC_ROM_API->usbdApiBase;


USB_Device_t* USB_Device_t::get_USB_Device(uint8_t num)
{
	if(num >= num_of_usb) return (USB_Device_t*)NULL;
	if(USB_Device_t::USB_Device_ptrs[num] == (USB_Device_t*)NULL) USB_Device_t::USB_Device_ptrs[num] = new USB_Device_t(num);
	return USB_Device_t::USB_Device_ptrs[num];
}

USB_Device_t::USB_Device_t(uint8_t num):IRQ_LISTENER_t(),Callback_Interface_t()
{
	this->num = num;
	this->g_hUsb = NULL;
	this->g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
	g_Ep0BaseHdlr = (USB_EP_HANDLER_T)NULL;	/* variable to store the pointer to base EP0 handler */
	this->modules_buf_ptr = (USB_Devices_module_t**)NULL;
	this->num_of_modules = 0;

	if(num == 0) this->setCode((uint64_t)1<<USB0_IRQn);
	if(num == 1) this->setCode((uint64_t)1<<USB1_IRQn);
}


void USB_Device_t::void_callback(void* Intf_ptr, void* parameters)
{
	if((CONTROLLER_t*)Intf_ptr == CONTROLLER_t::get_CONTROLLER())
	{
		for(int i=0; i<this->num_of_modules; i++) this->modules_buf_ptr[i]->USB_Tasks();
	}
}

void USB_Device_t::Disconnect(void)
{
	USB_Device_t::g_pUsbApi->hw->Connect(this->g_hUsb, 0);
	CONTROLLER_t::get_CONTROLLER()->Delete_main_procedure(this);
}

void USB_Device_t::Connect(void)
{
	USB_Device_t::g_pUsbApi->hw->Connect(this->g_hUsb, 1);
	CONTROLLER_t::get_CONTROLLER()->Add_main_procedure(this);
}

void USB_Device_t::Reset(void)
{
	CONTROLLER_t::get_CONTROLLER()->Delete_main_procedure(this);
	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Delete_Peripheral_IRQ_Listener(this);

	if(this->num == 0)
	{
		NVIC_DisableIRQ(USB0_IRQn);
		NVIC_ClearPendingIRQ(USB0_IRQn);
	}
	else if(this->num == 1)
	{
		NVIC_DisableIRQ(USB1_IRQn);
		NVIC_ClearPendingIRQ(USB1_IRQn);
	}

	USB_Device_t::g_pUsbApi->hw->Reset(this->g_hUsb);
}

void USB_Device_t::IRQ(int8_t IRQ_num)
{
	USB_Device_t::g_pUsbApi->hw->ISR(this->g_hUsb);
}

ErrorCode_t USB_Device_t::EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	switch (event) {
	case USB_EVT_OUT_NAK:
		if (this->g_ep0RxBusy) {
			/* we already queued the buffer so ignore this NAK event. */
			return LPC_OK;
		}
		else {
			/* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
			this->g_ep0RxBusy = 1;
		}
		break;

	case USB_EVT_SETUP:	/* reset the flag when new setup sequence starts */
	case USB_EVT_OUT:
		/* we received the packet so clear the flag. */
		this->g_ep0RxBusy = 0;
		break;
	}
	return this->g_Ep0BaseHdlr(hUsb, data, event);
}

static ErrorCode_t USB0_EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_Device_t* USB0_dev_ptr = USB_Device_t::get_USB_Device(0);
	return USB0_dev_ptr->EP0_patch(hUsb,data,event);
}

static ErrorCode_t USB1_EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_Device_t* USB1_dev_ptr = USB_Device_t::get_USB_Device(1);
	return USB1_dev_ptr->EP0_patch(hUsb,data,event);
}


ErrorCode_t USB_Device_t::ResetEvent(USBD_HANDLE_T hUsb)
{
	#ifdef USE_CONSOLE
	#ifdef USB_console
	printf("USB Reset event\r\n\r\n");
	#endif
	#endif
	return ERR_USBD_UNHANDLED;
}

static ErrorCode_t Reset_event_USB0(USBD_HANDLE_T hUsb)
{
	USB_Device_t* USB0_dev_ptr = USB_Device_t::get_USB_Device(0);
	return USB0_dev_ptr->ResetEvent(hUsb);
}

static ErrorCode_t Reset_event_USB1(USBD_HANDLE_T hUsb)
{
	USB_Device_t* USB1_dev_ptr = USB_Device_t::get_USB_Device(1);
	return USB1_dev_ptr->ResetEvent(hUsb);
}

void USB_Device_t::Initialize(USB_Devices_module_t** modules_buf_ptr,uint8_t num_of_modules)
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;

	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif

	#ifdef USE_CONSOLE
	#ifdef USB_console
	printf("Initialize USB Device\r\n\r\n");
	#endif
	#endif

	if((num_of_modules == 0)||(modules_buf_ptr == (USB_Devices_module_t**)NULL))
	{
		#ifdef USE_CONSOLE
		#ifdef USB_console
		printf("USB ERROR! No USB modules\r\n\r\n");
		#endif
		#endif
		return;
	}
	this->num_of_modules = num_of_modules;
	this->modules_buf_ptr = modules_buf_ptr;

	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));

	if(this->num == 0)
	{
		Chip_USB0_Init();

		usb_param.usb_reg_base = LPC_USB0_BASE;
		usb_param.mem_base = USB0_STACK_MEM_BASE;
		usb_param.mem_size = USB0_STACK_MEM_SIZE;
		usb_param.max_num_ep = USB_MAX_EP_NUM;
		usb_param.USB_Reset_Event = Reset_event_USB0;

		/* Set the USB descriptors */
		desc.device_desc = (uint8_t *) USB_Device_t::USB0_DeviceDescriptor;
		desc.string_desc = (uint8_t *) USB_Device_t::USB0_StringDescriptor;
		desc.high_speed_desc = USB_Device_t::USB0_HsConfigDescriptor;
		desc.full_speed_desc = USB_Device_t::USB0_FsConfigDescriptor;
		desc.device_qualifier = (uint8_t *) USB_Device_t::USB0_DeviceQualifier;
	}
	if(this->num == 1)
	{
		Chip_USB1_Init();

		usb_param.usb_reg_base = LPC_USB1_BASE;
		usb_param.mem_base = USB1_STACK_MEM_BASE;
		usb_param.mem_size = USB1_STACK_MEM_SIZE;
		usb_param.max_num_ep = USB_MAX_EP_NUM;
		usb_param.USB_Reset_Event = Reset_event_USB1;

		/* Set the USB descriptors */
		desc.device_desc = (uint8_t *) USB_Device_t::USB1_DeviceDescriptor;
		desc.string_desc = (uint8_t *) USB_Device_t::USB1_StringDescriptor;
		desc.high_speed_desc = USB_Device_t::USB1_FsConfigDescriptor;
		desc.full_speed_desc = USB_Device_t::USB1_FsConfigDescriptor;
		desc.device_qualifier = 0;
	}

	/* USB Initialization */
	ret = USB_Device_t::g_pUsbApi->hw->Init(&(this->g_hUsb), &desc, &usb_param);
	if (ret != LPC_OK)
	{
		#ifdef USE_CONSOLE
		#ifdef USB_console
		printf("USB ERROR in HW Ini! 0x%X\r\n\r\n",ret);
		#endif
		#endif
	}
	else
	{
		#ifdef USE_CONSOLE
		#ifdef USB_console
		printf("New mem base after HW ini = 0x%X\r\n",usb_param.mem_base);
		printf("New mem size after HW ini = 0x%X\r\n\r\n",usb_param.mem_size);
		#endif
		#endif
	}

	/*	WORKAROUND for artf45032 ROM driver BUG:
	    Due to a race condition there is the chance that a second NAK event will
	    occur before the default endpoint0 handler has completed its preparation
	    of the DMA engine for the first NAK event. This can cause certain fields
	    in the DMA descriptors to be in an invalid state when the USB controller
	    reads them, thereby causing a hang.
	 */
	pCtrl = (USB_CORE_CTRL_T *) this->g_hUsb;	/* convert the handle to control structure */
	this->g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */

	if(this->num == 0) pCtrl->ep_event_hdlr[0] = USB0_EP0_patch;/* set our patch routine as EP0_OUT handler */
	if(this->num == 1) pCtrl->ep_event_hdlr[0] = USB1_EP0_patch;/* set our patch routine as EP0_OUT handler */
	
	for(int i = 0; i<this->num_of_modules; i++)
	{
		if(this->modules_buf_ptr[i] != (USB_Devices_module_t*)NULL)
		{
			this->modules_buf_ptr[i]->USB_SetParameter(USB_num_param, (void*)&this->num);
			ret = this->modules_buf_ptr[i]->USB_Initialize(this->g_hUsb, &desc, &usb_param);
		}
		else break;
		if (ret != LPC_OK)
		{
			#ifdef USE_CONSOLE
			#ifdef USB_console
			printf("USB ERROR in USB Module %i Ini! 0x%X\r\n\r\n",i, ret);
			#endif
			#endif
		}
		else
		{
			#ifdef USE_CONSOLE
			#ifdef USB_console
			printf("New mem base after USB Module %i ini = 0x%X\r\n",i,usb_param.mem_base);
			printf("New mem size after USB Module %i ini = 0x%X\r\n\r\n",i,usb_param.mem_size);
			#endif
			#endif
		}
	}

	IRQ_CONTROLLER_t& IRQ_Control = IRQ_CONTROLLER_t::getIRQController();
	IRQ_Control.Add_Peripheral_IRQ_Listener(this);

	if(this->num == 0)
	{
		#ifdef CORE_M4
		prioritygroup = NVIC_GetPriorityGrouping();
		NVIC_SetPriority(USB0_IRQn, NVIC_EncodePriority(prioritygroup, USB0_Interrrupt_priority, 0));
		#endif
		#ifdef CORE_M0
		NVIC_SetPriority(USB0_IRQn,USB0_Interrrupt_priority);
		#endif
		NVIC_ClearPendingIRQ(USB0_IRQn);
		/* enable USB interrupts */
		NVIC_EnableIRQ(USB0_IRQn);
	}
	if(this->num == 1)
	{
		#ifdef CORE_M4
		prioritygroup = NVIC_GetPriorityGrouping();
		NVIC_SetPriority(USB1_IRQn, NVIC_EncodePriority(prioritygroup, USB1_Interrrupt_priority, 0));
		#endif
		#ifdef CORE_M0
		NVIC_SetPriority(USB1_IRQn,USB1_Interrrupt_priority);
		#endif
		NVIC_ClearPendingIRQ(USB1_IRQn);
		/* enable USB interrupts */
		NVIC_EnableIRQ(USB1_IRQn);
	}

	/* now connect */
	USB_Device_t::g_pUsbApi->hw->Connect(this->g_hUsb, 1);
	CONTROLLER_t::get_CONTROLLER()->Add_main_procedure(this);
}

