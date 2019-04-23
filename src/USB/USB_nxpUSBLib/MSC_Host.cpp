/*
 * MSC_Host.cpp
 *
 *  Created on: 7 ����. 2018 �.
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef USE_CONSOLE
#ifdef USB_console
#include "stdio.h"
#endif
#endif

#include "USB/USB_nxpUSBLib/MSC_Host.hpp"
#include "CONTROLLER.hpp"

MSC_Host_t* MSC_Host_t::mscHost_ptr[LPC18_43_MAX_USB_CORE] = {NULL, NULL};


MSC_Host_t* MSC_Host_t::getMSCHost(uint8_t portNumber){

	if(portNumber >= LPC18_43_MAX_USB_CORE) return NULL;
	if(MSC_Host_t::mscHost_ptr[portNumber] == NULL)
		MSC_Host_t::mscHost_ptr[portNumber] = new MSC_Host_t(portNumber);
	return MSC_Host_t::mscHost_ptr[portNumber];
}

MSC_Host_t::MSC_Host_t(uint8_t portNumber){

	this->flashDiskMSInterface.Config.DataINPipeNumber = 1;
	this->flashDiskMSInterface.Config.DataINPipeDoubleBank = false;

	this->flashDiskMSInterface.Config.DataOUTPipeNumber = 2;
	this->flashDiskMSInterface.Config.DataOUTPipeDoubleBank = false;

	this->flashDiskMSInterface.Config.PortNumber = portNumber;

	this->Disc_Stat = STA_NOINIT;
}

USB_ClassInfo_MS_Host_t* MSC_Host_t::getFlashDiskMSInterfacePtr(void){
	return &this->flashDiskMSInterface;
}

void MSC_Host_t::initialize(void){

	switch(this->flashDiskMSInterface.Config.PortNumber){
		case 0:
			Chip_USB0_Init();
			this->setCode((uint64_t)1<<USB0_IRQn);
			break;

		case 1:
			Chip_USB1_Init();
			this->setCode((uint64_t)1<<USB1_IRQn);
			break;

		default:
			return;
	}

	USB_Init(this->flashDiskMSInterface.Config.PortNumber, USB_MODE_Host);
	IRQ_CONTROLLER_t::getIRQController().Add_Peripheral_IRQ_Listener(this);
	CONTROLLER_t::get_CONTROLLER()->Add_main_procedure(this);
}

void MSC_Host_t::deInitialize(void){

	CONTROLLER_t::get_CONTROLLER()->Delete_main_procedure(this);
	IRQ_CONTROLLER_t::getIRQController().Delete_Peripheral_IRQ_Listener(this);
	USB_Disable(this->flashDiskMSInterface.Config.PortNumber, USB_MODE_Host);
}

void MSC_Host_t::IRQ(int8_t IRQ_num){
	if(this->flashDiskMSInterface.Config.PortNumber == 0) USBLib_USB0_IRQHandler();
	if(this->flashDiskMSInterface.Config.PortNumber == 1) USBLib_USB1_IRQHandler();
}

void MSC_Host_t::void_callback(void* Intf_ptr, void* parameters){
	MS_Host_USBTask(&this->flashDiskMSInterface);
	USB_USBTask(this->flashDiskMSInterface.Config.PortNumber, USB_MODE_Host);
}

#ifdef __cplusplus
extern "C" {
#endif
/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum){

#ifdef USE_CONSOLE
#ifdef USB_console
	printf(("Device Attached on port %d\r\n"), corenum);
#endif
#endif
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum){

	MSC_Host_t::getMSCHost(corenum)->setDiscStatPtr(STA_NOINIT);

#ifdef USE_CONSOLE
#ifdef USB_console
	printf(("\r\nDevice Unattached on port %d\r\n"), corenum);
#endif
#endif
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum){

	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];
	USB_ClassInfo_MS_Host_t* flashDiskMSInterfacePtr = MSC_Host_t::getMSCHost(corenum)->getFlashDiskMSInterfacePtr();

	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
			sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error Retrieving Configuration Descriptor.\r\n");
	#endif
	#endif
		return;
	}

	if (MS_Host_ConfigurePipes(flashDiskMSInterfacePtr, ConfigDescriptorSize,
			ConfigDescriptorData) != MS_ENUMERROR_NoError) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Attached Device Not a Valid Mass Storage Device.\r\n");
	#endif
	#endif
		return;
	}

	if (USB_Host_SetDeviceConfiguration(corenum, 1) != HOST_SENDCONTROL_Successful) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error Setting Device Configuration.\r\n");
	#endif
	#endif
		return;
	}

	uint8_t MaxLUNIndex;
	if (MS_Host_GetMaxLUN(flashDiskMSInterfacePtr, &MaxLUNIndex)) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error retrieving max LUN index.\r\n");
	#endif
	#endif
		USB_Host_SetDeviceConfiguration(corenum, 0);
		return;
	}

#ifdef USE_CONSOLE
#ifdef USB_console
	printf(("Total LUNs: %d - Using first LUN in device.\r\n"), (MaxLUNIndex + 1));
#endif
#endif

	if (MS_Host_ResetMSInterface(flashDiskMSInterfacePtr)) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error resetting Mass Storage interface.\r\n");
	#endif
	#endif
		USB_Host_SetDeviceConfiguration(corenum, 0);
		return;
	}

	SCSI_Request_Sense_Response_t SenseData;
	if (MS_Host_RequestSense(flashDiskMSInterfacePtr, 0, &SenseData) != 0) {
	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error retrieving device sense.\r\n");
	#endif
	#endif
		USB_Host_SetDeviceConfiguration(corenum, 0);
		return;
	}

	SCSI_Inquiry_Response_t InquiryData;
	if (MS_Host_GetInquiryData(flashDiskMSInterfacePtr, 0, &InquiryData)) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error retrieving device Inquiry data.\r\n");
	#endif
	#endif
		USB_Host_SetDeviceConfiguration(corenum, 0);
		return;
	}

#ifdef USE_CONSOLE
#ifdef USB_console
	printf("Vendor \"%.8s\", Product \"%.16s\", RevisionID \"%.4s\" , Version \"%i\"\r\n",
			InquiryData.VendorID, InquiryData.ProductID, InquiryData.RevisionID, InquiryData.Version);
	printf("Mass Storage Device Enumerated.\r\n");
#endif
#endif
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode){

	MSC_Host_t::getMSCHost(corenum)->deInitialize();
#ifdef USE_CONSOLE
#ifdef USB_console
	printf(("Host Mode Error\r\n -- Error port %d\r\n -- Error Code %d\r\n" ), corenum, ErrorCode);
#endif
#endif
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
		const uint8_t ErrorCode, const uint8_t SubErrorCode){

#ifdef USE_CONSOLE
#ifdef USB_console
	printf(("Dev Enum Error\r\n -- Error port %d\r\n -- Error Code %d\r\n"
			" -- Sub Error Code %d\r\n -- In State %d\r\n" ),
			corenum, ErrorCode, SubErrorCode, USB_HostState[corenum]);
#endif
#endif
}

#ifdef __cplusplus
}
#endif

void MSC_Host_t::setDiscStatPtr(DSTATUS Disc_Stat){
	this->Disc_Stat = Disc_Stat;
}

bool MSC_Host_t::FSUSB_DiskAcquire(void){

#ifdef USE_CONSOLE
#ifdef USB_console
	printf("Waiting for ready...");
#endif
#endif

	while(1){
		uint8_t ErrorCode = MS_Host_TestUnitReady(&this->flashDiskMSInterface, 0);
		if(!(ErrorCode)) break;

		/* Check if an error other than a logical command error (device busy) received */
		if (ErrorCode != MS_ERROR_LOGICAL_CMD_FAILED) {

		#ifdef USE_CONSOLE
		#ifdef USB_console
			printf("Failed\r\n");
		#endif
		#endif
			USB_Host_SetDeviceConfiguration(this->flashDiskMSInterface.Config.PortNumber, 0);
			return false;
		}
	}

