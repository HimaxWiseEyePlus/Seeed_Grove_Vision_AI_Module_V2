/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2019          /
/-----------------------------------------------------------------------*/

#include "ff.h"
#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

/* Command structure for iSDIO ioctl command */
typedef struct {
	BYTE	func;	/* Function number: 0..7 */
	WORD	ndata;	/* Number of bytes to transfer: 1..512, or mask + data */
	DWORD	addr;	/* Register address: 0..0x1FFFF */
	void*	data;	/* Pointer to the data (to be written | read buffer) */
} SDIO_CTRL;


/*---------------------------------------*/
/* Prototypes for disk control functions */
/*---------------------------------------*/


DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
void disk_timerproc (void);


/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

#if 0 //older 2014
/* Generic command (Not used by FatFs) */
#define CTRL_FORMAT			5	/* Create physical format on the media */
#define CTRL_POWER_IDLE		6	/* Put the device idle state */
#define CTRL_POWER_OFF		7	/* Put the device off state */
#define CTRL_LOCK			8	/* Lock media removal */
#define CTRL_UNLOCK			9	/* Unlock media removal */
#define CTRL_EJECT			10	/* Eject media */
#define CTRL_GET_SMART		11	/* Read SMART information */
#endif
/* Generic command (Not used by FatFs) */  //2019
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

#if 0 //older 2014
/* MMC/SDC specific command (Not used by FatFs) */
#define MMC_GET_TYPE		50	/* Get card type */
#define MMC_GET_CSD			51	/* Read CSD */
#define MMC_GET_CID			52	/* Read CID */
#define MMC_GET_OCR			53	/* Read OCR */
#define MMC_GET_SDSTAT		54	/* Read SD status */
#else
/* MMC/SDC specific ioctl command */ //2019
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#endif

#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

#if 0 //older 2014
/* ATA/CF specific command (Not used by FatFs) */
#define ATA_GET_REV			60	/* Get F/W revision */
#define ATA_GET_MODEL		61	/* Get model name */
#define ATA_GET_SN			62	/* Get serial number */
#else
/* ATA/CF specific ioctl command */ //2019
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */
#endif

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC3		0x01		/* MMC ver 3 */
#define CT_MMC4		0x02		/* MMC ver 4+ */
#define CT_MMC		0x03		/* MMC */
#define CT_SDC1		0x04		/* SD ver 1 */
#define CT_SDC2		0x08		/* SD ver 2+ */
#define CT_SDC		0x0C		/* SD */
#define CT_BLOCK	0x10		/* Block addressing */


#ifdef __cplusplus
}
#endif

#endif
