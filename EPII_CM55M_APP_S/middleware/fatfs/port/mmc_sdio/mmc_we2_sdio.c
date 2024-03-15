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
#include "hx_drv_sdio.h"
#include "ff.h"            /* Obtains integer types */
#include "diskio.h"        /* Declarations of disk functions */
#include "mmc_we2.h"
#include "driver_interface.h"

//#include "xprintf.h"

//#define TRACE_PRINTF(fmt, ...)  printf("%s:%s:%d " fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define TRACE_PRINTF(fmt, ...)  printf("%s " fmt, __func__, ##__VA_ARGS__)
//#define TRACE_PRINTF(fmt, ...)  printf(fmt, ##__VA_ARGS__)
//#define TRACE_PRINTF(fmt, ...)

#define HX_SDIO_SUPPORT_SDC_VER2_ONLY   (1)
#define HX_SDIO_BUS_WIDTH_1 (1)
#define HX_SDIO_BUS_WIDTH_4 (4)
#define HX_SDIO_SECTOR_SIZE (512UL)

#if FF_MIN_SS != HX_SDIO_SECTOR_SIZE
#error FF_MIN_SS != HX_SDIO_SECTOR_SIZE
#endif

static volatile DSTATUS Stat = STA_NOINIT;    /* Physical drive status */

//static BYTE CardType;    /* Card type flags */

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS mmc_disk_initialize (void)
{
    TRACE_PRINTF("\r\n");
    unsigned int ref_freq;
    if (!(Stat & STA_NOINIT))
        return Stat;

    unsigned int clk = 25000;  //25MHZ

    drv_interface_get_freq(SCU_CLK_FREQ_TYPE_HSC_SDIO_CLK, &ref_freq);

    if(clk > ref_freq)
        clk = ref_freq;

    if (hx_drv_sdio_init(clk, HX_SDIO_BUS_WIDTH_4, SD_REG_BASE) != SDIO_PASS) {
        Stat = STA_NOINIT;
		return Stat;
	}

    Stat &= ~STA_NOINIT; 
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS mmc_disk_status(void)
{
    TRACE_PRINTF("%d\r\n", Stat);
    return Stat;    /* Return disk status */
}

/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT mmc_disk_read (
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count            /* Sector count (1..128) */
)
{
    TRACE_PRINTF("0x%x %d %d\r\n", (uint32_t)buff, sector, count);

    SDIO_ERROR_E ercd = SDIO_PASS;

    if (!count)
	    return RES_PARERR;        /* Check parameter */

    if (Stat & STA_NOINIT)
	    return RES_NOTRDY;    /* Check if drive is ready */

    ercd = hx_drv_sdio_mem_read((unsigned int)sector * HX_SDIO_SECTOR_SIZE, 
                                (unsigned int)count * HX_SDIO_SECTOR_SIZE, 
                                (unsigned char *)buff);        
	if(ercd !=SDIO_PASS) {

		return RES_ERROR;
	}
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT mmc_disk_write (
    const BYTE *buff,    /* Pointer to the data to be written */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count            /* Sector count (1..128) */
)
{
    TRACE_PRINTF("0x%x %d %d\r\n", (uint32_t)buff, sector, count);

    if (!count)
	    return RES_PARERR;        /* Check parameter */

    if (Stat & STA_NOINIT)
	    return RES_NOTRDY;    /* Check drive status */

    if (Stat & STA_PROTECT)
	    return RES_WRPRT;    /* Check write protect */

    if (hx_drv_sdio_mem_write(sector * HX_SDIO_SECTOR_SIZE, 
                              count * HX_SDIO_SECTOR_SIZE, 
                              (unsigned char *)buff)!=SDIO_PASS) {
        return RES_ERROR;
	}

    return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/
DRESULT mmc_disk_ioctl (
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    TRACE_PRINTF("%d 0x%x\r\n", cmd, (uint32_t)buff);

    DRESULT res;
    BYTE n, csd[16], SD_s[64];
    DWORD st, ed, bc;
    DWORD csize;
    LBA_t *dp;

    if (Stat & STA_NOINIT)
	    return RES_NOTRDY;    /* Check if drive is ready */

    res = RES_ERROR;

    switch (cmd) {
    case CTRL_SYNC :	/* Make sure that all data has been written on the media */
        if (hx_drv_sdio_WaitInReq() == 0)
            res = RES_OK;
        break;

    case GET_SECTOR_COUNT :    /* Get drive capacity in unit of sector (DWORD) */
        if (hx_drv_sdio_readCSD(csd) == SDIO_PASS)
        {
            if ((csd[15] >> 6) == 1) {	/* SDC ver 2.00 */
                csize = csd[6] + ((uint16_t)csd[7] << 8) + ((uint32_t)(csd[8] & 63) << 16) + 1;
                *(LBA_t*)buff = csize << 10;
            } else {					/* SDC ver 1.XX or MMC */
                csize = (csd[7]>>6) + ((WORD)csd[8] << 2) + ((WORD)(csd[9] & 3) << 10) + 1;
                n = (csd[10] & 15) +  ((csd[5] & 128) >> 7) + ((csd[6] & 3) << 1) + 2;
                *(LBA_t*)buff = csize << (n - 9); //div 512 bytes to get counts
            }
            res = RES_OK;
        }
        break;

	case GET_SECTOR_SIZE :	/* Get sector size in unit of byte (WORD) (needed at FF_MAX_SS != FF_MIN_SS) */
		*(LBA_t*)buff = (DWORD)HX_SDIO_SECTOR_SIZE;
		res = RES_OK;
		break;

    case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */ 
        if(hx_drv_sdio_readCSD(csd) == SDIO_PASS)
        {
            if ((csd[15] >> 6) == 1) {	/* SDC ver 2.00 */
                if (hx_drv_sdio_readSDstatus(SD_s) == SDIO_PASS)
                {
                    *(DWORD*)buff = 16UL << (SD_s[10] >> 4);
                    res = RES_OK;
                }
            } else {					/* SDC ver 1.XX or MMC */
                /* SDC ver 1.XX, and no support MMC */
                *(DWORD*)buff = (((csd[5] & 63) << 1) + ((WORD)(csd[4] & 128) >> 7) + 1) << ((csd[2] >> 6) - 1);  
                res = RES_OK;    
            }
        }  
        break;

#if FF_USE_TRIM 
    case CTRL_TRIM :		/* Erase a block of sectors */
        dp = buff; st = (DWORD)dp[0]; ed = (DWORD)dp[1];    /* Load sector block */
        bc = ed-st;
        st *= HX_SDIO_SECTOR_SIZE; 
        bc *= HX_SDIO_SECTOR_SIZE;

        if (hx_drv_sdio_mem_erase(st, bc) != SDIO_PASS) {
            TRACE_PRINTF("write fail\r\n");
            break;                   
        }
#endif

    default:
        res = RES_PARERR;              
	}
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Device timer function                                                 */
/*-----------------------------------------------------------------------*/
/* This function must be called from timer interrupt routine in period
/  of 1 ms to generate card control timing.
*/
void mmc_disk_timerproc (void)
{
    TRACE_PRINTF("\r\n");
}
