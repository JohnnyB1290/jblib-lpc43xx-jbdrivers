/*
 * Workspace_defines.h
 *
 *  Created on: 3 мая 2018 г.
 *      Author: Stalker1290
 */

#ifndef WORKSPACE_DEFINES_H_
#define WORKSPACE_DEFINES_H_


/*********************************ADRESSES************************************************************/

#define FLASH_A_BASE 0x1A000000
#define FLASH_B_BASE 0x1B000000
#define FLASH_PAGE_SIZE (512UL)

#define M4_APP_START_ADDR 0x1A040000
#define M4_APP_END_ADDR 0x1A080000
#define M4_APP_FIRST_SECT_NUM 11
#define M4_APP_LAST_SECT_NUM 14

#define M0_APP_START_ADDR 0x1B010000
#define M0_APP_END_ADDR 0x1B050000
#define M0_APP_FIRST_SECT_NUM 8
#define M0_APP_LAST_SECT_NUM 11

//Crypto_check
#define Signature_base 0x1A01FE00
#define Signature_sect_num 8
#define Signature_bank_num 0
//

#define MSC_IFLASH_DISK_BASE               	0x1A020000
#define MSC_IFLASH_DISK_SIZE               	((uint32_t) (128 * 1024))
#define MSC_IFLASH_DISK_BLOCK_SIZE         	FLASH_PAGE_SIZE
#define MSC_IFLASH_DISK_BLOCK_COUNT        	(MSC_IFLASH_DISK_SIZE / MSC_IFLASH_DISK_BLOCK_SIZE)
#define MSC_BUFF_SIZE						((uint32_t) (10*1024))

#define SHARED_MEM_M4          0x10007C00 //IPC Queue structs
#define SHARED_MEM_M0          0x10007E00

#ifdef CORE_M4

#define M0_APP_START_ADDR 0x1B010000

/* On LPC18xx/43xx the USB controller requires endpoint queue heads to start on
   a 4KB aligned memory. Hence the mem_base value passed to USB stack init should
   be 4KB aligned. The following manifest constants are used to define this memory.
 */
#define USB0_STACK_MEM_BASE     		0x20008000
#define USB0_STACK_MEM_SIZE      		0x00004000

#define USB1_STACK_MEM_BASE     		0x2000C000
#define USB1_STACK_MEM_SIZE      		0x00004000

#endif

/******************************************************************************************************/



#endif /* WORKSPACE_DEFINES_H_ */
