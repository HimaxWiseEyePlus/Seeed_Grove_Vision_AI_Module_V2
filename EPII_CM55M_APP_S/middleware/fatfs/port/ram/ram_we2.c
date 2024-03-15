/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "WE2_device.h"
#include "board.h"
#include "ff.h"            /* Obtains integer types */
#include "diskio.h"        /* Declarations of disk functions */
#include "ram_we2.h"

#define RAMBASE        (BASE_ADDR_SRAM2_ALIAS)
#define RAMSIZE        (SRAM2_SIZE)
#define ERASE_VALUE    (0xFF)
#define SECTOR_SIZE    (FF_MAX_SS)
#define SECTOR_COUNT   (RAMSIZE / SECTOR_SIZE)

static volatile DSTATUS Stat = STA_NOINIT;    /* Physical drive status */
static uint32_t rambase = RAMBASE;

//#define TRACE_PRINTF(fmt, ...)  printf("%s:%s:%d " fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define TRACE_PRINTF(fmt, ...)  printf("%s " fmt, __func__, ##__VA_ARGS__)
//#define TRACE_PRINTF(fmt, ...)  printf(fmt, ##__VA_ARGS__)
//#define TRACE_PRINTF(fmt, ...)

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS ram_disk_initialize (void)
{
    TRACE_PRINTF("\r\n");

    if (!(Stat & STA_NOINIT))
        return Stat;

    assert((SECTOR_COUNT * SECTOR_SIZE) == RAMSIZE);

    if (Stat == STA_NOINIT)
        memset((void *)rambase, ERASE_VALUE, RAMSIZE);

    Stat &= ~STA_NOINIT;    /* Clear STA_NOINIT flag */
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS ram_disk_status(void)
{
    TRACE_PRINTF("%d\r\n", Stat);
    return Stat;    /* Return disk status */
}

/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT ram_disk_read (
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count            /* Sector count (1..128) */
)
{
    TRACE_PRINTF("0x%x %d %d\r\n", (uint32_t)buff, sector, count);

    if (!count)
        return RES_PARERR;        /* Check parameter */

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;    /* Check if drive is ready */

    for (; count > 0; count--) {
        memcpy(buff, (void *)(rambase + sector * SECTOR_SIZE), SECTOR_SIZE);
        sector++;
        buff += SECTOR_SIZE;
    }

    return RES_OK;    /* Return result */
}

/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT ram_disk_write (
    const BYTE *buff,    /* Pointer to the data to be written */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count            /* Sector count (1..128) */
)
{
    TRACE_PRINTF("0x%x %d %d\r\n", (uint32_t)buff, sector, count);

    if (!count)
        return RES_PARERR;        /* Check parameter */

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;    /* Check if drive is ready */

    for (; count > 0; count--) {
        memcpy((void *)(rambase + sector * SECTOR_SIZE), buff, SECTOR_SIZE);
        sector++;
        buff += SECTOR_SIZE;
    }

    return RES_OK;    /* Return result */
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/
DRESULT ram_disk_ioctl (
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    TRACE_PRINTF("%d 0x%x\r\n", cmd, (uint32_t)buff);

    DRESULT res;
    LBA_t *dp;
    LBA_t sector;

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;    /* Check if drive is ready */

    res = RES_ERROR;

    switch (cmd) {
    case CTRL_SYNC :        /* Wait for end of internal write process of the drive */
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT :    /* Get drive capacity in unit of sector (DWORD) */
        *(DWORD*)buff = SECTOR_COUNT;
        res = RES_OK;
        break;

    case GET_BLOCK_SIZE :    /* Get erase block size in unit of sector (DWORD) */
        *(WORD*)buff = 1;    // block size = 1 sector 
        res = RES_OK;
        break;

    case GET_SECTOR_SIZE :   /* Get sector size in unit of byte (WORD) (needed at FF_MAX_SS != FF_MIN_SS) */
        *(WORD*)buff = SECTOR_SIZE;
        res = RES_OK;
        break;

#if FF_USE_TRIM
    case CTRL_TRIM :    /* Erase a block of sectors (used when _USE_TRIM == 1) */
        dp = buff;
        for (sector = (DWORD)dp[0]; sector > (DWORD)dp[1]; sector++) {
            memset((void *)(rambase + sector * SECTOR_SIZE), ERASE_VALUE, SECTOR_SIZE);
        }

        break;
#endif

    default:
        res = RES_PARERR;
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Device timer function                                                 */
/*-----------------------------------------------------------------------*/
/* This function must be called from timer interrupt routine in period
/  of 1 ms to generate card control timing.
*/
void ram_disk_timerproc (void)
{
    TRACE_PRINTF("\r\n");
}
