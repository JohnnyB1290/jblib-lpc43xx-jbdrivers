/*
 * USB_Descriptors.c
 *
 *  Created on: 13.03.2017
 *      Author: Stalker1290
 */
#include "Defines.h"
#include "USB/USB_Device_use_rom/USB_Device.hpp"

/**
 * USB Standard Device Descriptor. In this example we will overwrite
 * this descriptor in DFU_AppDetach state so this array has to be in
 * RW area hence no const keyword.
 */
ALIGNED(4) uint8_t USB_Device_t::USB0_DeviceDescriptor[] = {
	USB_DEVICE_DESC_SIZE,				/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0200),						/* bcdUSB: 2.00 */
	0x00,								/* bDeviceClass */
	0x00,								/* bDeviceSubClass */
	0x00,								/* bDeviceProtocol */
	USB_MAX_PACKET0,					/* bMaxPacketSize0 */
	WBVAL(0x1FC9),						/* idVendor */
	WBVAL(0x1290),						/* idProduct */
	WBVAL(0x0100),						/* bcdDevice: 1.00 */
	0x01,								/* iManufacturer */
	0x02,								/* iProduct */
	0x03,								/* iSerialNumber */
	0x01								/* bNumConfigurations */
};

/** USB Device Qualifier Descriptor */
ALIGNED(4) uint8_t USB_Device_t::USB0_DeviceQualifier[] = {
	USB_DEVICE_QUALI_SIZE,				/* bLength */
	USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(0x0200),						/* bcdUSB: 2.00 */
	0x00,								/* bDeviceClass */
	0x00,								/* bDeviceSubClass */
	0x00,								/* bDeviceProtocol */
	USB_MAX_PACKET0,					/* bMaxPacketSize0 */
	0x01,								/* bNumOtherSpeedConfigurations */
	0x00								/* bReserved */
};

/**
 * USB FSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_Device_t::USB0_FsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		1 * USB_CONFIGURATION_DESC_SIZE     +

		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */ //RNDIS over CDC
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */

		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */ //Virtual COM over CDC
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */
		0),
	0x04,
	0x01,								/* bConfigurationValue */
	0x00,								/* iConfiguration */
	USB_CONFIG_SELF_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(500),			/* bMaxPower */
																		//RNDIS
	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB0_RNDIS_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	0xEF,									/* bFunctionClass */
	0x04,								/* bFunctionSubClass */
	0x01,									/* bFunctionProtocol */
	0x04,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 0, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	0xEF,								/* bInterfaceClass: Miscellaneous  */
	0x04,								/* bInterfaceSubClass: RNDIS */
	0x01,								/* bInterfaceProtocol: RNDIS */
	0x04,								/* iInterface: */

	/* Header Functional Descriptor*/
	0x05,								/* bLength: CDC header Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),					/* bcdCDC 1.10 */

	/* Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x00,								/* bmCapabilities: device handles call management */
	USB0_RNDIS_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/* Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x00,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/* Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB0_RNDIS_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB0_RNDIS_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/* Endpoint 1 Descriptor*/
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_INT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x02,			/* 2ms */           /* bInterval */

	/* Interface 1, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x04,								/* iInterface: */

	/* Endpoint, EP Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, EP Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB0_VCOM_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bFunctionSubClass */
	0,									/* bFunctionProtocol */
	0x05,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 1, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bInterfaceClass: Communication Interface Class */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bInterfaceSubClass: Abstract Control Model */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/*Header Functional Descriptor*/
	0x05,								/* bLength: Endpoint Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),	/* 1.10 */       /* bcdCDC */

	/*Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x01,								/* bmCapabilities: device handles call management */
	USB0_VCOM_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/*Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x02,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/*Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB0_VCOM_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB0_VCOM_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/*Endpoint 1 Descriptor*/          /* event notification (optional) */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_INT_EP,					/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x02,			/* 2ms */           /* bInterval */

	/* Interface 2, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/* Endpoint, CDC Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, CDC Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Interface 3 Alternate Setting 0, MSC Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_MSC_IF_NUM,						/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x02,								/* bNumEndpoints */
	USB_DEVICE_CLASS_STORAGE,			/* bInterfaceClass */
	MSC_SUBCLASS_SCSI,					/* bInterfaceSubClass */
	MSC_PROTOCOL_BULK_ONLY,				/* bInterfaceProtocol */
	0x06,								/* iInterface */
	/* Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_MSC_IN_EP,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */
	/* Bulk Out Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_MSC_OUT_EP,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */
	/* Terminator */
	0									/* bLength */
};

