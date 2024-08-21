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
 * The other approach is to use the
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
 */

/*************************************** Includes *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#include "printf_x.h"
#include "xprintf.h"

// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "fatfs_task.h"
#include "app_msg.h"
#include "CLI-commands.h"
#include "ww130_cli.h"
#include "ff.h"
#include "CLI-FATFS-commands.h"

// TODO I am not using the public functions in this. Can we move the important bits of this to here?
#include "spi_fatfs.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define fatfs_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define FATFS_TASK_QUEUE_LEN   		10

#define DRV         ""
#define CAPTURE_DIR "CaptureImage"

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vFatFsTask(void *pvParameters);

static FRESULT printDiskInfo(bool printDiskInfo);
static FRESULT fatFsInit(void);
static FRESULT scan_files (char* path);
static FRESULT list_dir (const char *path);

// These are separate event handlers, one for each of the possible state machine state
//static APP_MSG_DEST_T  handleEventForInit(APP_MSG_T rxMessage);
//static APP_MSG_DEST_T  handleEventForState2(APP_MSG_T rxMessage);
//static APP_MSG_DEST_T  handleEventForState3(APP_MSG_T rxMessage);
//static APP_MSG_DEST_T  handleEventForError(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T  flagUnexpectedEvent(APP_MSG_T rxMessage);



/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t 		fatFs_task_id;

QueueHandle_t     	xFatTaskQueue;

// These are the handles for the input queues of Task2. So we can send it messages
//extern QueueHandle_t     xFatTaskQueue;

volatile APP_FATFS_STATE_E fatFs_task_state = APP_FATFS_STATE_UNINIT;

static FATFS fs;             /* Filesystem object */

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char * fatFsTaskStateString[APP_FATFS_STATE_ERROR + 1] = {
		"Uninitialised",
		"Idle",
		"I2C RX State",
		"I2C TX State",
		"PA0 State",
		"Error"
};

// Strings for expected messages. Values must match Messages directed to IF Task in app_msg.h
// Experiment: If I define xxx_FIRST and xx_LAST a I have done, then this should work:

// TODO - FIXME

const char* fatFsTaskEventString[APP_MSG_IFTASK_LAST - APP_MSG_IFTASK_FIRST] = {
		"I2C Rx",
		"I2C Tx",
		"I2C Error",
		"CLI response",
		"PA0 Interrupt In",
		"PA0 Interrupt Out",
		"PA0 Timer",
		"MM Timer",
};

/********************************** Private Function definitions  *************************************/


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
 * Note that although the hardware includes a card detect signal, card detect is alwasy true:
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


/**
 * Borrowed from ww130_test
 * where it was in the fatfs_init() routine
 */
static FRESULT printDiskInfo(bool printDiskInfo) {
    FRESULT res;        /* API result code */
    FILINFO fno;
    char file_dir[20];
    UINT file_dir_idx = 0;
    //char filename[20];
    //char filecontent[256];
    char cur_dir[128];
    UINT len = 128;

    // This scans the disk and prints all directories and files
    // Let's add a switch so we only do it once
    if (printDiskInfo) {
    	res = f_getcwd(cur_dir, len);      /* Get current directory */
    	if (res)  {
    		XP_RED;
    		printf("f_getcwd res = %d\r\n", res);
    		XP_WHITE;
    		return res;	// exit with the error code
    	}
    	else  {
    		printf("cur_dir = %s\r\n", cur_dir);
    	}

    	res = list_dir(cur_dir);
    	if (res)  {
    		XP_RED;
    		printf("list_dir res = %d\r\n", res);
    		XP_WHITE;
    		return res;	// exit with the error code
    	}

    	res = scan_files(cur_dir);
    	if (res)  {
    		XP_RED;
    		printf("scan_files res = %d\r\n", res);
    		XP_WHITE;
    		return res;	// exit with the error code
    	}
    }
    else {
    	printf("Initialising fatfs - searching for a directory:\r\n");
    }

    while ( 1 )  {
    	xsprintf(file_dir, "%s%04d", CAPTURE_DIR, file_dir_idx);
    	res = f_stat(file_dir, &fno);
    	if (res == FR_OK)  {
    		// Don't print this as we get a large number of directories quickly...
    		//printf("Directory '%s' exists.\r\n", file_dir);
    		file_dir_idx++;
    	}
    	else {
    		printf("Create directory '%s'\r\n", file_dir);
    		res = f_mkdir(file_dir);
            if (res) { printf("f_mkdir res = %d\r\n", res); }

            //printf("Change directory '%s'\r\n", file_dir);
            res = f_chdir(file_dir);

            res = f_getcwd(cur_dir, len);      /* Get current directory */
            //printf("cur_dir = %s\r\n", cur_dir);
            break;
        }
    }

    // TODO will this aloways be the right return value?
    return FR_OK;
}