#ifdef USE_CONSOLE
#ifdef USB_console
	printf("Done.\r\n");
#endif
#endif

	if (MS_Host_ReadDeviceCapacity(&this->flashDiskMSInterface, 0, &this->DiskCapacity)) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error retrieving device capacity.\r\n");
	#endif
	#endif
		USB_Host_SetDeviceConfiguration(this->flashDiskMSInterface.Config.PortNumber, 0);
		return false;
	}

#ifdef USE_CONSOLE
#ifdef USB_console
	printf(("%lu blocks of %lu bytes.\r\n"), this->DiskCapacity.Blocks, this->DiskCapacity.BlockSize);
#endif
#endif

	return true;
}

DSTATUS MSC_Host_t::disk_initialize(void){

	if (this->Disc_Stat != STA_NOINIT) return this->Disc_Stat;

	if(USB_HostState[this->flashDiskMSInterface.Config.PortNumber] != HOST_STATE_Configured){
		this->Disc_Stat = STA_NOINIT;
		return this->Disc_Stat;
	}

	/* Enumerate the card once detected. Note this function may block for a little while. */
	if (!this->FSUSB_DiskAcquire()) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Disk Enumeration failed...\r\n");
	#endif
	#endif
		this->Disc_Stat = STA_NOINIT;
	}
	else this->Disc_Stat &= ~STA_NOINIT;

	return this->Disc_Stat;
}