/**
 * USB HSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_Device_t::USB0_HsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		1 * USB_CONFIGURATION_DESC_SIZE     +

		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */ //RNDIS over CDC
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */

		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */ //Virtual COM over CDC
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */
		0
		),
	0x04,
	0x01,								/* bConfigurationValue */
	0x00,								/* iConfiguration */
	USB_CONFIG_SELF_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(500),			/* bMaxPower */
																			//RNDIS
	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB0_RNDIS_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	0xEF,									/* bFunctionClass */
	0x04,								/* bFunctionSubClass */
	0x01,									/* bFunctionProtocol */
	0x04,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 0, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	0xEF,								/* bInterfaceClass: Miscellaneous  */
	0x04,								/* bInterfaceSubClass: RNDIS */
	0x01,								/* bInterfaceProtocol: RNDIS */
	0x04,								/* iInterface: */

	/* Header Functional Descriptor*/
	0x05,								/* bLength: CDC header Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),					/* bcdCDC 1.10 */

	/* Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x00,								/* bmCapabilities: device handles call management */
	USB0_RNDIS_DIF_NUM,					/* bDataInterface: CDC data IF ID */
	/* Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x00,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */
	/* Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB0_RNDIS_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB0_RNDIS_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/* Endpoint 1 Descriptor*/
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_INT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x10,			/* 2ms */           /* bInterval */

	/* Interface 1, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x04,								/* iInterface: */
	/* Endpoint, EP Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */
	/* Endpoint, EP Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_RNDIS_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),							/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB0_VCOM_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bFunctionSubClass */
	0,									/* bFunctionProtocol */
	0x05,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 1, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bInterfaceClass: Communication Interface Class */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bInterfaceSubClass: Abstract Control Model */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/*Header Functional Descriptor*/
	0x05,								/* bLength: Endpoint Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),	/* 1.10 */       /* bcdCDC */

	/*Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x01,								/* bmCapabilities: device handles call management */
	USB0_VCOM_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/*Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x02,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/*Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB0_VCOM_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB0_VCOM_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/*Endpoint 1 Descriptor*/          /* event notification (optional) */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_INT_EP,					/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x10,			/* 2ms */           /* bInterval */

	/* Interface 2, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/* Endpoint, CDC Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, CDC Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_VCOM_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Interface 3 Alternate Setting 0, MSC Class */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_MSC_IF_NUM,						/* bInterfaceNumber */
	0x00,								/* bAlternateSetting */
	0x02,								/* bNumEndpoints */
	USB_DEVICE_CLASS_STORAGE,			/* bInterfaceClass */
	MSC_SUBCLASS_SCSI,					/* bInterfaceSubClass */
	MSC_PROTOCOL_BULK_ONLY,				/* bInterfaceProtocol */
	0x06,								/* iInterface */

	/* Bulk In Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_MSC_IN_EP,							/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */

	/* Bulk Out Endpoint */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB0_MSC_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_HS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0,									/* bInterval */
	/* Terminator */
	0									/* bLength */
};

/**
 * USB String Descriptor (optional)
 */
