/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2016
/-----------------------------------------------------------------------*/

#ifndef _RAM_DEFINED
#define _RAM_DEFINED

#include "ff.h"
#include "diskio.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS ram_disk_initialize (void);
DSTATUS ram_disk_status (void);
DRESULT ram_disk_read (BYTE* buff, LBA_t sector, UINT count);
DRESULT ram_disk_write (const BYTE* buff, LBA_t sector, UINT count);
DRESULT ram_disk_ioctl (BYTE cmd, void* buff);
void ram_disk_timerproc (void);

#ifdef __cplusplus
}
#endif

#endif
