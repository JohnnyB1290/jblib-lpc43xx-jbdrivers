/*
 * MSC_Host.hpp
 *
 *  Created on: 7 сент. 2018 г.
 *      Author: Stalker1290
 */

#ifndef MSC_HOST_HPP_
#define MSC_HOST_HPP_

#include "FileSystem.hpp"
#include "USB.h"
#include "ctype.h"
#include "IRQ_Controller.hpp"

class MSC_Host_t:public Storage_Device_FS_t,protected IRQ_LISTENER_t,public Callback_Interface_t{
public:
	static MSC_Host_t* getMSCHost(uint8_t portNumber);
	USB_ClassInfo_MS_Host_t* getFlashDiskMSInterfacePtr(void);
	void setDiscStatPtr(DSTATUS Disc_Stat);
	void initialize(void);
	void deInitialize(void);
	virtual DSTATUS disk_initialize(void);
	virtual DSTATUS disk_status(void);
	virtual DRESULT disk_read(BYTE* buff, DWORD sector, UINT count);
	virtual DRESULT disk_write(const BYTE* buff, DWORD sector, UINT count);
	virtual DRESULT disk_ioctl(BYTE cmd, void* buff);
private:
	MSC_Host_t(uint8_t portNumber);
	virtual void IRQ(int8_t IRQ_num);
	virtual void void_callback(void* Intf_ptr, void* parameters);
	bool FSUSB_DiskAcquire(void);

	static MSC_Host_t* mscHost_ptr[LPC18_43_MAX_USB_CORE];
	USB_ClassInfo_MS_Host_t flashDiskMSInterface;

	DSTATUS Disc_Stat;
	SCSI_Capacity_t DiskCapacity;
};



#endif /* LPC43XX_JB_DRIVERS_INCLUDES_USB_USB_NXPUSBLIB_MSC_HOST_HPP_ */