/* List contents of a directory */
static FRESULT list_dir (const char *path) {
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
/* Start node to be scanned (***also used as work area***) */
static FRESULT scan_files (char* path) {
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


/**
 * FreeRTOS task responsible for handling interactions with the FatFS
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

	// CLI for FATFS
	cli_fatfs_init();

	// One-off initialisation here...
	res = fatFsInit();

    if ( res == FR_OK ) {
    	fatFs_task_state = APP_FATFS_STATE_IDLE;
    	printDiskInfo(true);
    }
    else {
    	fatFs_task_state = APP_FATFS_STATE_ERROR;
        xprintf("Fat FS init fail (reason %d)\r\n", res);
    }

	// The task loops forever here, waiting for messages to arrive in its input queue
	for (;;)  {
		if (xQueueReceive ( xFatTaskQueue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {
			// convert event to a string
			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

			if ((event >= APP_MSG_IFTASK_FIRST) && (event < APP_MSG_IFTASK_LAST)) {
				eventString = fatFsTaskEventString[event - APP_MSG_IFTASK_FIRST];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nFatFS Task");
			XP_WHITE;
			xprintf(" received event '%s' (0x%04x). Value = 0x%08x\r\n", eventString, event, rxData);\

			old_state = fatFs_task_state;

    		// switch on state - and call individual event handling functions
    		switch (fatFs_task_state) {

    		case APP_FATFS_STATE_UNINIT:
    			txMessage = flagUnexpectedEvent(rxMessage);
    			break;
//
//    		case APP_FATFS_STATE_IDLE:
//    			//txMessage = handleEventForIdle(rxMessage);
//    			break;
//
//    		case APP_FATFS_STATE_I2C_RX:
//    			//txMessage = handleEventForStateI2CRx(rxMessage);
//    			break;
//
//    		case APP_FATFS_STATE_I2C_TX:
//    			//txMessage = handleEventForStateI2CTx(rxMessage);
//    			break;
//
//    		case APP_FATFS_STATE_PA0:
//    			//txMessage = handleEventForStatePA0(rxMessage);
//    			break;
//
//    		case APP_FATFS_STATE_ERROR:
//    			//txMessage = handleEventForError(rxMessage);
//    			break;

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
    				xprintf("send send_msg=0x%x fail\r\n", send_msg.msg_event);
    			}
    			else {
    				xprintf("State machine sending event 0x%04x. Value = 0x%08x\r\n", send_msg.msg_event, send_msg.msg_data);
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
void fatfs_createTask(void) {

	xFatTaskQueue  = xQueueCreate( FATFS_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xFatTaskQueue == 0) {
		xprintf("Failed to create xFatTaskQueue\n");
		configASSERT(0);	// TODO add debug messages?
	}

	if (xTaskCreate(vFatFsTask, (const char *)"FAT",
			3 * configMINIMAL_STACK_SIZE + CLI_CMD_LINE_BUF_SIZE + CLI_OUTPUT_BUF_SIZE,
			NULL, tskIDLE_PRIORITY+1, &fatFs_task_id) != pdPASS)  {
		xprintf("Failed to create vFatFsTask\n");
		configASSERT(0);	// TODO add debug messages?
	}
}

