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
#include "Driver_Flash.h"
#include "ff.h"            /* Obtains integer types */
#include "diskio.h"        /* Declarations of disk functions */
#include "flash_we2.h"

#define ASSERT_HIGH(X)  assert(X == ARM_DRIVER_OK)

extern ARM_DRIVER_FLASH Driver_Flash0;
static volatile DSTATUS Stat = STA_NOINIT;    /* Physical drive status */
static uint32_t sector_count;
static uint32_t sector_size;

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
DSTATUS flash_disk_initialize (void)
{
    TRACE_PRINTF("\r\n");

    if (!(Stat & STA_NOINIT))
        return Stat;

    int32_t ret;

    ret = Driver_Flash0.Initialize(NULL);
    ASSERT_HIGH(ret);

    if (ret != ARM_DRIVER_OK)
        return RES_ERROR;

    ret = Driver_Flash0.PowerControl(ARM_POWER_FULL);
    ASSERT_HIGH(ret);

    if (ret != ARM_DRIVER_OK)
        return RES_ERROR;

    sector_count = Driver_Flash0.GetInfo()->sector_count;
    sector_size = Driver_Flash0.GetInfo()->sector_size;
    assert(FF_MAX_SS == sector_size); //sector_size must be FF_MAX_SS

    Stat &= ~STA_NOINIT;    /* Clear STA_NOINIT flag */
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS flash_disk_status(void)
{
    TRACE_PRINTF("%d\r\n", Stat);
    return Stat;    /* Return disk status */
}

/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT flash_disk_read (
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

    int ret = 0;

    for (; count > 0; count--) {
        ret = Driver_Flash0.ReadData(sector * sector_size, (void *)buff, sector_size);
        assert(ret == sector_size);
        sector++;
        buff += sector_size;

        if (ret != sector_size)
            return RES_ERROR;
    }

    return RES_OK;    /* Return result */
}

/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT flash_disk_write (
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

    int ret = 0;

    for (; count > 0; count--) {
        ret = Driver_Flash0.EraseSector(sector * sector_size);
        ASSERT_HIGH(ret);

        if (ret != ARM_DRIVER_OK)
            return RES_ERROR;

        ret = Driver_Flash0.ProgramData(sector * sector_size, (void *)buff, sector_size);
        assert(ret == sector_size);
        sector++;
        buff += sector_size;

        if (ret != sector_size)
            return RES_ERROR;
    }

    return RES_OK;    /* Return result */
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/
DRESULT flash_disk_ioctl (
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    TRACE_PRINTF("%d 0x%x\r\n", cmd, (uint32_t)buff);

    DRESULT res;
    LBA_t *dp;
    LBA_t sector;
#if FF_USE_TRIM
    int ret = 0;
#endif
    if (Stat & STA_NOINIT)
        return RES_NOTRDY;    /* Check if drive is ready */

    res = RES_ERROR;

    switch (cmd) {
    case CTRL_SYNC :        /* Wait for end of internal write process of the drive */
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT :    /* Get drive capacity in unit of sector (DWORD) */
        *(DWORD*)buff = sector_count;
        res = RES_OK;
        break;

    case GET_BLOCK_SIZE :    /* Get erase block size in unit of sector (DWORD) */
        *(WORD*)buff = 1;    // block size = 1 sector 
        res = RES_OK;
        break;

    case GET_SECTOR_SIZE :   /* Get sector size in unit of byte (WORD) (needed at FF_MAX_SS != FF_MIN_SS) */
        *(WORD*)buff = sector_size;
        res = RES_OK;
        break;

#if FF_USE_TRIM
    case CTRL_TRIM :    /* Erase a block of sectors (used when _USE_TRIM == 1) */
        dp = buff;
        res = RES_OK;

        for (sector = (DWORD)dp[0]; sector > (DWORD)dp[1]; sector++) {
            ret = Driver_Flash0.EraseSector(sector * sector_size);
            ASSERT_HIGH(ret);

            if (ret != ARM_DRIVER_OK) {
                res = RES_ERROR;
                break;
            }
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
void flash_disk_timerproc (void)
{
    TRACE_PRINTF("\r\n");
}
