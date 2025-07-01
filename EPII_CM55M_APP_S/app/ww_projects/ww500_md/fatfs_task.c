/*
 * fatfs_task.c
 *
 *  Created on: 19 Aug 2024
 *      Author: CGP
 *
 * My initial approach is to use the fatfs routines which are used in ww130_test and elsewhere,
 * then encapsulate these in a FreeRTOS task.
 *
 * It looks like the Himax code uses the "ChaN" code, which is present in the middleware/fatfs folder.
 * And within that code the mmc_spi drivers are used.
 *
 * The other approach is to use the FreeRTOS+FAT here:
 * 		https://github.com/FreeRTOS/Lab-Project-FreeRTOS-FAT
 *
 * According to Copilot:
 *
 * FreeRTOS+FAT builds upon the FatFs library by ChaN.
 * It adapts the FatFs code to work within the FreeRTOS environment, adding thread safety
 * and integration with FreeRTOS tasks.
 * FreeRTOS+FAT includes additional features specific to FreeRTOS, such as support for mutexes
 * and semaphores.
 * Essentially, FreeRTOS+FAT extends the capabilities of FatFs to make it suitable for
 * multitasking systems like FreeRTOS.
 *
 * You can also ask Copilot: "What are the advantages of using FreeRTOS+FAT over FatFs?"
 *
 * It looks like I spent time in 2022 getting FreeRTOS+FAT working on the MAX78000 - see here:
 * https://forums.freertos.org/t/freertos-fat-example-required-for-sd-card-using-spi-interface/15503/15
 *
 * Notes on 8.3 file names
 * -----------------------
 * Apparently mcuh faster. Do this by setting FF_USE_LFN to 0 in ffconf.h
 * See this ChatGPT discussion: https://chatgpt.com/share/6861bbf0-f8e0-8005-af0a-3f42d0fcb775
 *
 * Notes on SD cards > 32G
 * -------------------------
 * These are probaly supporting exFAT. The above ChatGPT conversation suggested I install fat32format.exe
 * from here: http://ridgecrop.co.uk/index.htm?fat32format.htm
 * This worked for me - formatted 64G cards as FAT32
 *
 */

/*************************************** Includes *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "WE2_device.h"
#include "WE2_debug.h"
#include "WE2_core.h"
#include "board.h"

#include "printf_x.h"
#include "xprintf.h"

// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "fatfs_task.h"
#include "image_task.h"
#include "app_msg.h"
#include "CLI-commands.h"
#include "ff.h"
#include "CLI-FATFS-commands.h"
#include "time_handling.h"

// TODO I am not using the public functions in this. Can we move the important bits of this to here?
#include "spi_fatfs.h"
#include "hx_drv_rtc.h"
#include "exif_utc.h"
#include "ww500_md.h"
#include "inactivity.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define fatfs_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define FATFS_TASK_QUEUE_LEN   		10

#define DRV         ""

// Warning: if using 8.3 file names then this applies to directories also
#if FF_USE_LFN
#define CAPTURE_DIR "HM0360_Test"
#define STATE_FILE "configuration.txt"
#else
#define CAPTURE_DIR "IMAGES.000"
#define STATE_FILE "CONFIG.TXT"
#endif	// FF_USE_LFN

// Length of lines in configuration.txt
#define MAXCOMMENTLENGTH  80
// Max number of comment lines in configuration.txt
#define MAXNUMCOMMENTS    OP_PARAMETER_NUM_ENTRIES + 5

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vFatFsTask(void *pvParameters);

static FRESULT fatFsInit(void);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T  handleEventForUninit(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForIdle(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForBusy(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T  flagUnexpectedEvent(APP_MSG_T rxMessage);


static FRESULT fileRead(fileOperation_t * fileOp);
static FRESULT fileWrite(fileOperation_t * fileOp);

// Warning: list_dir() is in spi_fatfs.c - how to declare it and reuse it?
FRESULT list_dir (const char *path);

static FRESULT create_deployment_folder(void);

static FRESULT load_configuration(const char *filename);
static FRESULT save_configuration(const char *filename);

/*************************************** External variables *******************************************/


/*************************************** Local variables *******************************************/

static APP_WAKE_REASON_E woken;

// This is the handle of the task
TaskHandle_t 		fatFs_task_id;
QueueHandle_t     	xFatTaskQueue;
extern QueueHandle_t     xIfTaskQueue;
extern QueueHandle_t     xImageTaskQueue;

