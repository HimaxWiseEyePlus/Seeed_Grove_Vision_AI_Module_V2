/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "mmc_we2.h"

/* Definitions of physical drive number for each drive */
#if FF_VOLUMES == 1
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 */
#elif FF_VOLUMES == 2
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 1 */
#else
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
#if defined(DEV_RAM)
	case DEV_RAM :
		return ram_disk_status();
#endif
#if defined(DEV_MMC)
	case DEV_MMC :
		return mmc_disk_status();
#endif
#if defined(DEV_USB)
	case DEV_USB :
		return usb_disk_status();
#endif
	}

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
#if defined(DEV_RAM)
	case DEV_RAM :
		return RAM_MAX();
#endif
#if defined(DEV_MMC)
	case DEV_MMC :
		return mmc_disk_initialize();
#endif
#if defined(DEV_USB)
	case DEV_USB :
		return usb_disk_initialize();
#endif
	}

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	switch (pdrv) {
#if defined(DEV_RAM)
	case DEV_RAM :
		return ram_disk_read(buff, sector, count);
#endif
#if defined(DEV_MMC)
	case DEV_MMC :
		return mmc_disk_read(buff, sector, count);
#endif
#if defined(DEV_USB)
	case DEV_USB :
		return usb_disk_read(buff, sector, count);
#endif
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	switch (pdrv) {
#if defined(DEV_RAM)
	case DEV_RAM :
		return ram_disk_write(buff, sector, count);
#endif
#if defined(DEV_MMC)
	case DEV_MMC :
		return mmc_disk_write(buff, sector, count);
#endif
#if defined(DEV_USB)
	case DEV_USB :
		return usb_disk_write(buff, sector, count);
#endif
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
#if defined(DEV_RAM)
	case DEV_RAM :
		return ram_disk_ioctl(cmd, buff);
#endif
#if defined(DEV_MMC)
	case DEV_MMC :
		return mmc_disk_ioctl(cmd, buff);
#endif
#if defined(DEV_USB)
	case DEV_USB :
		return usb_disk_ioctl(cmd, buff);
#endif
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Timer driven procedure                                                */
/*-----------------------------------------------------------------------*/
void disk_timerproc (void)
{
#if defined(DEV_MMC)
	mmc_disk_timerproc();
#endif
}
