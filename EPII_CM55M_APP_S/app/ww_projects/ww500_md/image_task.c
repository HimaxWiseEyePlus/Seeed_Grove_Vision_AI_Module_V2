
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/time.h>
#include <time.h>

#include "WE2_device.h"
#include "WE2_debug.h"
#include "WE2_core.h"
#include "board.h"
#include "printf_x.h"
#include "hx_drv_pwm.h"

// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif

#include "image_task.h"
#include "fatfs_task.h"
#include "app_msg.h"
#include "CLI-commands.h"
#include "ff.h"
#include "cisdp_sensor.h"
#include "app_msg.h"
#include "hx_drv_pmu.h"
#include "sleep_mode.h"

#include "driver_interface.h"
#include "cvapp.h"
#include "common_config.h"

#include "exif_utc.h"
#include "fatfs_task.h"
#include "ww500_md.h"

#include "cis_file.h"
#include "c_api_types.h"	// Tensorflow errors
#include "hx_drv_scu.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define image_task_PRIORITY (configMAX_PRIORITIES - 2)

#define IMAGE_TASK_QUEUE_LEN 10

// This is experimental. TODO check it is ok
#define MSGTOMASTERLEN 100

// defaults for PWM output on PB9 for Flash LED brightness
// default 20kHz
#define FLASHLEDFREQ 20000

// enable this to leave the PWM running so we can watch it on the scope.
//#define FLASHLEDTEST 1

// Name of file containing extra HM0360 register settings
#define HM0360_EXTRA_FILE "hm0360_extra.bin"

// Name of file containing extra RP register settings
#define RP_EXTRA_FILE "rpv2_extra.bin"

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vImageTask(void *pvParameters);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T handleEventForInit(APP_MSG_T img_recv_msg);
static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T rxMessage);
static APP_MSG_DEST_T handleEventForNNProcessing(APP_MSG_T img_recv_msg);
static APP_MSG_DEST_T handleEventForWaitForTimer(APP_MSG_T img_recv_msg);
static APP_MSG_DEST_T handleEventForSaveState(APP_MSG_T img_recv_msg);

// This is to process an unexpected event
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T rxMessage);

static bool configure_image_sensor(CAMERA_CONFIG_E operation);

// Send unsolicited message to the master
static void sendMsgToMaster(char * str);

static void setFileOpFromJpeg(uint32_t jpeg_sz, uint32_t jpeg_addr, uint32_t frame_num);

// Insert the EXIF metadata into the jpeg buffer
static void insertExif(uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr, int8_t * outCategories, uint16_t categoriesCount);

// When final activity from the FatFS Task and IF Task are complete, enter DPD
static void sleepWhenPossible(void);
static void sleepNow(void);

// Three function that control the behaviour of the GPIO pin that produces the PWM signal for LED brightness
static void flashLEDPWMInit(void);
static void flashLEDPWMOn(uint8_t duty);
static void flashLEDPWMOff(void);

static void captureSequenceComplete(void);

/*************************************** External variables *******************************************/

extern QueueHandle_t xFatTaskQueue;
extern QueueHandle_t     xIfTaskQueue;

extern SemaphoreHandle_t xFatCanSleepSemaphore;
extern SemaphoreHandle_t xIfCanSleepSemaphore;

/*************************************** Local variables *******************************************/

static APP_WAKE_REASON_E woken;

// This is the handle of the task
TaskHandle_t image_task_id;
QueueHandle_t xImageTaskQueue;

volatile APP_IMAGE_TASK_STATE_E image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

// For the current request increments
static uint32_t g_cur_jpegenc_frame;
// For current request captures to take
static uint32_t g_captures_to_take;
// For the accumulative total captures
// TODO perhaps it should be reset at the start of each day? It is only used in filenames
static uint32_t g_frames_total;
static uint32_t g_timer_period;	// Interval between pictures in ms

static TimerHandle_t capture_timer;

static fileOperation_t fileOp;

uint32_t g_img_data;
uint32_t wakeup_event;
uint32_t wakeup_event1;

static uint16_t g_imageSeqNum; // 0 indicates no SD card

// Strings for each of these states. Values must match APP_IMAGE_TASK_STATE_E in image_task.h
const char *imageTaskStateString[APP_IMAGE_TASK_STATE_NUMSTATES] = {
    "Uninitialised",
    "Init",
    "Capturing",
    "NN Processing",
    "Wait For Timer",
	"Save State",
};

// Strings for expected messages. Values must match messages directed to image Task in app_msg.h
const char *imageTaskEventString[APP_MSG_IMAGETASK_LAST - APP_MSG_IMAGETASK_FIRST] = {
    "Image Event Inactivity",
    "Image Event Start Capture",
    "Image Event Stop Capture",
    "Image Event ReCapture",
    "Image Event Frame Ready",
    "Image Event Done",
    "Image Event Disk Write Complete",
    "Image Event Disk Read Complete",
    "Image Event Error",
};

TickType_t xLastWakeTime;

// There is only one file name for images - this can be declared here - does not need malloc
static char imageFileName[IMAGEFILENAMELEN];

// This is the most recently written file name
static char lastImageFileName[IMAGEFILENAMELEN] = "";

static char msgToMaster[MSGTOMASTERLEN];

static bool nnPositive;

/********************************** Local Functions  *************************************/

/**
 * This is the local callback that executes when the capture_timer expires
 *
 * It calls this function that was registered in the initialisation process.
 *
 * This sends a APP_MSG_IMAGETASK_RECAPTURE event
 */
static void capture_timer_callback(TimerHandle_t xTimer) {
	APP_MSG_T send_msg;

	// Send back to our own queue
	send_msg.msg_data = 0;
	send_msg.msg_parameter = 0;
	send_msg.msg_event = APP_MSG_IMAGETASK_RECAPTURE;

	// Timer callbacks run in the context of a task, so the non ISR version can be used
	if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("send_msg=0x%x fail\r\n", send_msg.msg_event);
	}
}