// These are the handles for the input queues of Task2. So we can send it messages
//extern QueueHandle_t     xFatTaskQueue;

volatile APP_FATFS_STATE_E fatFs_task_state = APP_FATFS_STATE_UNINIT;

static FATFS fs;             /* Filesystem object */

static bool mounted;

static TickType_t xStartTime;

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char * fatFsTaskStateString[APP_FATFS_STATE_NUMSTATES] = {
		"Uninitialised",
		"Idle",
		"Busy"
};

// Strings for expected messages. Values must match messages directed to fatfs Task in app_msg.h
const char* fatFsTaskEventString[APP_MSG_FATFSTASK_LAST - APP_MSG_FATFSTASK_WRITE_FILE] = {
		"Write file",
		"Read file",
		"File op done",
		"Save State",
};

// Number of pictures to take after motion detect wake
uint32_t numPicturesToGrab = NUMPICTURESTOGRAB;

// Interval between pictures (for now seconds, but let's change this to ms later
uint32_t pictureInterval = PICTUREINTERVAL;

// Values to read from the configuration.txt file
uint16_t op_parameter[OP_PARAMETER_NUM_ENTRIES];

/********************************** Private Function definitions  *************************************/

/** Another task asks us to write a file for them
 *
 */
static FRESULT fileWrite(fileOperation_t * fileOp) {
	FIL fdst;      		// File object
	FRESULT res;        // FatFs function common result code
	UINT bw;		// Bytes written

	// TODO omit this soon as it might not handle long files or binary files
	xprintf("DEBUG: writing %d bytes to '%s' from address 0x%08x. Contents:\n%s\n",
			fileOp->length, fileOp->fileName, fileOp->buffer, fileOp->buffer );

	res = f_open(&fdst, fileOp->fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (res) {
		xprintf("Fail opening file %s\n", fileOp->fileName);
	    fileOp->length = 0;
		fileOp->res = res;
		return res;
	}

    res = f_write(&fdst, fileOp->buffer, fileOp->length, &bw);
	if (res) {
		xprintf("Fail writing to file %s\n", fileOp->fileName);
	    fileOp->length = bw;
		fileOp->res = res;
		return res;
	}

	//TODO experimental:leave file open so it can be appended? TODO need to make stuff static?
	if (fileOp->closeWhenDone) {
		res = f_close(&fdst);

		if (res) {
			xprintf("Fail closing file %s\n", fileOp->fileName);
		    fileOp->length = bw;
			fileOp->res = res;
			return res;
		}
	}

    if  (bw != (fileOp->length)) {
    	xprintf("Error. Wrote %d bytes rather than %d\n", bw, fileOp->length);
    	res =FR_DISK_ERR;	// TODO find a better error code? Disk full?
    }
    else {
    	xprintf("Wrote %d bytes\n", bw);
    	res = FR_OK;
    }
	XP_GREEN
	xprintf("Wrote file to SD %s\n", fileOp->fileName);
	XP_WHITE;
	fileOp->res = res;
	return res;
}

/** Image writing function, will primiliarly be called from the image task
 * 		parameters: fileOperation_t fileOp
 * 		returns: FRESULT res
 */
static FRESULT fileWriteImage(fileOperation_t * fileOp) {
	FRESULT res;
	rtc_time time;

	// fastfs_write_image() expects filename is a uint8_t array
	// TODO resolve this warning! "warning: passing argument 1 of 'fastfs_write_image' makes integer from pointer without a cast"
	res = fastfs_write_image( (uint32_t) (fileOp->buffer), fileOp->length, (uint8_t * ) fileOp->fileName);
	if (res != FR_OK) {
		xprintf("Error writing file %s\n", fileOp->fileName);
		fileOp->length = 0;
		fileOp->res = res;
		return res;
	}

	XP_GREEN
	xprintf("Wrote image to SD: %s ", fileOp->fileName);
	XP_WHITE;

	// Printing the time seems redundant, as the time is already in the file name
	exif_utc_get_rtc_as_time(&time);

	xprintf("at %d:%d:%d %d/%d/%d\n",
			time.tm_hour, time.tm_min, time.tm_sec,
			time.tm_mday, time.tm_mon, time.tm_year);

	return res;
}


/** Another task asks us to read a file for them
 *
 */
static FRESULT fileRead(fileOperation_t * fileOp) {
	FIL fsrc;      		// File object
	FRESULT res;        // FatFs function common result code
	UINT br;			// Bytes read

//	xprintf("DEBUG: reading file %s to buffer at address 0x%08x (%d bytes)\n",
//			fileOp->fileName, fileOp->buffer, fileOp->length);

	res = f_open(&fsrc, fileOp->fileName, FA_READ);
	if (res) {
		xprintf("Fail opening file %s\n", fileOp->fileName);
	    fileOp->length = 0;
		fileOp->res = res;
		return res;
	}

	//Read a chunk of data from the source file
	res = f_read(&fsrc, fileOp->buffer, fileOp->length, &br);

	//TODO experimental: leave file open so it can be appended? TODO need to make stuff static?
	if (fileOp->closeWhenDone) {
		res = f_close(&fsrc);

		if (res) {
			xprintf("Fail closing file %s\n", fileOp->fileName);
		    fileOp->length = 0;
			fileOp->res = res;
			return res;
		}
	}

    xprintf("Read %d bytes\n", br);
    fileOp->length = br;
	fileOp->res = res;
    return res;
}


/**
 * Implements state machine when in APP_FATFS_STATE_UNINIT
 *
 * If disk operation requests happen they are
 *
 */
static APP_MSG_DEST_T handleEventForUninit(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	FRESULT res;
	fileOperation_t * fileOp;
	static APP_MSG_DEST_T sendMsg;

	sendMsg.destination = NULL;
	res = FR_OK;

	event = rxMessage.msg_event;

	fileOp = (fileOperation_t *) rxMessage.msg_data;

	switch (event) {

	case APP_MSG_FATFSTASK_WRITE_FILE:
		// someone wants a file written. Send back an error message

    	// Inform the if task that the disk operation is complete
    	sendMsg.message.msg_data = (uint32_t) FR_NO_FILESYSTEM;
    	sendMsg.destination = fileOp->senderQueue;
    	// The message to send depends on the destination! In retrospect it would have been better
    	// if the messages were grouped by the sender rather than the receiver, so this next test was not necessary:
    	if (sendMsg.destination == xIfTaskQueue) {
        	sendMsg.message.msg_event = APP_MSG_IFTASK_DISK_WRITE_COMPLETE;
    	}
    	else if (sendMsg.destination == xImageTaskQueue) {
			sendMsg.message.msg_event = APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE;
		}
//    	// Complete this as necessary
//    	else if (sendMsg.destination == anotherTaskQueue) {
//        	sendMsg.message.msg_event = APP_MSG_ANOTHERTASK_DISK_WRITE_COMPLETE;
//    	}
    	else {
    		// assumed to be CLI task.
        	sendMsg.message.msg_event = APP_MSG_CLITASK_DISK_WRITE_COMPLETE;
    	}
		break;

	case APP_MSG_FATFSTASK_READ_FILE:
		// someone wants a file read. Send back an error message

    	// Inform the if task that the disk operation is complete
    	sendMsg.message.msg_data = (uint32_t) FR_NO_FILESYSTEM;
    	sendMsg.destination = fileOp->senderQueue;
    	// The message to send depends on the destination! In retrospect it would have been better
    	// if the messages were grouped by the sender rather than the receiver, so this next test was not necessary:
    	if (sendMsg.destination == xIfTaskQueue) {
        	sendMsg.message.msg_event = APP_MSG_IFTASK_DISK_READ_COMPLETE;
    	}
//    	// Complete this as necessary
//    	else if (sendMsg.destination == anotherTaskQueue) {
//        	sendMsg.message.msg_event = APP_MSG_ANOTHERTASK_DISK_READ_COMPLETE;
//    	}
    	else {
    		// assumed to be CLI task.
        	sendMsg.message.msg_event = APP_MSG_CLITASK_DISK_READ_COMPLETE;
    	}

		break;

	case APP_MSG_FATFSTASK_SAVE_STATE:
		// Save the state of the imageSequenceNumber
		// This is the last thing we will do before sleeping.

    	sendMsg.message.msg_data = (uint32_t) FR_NO_FILESYSTEM;

		// Signal to the caller that it may enter DPD.
    	sendMsg.destination = xImageTaskQueue; // fileOp->senderQueue;
    	sendMsg.message.msg_event = APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE;
       	sendMsg.message.msg_data = (uint32_t) res;

		break;

	case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}
	return sendMsg;
}


/**
 * Implements state machine when in APP_FATFS_STATE_IDLE
 *
 */
static APP_MSG_DEST_T handleEventForIdle(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	FRESULT res;
	fileOperation_t * fileOp;
	static APP_MSG_DEST_T sendMsg;

	sendMsg.destination = NULL;
	res = FR_OK;

	event = rxMessage.msg_event;

	fileOp = (fileOperation_t *) rxMessage.msg_data;

	switch (event) {

	case APP_MSG_FATFSTASK_WRITE_FILE:
		// someone wants a file written. Structure including file name a buffer is passed in data
    	fatFs_task_state = APP_FATFS_STATE_BUSY;
    	xStartTime = xTaskGetTickCount();

		if( fileOp->senderQueue == xImageTaskQueue) {
			//writes image
			res = fileWriteImage(fileOp);
		}
		else {
			//writes file
			res = fileWrite(fileOp);
		}

		xprintf("File write took %dms\n", (xTaskGetTickCount() - xStartTime) * portTICK_PERIOD_MS );

    	fatFs_task_state = APP_FATFS_STATE_IDLE;

    	// Inform the if task that the disk operation is complete
    	sendMsg.message.msg_data = (uint32_t) res;
    	sendMsg.destination = fileOp->senderQueue;
    	// The message to send depends on the destination! In retrospect it would have been better
    	// if the messages were grouped by the sender rather than the receiver, so this next test was not necessary:
    	if (sendMsg.destination == xIfTaskQueue) {
        	sendMsg.message.msg_event = APP_MSG_IFTASK_DISK_WRITE_COMPLETE;
    	}
    	else if (sendMsg.destination == xImageTaskQueue) {
			sendMsg.message.msg_event = APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE;
		}
//    	// Complete this as necessary
//    	else if (sendMsg.destination == anotherTaskQueue) {
//        	sendMsg.message.msg_event = APP_MSG_ANOTHERTASK_DISK_WRITE_COMPLETE;
//    	}
    	else {
    		// assumed to be CLI task.
        	sendMsg.message.msg_event = APP_MSG_CLITASK_DISK_WRITE_COMPLETE;
    	}
		break;

	case APP_MSG_FATFSTASK_READ_FILE:
		// someone wants a file read
    	fatFs_task_state = APP_FATFS_STATE_BUSY;
    	xStartTime = xTaskGetTickCount();
		res = fileRead(fileOp);

		xprintf("Elapsed time (fileRead) %dms. Result code %d\n", (xTaskGetTickCount() - xStartTime) * portTICK_PERIOD_MS, res );

    	fatFs_task_state = APP_FATFS_STATE_IDLE;

    	// Inform the if task that the disk operation is complete
    	sendMsg.message.msg_data = (uint32_t) res;
    	sendMsg.destination = fileOp->senderQueue;
    	// The message to send depends on the destination! In retrospect it would have been better
    	// if the messages were grouped by the sender rather than the receiver, so this next test was not necessary:
    	if (sendMsg.destination == xIfTaskQueue) {
        	sendMsg.message.msg_event = APP_MSG_IFTASK_DISK_READ_COMPLETE;
    	}
//    	// Complete this as necessary
//    	else if (sendMsg.destination == anotherTaskQueue) {
//        	sendMsg.message.msg_event = APP_MSG_ANOTHERTASK_DISK_READ_COMPLETE;
//    	}
    	else {
    		// assumed to be CLI task.
        	sendMsg.message.msg_event = APP_MSG_CLITASK_DISK_READ_COMPLETE;
    	}

		break;

	case APP_MSG_FATFSTASK_SAVE_STATE:
		// Save the state of the imageSequenceNumber
		// This is the last thing we will do before sleeping.
		if (fatfs_getOperationalParameter(OP_PARAMETER_SEQUENCE_NUMBER) > 0) {
			res = save_configuration(STATE_FILE);
			f_unmount(DRV);

			if (res) {
				xprintf("Error %d saving state\n", res);
			}
			else{
				xprintf("Saved state to SD card. Image sequence number = %d\n",
						fatfs_getImageSequenceNumber());
			}
		}

		// Signal to the caller that it may enter DPD.
    	sendMsg.destination = xImageTaskQueue; // fileOp->senderQueue;
    	sendMsg.message.msg_event = APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE;
       	sendMsg.message.msg_data = (uint32_t) res;

		break;

	case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}
	return sendMsg;
}

/**
 * Implements state machine when in APP_FATFS_STATE_BUSY
 *
 */
static APP_MSG_DEST_T handleEventForBusy(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {

	case APP_MSG_FATFSTASK_DONE:
		// someone wants a file written
    	fatFs_task_state = APP_FATFS_STATE_IDLE;
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * For state machine: Print a red message to see if there are unhandled events we should manage
 */
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;

	XP_LT_RED;
	if ((event >= APP_MSG_IFTASK_FIRST) && (event < APP_MSG_IFTASK_LAST)) {
		xprintf("FatFS Task unhandled event '%s' in '%s'\r\n", fatFsTaskEventString[event - APP_MSG_IFTASK_FIRST], fatFsTaskStateString[fatFs_task_state]);
	}
	else {
		xprintf("FatFS Task unhandled event 0x%04x in '%s'\r\n", event, fatFsTaskStateString[fatFs_task_state]);
	}
	XP_WHITE;

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Initialise FatFS system
 *
 * Earlier versions (e.g. in spi_fatfs.c) initialise the PB2-5 pins used for the SD card interface,
 * but this largely replicated code in spi_m_pinmux_cfg() (called from pinmux_init() via app_main())
 * and I think it is cleaner if the pin setup is done there, in one place.
 *
 * It looks like the low-level drivers are done in mmc_we2_spi.c, selected by these lines in the makefile:
 * 	MID_SEL = fatfs
 * 	FATFS_PORT_LIST = mmc_spi
 *
 * Note that although the hardware includes a card detect signal, card detect is always true:
 * 	#define MMC_CD()    1
 * And the lower-level SPI operations use ARM_DRIVER_SPI in Driver_SPI.h
 *
 * It is not clear to me what links there is between the hardware and the chip's SPI block, and the ARM driver...
 * Though there are calls to SSPI_CS_GPIO_Pinmux(), SSPI_CS_GPIO_Output_Level() and SSPI_CS_GPIO_Dir()
 * which are obviously used to control the /CS pin.
 */
static FRESULT fatFsInit(void) {
    FRESULT res;

    XP_CYAN;
    xprintf("Mounting FatFS on SD card ");
    XP_WHITE;

    // This is probably blocking...
    res = f_mount(&fs, DRV, 1);

    if (res) {
    	XP_RED;
        xprintf("Failed error = %d\r\n", res);
        XP_WHITE;
        mounted = false;
    }
    else {
        xprintf("OK\n");
        mounted = true;
    }
    return res;
}

/**
 * Looks for a directory to save images.
 * If it exists, use it. Otherwise, create it.
 */
static FRESULT create_deployment_folder(void) {
	FRESULT res;
	FILINFO fno;
	// TODO stop using magic numbers
	char file_dir[128];
	UINT len = 128;

	res = f_getcwd(file_dir, len); /* Get current directory */
	if (res) {
		printf("f_getcwd() failed (%d)\r\n", res);
		return res;
	}
	else {
	    printf("Current directory is '%s'\r\n", file_dir);
	}

    // I want all files in the same directory
    xsprintf(file_dir, CAPTURE_DIR);

    res = f_stat(file_dir, &fno);
    if (res == FR_OK) {
        printf("Directory '%s' exists\r\n", file_dir);
    }
    else {
        printf("Create directory %s\r\n", file_dir);
        res = f_mkdir(file_dir);
        if (res != FR_OK) {
        	printf("f_mkdir() failed (%d)\r\n", res);
        	return res;
        }
    }

    printf("Change directory to '%s'.\r\n", file_dir);
    res = f_chdir(file_dir);
	if (res) {
		printf("ff_chdir() failed (%d)\r\n", res);
		return res;
	}

    res = f_getcwd(file_dir, len);
	if (res) {
		printf("f_getcwd() failed (%d)\r\n", res);
	}
	else {
	    printf("Current directory is '%s'\r\n", file_dir);
	}

	return res;
}

/**
 * Loads configuration information from a file.
 *
 * The file comprises several lines each with two integers.
 * The first integer is an index into the configuration[] array.
 * The second integer is the value to place into the array.
 *
 * Default values for configuration[] are set in the task initialisation.
 *
 * @param file name
 * @return error code
 */
static FRESULT load_configuration(const char *filename) {
    FIL file;
    FRESULT res;
    char line[64];
    char *token;
    //int index, value;
    uint8_t index;
    uint16_t value;

    if (!fatfs_mounted()) {
        xprintf("SD card not mounted.\n");
    	return FR_NO_FILESYSTEM;
    }

    // Open the file
    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        return res;  // File not found or error opening
    }

    // Read lines from the file
    while (f_gets(line, sizeof(line), &file)) {
        // Remove trailing newline if present
        char *newline = strchr(line, '\n');
        if (newline) {
        	*newline = '\0';
        }

        // Skip comments which start with #
        if (line[0] == '#') {
        	continue;
        }

        // The first call to strtok() should have a pointer to the string which
        // should be split, while any following calls should use NULL as an
        // argument. Each time the function is called a pointer to a different
        // token is returned until there are no more tokens.
        // At that point each function call returns NULL.
        token = strtok(line, " ");
        if (token == NULL) {
        	continue;
        }

        index = (uint8_t) atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL) {
        	continue;
        }

        value = (uint16_t) atoi(token);

        // Set array value if index is in range
        if (index >= 0 && index < OP_PARAMETER_NUM_ENTRIES) {
            op_parameter[index] = value;
            //xprintf("   op_parameter[%d] = %d\n", index, value);
        }
    }

    f_close(&file);
    return FR_OK;
}

static FRESULT save_configuration(const char *filename) {
    static FIL file;
    FRESULT res;
    UINT bytesWritten;
    char line[MAXCOMMENTLENGTH];
    char comment_lines[MAXNUMCOMMENTS][MAXCOMMENTLENGTH];
    uint16_t comment_count = 0;

    if (!fatfs_mounted()) {
        xprintf("SD card not mounted.\n");
    	return FR_NO_FILESYSTEM;
    }

    // --- First Pass: Read and store comment lines ---
    res = f_open(&file, filename, FA_READ);
    if (res == FR_OK) {
        while (f_gets(line, sizeof(line), &file)) {
            if (line[0] == '#') {
                if (comment_count < MAXNUMCOMMENTS) {
                    strncpy(comment_lines[comment_count], line, MAXCOMMENTLENGTH);
                    comment_lines[comment_count][MAXCOMMENTLENGTH - 1] = '\0'; // ensure null-termination
                    comment_count++;
                }
                else {
                    break; // Reached comment storage limit
                }
            }
        }
        f_close(&file);
    }
    else if (res != FR_NO_FILE) {
        return res;  // Error reading file (not just "file not found")
    }

    // --- Second Pass: Open file for write (overwrite) ---
    res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        return res;
    }

    // Write preserved comment lines
    for (uint16_t i = 0; i < comment_count; i++) {
        f_write(&file, comment_lines[i], strlen(comment_lines[i]), &bytesWritten);
    }

    // Write updated index-value pairs
    for (uint8_t i = 0; i < OP_PARAMETER_NUM_ENTRIES; i++) {
        snprintf(line, sizeof(line), "%d %d\n", i, op_parameter[i]);
        f_write(&file, line, strlen(line), &bytesWritten);
    }

    f_close(&file);
    return FR_OK;
}