uint8_t USB_Device_t::USB0_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04,								/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0409),						/* wLANGID  0x0409 = US English*/
	/* Index 0x01: Manufacturer */
	(12 * 2 + 2),						/* bLength (12 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'J', 0,
	'o', 0,
	'h', 0,
	'n', 0,
	'n', 0,
	'y', 0,
	' ', 0,
	'B', 0,
	'r', 0,
	'a', 0,
	'v', 0,
	'o', 0,
	/* Index 0x02: Product */
	(9 * 2 + 2),						/* bLength (9 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'K', 0,
	'T', 0,
	'P', 0,
	' ', 0,
	'M', 0,
	'o', 0,
	'd', 0,
	'e', 0,
	'm', 0,
	/* Index 0x03: Serial Number */
	(8 * 2 + 2),						/* bLength (8 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'1', 0,
	/* Index 0x04: Interface 0, Alternate Setting 0 */
	(18 * 2 + 2),						/* bLength (18 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'J', 0,
	'o', 0,
	'h', 0,
	'n', 0,
	'n', 0,
	'y', 0,
	' ', 0,
	'B', 0,
	'r', 0,
	'a', 0,
	'v', 0,
	'o', 0,
	' ', 0,
	'R', 0,
	'N', 0,
	'D', 0,
	'I', 0,
	'S', 0,
	/* Index 0x05 : Interface 1, Alternate Setting 0 */
	(24 * 2 + 2),						/* bLength (24 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'J', 0,
	'o', 0,
	'h', 0,
	'n', 0,
	'n', 0,
	'y', 0,
	' ', 0,
	'B', 0,
	'r', 0,
	'a', 0,
	'v', 0,
	'o', 0,
	' ', 0,
	'V', 0,
	'i', 0,
	'r', 0,
	't', 0,
	'u', 0,
	'a', 0,
	'l', 0,
	' ', 0,
	'C', 0,
	'O', 0,
	'M', 0,
#ifdef MCS_BOOT
	/* Index 0x06: Interface 1, Alternate Setting 0 */
	(6 * 2 + 2),						/* bLength (6 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'M', 0,
	'e', 0,
	'm', 0,
	'o', 0,
	'r', 0,
	'y', 0,
#endif
};



/**
 * USB Standard Device Descriptor. In this example we will overwrite
 * this descriptor in DFU_AppDetach state so this array has to be in
 * RW area hence no const keyword.
 */
ALIGNED(4) uint8_t USB_Device_t::USB1_DeviceDescriptor[] = {
	USB_DEVICE_DESC_SIZE,				/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0200),						/* bcdUSB: 2.00 */
	0x00,								/* bDeviceClass */
	0x00,								/* bDeviceSubClass */
	0x00,								/* bDeviceProtocol */
	USB_MAX_PACKET0,					/* bMaxPacketSize0 */
	WBVAL(0x1FC9),						/* idVendor */
	WBVAL(0x1290),						/* idProduct */
	WBVAL(0x0100),						/* bcdDevice: 1.00 */
	0x01,								/* iManufacturer */
	0x02,								/* iProduct */
	0x03,								/* iSerialNumber */
	0x01								/* bNumConfigurations */
};

/**
 * USB FSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_Device_t::USB1_FsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE,		/* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(								/* wTotalLength */
		1 * USB_CONFIGURATION_DESC_SIZE     +

		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */ //RNDIS over CDC
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */

		1 * USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association */ //Virtual COM over CDC
		1 * USB_INTERFACE_DESC_SIZE         +	/* communication interface */
		0x0013                              +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE          +	/* interrupt endpoint */
		1 * USB_INTERFACE_DESC_SIZE         +	/* data interface */
		2 * USB_ENDPOINT_DESC_SIZE          +	/* bulk endpoints */

		0),
	0x04,								/* bNumInterfaces */
	0x01,								/* bConfigurationValue */
	0x00,								/* iConfiguration */
	USB_CONFIG_SELF_POWERED,			/* bmAttributes */
	USB_CONFIG_POWER_MA(500),			/* bMaxPower */
																		//RNDIS
	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB1_RNDIS_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	0xEF,									/* bFunctionClass */
	0x04,								/* bFunctionSubClass */
	0x01,									/* bFunctionProtocol */
	0x04,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 0, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_RNDIS_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	0xEF,								/* bInterfaceClass: Miscellaneous  */
	0x04,								/* bInterfaceSubClass: RNDIS */
	0x01,								/* bInterfaceProtocol: RNDIS */
	0x04,								/* iInterface: */

	/* Header Functional Descriptor*/
	0x05,								/* bLength: CDC header Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),					/* bcdCDC 1.10 */

	/* Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x00,								/* bmCapabilities: device handles call management */
	USB1_RNDIS_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/* Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x00,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/* Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB1_RNDIS_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB1_RNDIS_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/* Endpoint 1 Descriptor*/
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_RNDIS_INT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x02,			/* 2ms */           /* bInterval */

	/* Interface 1, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_RNDIS_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x04,								/* iInterface: */

	/* Endpoint, EP Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_RNDIS_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, EP Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_RNDIS_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOC_DESC_SIZE,		/* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	/* bDescriptorType */
	USB1_VCOM_CIF_NUM,					/* bFirstInterface */
	2,									/* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bFunctionSubClass */
	0,									/* bFunctionProtocol */
	0x05,								/* iFunction (Index of string descriptor describing this function) */

	/* Interface 1, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_VCOM_CIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: Alternate setting */
	0x01,								/* bNumEndpoints: One endpoint used */
	CDC_COMMUNICATION_INTERFACE_CLASS,	/* bInterfaceClass: Communication Interface Class */
	CDC_ABSTRACT_CONTROL_MODEL,			/* bInterfaceSubClass: Abstract Control Model */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/*Header Functional Descriptor*/
	0x05,								/* bLength: Endpoint Descriptor size */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_HEADER,							/* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10),	/* 1.10 */       /* bcdCDC */

	/*Call Management Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT,				/* bDescriptorSubtype: Call Management Func Desc */
	0x01,								/* bmCapabilities: device handles call management */
	USB1_VCOM_DIF_NUM,					/* bDataInterface: CDC data IF ID */

	/*Abstract Control Management Functional Descriptor*/
	0x04,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT,	/* bDescriptorSubtype: Abstract Control Management desc */
	0x02,								/* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */

	/*Union Functional Descriptor*/
	0x05,								/* bFunctionLength */
	CDC_CS_INTERFACE,					/* bDescriptorType: CS_INTERFACE */
	CDC_UNION,							/* bDescriptorSubtype: Union func desc */
	USB1_VCOM_CIF_NUM,					/* bMasterInterface: Communication class interface is master */
	USB1_VCOM_DIF_NUM,					/* bSlaveInterface0: Data class interface is slave 0 */

	/*Endpoint 1 Descriptor*/          /* event notification (optional) */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_VCOM_INT_EP,					/* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
	WBVAL(0x0010),						/* wMaxPacketSize */
	0x02,			/* 2ms */           /* bInterval */

	/* Interface 2, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE,			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_VCOM_DIF_NUM,					/* bInterfaceNumber: Number of Interface */
	0x00,								/* bAlternateSetting: no alternate setting */
	0x02,								/* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS,			/* bInterfaceClass: Data Interface Class */
	0x00,								/* bInterfaceSubClass: no subclass available */
	0x00,								/* bInterfaceProtocol: no protocol used */
	0x05,								/* iInterface: */

	/* Endpoint, CDC Bulk Out */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_VCOM_OUT_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Endpoint, CDC Bulk In */
	USB_ENDPOINT_DESC_SIZE,				/* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
	USB1_VCOM_IN_EP,						/* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK,				/* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET),		/* wMaxPacketSize */
	0x00,								/* bInterval: ignore for Bulk transfer */

	/* Terminator */
	0									/* bLength */
};