/**
 * Sets the fileOp pointers for the data retrieved from cisdp_get_jpginfo()
 *
 * This includes setting the pointer to the jpeg buffer and setting a file name
 *
 * Parameters: uint32_t - jpeg_sz, jpeg_addr, frame_num
 */
static void setFileOpFromJpeg(uint32_t jpeg_sz, uint32_t jpeg_addr, uint32_t frame_num) {
	rtc_time time;

    // Create a file name
	// file name: 'image_2025-02-03_1234.jpg' = 25 characters, plus trailing '\0'

	exif_utc_get_rtc_as_time(&time);

    snprintf(imageFileName, IMAGEFILENAMELEN, "image_%d-%02d-%02d_%04d.jpg",
    		time.tm_year, time.tm_mon, time.tm_mday, (uint16_t) frame_num);

    fileOp.fileName = imageFileName;
    fileOp.buffer = (uint8_t *)jpeg_addr;
    fileOp.length = jpeg_sz;
    fileOp.senderQueue = xImageTaskQueue;
    fileOp.closeWhenDone = true;
}

/*
 * Callback from datapath processing
 *
 * This is registered as a callback by cisdp_dp_init() in app_start_state()
 * Events are sent from here to the dp_task queue
 *
 * The common event is SENSORDPLIB_STATUS_XDMA_FRAME_READY which results in a
 * APP_MSG_DPEVENT_XDMA_FRAME_READY message in dp_task queue
 */
void os_app_dplib_cb(SENSORDPLIB_STATUS_E event) {
    APP_MSG_T dp_msg;
    BaseType_t xHigherPriorityTaskWoken;

    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    dp_msg.msg_event = APP_MSG_DPEVENT_UNKOWN;

    //dbg_printf(DBG_LESS_INFO, "os_app_dplib_cb event = %d\n", event);

    switch (event) {
    case SENSORDPLIB_STATUS_ERR_FS_HVSIZE:
    case SENSORDPLIB_STATUS_ERR_FE_TOGGLE:
    case SENSORDPLIB_STATUS_ERR_FD_TOGGLE:
    case SENSORDPLIB_STATUS_ERR_FS_TOGGLE:
    case SENSORDPLIB_STATUS_ERR_BLANK_ERR: /*reg_inpparser_stall_error*/
    case SENSORDPLIB_STATUS_ERR_CRC_ERR:   /*reg_inpparser_crc_error*/
    case SENSORDPLIB_STATUS_ERR_FE_ERR:    /*reg_inpparser_fe_cycle_error*/
    case SENSORDPLIB_STATUS_ERR_HSIZE_ERR: /*reg_inpparser_hsize_error*/
    case SENSORDPLIB_STATUS_ERR_FS_ERR:    /*reg_inpparser_fs_cycle_error*/
        hx_drv_inp1bitparser_clear_int();
        dp_msg.msg_event = APP_MSG_DPEVENT_1BITPARSER_ERR;
        break;
    case SENSORDPLIB_STATUS_EDM_WDT1_TIMEOUT:
        dp_msg.msg_event = APP_MSG_DPEVENT_EDM_WDT1_TIMEOUT;
        break;
    case SENSORDPLIB_STATUS_EDM_WDT2_TIMEOUT:
        dp_msg.msg_event = APP_MSG_DPEVENT_EDM_WDT2_TIMEOUT;
        break;
    case SENSORDPLIB_STATUS_EDM_WDT3_TIMEOUT:
        dp_msg.msg_event = APP_MSG_DPEVENT_SENSORCTRL_WDT_OUT;
        break;
    case SENSORDPLIB_STATUS_SENSORCTRL_WDT_OUT:
        dp_msg.msg_event = APP_MSG_DPEVENT_SENSORCTRL_WDT_OUT;
        break;

    case SENSORDPLIB_STATUS_CDM_FIFO_OVERFLOW:
        dp_msg.msg_event = APP_MSG_DPEVENT_CDM_FIFO_OVERFLOW;
        break;
    case SENSORDPLIB_STATUS_CDM_FIFO_UNDERFLOW:
        dp_msg.msg_event = APP_MSG_DPEVENT_CDM_FIFO_UNDERFLOW;
        break;

    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL1:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL1;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL2:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL2;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL3:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL3;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL4:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL4;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL5:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL5;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL6:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL6;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL7:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL7;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL8:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL8;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL9:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL9;
        break;

    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL1:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL1;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL2:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL2;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL3:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL3;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL4:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL4;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL5:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL5;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL6:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL6;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL7:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL7;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL1:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL1;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL2:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL2;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL3:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL3;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL4:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL4;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL5:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL5;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL6:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL6;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL7:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL7;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL8:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL8;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL9:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL9;
        break;

    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL1:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL1;
        break;
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL2:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL2;
        break;
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL3:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL3;
        break;
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL4:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL4;
        break;
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL5:
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL5;
        break;

    case SENSORDPLIB_STATUS_CDM_MOTION_DETECT:
        dp_msg.msg_event = APP_MSG_DPEVENT_CDM_MOTION_DETECT;
        break;
    case SENSORDPLIB_STATUS_XDMA_FRAME_READY:
        dp_msg.msg_event = APP_MSG_IMAGETASK_FRAME_READY;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_FINISH:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_FINISH:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_FINISH:
    case SENSORDPLIB_STATUS_XDMA_RDMA_FINISH:
        break;

    case SENSORDPLIB_STATUS_RSDMA_FINISH:
        break;
    case SENSORDPLIB_STATUS_HOGDMA_FINISH:
        break;
    case SENSORDPLIB_STATUS_TIMER_FIRE_APP_NOTREADY:
        dp_msg.msg_event = APP_MSG_DPEVENT_TIMER_FIRE_APP_NOTREADY;
        break;
    case SENSORDPLIB_STATUS_TIMER_FIRE_APP_READY:
        dp_msg.msg_event = APP_MSG_DPEVENT_TIMER_FIRE_APP_READY;
        break;
    default:
        dp_msg.msg_event = APP_MSG_DPEVENT_UNKOWN;
        break;
    }

    dp_msg.msg_data = 0;
//    dbg_printf(DBG_LESS_INFO, "Received event 0x%04x from Sensor Datapath. Sending 0x%04x  to Image Task\r\n",
//    		dp_msg.msg_event, dp_msg.msg_data);
    xQueueSendFromISR(xImageTaskQueue, &dp_msg, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)  {
        taskYIELD();
    }
}

