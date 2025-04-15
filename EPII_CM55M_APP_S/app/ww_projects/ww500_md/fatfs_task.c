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
#include "app_msg.h"
#include "CLI-commands.h"
#include "ww500_md.h"
#include "ff.h"
#include "CLI-FATFS-commands.h"
#include "time_handling.h"

// TODO I am not using the public functions in this. Can we move the important bits of this to here?
#include "spi_fatfs.h"
#include "hx_drv_rtc.h"
#include "exif_utc.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define fatfs_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define FATFS_TASK_QUEUE_LEN   		10

#define DRV         ""

#ifdef ALT_FILENAMES
#define CAPTURE_DIR "HM0360_Test"
#define SEQUENCE_FILE "sequence.txt"
#else
#define CAPTURE_DIR "Deployment"
#endif // ALT_FILENAMES

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vFatFsTask(void *pvParameters);

static FRESULT fatFsInit(void);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T  handleEventForIdle(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForBusy(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T  flagUnexpectedEvent(APP_MSG_T rxMessage);


static FRESULT fileRead(fileOperation_t * fileOp);
static FRESULT fileWrite(fileOperation_t * fileOp);

// Warning: list_dir() is in spi_fatfs.c - how to declare it and reuse it?
FRESULT list_dir (const char *path);

static FRESULT create_deployment_folder(void);

#ifdef ALT_FILENAMES
static uint16_t loadSequenceNumber(void);
#endif // ALT_FILENAMES

/*************************************** External variables *******************************************/

extern SemaphoreHandle_t xI2CTxSemaphore;

// TODO - I DONT THINK THIS SHOULD BE A GLOBAL. iT SHOULD BE LOCAL TO THE CALLING FUNCTION!
//extern fileOperation_t *fileOp;

/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t 		fatFs_task_id;
QueueHandle_t     	xFatTaskQueue;
extern QueueHandle_t     xIfTaskQueue;
extern QueueHandle_t     xImageTaskQueue;
//int g_cur_jpegenc_frame = 0; // This variable is in image_task.c - does not belong here.

// These are the handles for the input queues of Task2. So we can send it messages
//extern QueueHandle_t     xFatTaskQueue;

volatile APP_FATFS_STATE_E fatFs_task_state = APP_FATFS_STATE_UNINIT;

static FATFS fs;             /* Filesystem object */

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
};

// If initialised as 0 there is no SD card, or a file system error
uint16_t imageSequenceNumber = 0;

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
//	else{
//		g_cur_jpegenc_frame++;
//	}

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
 * Implements state machine when in APP_FATFS_STATE_IDLE
 *
 */
static APP_MSG_DEST_T handleEventForIdle(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	static APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;
	FRESULT res;
	fileOperation_t * fileOp;

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

		xprintf("Elapsed time (fileWrite) %dms\n", (xTaskGetTickCount() - xStartTime) * portTICK_PERIOD_MS );

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
		xprintf("UNHANDLED event '%s' in '%s'\r\n", fatFsTaskEventString[event - APP_MSG_IFTASK_FIRST], fatFsTaskStateString[fatFs_task_state]);
	}
	else {
		xprintf("UNHANDLED event 0x%04x in '%s'\r\n", event, fatFsTaskStateString[fatFs_task_state]);
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
    }
    else {
        xprintf("OK\n");
    }
    return res;
}

#ifdef ALT_FILENAMES

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
 * Reads sequence number from file or initializes it if missing.
 *
 * @return The sequence number.
 */
