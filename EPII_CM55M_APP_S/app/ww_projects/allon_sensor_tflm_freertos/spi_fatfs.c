#include "xprintf.h"
#include "ff.h"
#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"
#include "sleep_mode.h"

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
    FRESULT res;        /* API result code */
    FILINFO fno;
    char file_dir[20];
    UINT file_dir_idx = 0;
    char cur_dir[128];
    UINT len = 128;
	rtc_time tm;

//	Not used
//    FIL fil_w, fil_r;   /* File object */
//
//    char filename[20];
//    char filecontent[256];
//
//    UINT bw;            /* Bytes written */
//    UINT br;            /* Bytes read */
//    BYTE buffer[128];
//    DIR dir;

    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 1);

    printf("Mount SD card fatfs\r\n");

    res = f_mount(&fs, DRV, 1);
    if (res)
    {
        printf("f_mount fail, res = %d\r\n", res);
        printf("Please insert SD card!\r\n");
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

#if 0   // do not list files
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
#endif

    RTC_GetTime(&tm);
    xsprintf(file_dir, "%04d%02d%02d_%02d%02d%02d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    while ( 1 )
    {
        res = f_stat(file_dir, &fno);
        if (res == FR_OK)
        {
            printf("%s is exist, create next one.\r\n", file_dir);
            file_dir_idx++;
            xsprintf(file_dir, "%04d%02d%02d_%02d%02d%02d_%d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, file_dir_idx);
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

// CGP Change required 
//    res = f_open(&fil_w, filename, FA_CREATE_NEW | FA_WRITE);

    res = f_open(&fil_w, (const TCHAR*) filename, FA_CREATE_NEW | FA_WRITE);
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
                printf("%10u %s\r\n", (int) fno.fsize, fno.fname);
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


#define MAKE_STRING         "Himax"
#define MODEL_STRING        "HM0360"
// #define DATETIME_STRING     "2025:07:02 09:08:48"
// #define CREATE_DATE_STRING  "2025:07:02 09:08:48"

#define TAG_MAKE            0x010F
#define TAG_MODEL           0x0110
#define TAG_DATETIME        0x9003
#define TAG_CREATE_DATE     0x9004  // DateTimeDigitized

typedef struct {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint32_t value_offset;
} __attribute__((packed)) ExifTag;

static void write_u16_le(uint8_t* p, uint16_t val) {
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)(val >> 8);
}

static void write_u32_le(uint8_t* p, uint32_t val) {
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
    p[2] = (uint8_t)((val >> 16) & 0xFF);
    p[3] = (uint8_t)((val >> 24) & 0xFF);
}

int insert_exif_in_memory(
    uint32_t input_image,
    size_t input_size,
    uint32_t output_image,
    size_t max_output_size,
    size_t* output_size
) {
    rtc_time tm;
    char datetime_str[20];

    if (!input_image || !output_image || !output_size || input_size < 2)
        return -1;

    SCB_InvalidateDCache_by_Addr ((void *)input_image, input_size);

    const uint8_t* in = (const uint8_t*)input_image;
    uint8_t* out = (uint8_t*)output_image;

    if (in[0] != 0xFF || in[1] != 0xD8)
        return -1;

    size_t offset = 0;
    out[offset++] = in[0];
    out[offset++] = in[1];

    uint8_t exif[512];
    uint8_t* p = exif;

    *p++ = 0xFF;
    *p++ = 0xE1;

    uint8_t* len_ptr = p;
    p += 2;

    memcpy(p, "Exif\0\0", 6); p += 6;

    write_u16_le(p, 0x4949); p += 2;
    write_u16_le(p, 0x002A); p += 2;
    write_u32_le(p, 8);      p += 4;

    write_u16_le(p, 4); p += 2;  // 4 IFD entries

    uint32_t data_offset_base = 8 + 2 + 4 * sizeof(ExifTag) + 4;
    uint32_t data_offset = data_offset_base;

    // DATETIME_STRING
    RTC_GetTime(&tm);
    xsprintf(datetime_str, "%04d:%02d:%02d %02d:%02d:%02d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Tag 1: Make
    ExifTag* tag = (ExifTag*)p;
    write_u16_le((uint8_t*)&tag->tag, TAG_MAKE);
    write_u16_le((uint8_t*)&tag->type, 2);
    write_u32_le((uint8_t*)&tag->count, strlen(MAKE_STRING) + 1);
    write_u32_le((uint8_t*)&tag->value_offset, data_offset);
    p += sizeof(ExifTag);
    data_offset += strlen(MAKE_STRING) + 1;

    // Tag 2: Model
    tag = (ExifTag*)p;
    write_u16_le((uint8_t*)&tag->tag, TAG_MODEL);
    write_u16_le((uint8_t*)&tag->type, 2);
    write_u32_le((uint8_t*)&tag->count, strlen(MODEL_STRING) + 1);
    write_u32_le((uint8_t*)&tag->value_offset, data_offset);
    p += sizeof(ExifTag);
    data_offset += strlen(MODEL_STRING) + 1;

    // Tag 3: DateTimeOriginal
    tag = (ExifTag*)p;
    write_u16_le((uint8_t*)&tag->tag, TAG_DATETIME);
    write_u16_le((uint8_t*)&tag->type, 2);
    write_u32_le((uint8_t*)&tag->count, strlen(datetime_str) + 1);
    write_u32_le((uint8_t*)&tag->value_offset, data_offset);
    p += sizeof(ExifTag);
    data_offset += strlen(datetime_str) + 1;

    // Tag 4: CreateDate
    tag = (ExifTag*)p;
    write_u16_le((uint8_t*)&tag->tag, TAG_CREATE_DATE);
    write_u16_le((uint8_t*)&tag->type, 2);
    write_u32_le((uint8_t*)&tag->count, strlen(datetime_str) + 1);
    write_u32_le((uint8_t*)&tag->value_offset, data_offset);
    p += sizeof(ExifTag);
    data_offset += strlen(datetime_str) + 1;

    // IFD0 next pointer
    write_u32_le(p, 0); p += 4;

    // Data area
    strcpy((char*)p, MAKE_STRING);  p += strlen(MAKE_STRING) + 1;
    strcpy((char*)p, MODEL_STRING); p += strlen(MODEL_STRING) + 1;
    strcpy((char*)p, datetime_str); p += strlen(datetime_str) + 1;  // DATETIME_STRING
    strcpy((char*)p, datetime_str); p += strlen(datetime_str) + 1;  // CREATE_DATE_STRING

    uint16_t exif_len = (p - (exif + 2));
    len_ptr[0] = (exif_len >> 8) & 0xFF;
    len_ptr[1] = exif_len & 0xFF;

    size_t exif_size = p - exif;
    if (offset + exif_size + input_size - 2 > max_output_size)
        return -1;

    memcpy(out + offset, exif, exif_size);
    offset += exif_size;

    memcpy(out + offset, in + 2, input_size - 2);
    offset += input_size - 2;

    SCB_CleanDCache();

    *output_size = offset;
    return 0;
}


DWORD get_fattime(void)
{
    rtc_time tm;

    RTC_GetTime(&tm);
    return ((DWORD)(tm.tm_year-1980) << 25) |
           ((DWORD)(tm.tm_mon) << 21) |
           ((DWORD)tm.tm_mday << 16) |
           ((DWORD)tm.tm_hour << 11) |
           ((DWORD)tm.tm_min << 5) |
           ((DWORD)(tm.tm_sec / 2));
}