/**
 * Implements state machine when in APP_IMAGE_TASK_STATE_INIT
 *
 * This is the state when we are idle and waiting for an instruction.
 *
 * Expected events:
 * 		APP_MSG_IMAGETASK_STARTCAPTURE - from CLI
 * 		APP_MSG_IMAGETASK_INACTIVITY - from inactivity detector
 *
 * @param APP_MSG_T img_recv_msg
 * @return APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForInit(APP_MSG_T img_recv_msg) {
	APP_MSG_DEST_T send_msg;
	APP_MSG_EVENT_E event;
	uint16_t requested_captures;
	uint16_t requested_period;

	event = img_recv_msg.msg_event;
	send_msg.destination = NULL;

	switch (event)  {

	case APP_MSG_IMAGETASK_STARTCAPTURE:
		// MD or timer or the CLI task has asked us to start capturing
		requested_captures = (uint16_t) img_recv_msg.msg_data;
		requested_period = img_recv_msg.msg_parameter;

		// Check parameters are acceptable
		if ((requested_captures < MIN_IMAGE_CAPTURES) || (requested_captures > MAX_IMAGE_CAPTURES) ||
				(requested_period < MIN_IMAGE_INTERVAL) || (requested_period > MAX_IMAGE_INTERVAL) ) {
			xprintf("Invalid parameter values %d or %d\n", requested_captures, requested_period);
		}
		else {
			g_captures_to_take = requested_captures;
			g_timer_period = requested_period;
			XP_LT_GREEN
			xprintf("Images to capture: %d\n", g_captures_to_take);
			xprintf("Interval: %dms\n", g_timer_period);
			XP_WHITE;

			xLastWakeTime = xTaskGetTickCount();

	    	// Turn on the PWM that determines the flash intensity
	    	flashLEDPWMOn(FLASHLEDDUTY);

			// Now start the image sensor.
			configure_image_sensor(CAMERA_CONFIG_RUN);

			// The next thing we expect is a frame ready message: APP_MSG_IMAGETASK_FRAME_READY
			image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
		}
		break;

	case APP_MSG_IMAGETASK_INACTIVITY:
		// Inactivity detected. Prepare to enter DPD
		configure_image_sensor(CAMERA_CONFIG_STOP); // run some sensordplib_stop functions then run HM0360_stream_off commands to the HM0360

		if (fatfs_getImageSequenceNumber() > 0) {
			// TODO - can we call this without the if() and expect fatfs_task to handle it?
			// Ask the FatFS task to save state onto the SD card
			send_msg.destination = xFatTaskQueue;
			send_msg.message.msg_event = APP_MSG_FATFSTASK_SAVE_STATE;
			send_msg.message.msg_data = 0;
			image_task_state = APP_IMAGE_TASK_STATE_SAVE_STATE;
		}
		else {
			// No SD card therefore can't save state
	    	// Wait till the IF Task is also ready, then sleep
	    	sleepWhenPossible();	// does not return
		}

		break;

	default:
		flagUnexpectedEvent(img_recv_msg);

	}	// switch

	return send_msg;
}

/**
 * Implements state machine when in APP_IMAGE_TASK_STATE_CAPTURING
 *
 * This is the state when we are in the process of capturing an image.
 *
 * Expected events:
 * 		APP_MSG_IMAGETASK_FRAME_READY
 * 		Some error events
 *
 * @param APP_MSG_T img_recv_msg
 * @return APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    uint32_t jpeg_addr;
    uint32_t jpeg_sz;

	TickType_t startTime;
	TickType_t elapsedTime;
	uint32_t elapsedMs;
	TfLiteStatus ret;

	// Signed integers
    int8_t outCategories[CATEGORIESCOUNT];

    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;

    switch (event)  {

    case APP_MSG_IMAGETASK_FRAME_READY:
    	// Here when the image sub-ssytem has captured an image.

    	// Turn off the Flash LED PWN signal
    	//flashLEDPWMOff();

    	// frame ready event received from os_app_dplib_cb
        g_cur_jpegenc_frame++;	// The number in this sequence
        g_frames_total++;		// The number since the start of time.

        // run NN processing
        // This gets the input image address and dimensions from:
        // app_get_raw_addr(), app_get_raw_width(), app_get_raw_height()

		startTime = xTaskGetTickCount();

		ret = cv_run(outCategories, CATEGORIESCOUNT);

		elapsedTime = xTaskGetTickCount() - startTime;
		elapsedMs = (elapsedTime * 1000) / configTICK_RATE_HZ;

		if (ret == kTfLiteOk) {
			// OK
        	fatfs_incrementOperationalParameter(OP_PARAMETER_NUM_NN_ANALYSES);

			if (outCategories[1] > 0) {
				XP_LT_GREEN;
				xprintf("PERSON DETECTED!\n");
				// NOTE this only works if CATEGORIESCOUNT == 2
	        	fatfs_incrementOperationalParameter(OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES);
	        	nnPositive = true;
			}
			else {
				XP_LT_RED;
				xprintf("No person detected.\n");
				XP_WHITE;
			}
			XP_WHITE;
			xprintf("Score %d/128. NN processing took %dms\n\n", outCategories[1], elapsedMs);
		}
		else {
			// NN error.
			// What do we do here?
			XP_RED;
			xprintf("NN error %d. NN processing took %dms\n\n", ret, elapsedMs);
			XP_WHITE;
		}
#if 1
		// Proceed to write the jeg file, even if there is no SD card
		// since the fatfs_task will handle that.
    	cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
    	insertExif(&jpeg_sz, &jpeg_addr, outCategories, CATEGORIESCOUNT);

        g_imageSeqNum = fatfs_getImageSequenceNumber();
		// Set the fileOp structure. This includes setting the file name
		setFileOpFromJpeg(jpeg_sz, jpeg_addr, g_imageSeqNum);

		dbg_printf(DBG_LESS_INFO, "Writing %d bytes to '%s'\n",
				fileOp.length, fileOp.fileName);

		// Save the file name as the most recent image
		snprintf(lastImageFileName, IMAGEFILENAMELEN, "%s", fileOp.fileName);

		send_msg.destination = xFatTaskQueue;
		send_msg.message.msg_event = APP_MSG_FATFSTASK_WRITE_FILE;
		send_msg.message.msg_data = (uint32_t)&fileOp;

		// Also see if it is appropriate to send a "Event" message to the BLE processor
		if ((g_cur_jpegenc_frame == g_captures_to_take) && nnPositive) {
			// Inform BLE processor
			// For the moment the message body is identical to the "Sleep " message but we might chnage this later
			snprintf(msgToMaster, MSGTOMASTERLEN, "Event ");

			for (uint8_t i=0; i < OP_PARAMETER_NUM_ENTRIES; i++) {
				uint8_t next = strlen(msgToMaster);	// points to the place where we should write the next parameter
				snprintf(&msgToMaster[next], sizeof(msgToMaster), "%d ", fatfs_getOperationalParameter(i));
			}
			sendMsgToMaster(msgToMaster);
		}


		// Wait in this state till the disk write completes - expect APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE
    	image_task_state = APP_IMAGE_TASK_STATE_NN_PROCESSING;

#else
		// old code
    	if (g_imageSeqNum > 0) {
    		// An SD card is present, so do activities required to save file

        	cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
        	insertExif(&jpeg_sz, &jpeg_addr, outCategories, CATEGORIESCOUNT);

            g_imageSeqNum = fatfs_getImageSequenceNumber();
    		// Set the fileOp structure. This includes setting the file name
    		setFileOpFromJpeg(jpeg_sz, jpeg_addr, g_imageSeqNum);

    		dbg_printf(DBG_LESS_INFO, "Writing %d bytes to '%s'\n",
    				fileOp.length, fileOp.fileName);

    		// Save the file name as the most recent image
    		snprintf(lastImageFileName, IMAGEFILENAMELEN, "%s", fileOp.fileName);

    		send_msg.destination = xFatTaskQueue;
    		send_msg.message.msg_event = APP_MSG_FATFSTASK_WRITE_FILE;
    		send_msg.message.msg_data = (uint32_t)&fileOp;

    		// Wait in this state till the disk write completes
        	image_task_state = APP_IMAGE_TASK_STATE_NN_PROCESSING;
    	}
    	else {
    		dbg_printf(DBG_LESS_INFO, "No SD card. Skipping file activities.\n");

    		if (g_cur_jpegenc_frame == g_captures_to_take) {
    			captureSequenceComplete();
    			image_task_state = APP_IMAGE_TASK_STATE_INIT;
    		}
    		// Else we stay in this state APP_IMAGE_TASK_STATE_CAPTURING and wait for the next frame ready
    	}
#endif // 1
    	break;

    case APP_MSG_DPEVENT_EDM_WDT1_TIMEOUT:
    case APP_MSG_DPEVENT_EDM_WDT2_TIMEOUT:
    case APP_MSG_DPEVENT_SENSORCTRL_WDT_OUT:
    	// Unfortunately I see this. EDM WDT2 Timeout = 0x011c
    	// probably if HM0360 is not receiving I2C commands...
    	dbg_printf(DBG_LESS_INFO, "Received a timeout event. TODO - re-initialise HM0360?\n");
    	// For now, deliberate fall through

    default:
    	flagUnexpectedEvent(img_recv_msg);
    }

    return send_msg;
}

/**
 * Implements state machine when in APP_IMAGE_TASK_STATE_NN_PROCESSING
 *
 * This is the state when we are waiting for a disk write to finish.
 *
 * Expected events:
 * 		APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE
 *
 * @param APP_MSG_T img_recv_msg
 * @return APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForNNProcessing(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    uint32_t diskStatus;

    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;
    diskStatus = img_recv_msg.msg_data;

    switch (event) {

    case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
        if ( diskStatus == 0) {
        	fatfs_incrementOperationalParameter(OP_PARAMETER_SEQUENCE_NUMBER);
        }
        else {
            dbg_printf(DBG_LESS_INFO, "Image not written. Error: %d\n", diskStatus);
        }

        // This represents the point at which an image has been captured and processed.

		if (g_cur_jpegenc_frame == g_captures_to_take) {
			captureSequenceComplete();
			image_task_state = APP_IMAGE_TASK_STATE_INIT;
		}
#ifdef USE_HM0360
        else {
			// The HM0360 uses an internal timer to determine the time for the next image
        	// Re-start the image sensor.
			configure_image_sensor(CAMERA_CONFIG_CONTINUE);
        	// Expect another frame ready event
        	image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        }
#else
        else {
        	// Start a timer that delays for the defined interval.
        	// When it expires, switch to CAPTURUNG state and request another image
            if (capture_timer != NULL)  {
                // Change the period and start the timer
            	// The callback issues a APP_MSG_IMAGETASK_RECAPTURE event
                xTimerChangePeriod(capture_timer, pdMS_TO_TICKS(g_timer_period), 0);
                // Expect a APP_MSG_IMAGETASK_RECAPTURE event from the capture_timer
                image_task_state = APP_IMAGE_TASK_STATE_WAIT_FOR_TIMER;
            }
            else {
            	// error
            	flagUnexpectedEvent(img_recv_msg);
                image_task_state = APP_IMAGE_TASK_STATE_INIT;
            }
        }
#endif // USE_HM0360
        break;

    default:
        flagUnexpectedEvent(img_recv_msg);
        break;
    }

    return send_msg;
}

/**
 * Implements state machine for APP_IMAGE_TASK_STATE_WAIT_FOR_TIMER
 *
 * This is the state when we are waiting for the capture_timer to expire
 *
 * Expected events:
 * 		APP_MSG_IMAGETASK_RECAPTURE
 *
 * @param APP_MSG_T img_recv_msg
 * @return APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForWaitForTimer(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;

    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;

    switch (event) {

    case APP_MSG_IMAGETASK_RECAPTURE:
    	// here when the capture_timer expires

    	// Turn on the PWM that determines the flash intensity
    	//flashLEDPWMOn(FLASHLEDDUTY);

        sensordplib_retrigger_capture();
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        break;

    default:
        flagUnexpectedEvent(img_recv_msg);
        break;
    }

    return send_msg;
}

/**
 * Implements state machine for APP_IMAGE_TASK_STATE_SAVE_STATE
 *
 * This is the state when we preparing to enter DPD
 *
 * Expected events:
 *		APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE
 *
 * @param APP_MSG_T img_recv_msg
 * @return APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForSaveState(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;

    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;

    switch (event) {

    case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
    	// Here when the FatFS task has saved state
    	// Wait till the IF Task is also ready, then sleep
    	sleepWhenPossible();	// does not return
        break;

    default:
        flagUnexpectedEvent(img_recv_msg);
        break;
    }

    return send_msg;
}

/**
 * For state machine: Print a red message to see if there are unhandled events we should manage
 *
 * TODO - what should we really do in this case?
 *
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T img_recv_msg) {
    APP_MSG_EVENT_E event;
    APP_MSG_DEST_T send_msg;

    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;

    XP_LT_RED;
    if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST)) {
        xprintf("IMAGE task unhandled event '%s' in '%s'\r\n", imageTaskEventString[event - APP_MSG_IMAGETASK_FIRST], imageTaskStateString[image_task_state]);
    }
    else  {
        xprintf("IMAGE task unhandled event 0x%04x in '%s'\r\n", event, imageTaskStateString[image_task_state]);
    }
    XP_WHITE;

    // If non-null then our task sends another message to another task
    return send_msg;
}

/**
 * When the desired number of images have been captured
 *
 * Placed here as a separate routine as it is called from 2 places.
 */