static uint16_t loadSequenceNumber(void) {
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

#else
/**
 * Creates the deployment folder for the captured images
 */
static FRESULT create_deployment_folder(void) {
	FRESULT res;
	FILINFO fno;
	char cur_dir[128];
	char deployment_dir[20];
	char images_dir[20];
	UINT len = 128;
	UINT file_dir_idx = 1;

	res = f_getcwd(cur_dir, len); /* Get current directory */
	if (res) {
		printf("f_getcwd() failed (%d)\r\n", res);
		return res;
	}
	else {
	    printf("Current directory is '%s'\r\n", file_dir);
	}

	res = list_dir(cur_dir);
	if (res)
	{
		printf("list_dir() failed (%d)\r\n", res);
	}

	while (1)
	{
		sprintf(deployment_dir, "%s_%04d", CAPTURE_DIR, file_dir_idx);
		res = f_stat(deployment_dir, &fno);
		if (res == FR_OK)
		{
			printf("%s exists, creating next one.\r\n", deployment_dir);
			file_dir_idx++;
		}
		else
		{
			// Create deployment folder
			printf("Create directory %s.\r\n", deployment_dir);
			res = f_mkdir(deployment_dir);
	        if (res != FR_OK) {
	        	printf("f_mkdir() failed (%d)\r\n", res);
	        	return res;
	        }

			res = f_chdir(deployment_dir);
			if (res) {
				printf("f_chdir() failed (%d)\r\n", res);
				return res;
			}

			res = f_getcwd(deployment_dir, len);
			if (res) {
				printf("f_getcwd() failed (%d)\r\n", res);
				return res;
			}
			else {
			    printf("Current directory is '%s'\r\n", file_dir);
			}

			// Create images folder within deployment directory
			sprintf(images_dir, "images");

			xprintf("Create directory '%s' within '%s'\n", images_dir, deployment_dir);
			res = f_mkdir(images_dir);
	        if (res != FR_OK) {
	        	printf("f_mkdir() failed (%d)\r\n", res);
	        	return res;
	        }

			// Change directory to images directory
			res = f_chdir(images_dir);
			printf("Change directory to %s\r\n", images_dir);
			if (res) {
				printf("f_chdir() failed (%d)\r\n", res);
			}

			return res;
	}
}
#endif // ALT_FILENAMES


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
    APP_MSG_T 		send_msg;
	FRESULT res;

    APP_FATFS_STATE_E old_state;
	const char * eventString;
	APP_MSG_EVENT_E event;
	uint32_t rxData;

	// One-off initialisation here...
	res = fatFsInit();

    if ( res == FR_OK ) {
    	fatFs_task_state = APP_FATFS_STATE_IDLE;
    	// Only if the file system is working should we add CLI commands for FATFS
    	cli_fatfs_init();

#ifdef ALT_FILENAMES
    	res = create_deployment_folder();
    	if ( res == FR_OK) {
    		imageSequenceNumber = loadSequenceNumber();
    	}
#else
    	res = create_deployment_folder();
#endif
    }
    if (res) {
    	// Failure.
    	xprintf("SD card initialisation failed (reason %d)\r\n", res);
    }
    else {
    	xprintf("SD card initialised. (next image #%d)\r\n", imageSequenceNumber);
    }

	// The task loops forever here, waiting for messages to arrive in its input queue
	for (;;)  {
		if (xQueueReceive ( xFatTaskQueue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {
			// convert event to a string
			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

			if ((event >= APP_MSG_FATFSTASK_FIRST) && (event < APP_MSG_FATFSTASK_LAST)) {
				eventString = fatFsTaskEventString[event - APP_MSG_FATFSTASK_FIRST];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nFatFS Task event ");
			XP_WHITE;
			xprintf("received '%s' (0x%04x). Value = 0x%08x\r\n", eventString, event, rxData);\

			old_state = fatFs_task_state;

    		// switch on state - and call individual event handling functions
    		switch (fatFs_task_state) {

    		case APP_FATFS_STATE_UNINIT:
    			txMessage = flagUnexpectedEvent(rxMessage);
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
    		if (txMessage.destination == NULL) {
				xprintf("No outgoing messages.\n");
    		}
    		else {
    			send_msg = txMessage.message;
    			targetQueue = txMessage.destination;

    			if(xQueueSend( targetQueue , (void *) &send_msg , __QueueSendTicksToWait) != pdTRUE) {
    				xprintf("FAT task sending event 0x%x failed\r\n", send_msg.msg_event);
    			}
    			else {
    				xprintf("FAT task sending event 0x%04x. Value = 0x%08x\r\n", send_msg.msg_event, send_msg.msg_data);
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

TaskHandle_t fatfs_createTask(int8_t priority) {

	if (priority < 0){
		priority = 0;
	}

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
 * Returns the internal state as a string
 */
const char * fatfs_getStateString(void) {
	return * &fatFsTaskStateString[fatFs_task_state];
}


#ifdef ALT_FILENAMES
/**
 * Gets an integer which increments every time an image file is written.
 *
 * This is a number which makes part of the image file name.
 *
 * @return - an integer. 0 means SD card is not present. otherwise, use the number returned in the file name.
 */
uint16_t fatfs_getImageSequenceNumber(void) {
	return imageSequenceNumber;
}

/**
 * Increment this when the file has been written
 */
void fatfs_incrementImageSequenceNumber(void) {
	imageSequenceNumber++;
}


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
		xprintf("Saved image sequence number as %d\n", sequenceNumber);
        return sequenceNumber;
    }
    else {
    	return 0;
    }
    return 0;
}
#endif // ALT_FILENAMES