DSTATUS MSC_Host_t::disk_status(void){
	return this->Disc_Stat;
}

DRESULT MSC_Host_t::disk_read(BYTE* buff, DWORD sector, UINT count){

	if (!count) return RES_PARERR;
	if (this->Disc_Stat & STA_NOINIT) return RES_NOTRDY;


	if (MS_Host_ReadDeviceBlocks(&this->flashDiskMSInterface, 0,
			sector, count, this->DiskCapacity.BlockSize, buff)) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error reading device block.\r\n");
	#endif
	#endif
		USB_Host_SetDeviceConfiguration(this->flashDiskMSInterface.Config.PortNumber, 0);
		return RES_ERROR;
	}
	return RES_OK;
}

DRESULT MSC_Host_t::disk_write(const BYTE* buff, DWORD sector, UINT count){

	if (!count) return RES_PARERR;

	if (this->Disc_Stat & STA_NOINIT) return RES_NOTRDY;

	if (MS_Host_WriteDeviceBlocks(&this->flashDiskMSInterface, 0,
			sector, count, this->DiskCapacity.BlockSize, buff)) {

	#ifdef USE_CONSOLE
	#ifdef USB_console
		printf("Error writing device block.\r\n");
	#endif
	#endif
		return RES_ERROR;
	}
	return RES_OK;
}

DRESULT MSC_Host_t::disk_ioctl(BYTE cmd, void* buff){

	DRESULT res;

	if(this->Disc_Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC:	/* Make sure that no pending write process */
		CONTROLLER_t::get_CONTROLLER()->_delay_us(150);
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT:	/* Get number of sectors on the disk (DWORD) */
		*(DWORD *) buff = this->DiskCapacity.Blocks;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:	/* Get R/W sector size (WORD) */
		*(WORD *) buff = this->DiskCapacity.BlockSize;
		res = RES_OK;
		break;

	/*Returns erase block size of the flash memory media in unit of sector into the DWORD variable pointed by buff.
	 The allowable value is 1 to 32768 in power of 2.
	 Return 1 if the erase block size is unknown or non flash memory media.
	 This command is used by only f_mkfs function and it attempts to align data area on the erase block boundary.
	 Required at FF_USE_MKFS == 1.*/
	case GET_BLOCK_SIZE:/* Get erase block size in unit of sector (DWORD) */
		/* Erase block size fixed to 4K */
		//*(DWORD *) buff = (4 * 1024);
		/* Erase block size fixed to 128 sectors(512b) */
		*(DWORD *) buff = 128;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
		break;
	}

	return res;
}