static void captureSequenceComplete(void) {
    // Current captures sequence completed
     XP_GREEN;
     xprintf("Current captures completed: %d\n", g_captures_to_take);
     xprintf("Total frames captured since last reset: %d\n", g_frames_total);
     XP_WHITE;

     // Inform BLE processor
     snprintf(msgToMaster, MSGTOMASTERLEN, "Captured %d images. Last is %s",
     		(int) g_captures_to_take, lastImageFileName);
     sendMsgToMaster(msgToMaster);

     // Reset counters
     g_captures_to_take = 0;
     g_cur_jpegenc_frame = 0;
     flashLEDPWMOff();
}

// Three function that control the behaviour of the GPIO pin that produces the PWM signal for LED brightness

/**
 * Initialise a GPIO pin as a PWM output for control of Flash LED brightness.
 *
 * This is PB9, SENSOR_GPIO, routed to the PSU chip that drives the IC that drives the Flash LED.
 *
 * TODO move to pinmux_init()?
 */
static void flashLEDPWMInit(void) {
	PWM_ERROR_E ret;

	// The output pin of PWM1 is routed to PB9
	ret = hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_PWM1, 1);

	XP_LT_BLUE;
	if (ret == PWM_NO_ERROR) {
		xprintf("Initialised PB9 for PWM output\n");
		// initializes the PWM1
		ret = hx_drv_pwm_init(PWM1, HW_PWM1_BASEADDR);

		if (ret == PWM_NO_ERROR) {
			xprintf("Initialised flash LED on PB9\n");
		}
		else {
			xprintf("Flash LED initialisation on PB9 fails: %d\n", ret);
		}
	}
	else {
		xprintf("PB9 init for PWM output fails: %d\n", ret);
	}

	XP_WHITE;
}

