#include "xprintf.h"
#include "ff.h"
#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
FRESULT list_dir (const char *path);
FRESULT scan_files (char* path);

/*******************************************************************************
 * Code
 ******************************************************************************/
#define DRV         ""
#define CAPTURE_DIR "CaptureImage"

static FATFS fs;             /* Filesystem object */

//app implement GPIO_Output_Level/GPIO_Pinmux/GPIO_Dir for fatfs\port\mmc_spi\mmc_we2_spi.c ARM_SPI_SS_MASTER_SW
void SSPI_CS_GPIO_Output_Level(bool setLevelHigh)
{
    hx_drv_gpio_set_out_value(GPIO16, (GPIO_OUT_LEVEL_E) setLevelHigh);
}

void SSPI_CS_GPIO_Pinmux(bool setGpioFn)
{
    if (setGpioFn)
        hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_GPIO16, 0);
    else
        hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 0);
}

void SSPI_CS_GPIO_Dir(bool setDirOut)
{
    if (setDirOut)
        hx_drv_gpio_set_output(GPIO16, GPIO_OUT_HIGH);
    else
        hx_drv_gpio_set_input(GPIO16);
}


int fatfs_init(void)
{
    FIL fil_w, fil_r;   /* File object */
    FRESULT res;        /* API result code */
    UINT bw;            /* Bytes written */
    UINT br;            /* Bytes read */
    BYTE buffer[128];
    DIR dir;
    FILINFO fno;
    char file_dir[20];
    UINT file_dir_idx = 0;
    char filename[20];
    char filecontent[256];
    char cur_dir[128];
    UINT len = 128;

    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 1);

    printf("Mount SD card fatfs\r\n");

    res = f_mount(&fs, DRV, 1);
    if (res)
    {
        printf("f_mount fail, res = %d\r\n", res);
        while(1);
    }

    res = f_getcwd(cur_dir, len);      /* Get current directory */
    if (res)
    {
        printf("f_getcwd res = %d\r\n", res);
    }
    else
    {
        printf("cur_dir = %s\r\n", cur_dir);
    }

    res = list_dir(cur_dir);
    if (res)
    {
        printf("list_dir res = %d\r\n", res);
    }

    res = scan_files(cur_dir);
    if (res)
    {
        printf("scan_files res = %d\r\n", res);
    }

    while ( 1 )
    {
        xsprintf(file_dir, "%s%04d", CAPTURE_DIR, file_dir_idx);
        res = f_stat(file_dir, &fno);
        if (res == FR_OK)
        {
            printf("%s is exist, create next one.\r\n", file_dir);
            file_dir_idx++;
        }
        else
        {
            printf("Create directory %s.\r\n", file_dir);
            res = f_mkdir(file_dir);
            if (res) { printf("f_mkdir res = %d\r\n", res); }
            
            printf("Change directory \"%s\".\r\n", file_dir);
            res = f_chdir(file_dir);

            res = f_getcwd(cur_dir, len);      /* Get current directory */
            printf("cur_dir = %s\r\n", cur_dir);
            break;
        }
    }

    return 0;
}


int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename)
{
    FIL fil_w;          /* File object */
    FRESULT res;        /* API result code */
    UINT bw;            /* Bytes written */

    res = f_open(&fil_w, filename, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK)
    {
        SCB_InvalidateDCache_by_Addr ((void *)SRAM_addr, img_size);
        printf("write file : %s.\r\n", filename);
        res = f_write(&fil_w, (void *)SRAM_addr, img_size, &bw);
        if (res) { printf("f_write res = %d\r\n", res); }
        f_close(&fil_w);
    }
    else
    {
        printf("f_open res = %d\r\n", res);
    }
    return 0;
}


/* List contents of a directory */
FRESULT list_dir (const char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        nfile = ndir = 0;
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Error or end of dir */
            if (fno.fattrib & AM_DIR) {            /* Directory */
                printf("   <DIR>   %s\r\n", fno.fname);
                ndir++;
            } else {                               /* File */
                printf("%10u %s\r\n", fno.fsize, fno.fname);
                nfile++;
            }
        }
        f_closedir(&dir);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
    } else {
        printf("Failed to open \"%s\". (%u)\r\n", path, res);
    }
    return res;
}


/* Recursive scan of all items in the directory */
FRESULT scan_files (char* path)     /* Start node to be scanned (***also used as work area***) */
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\r\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}