/**
 * USB String Descriptor (optional)
 */
uint8_t USB_Device_t::USB1_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04,								/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	WBVAL(0x0409),						/* wLANGID  0x0409 = US English*/
	/* Index 0x01: Manufacturer */
	(12 * 2 + 2),						/* bLength (12 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'J', 0,
	'o', 0,
	'h', 0,
	'n', 0,
	'n', 0,
	'y', 0,
	' ', 0,
	'B', 0,
	'r', 0,
	'a', 0,
	'v', 0,
	'o', 0,
	/* Index 0x02: Product */
	(9 * 2 + 2),						/* bLength (9 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'K', 0,
	'T', 0,
	'P', 0,
	' ', 0,
	'M', 0,
	'o', 0,
	'd', 0,
	'e', 0,
	'm', 0,
	/* Index 0x03: Serial Number */
	(8 * 2 + 2),						/* bLength (8 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'1', 0,
	/* Index 0x04: Interface 0, Alternate Setting 0 */
	(18 * 2 + 2),						/* bLength (18 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'J', 0,
	'o', 0,
	'h', 0,
	'n', 0,
	'n', 0,
	'y', 0,
	' ', 0,
	'B', 0,
	'r', 0,
	'a', 0,
	'v', 0,
	'o', 0,
	' ', 0,
	'R', 0,
	'N', 0,
	'D', 0,
	'I', 0,
	'S', 0,
	/* Index 0x05 : Interface 1, Alternate Setting 0 */
	(24 * 2 + 2),						/* bLength (24 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE,			/* bDescriptorType */
	'J', 0,
	'o', 0,
	'h', 0,
	'n', 0,
	'n', 0,
	'y', 0,
	' ', 0,
	'B', 0,
	'r', 0,
	'a', 0,
	'v', 0,
	'o', 0,
	' ', 0,
	'V', 0,
	'i', 0,
	'r', 0,
	't', 0,
	'u', 0,
	'a', 0,
	'l', 0,
	' ', 0,
	'C', 0,
	'O', 0,
	'M', 0,
};