/**
 * Turns on the PWM signal
 *
 * @param duty - value between 1 and 99 representing duty cycle in percent
 */
static void flashLEDPWMOn(uint8_t duty) {
	pwm_ctrl ctrl;

	if ((duty > 0) && (duty <100)) {
		// PWM1 starts outputting according to the set value.
		// (The high period is 20%, and the low period is 80%)
		ctrl.mode = PWM_MODE_CONTINUOUS;
		ctrl.pol = PWM_POL_NORMAL;
		ctrl.freq = FLASHLEDFREQ;
		ctrl.duty = duty;
		hx_drv_pwm_start(PWM1, &ctrl);
	}
	else {
		xprintf("Invalid PWM duty cycle\n");
		hx_drv_pwm_stop(PWM1);
	}
}

/**
 * Turns off the PWM signal.
 *
 */
static void flashLEDPWMOff(void) {

#ifdef FLASHLEDTEST
	// leave it on so we can check on the scope
#else
	hx_drv_pwm_stop(PWM1);
#endif //  FLASHLEDTEST
}


/********************************** FreeRTOS Task  *************************************/

/**
 * Image processing task.
 *
 * This is called when the scheduler starts.
 * Various entities have already be set up by image_createTask()
 *
 * After some one-off activities it waits for events to arrive in its xImageTaskQueue
 *
 */
