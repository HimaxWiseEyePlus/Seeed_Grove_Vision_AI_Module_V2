#include "xprintf.h"
#include "ff.h"
#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
FRESULT list_dir (const char *path);
FRESULT scan_files (char* path);

#if 0
typedef struct
{
  char riff[4] = {'R', 'I', 'F', 'F'};  /* "RIFF"                                  */
  int32_t flength;                      /* file length in bytes                    */
  char wave[4] = {'W', 'A', 'V', 'E'};  /* "WAVE"                                  */
  char fmt[4] = {'f', 'm', 't', ' '};   /* "fmt "                                  */
  int32_t chunk_size = 16;              /* size of FMT chunk in bytes (usually 16) */
  int16_t format_tag = 1;               /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
  int16_t num_chans = 1;                /* 1=mono, 2=stereo                        */
  int32_t srate = 16000;                /* Sampling rate in samples per second     */
  int32_t bytes_per_sec = 16000 * 2;    /* bytes per second = srate*bytes_per_samp */
  int16_t bytes_per_samp = 2;           /* 2=16-bit mono, 4=16-bit stereo          */
  int16_t bits_per_samp = 16;           /* Number of bits per sample               */
  char data[4] = {'d', 'a', 't', 'a'};  /* "data"                                  */
  int32_t dlength;                      /* data length in bytes (filelength - 44)  */
} WAV_HDR;
#else
typedef struct
{
  char riff[4];           /* "RIFF"                                  */
  int32_t flength;        /* file length in bytes                    */
  char wave[4];           /* "WAVE"                                  */
  char fmt[4];            /* "fmt "                                  */
  int32_t chunk_size;     /* size of FMT chunk in bytes (usually 16) */
  int16_t format_tag;     /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
  int16_t num_chans;      /* 1=mono, 2=stereo                        */
  int32_t srate;          /* Sampling rate in samples per second     */
  int32_t bytes_per_sec;  /* bytes per second = srate*bytes_per_samp */
  int16_t bytes_per_samp; /* 2=16-bit mono, 4=16-bit stereo          */
  int16_t bits_per_samp;  /* Number of bits per sample               */
  char data[4];           /* "data"                                  */
  int32_t dlength;        /* data length in bytes (filelength - 44)  */
} WAV_HDR;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#define DRV         ""
#define RECORD_DIR "RecordAudio"

static FATFS fs;            /* Filesystem object */
static WAV_HDR wavh;         /* WAV header       */

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

int wav_header_init(int32_t srate, int16_t num_chans)
{
  strncpy(wavh.riff, "RIFF", 4);
  strncpy(wavh.wave, "WAVE", 4);
  strncpy(wavh.fmt, "fmt ", 4);
  strncpy(wavh.data, "data", 4);

  wavh.chunk_size = 16;             /* size of FMT chunk in bytes (usually 16) */
  wavh.format_tag = 1;              /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
  wavh.num_chans = num_chans;       /* 1=mono, 2=stereo                        */
  wavh.srate = srate;               /* Sampling rate in samples per second     */
  wavh.bytes_per_sec = 16000 * 2;   /* bytes per second = srate*bytes_per_samp */
  wavh.bytes_per_samp = 2;          /* 2=16-bit mono, 4=16-bit stereo          */
  wavh.bits_per_samp = 16;          /* Number of bits per sample               */
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
        return res;
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

#if 0
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

    while ( 1 )
    {
        xsprintf(file_dir, "%s%04d", RECORD_DIR, file_dir_idx);
        res = f_stat(file_dir, &fno);
        if (res == FR_OK)
        {
            //printf("%s is exist, create next one.\r\n", file_dir);
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


int fastfs_write_audio(uint32_t SRAM_addr, uint32_t pcm_size, uint8_t *filename)
{
    FIL fil_w;          /* File object */
    FRESULT res;        /* API result code */
    UINT bw;            /* Bytes written */

    wavh.dlength = pcm_size;
    wavh.flength = wavh.dlength + sizeof(WAV_HDR) - 8;

    res = f_open(&fil_w, filename, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK)
    {
        printf("Write file : %s.\r\n", filename);
        res = f_write(&fil_w, (void*)&wavh, sizeof(WAV_HDR), &bw);
        res = f_write(&fil_w, (void*)SRAM_addr, pcm_size, &bw);
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