/********************************** FreeRTOS Task  *************************************/

/**
* FreeRTOS task responsible for handling interactions with the FatFS
*
* This is called when the scheduler starts.
* Various entities have already be set up by fatfs_createTask()
*
* After some one-off activities it waits for events to arrive in its xFatTaskQueue
*/
static void vFatFsTask(void *pvParameters) {
    APP_MSG_T       rxMessage;
    APP_MSG_DEST_T  txMessage;
	QueueHandle_t   targetQueue;
	FRESULT 		res;
	uint16_t		inactivityPeriod;
	APP_MSG_T 		sendMsg;

    APP_FATFS_STATE_E old_state;
	const char * eventString;
	APP_MSG_EVENT_E event;
	uint32_t rxData;
	bool enabled;

    XP_CYAN;
    // Observing these messages confirms the initialisation sequence
    xprintf("Starting FatFS Task\n");
    XP_WHITE;

    // Initialise the configuration[] array
    // TODO - there are more efficient ways to do this...
    op_parameter[OP_PARAMETER_SEQUENCE_NUMBER] = 0;		// 0 indicates no SD card
    op_parameter[OP_PARAMETER_NUM_PICTURES] = NUMPICTURESTOGRAB;
    op_parameter[OP_PARAMETER_PICTURE_INTERVAL] = PICTUREINTERVAL;
    op_parameter[OP_PARAMETER_TIMELAPSE_INTERVAL] = TIMELAPSEINTERVAL;
    op_parameter[OP_PARAMETER_INTERVAL_BEFORE_DPD] = INACTIVITYTIMEOUT;
    op_parameter[OP_PARAMETER_LED_FLASH_DUTY] = FLASHLEDDUTY;
    op_parameter[OP_PARAMETER_NUM_NN_ANALYSES] = 0;
    op_parameter[OP_PARAMETER_NUM_COLD_BOOTS] = 0;
    op_parameter[OP_PARAMETER_NUM_WARM_BOOTS] = 0;
    op_parameter[OP_PARAMETER_CAMERA_ENABLED] = 0;	// disabled

	// One-off initialisation here...
	res = fatFsInit();

    if (res == FR_OK ) {
    	fatFs_task_state = APP_FATFS_STATE_IDLE;
    	// Only if the file system is working should we add CLI commands for FATFS
    	cli_fatfs_init();

    	res = create_deployment_folder();

    	if (res == FR_OK) {

    		xprintf("SD card initialised. ");

    		// Load all the saved configuration values, including the image sequence number
    		res = load_configuration(STATE_FILE);
    	    if ( res == FR_OK ) {
    	    	// File exists and op_parameter[] has been initialised
    	    	enabled = op_parameter[OP_PARAMETER_CAMERA_ENABLED];
    	    	xprintf("'%s' found. (Next image #%d), camera %senabled.\r\n",
    	    			STATE_FILE,
						fatfs_getImageSequenceNumber(),
						(enabled == 1)? "":"not ");
    	    }
    	    else {
    	    	fatfs_setOperationalParameter(OP_PARAMETER_SEQUENCE_NUMBER, 1);
    	    	xprintf("'%s' NOT found. (Next image #1)\r\n", STATE_FILE);
    	    }
    	}
    }
    else {
    	// Failure.
    	xprintf("SD card initialisation failed (reason %d)\r\n", res);
    }

	// Start a timer that detects inactivity in every task, exceeding op_parameter[OP_PARAMETER_INTERVAL_BEFORE_DPD]
	if (woken == APP_WAKE_REASON_COLD) {
		// Short timeout after cold boot.
		inactivityPeriod = INACTIVITYTIMEOUTCB;
		fatfs_incrementOperationalParameter(OP_PARAMETER_NUM_COLD_BOOTS);
	}
	else {
		inactivityPeriod = op_parameter[OP_PARAMETER_INTERVAL_BEFORE_DPD];
		fatfs_incrementOperationalParameter(OP_PARAMETER_NUM_WARM_BOOTS);
	}

	xprintf("Inactivity period set at %dms\n", inactivityPeriod);
	inactivity_init(inactivityPeriod,  app_onInactivityDetection);

	// Now the Operation Parameters are loaded, send a message to the BLE processor
	sendMsg.msg_event = APP_MSG_IFTASK_AWAKE;
	sendMsg.msg_data = 0;
	sendMsg.msg_parameter = 0;

	// But wait a short time if it is a cold boot, so the BLE processor is initialised and ready
	if (woken == APP_WAKE_REASON_COLD) {
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	if (xQueueSend(xIfTaskQueue, (void *)&sendMsg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("sendMsg=0x%x fail\r\n", sendMsg.msg_event);
	}

	// The task loops forever here, waiting for messages to arrive in its input queue
	for (;;)  {
		if (xQueueReceive ( xFatTaskQueue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {
			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

			// convert event to a string
			if ((event >= APP_MSG_FATFSTASK_FIRST) && (event < APP_MSG_FATFSTASK_LAST)) {
				eventString = fatFsTaskEventString[event - APP_MSG_FATFSTASK_FIRST];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nFatFS Task ");
			XP_WHITE;
			xprintf("received event '%s' (0x%04x). Rx data = 0x%08x\r\n", eventString, event, rxData);\

			old_state = fatFs_task_state;

    		// switch on state - and call individual event handling functions
    		switch (fatFs_task_state) {

    		case APP_FATFS_STATE_UNINIT:
    			txMessage = handleEventForUninit(rxMessage);
    			break;

    		case APP_FATFS_STATE_IDLE:
    			txMessage = handleEventForIdle(rxMessage);
    			break;

    		case APP_FATFS_STATE_BUSY:
    			txMessage = handleEventForBusy(rxMessage);
    			break;

    		default:
    			// should not happen
    			txMessage = flagUnexpectedEvent(rxMessage);
    			break;
    		}

    		if (old_state != fatFs_task_state) {
    			// state has changed
        		XP_LT_CYAN;
        		xprintf("FatFS Task state changed ");
        		XP_WHITE;
        		xprintf("from '%s' (%d) to '%s' (%d)\r\n",
        				fatFsTaskStateString[old_state], old_state,
						fatFsTaskStateString[fatFs_task_state], fatFs_task_state);
    		}

    		// The processing functions might want us to send a message to another task
    		if (txMessage.destination != NULL) {
    			sendMsg = txMessage.message;
    			targetQueue = txMessage.destination;

    			if(xQueueSend( targetQueue , (void *) &sendMsg , __QueueSendTicksToWait) != pdTRUE) {
    				xprintf("FatFS task sending event 0x%x failed\r\n", sendMsg.msg_event);
    			}
    			else {
    				xprintf("FatFS task sending event 0x%04x. Tx data = 0x%08x\r\n", sendMsg.msg_event, sendMsg.msg_data);
    			}
    		}
        }
	}	// for(;;)
}

/********************************** Public Functions  *************************************/

/**
 * Creates the FatFS task.
 *
 * The task itself initialises the FatFS and then manages requests to access it.
 *
 * Not sure how big the stack needs to be...
 */

TaskHandle_t fatfs_createTask(int8_t priority, APP_WAKE_REASON_E wakeReason) {

	if (priority < 0){
		priority = 0;
	}

    // Save this. Determines inactivity period at cold boot
    woken = wakeReason;

	xFatTaskQueue  = xQueueCreate( FATFS_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xFatTaskQueue == 0) {
		xprintf("Failed to create xFatTaskQueue\n");
		configASSERT(0);	// TODO add debug messages?
	}

	if (xTaskCreate(vFatFsTask, (const char *)"FAT",
			3 * configMINIMAL_STACK_SIZE + CLI_CMD_LINE_BUF_SIZE + CLI_OUTPUT_BUF_SIZE,
			NULL, priority,
			&fatFs_task_id) != pdPASS)  {
		xprintf("Failed to create vFatFsTask\n");
		configASSERT(0);	// TODO add debug messages?
	}

	return fatFs_task_id;
}

/**
 * Returns the internal state as a number
 */
uint16_t fatfs_getState(void) {
	return fatFs_task_state;
}

/**
 * Returns true of the filesystem is mounted
 *
 * Use thsi before attempting disk operations, if unsure
 *
 * @return true if mounted
 */
bool fatfs_mounted(void) {
	return mounted;
}

/**
 * Returns the internal state as a string
 */
const char * fatfs_getStateString(void) {
	return * &fatFsTaskStateString[fatFs_task_state];
}

/**
 * Get one of the Operational Parameters
 *
 * Typically these are saved in a file on SD card and can be read to set operational
 * behaviour at boot time.
 *
 * Values are set to a default and can be over-written by raeding from a file
 * or by the setter function.
 *
 * @param parameter - one of a list of possible parameters
 * @return - the value (or 0 if parameter is not recognised)
 */
uint16_t fatfs_getOperationalParameter(OP_PARAMETERS_E parameter) {

	if ((parameter >= 0) && (parameter < OP_PARAMETER_NUM_ENTRIES)) {
		return op_parameter[parameter];
	}
	else {
		return 0;
	}
}

/**
 * Get the image sequence number
 *
 * Short-hand version of fatfs_getOperationalParameter(OP_PARAMETER_SEQUENCE_NUMBER)
 */
uint16_t fatfs_getImageSequenceNumber(void) {
	return op_parameter[OP_PARAMETER_SEQUENCE_NUMBER];
}

/**
 * Set one of the Operational Parameters
 *
 * Typically these are saved in a file on SD card and can be read to set operational
 * behaviour at boot time.
 *
 * Values are set to a default and can be over-written by reading from a file
 * or by the setter function.
 *
 * Values should be saved to the SD card before entering DPD
 *
 * @param parameter - one of a list of possible parameters
 * @param value - the value
 */
void fatfs_setOperationalParameter(OP_PARAMETERS_E parameter, int16_t value) {

	if ((parameter >= 0) && (parameter < OP_PARAMETER_NUM_ENTRIES)) {
		op_parameter[parameter] = value;
	}
	else {
		// error
	}
}

/**
 * Increment one of teh state variables
 */
void fatfs_incrementOperationalParameter(OP_PARAMETERS_E parameter) {
	if ((parameter >= 0) && (parameter < OP_PARAMETER_NUM_ENTRIES)) {
		// TODO - do we need to prevent roll-over?
		op_parameter[parameter]++;
	}
	else {
		// error
	}
}