static void vImageTask(void *pvParameters) {
    APP_MSG_T img_recv_msg;
    APP_MSG_T internal_msg;
    APP_MSG_DEST_T send_msg;
    QueueHandle_t target_queue;
    APP_IMAGE_TASK_STATE_E old_state;
    const char *event_string;
    APP_MSG_EVENT_E event;
    uint32_t recv_data;
    bool cameraInitialised;
    uint32_t interval;

    g_frames_total = 0;
    g_cur_jpegenc_frame = 0;
    g_captures_to_take = 0;
    g_timer_period = 0;
    g_img_data = 0;
    g_imageSeqNum = 0;	// 0 indicates no SD card

    nnPositive = false;	// true if the NN analysis detects something

    XP_CYAN;
    // Observing these messages confirms the initialisation sequence
    xprintf("Starting Image Task\n");
    XP_WHITE;

    // Should initialise the camera but not start taking images
#ifdef USE_HM0360
    if (woken == APP_WAKE_REASON_COLD)  {
    	cameraInitialised = configure_image_sensor(CAMERA_CONFIG_INIT_COLD);
    }
    else {
    	cameraInitialised = configure_image_sensor(CAMERA_CONFIG_INIT_WARM);
    }
#else
    // For RP camera the SENSOR_ENABLE signal has been turned off during DPD, so must re-initialise all registers
    cameraInitialised = configure_image_sensor(CAMERA_CONFIG_INIT_COLD);
#endif // USE_HM0360

    if (!cameraInitialised) {
    	xprintf("\nEnter DPD mode because there is no camera!\n\n");

    	sleep_mode_enter_dpd(SLEEPMODE_WAKE_SOURCE_WAKE_PIN, 0, false);	// Does not return
    }

    flashLEDPWMInit();

    // Computer vision init
    if (cv_init(true, true) < 0)  {
        xprintf("cv init fail\n");
        configASSERT(0);
    }
    else {
        xprintf("Initialised neural network.\n");
    }

    // A value of 0 means no SD card so we can skip all SD card activities
    g_imageSeqNum = fatfs_getImageSequenceNumber();

    // Initial state of the image task (initialized)
    image_task_state = APP_IMAGE_TASK_STATE_INIT;

    // Calculate the HM0360 inter-frame delay period based on value requested in configuration file
    interval = fatfs_getOperationalParameter(OP_PARAMETER_PICTURE_INTERVAL);
    //sleepTime = image_calculateSleepTime(interval); adjustment for HM0360

    // If we woke because of motion detection or timer then let's send ourselves an initial
    // message to take some photos
    xprintf("DEBUG: wake reason %d\n", woken);
    if ((woken == APP_WAKE_REASON_MD) || (woken == APP_WAKE_REASON_TIMER)) {
    	// Pass the parameters in the ImageTask message queue
    	internal_msg.msg_data = fatfs_getOperationalParameter(OP_PARAMETER_NUM_PICTURES);
    	//internal_msg.msg_parameter = fatfs_getOperationalParameter(OP_PARAMETER_PICTURE_INTERVAL);
    	internal_msg.msg_parameter = interval;
    	internal_msg.msg_event = APP_MSG_IMAGETASK_STARTCAPTURE;

    	if (xQueueSend(xImageTaskQueue, (void *)&internal_msg, __QueueSendTicksToWait) != pdTRUE) {
    		xprintf("Failed to send 0x%x to imageTask\r\n", internal_msg.msg_event);
    	}
    }

    // Loop forever, taking events from xImageTaskQueue as they arrive
    for (;;)  {
        // Wait for a message in the queue
        if(xQueueReceive(xImageTaskQueue, &(img_recv_msg), __QueueRecvTicksToWait) == pdTRUE)  {
            event = img_recv_msg.msg_event;
            recv_data = img_recv_msg.msg_data;

            // convert event to a string
            if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST))  {
                event_string = imageTaskEventString[event - APP_MSG_IMAGETASK_FIRST];
            }
            else   {
                event_string = "Unexpected";
            }

            XP_LT_CYAN
            xprintf("IMAGE Task ");
            XP_WHITE;
            xprintf("received event '%s' (0x%04x). Rx data = 0x%08x\r\n", event_string, event, recv_data);

            old_state = image_task_state;

            switch (image_task_state)  {
            case APP_IMAGE_TASK_STATE_UNINIT:
                send_msg = flagUnexpectedEvent(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_INIT:
                send_msg = handleEventForInit(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_CAPTURING:
                send_msg = handleEventForCapturing(img_recv_msg);
                break;
//
//            case APP_IMAGE_TASK_STATE_BUSY:
//                send_msg = handleEventForBusy(img_recv_msg);
//                break;

            case APP_IMAGE_TASK_STATE_NN_PROCESSING:
                send_msg = handleEventForNNProcessing(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_WAIT_FOR_TIMER:
                send_msg = handleEventForWaitForTimer(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_SAVE_STATE:
                send_msg = handleEventForSaveState(img_recv_msg);
                break;

            default:
                send_msg = flagUnexpectedEvent(img_recv_msg);
                break;
            }

            if (old_state != image_task_state)   {
				// state has changed
                XP_LT_CYAN;
                xprintf("IMAGE Task state changed ");
                XP_WHITE;
                xprintf("from '%s' (%d) to '%s' (%d)\r\n",
                        imageTaskStateString[old_state], old_state,
                        imageTaskStateString[image_task_state], image_task_state);
            }

            // Passes message to other tasks if required (commonly fatfs)
            if (send_msg.destination != NULL)   {
                target_queue = send_msg.destination;
                if (xQueueSend(target_queue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE) {
                    xprintf("IMAGE task sending event 0x%x failed\r\n", send_msg.message.msg_event);
                }
                else {
                    xprintf("IMAGE task sending event 0x%04x. Rx data = 0x%08x\r\n", send_msg.message.msg_event, send_msg.message.msg_data);
                }
            }

            xprintf("\r\n");
        }
    }
}

/**
 * Initialises camera capturing
 *
 * For RP camera expect:
 * 	- at cold boot or warm boot: CAMERA_CONFIG_INIT_COLD (since registers are lost in DPD)
 * 	- at APP_MSG_IMAGETASK_STARTCAPTURE event: CAMERA_CONFIG_RUN
 * 	- at APP_MSG_IMAGETASK_INACTIVITY event: CAMERA_CONFIG_STOP
 *
 * For HM0360 camera expect:
 * 	- at cold boot CAMERA_CONFIG_INIT_COLD (loads initial registers and extra regs from files)
 * 	- at warm boot boot CAMERA_CONFIG_INIT_WARM (since registers are retained in DPD)
 * 	- at APP_MSG_IMAGETASK_STARTCAPTURE event: CAMERA_CONFIG_RUN (selects CONTEXT_A registers)
 *	- at APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE event: CAMERA_CONFIG_CONTINUE (calls cisdp_dp_init() again)
 * 	- at APP_MSG_IMAGETASK_INACTIVITY event: CAMERA_CONFIG_STOP (STOP mode)
 * 	- just before DPD: CAMERA_CONFIG_MD (selects CONTEXT_B registers)
 *
 * @param CAMERA_CONFIG_E operation
 * @param numFrames - number of frames we ask the HM0360 to take
 * @return true if initialised. false if no working camera
 */
static bool configure_image_sensor(CAMERA_CONFIG_E operation) {

	switch (operation) {
	case CAMERA_CONFIG_INIT_COLD:
        if (cisdp_sensor_init(true) < 0) {
            xprintf("\r\nCIS Init fail\r\n");
            return false;
        }
        else {
#ifdef USE_HM0360
        	// Initialise extra registers from file
        	cis_file_process(HM0360_EXTRA_FILE);
#else
        	// extra settings for RP camera
        	cis_file_process(RP_EXTRA_FILE);
#endif // USE_HM0360

            // if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
            //  Datapath events give callbacks to os_app_dplib_cb() in dp_task
            if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
                xprintf("\r\nDATAPATH Init fail\r\n");
                return false;
            }
        }
		break;

	case CAMERA_CONFIG_INIT_WARM:
        if (cisdp_sensor_init(false) < 0) {
            xprintf("\r\nCIS Init fail\r\n");
            return false;
        }
        else {
            // if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
            //  Datapath events give callbacks to os_app_dplib_cb() in dp_task
            if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
                xprintf("\r\nDATAPATH Init fail\r\n");
                return false;
            }
        }
		break;

	case CAMERA_CONFIG_RUN:
        if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
            xprintf("\r\nDATAPATH Init fail\r\n");
            return false;
        }
#ifdef USE_HM0360
        cisdp_sensor_set_mode(CONTEXT_A, MODE_SW_NFRAMES_SLEEP, 1, g_timer_period);
#endif // USE_HM0360
		cisdp_sensor_start(); // Starts data path sensor control block
		break;

	case CAMERA_CONFIG_CONTINUE:
        if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
            xprintf("\r\nDATAPATH Init fail\r\n");
            return false;
        }
		cisdp_sensor_start(); // Starts data path sensor control block
		break;

	case CAMERA_CONFIG_STOP:
        xprintf("Stopping sensor\n");
        cisdp_sensor_stop();	// run some sensordplib_stop functions then run HM0360_stream_off commands to the HM0360
		break;

#ifdef USEHM0360
	case CAMERA_CONFIG_MD:
		// Now we can ask the HM0360 to get ready for DPD
		cisdp_sensor_md_init(); // select CONTEXT_B registers
		// Do some configuration of MD parameters
		//hm0360_x_set_threshold(10);
		break;
#endif // USEHM0360

	default:
		// should not happen
		return false;
		break;
	}

