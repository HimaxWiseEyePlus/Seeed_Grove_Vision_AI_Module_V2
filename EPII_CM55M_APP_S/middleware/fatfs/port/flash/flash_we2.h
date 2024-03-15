/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2016
/-----------------------------------------------------------------------*/

#ifndef _FLASH_DEFINED
#define _FLASH_DEFINED

#include "ff.h"
#include "diskio.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS flash_disk_initialize (void);
DSTATUS flash_disk_status (void);
DRESULT flash_disk_read (BYTE* buff, LBA_t sector, UINT count);
DRESULT flash_disk_write (const BYTE* buff, LBA_t sector, UINT count);
DRESULT flash_disk_ioctl (BYTE cmd, void* buff);
void flash_disk_timerproc (void);

#ifdef __cplusplus
}
#endif

#endif
