
#include "spi_fatfs.h"
#include "sleep_mode.h"

#include "xprintf.h"
#include "ff.h"
#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"

#include "FreeRTOS.h"
#include "task.h"

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

// This from ChatGPT:
#define SEQUENCE_FILE "sequence.txt"


/**
 * @brief Writes the sequence number to the file.
 * @param sequenceNumber The number to save.
 * @return the sequence number, or 0 on disk fail
 */
uint16_t fatfs_saveSequenceNumber(uint16_t sequenceNumber) {
    static FIL file;
    FRESULT res;
    UINT bytesWritten;
    char buffer[20];

    // Convert number to string
    snprintf(buffer, 20, "%d", sequenceNumber);

    // Open file for writing (create/overwrite)
    res = f_open(&file, SEQUENCE_FILE, FA_WRITE | FA_CREATE_ALWAYS);

    if (res == FR_OK) {
        // Write sequence number to file
        f_write(&file, buffer, strlen(buffer), &bytesWritten);
        f_close(&file);
        return sequenceNumber;
    }
    else {
    	return 0;
    }
    return 0;
}

/**
 * @brief Reads sequence number from file or initializes it if missing.
 * @return The sequence number.
 */
uint16_t loadSequenceNumber(void) {
    FIL file;
    FRESULT res;
    UINT bytesRead = 0;
    char buffer[10];  // Enough for a uint16_t
    uint16_t sequenceNumber = 1; // Default value

    // Try to open the file
    res = f_open(&file, SEQUENCE_FILE, FA_READ);

    if (res == FR_OK) {
        // Read the file content
        res = f_read(&file, buffer, sizeof(buffer) - 1, &bytesRead);
        f_close(&file);

        if (res == FR_OK && bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate for safe conversion
            sequenceNumber = (uint16_t) atoi(buffer);

            printf("Contents of '%s' is %d\r\n", SEQUENCE_FILE, sequenceNumber);
        }
    }

    if (res != FR_OK) {
        // If file does not exist or read failed, initialize it with 1
        printf("Initialising file '%s'\r\n", SEQUENCE_FILE);
    	sequenceNumber = fatfs_saveSequenceNumber(1);
    }

    return sequenceNumber;
}


/**
 * Initialise Fat FS
 *
 * @return the sequence number for the next image, or 0 on failure
 */
uint16_t fatfs_init(void) {
    FRESULT res;        /* API result code */

    FILINFO fno;
    char file_dir[20];
    //UINT file_dir_idx = 0;

    char cur_dir[128];
    UINT len = 128;
	//rtc_time tm;

    // CGP unused:
    // char filecontent[256];
	// char filename[20];
	// BYTE buffer[128];
    // UINT bw;            /* Bytes written */
    // UINT br;            /* Bytes read */
    // DIR dir;

    // FIL fil_w, fil_r;   /* File object */

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
       return 0;
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

    // CGP I want all files in the same directory
    xsprintf(file_dir, "HM0360_Test");

    res = f_stat(file_dir, &fno);
    if (res == FR_OK) {
        printf("Directory %s exists\r\n", file_dir);
        //file_dir_idx++;
        //xsprintf(file_dir, "%04d%02d%02d_%02d%02d%02d_%d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, file_dir_idx);
    }
    else {
        printf("Create directory %s.\r\n", file_dir);
        res = f_mkdir(file_dir);
        if (res != FR_OK) {
        	printf("f_mkdir res = %d\r\n", res);
        	return 0;
        }
    }

    printf("Change directory to '%s'.\r\n", file_dir);
    res = f_chdir(file_dir);

    res = f_getcwd(cur_dir, len);      /* Get current directory */
    printf("Current directory is = '%s'\r\n\n", cur_dir);

    // Now open a file that contains the sequence number
    uint16_t seqNum = loadSequenceNumber();
    printf("Next image is %d\n", seqNum);
    return seqNum;

#if 0
    // old code - new directory each time.
    RTC_GetTime(&tm);
    xsprintf(file_dir, "%04d%02d%02d_%02d%02d%02d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    while ( 1 )
    {
        res = f_stat(file_dir, &fno);
        if (res == FR_OK)
        {
            printf("Directory %s exists\r\n", file_dir);
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
            printf("cur_dir = %s\r\n\n", cur_dir);
            break;
        }
    }
    return true;

#endif // 0
}


int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename) {
	FIL fil_w;          /* File object */
	FRESULT res;        /* API result code */
	UINT bw;            /* Bytes written */
	TickType_t startTime;
	TickType_t endTime;

	startTime = xTaskGetTickCount();  // Get start time

	res = f_open(&fil_w, (const TCHAR*) filename, FA_CREATE_NEW | FA_WRITE);

	if (res == FR_OK)  {
		SCB_InvalidateDCache_by_Addr ((void *)SRAM_addr, img_size);
		//printf("write file : %s.\r\n", filename);
		res = f_write(&fil_w, (void *)SRAM_addr, img_size, &bw);

		if (res) {
			printf("f_write error = %d\r\n", res);
		}
		f_close(&fil_w);

		endTime = xTaskGetTickCount();  // Get end time

		// Convert ticks to milliseconds
		uint32_t elapsedMs = (endTime - startTime) * portTICK_PERIOD_MS;
		printf("File write took %lums\n", elapsedMs);
	}
	else  {
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
                printf("%10u %s\r\n", (unsigned int) fno.fsize, fno.fname);
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