//    if (state == APP_STATE_ALLON) {
//        if (cisdp_sensor_init(true) < 0) {
//            xprintf("\r\nCIS Init fail\r\n");
//            return false;
//        }
//    }
//    else if (operation == CAMERA_CONFIG_RUN)  {
//        if (cisdp_sensor_init(false) < 0)  {
//            xprintf("\r\nCIS Restart fail\r\n");
//            return false;
//        }
//    }
//    else if ( state == CAMERA_CONFIG_STOP )  {
//        xprintf("APP_STATE_STOP\n");
//        xprintf("Stopping sensor\n");
//        cisdp_sensor_stop();	// run some sensordplib_stop functions then run HM0360_stream_off commands to the HM0360
//
//        // TODO should this be done here?
//        //cisdp_sensor_md_init();
//
//        return true;
//    }
//    // if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
//    //  Datapath events give callbacks to os_app_dplib_cb() in dp_task
//    if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
//        xprintf("\r\nDATAPATH Init fail\r\n");
//        return false;
//    }


    //cisdp_sensor_start();
    return true;
}

/**
 * Send an unsolicited message to the MKL62BA.
 *
 * This is experimental...
 */
static void sendMsgToMaster(char * str) {
	APP_MSG_T send_msg;

	// Send back to MKL62BA - msg_data is the string
	send_msg.msg_data = (uint32_t)str;
	send_msg.msg_parameter = strnlen(str, MSGTOMASTERLEN);
	send_msg.msg_event = APP_MSG_IFTASK_MSG_TO_MASTER;

	if (xQueueSend(xIfTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("send_msg=0x%x fail\r\n", send_msg.msg_event);
	}
}

/**
 * Insert EXIF metadata into the buffer that contains the rest of the JPEG data
 *
 * The objective is to find the appropriate place in the jpeg file to insert the EXIF,
 * then insert the EXIF there, having shifted the second part of the buffer to make space
 *
 * EXIF data is probably:
 * 	- UTC time, obtained with exif_utc_get_rtc_as_exif_string()
 * 	- GPS location, obtained with exif_gps_generate_byte_array()
 * 	- Neural network output, passed as parameters to this function
 * 	- Other EXIF fields we have decided on
 *
 * NOTE: the jpeg_enc_addr pointer is to an array of 32-bit words.
 * NOTE: It is not clear to me whether the JPEG buffer is big enough to allow insertion of the data.
 * There is a definition in cispd_densor:
 * 		#define JPEG_BUFSIZE  76800 //640*480/4
 * It is not clear from a qucik look at the code whether there is more than one jpeg image using this buffer.
 * In any event, it is important not to overrun that buffer.
 *
 * It would be safer to make a new buffer e.g. with malloc (or the FreeRTOS version) and copy the jpeg
 * data into that. But maybe that is not necessary. Perhaps easiest initially to try to insert the EXIF
 * in place, and only use a different approach if that fails.
 *
 * There is another approach entirely, which is not to insert the EXIT data, but to write the JPEG file
 * in 3 chunks:
 * 		1 The JPEG data up to the insertion point
 * 		2 The EXIF data
 * 		3 The JPEG data after the insertion point
 *
 * A different approach again would be write the JPEG file to SD card without inserting the EXIF,
 * then write the EXIF data to a separate file (same file name but with the extension .exif)
 * then post-process the two files off-line.
 *
 * Regardless of the approach, a number of functions will be required, something like this:
 *
 * unit32_t * exifInsertionPoint((uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr);
 * void createExit(uint32_t exifBuffer, uint16_t exitBufferSize,  uint8_t * outCategories, uint16_t categoriesCount);
 *
 * IMPORTANT to figure out whetehr we are manipulating 8, 16 or 32 bit arrays.
 * Uncertainty here could be causing the trouble Tobyn has reported.
 *
 * See here for an investigation into exiftool:
 * 	https://chatgpt.com/share/68040174-01fc-8005-a30f-23fc363b98ec
 *
 * @param jpeg_enc_filesize - size of the jpeg buffer. Return the size of the expanded buffer, including the EXIF
 * @param jpeg_enc_addr - pointer to the buffer containing the jpeg data
 * @param outCategories - an array of integers, one for each of the neural network output categories
 * @param categoriesCount - size of that array
 */
static void insertExif(uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr, int8_t * outCategories, uint16_t categoriesCount) {

}

/**
 * When final activity from the FatFS Task and IF Task are complete, enter DPD
 *
 * A message has also been sent to the If Tasks asking it to send its final message.
 * When complete it will give its semaphore.
 */
static void sleepWhenPossible(void) {

	xprintf("Waiting for IF task to finish.\n");
	xSemaphoreTake(xIfCanSleepSemaphore, portMAX_DELAY);

	sleepNow();
}

/**
 * Enter DPD
 *
 * This is a separate routine from sleepWhenPossible since it is called from 2 places
 */
static void sleepNow(void) {
	uint32_t timelapseDelay;

#ifdef USE_HM0360
	xprintf("Preparing HM0360 for MD\n");
	configure_image_sensor(CAMERA_CONFIG_MD);
#endif	// USE_HM0360

	xprintf("\nEnter DPD mode!\n\n");

	timelapseDelay = fatfs_getOperationalParameter(OP_PARAMETER_TIMELAPSE_INTERVAL);

	if (timelapseDelay > 0) {
		// Enable wakeup on WAKE pin and timer
		sleep_mode_enter_dpd(SLEEPMODE_WAKE_SOURCE_WAKE_PIN | SLEEPMODE_WAKE_SOURCE_RTC,
				timelapseDelay, false);	// Does not return
	}
	else {
		// If the OP_PARAMETER_TIMELAPSE_INTERVAL setting is 0 then we don't enable a timer wakeup
		sleep_mode_enter_dpd(SLEEPMODE_WAKE_SOURCE_WAKE_PIN, 0, false);	// Does not return
	}
}

/********************************** Public Functions  *************************************/

/**
 * Creates the image task.
 *
 * The task itself initialises the Image sensor and then manages requests to access it.
 */
TaskHandle_t image_createTask(int8_t priority, APP_WAKE_REASON_E wakeReason) {

    if (priority < 0)  {
        priority = 0;
    }

    // Save this. Determines sensor initialisation and whether we take photos
    woken = wakeReason;

    image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

    xImageTaskQueue = xQueueCreate(IMAGE_TASK_QUEUE_LEN, sizeof(APP_MSG_T));
    if (xImageTaskQueue == 0) {
        xprintf("Failed to create xImageTaskQueue\n");
        configASSERT(0); // TODO add debug messages?
    }

    capture_timer = xTimerCreate("CaptureTimer",
            pdMS_TO_TICKS(1000),    // initial dummy period
            pdFALSE,                // one-shot timer
            NULL,                   // timer ID (optional)
			capture_timer_callback);

    if (capture_timer == NULL) {
        xprintf("Failed to create xImageTaskQueue\n");
        configASSERT(0); // TODO add debug messages?
    }

    if (xTaskCreate(vImageTask, /*(const char *)*/ "IMAGE",
                    3 * configMINIMAL_STACK_SIZE,
                    NULL, priority,
                    &image_task_id) != pdPASS) {
        xprintf("Failed to create vImageTask\n");
        configASSERT(0); // TODO add debug messages?
    }

    return image_task_id;
}

/**
 * Returns the internal state as a number
 */
uint16_t image_getState(void)
{
    return image_task_state;
}

/**
 * Returns the internal state as a string
 */
const char * image_getStateString(void)
{
    return *&imageTaskStateString[image_task_state];
}

/**
 * Returns the name of the most recently written file as a string
 */
const char * image_getLastImageFile(void) {
    return lastImageFileName;
}

// Called by CLI only.
// Temporary until I can make this work through the state machine
// Call back when inactivity is sensed and we want to enter DPD
// TODO this should really be done using the state machine events!
void image_hackInactive(void) {

	XP_LT_GREEN;
	xprintf("Inactive - in image_hackInactive() Remove this!\n");
	XP_WHITE;

	configure_image_sensor(CAMERA_CONFIG_STOP); // run some sensordplib_stop functions then run HM0360_stream_off commands to the HM0360

	sleepNow();
}

bool image_nnDetected(void) {
	return nnPositive;
}
