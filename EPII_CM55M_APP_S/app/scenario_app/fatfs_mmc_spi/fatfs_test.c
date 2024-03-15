#include "fatfs_test.h"
#include "xprintf.h"
#include "ff.h"
#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
FRESULT list_dir (const char *path);
FRESULT scan_files (char* path);
FRESULT delete_node (
    TCHAR* path,    /* Path name buffer with the sub-directory to delete */
    UINT sz_buff,   /* Size of path name buffer (items) */
    FILINFO* fno    /* Name read buffer */
);


/*******************************************************************************
 * Code
 ******************************************************************************/
#define DRV      ""
#define TESTFILE "mmc_spi.txt"
#define TESTSTR  "fatfs_mmc_spi_test_tz_s_only Hello, World!"

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


/*!
 * @brief Main function
 */
int fatfs_test(void) {
    FIL fil_w, fil_r;       /* File object */
    FRESULT res;            /* API result code */
    FATFS fs;               /* Filesystem object */
    DIR dir;
    FILINFO fno;
    uint32_t bw;            /* Bytes written */
    uint32_t br;            /* Bytes read */
    char buffer[128];
    char file_dir[20] = "20240101";
    char filename[20];
    char filecontent[256];
    char cur_dir[128];
    uint32_t len = 128;

    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 1);

    printf("Start to test SD card fatfs (%s, %s)\r\n", __DATE__, __TIME__);

    res = f_mount(&fs, DRV, 1);
    if (res)
    {
        printf("f_mount res = %d\r\n", res);
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

    xsprintf(buffer, "%s%s", cur_dir, file_dir);
    printf("Remove directory %s.\r\n", buffer);
    res = delete_node (buffer, sizeof(buffer), &fno);
    if (res) { printf("delete_node res = %d\r\n", res); }

    printf("Create directory %s.\r\n", file_dir);
    res = f_mkdir(file_dir);
    if (res) { printf("f_mkdir res = %d\r\n", res); }

    printf("Change directory \"%s\".\r\n", file_dir);
    res = f_chdir(file_dir);

    res = f_getcwd(cur_dir, len);      /* Get current directory */
    printf("cur_dir = %s\r\n", cur_dir);
    res = list_dir(cur_dir);

    for (int i = 0; i < 10; i++ )
    {
        xsprintf(filename, "file%04d.txt", i);
        xsprintf(filecontent, "%s [%d]", TESTSTR, i);
        res = f_open(&fil_w, filename, FA_CREATE_NEW | FA_WRITE);
        if (res == FR_OK)
        {
            printf("write file : %s .\r\n", filename);
            res = f_write(&fil_w, filecontent, sizeof(filecontent), &bw);
            if (res) { printf("f_write res = %d\r\n", res); }
            f_close(&fil_w);
        }
        else
        {
            printf("f_open res = %d\r\n", res);
        }
    }

    res = f_getcwd(cur_dir, len);      /* Get current directory */
    printf("cur_dir = %s\r\n", cur_dir);
    res = list_dir(cur_dir);

    for (int i = 0; i < 10; i++ )
    {
        xsprintf(filename, "file%04d.txt", i);
        res = f_open(&fil_r, filename, FA_OPEN_EXISTING | FA_READ);
        if (res) { printf("f_open res = %d\r\n", res); }

        res = f_read(&fil_r, buffer, sizeof(buffer), &br);
        if (res) { printf("f_read res = %d\r\n", res); }

        printf("read %s content\r\n", filename);
        printf("%s\r\n", buffer);

        f_close(&fil_r);
    }

    f_unmount(DRV);

    printf("SD card fatfs test done!\r\n", filename);
    for(;;) {}

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


/*------------------------------------------------------------/
/ Delete a sub-directory even if it contains any file
/-------------------------------------------------------------/
/ The delete_node() function is for R0.12+.
/ It works regardless of FF_FS_RPATH.
*/
FRESULT delete_node (
    TCHAR* path,    /* Path name buffer with the sub-directory to delete */
    UINT sz_buff,   /* Size of path name buffer (items) */
    FILINFO* fno    /* Name read buffer */
)
{
    UINT i, j;
    FRESULT fr;
    DIR dir;


    fr = f_opendir(&dir, path); /* Open the sub-directory to make it empty */
    if (fr != FR_OK) return fr;

    for (i = 0; path[i]; i++) ; /* Get current path length */
    path[i++] = _T('/');

    for (;;) {
        fr = f_readdir(&dir, fno);  /* Get a directory item */
        if (fr != FR_OK || !fno->fname[0]) break;   /* End of directory? */
        j = 0;
        do {    /* Make a path name */
            if (i + j >= sz_buff) { /* Buffer over flow? */
                fr = 100; break;    /* Fails with 100 when buffer overflow */
            }
            path[i + j] = fno->fname[j];
        } while (fno->fname[j++]);
        if (fno->fattrib & AM_DIR) {    /* Item is a sub-directory */
            fr = delete_node(path, sz_buff, fno);
        } else {                        /* Item is a file */
            fr = f_unlink(path);
        }
        if (fr != FR_OK) break;
    }

    path[--i] = 0;  /* Restore the path name */
    f_closedir(&dir);

    if (fr == FR_OK) fr = f_unlink(path);  /* Delete the empty sub-directory */
    return fr;
}
