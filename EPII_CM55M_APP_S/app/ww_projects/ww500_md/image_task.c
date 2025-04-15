
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
#include "ww500_md.h"
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

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define image_task_PRIORITY (configMAX_PRIORITIES - 2)

#define IMAGE_TASK_QUEUE_LEN 10

// This is experimental. TODO check it is ok
#define MSGTOMASTERLEN 100

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vImageTask(void *pvParameters);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T handleEventForInit(APP_MSG_T img_recv_msg);
static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T rxMessage);
static APP_MSG_DEST_T handleEventForNNProcessing(APP_MSG_T img_recv_msg);

// This is to process an unexpected event
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T rxMessage);

static bool app_start_state(APP_STATE_E state);

// Send unsolicited message to the master
static void sendMsgToMaster(char * str);

static void setFileOpFromJpeg(uint32_t jpeg_sz, uint32_t jpeg_addr, uint32_t frame_num);

/*************************************** External variables *******************************************/

extern SemaphoreHandle_t xI2CTxSemaphore;
extern QueueHandle_t xFatTaskQueue;
extern QueueHandle_t     xIfTaskQueue;

/*************************************** Local variables *******************************************/

static bool coldBoot;

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
static uint32_t g_timer_period;

static TimerHandle_t capture_timer;
//static void (*capture_timer_callback)(void) = NULL;

static fileOperation_t fileOp;

uint32_t g_img_data;
uint32_t wakeup_event;
uint32_t wakeup_event1;

// Strings for each of these states. Values must match APP_IMAGE_TASK_STATE_E in image_task.h
const char *imageTaskStateString[APP_IMAGE_TASK_STATE_NUMSTATES] = {
    "Uninitialised",
    "Init",
    "Capturing",
    "NN Processing",
    "Wait for Timer",
    "Busy",
};

// Strings for expected messages. Values must match messages directed to image Task in app_msg.h
const char *imageTaskEventString[APP_MSG_IMAGETASK_LAST - APP_MSG_IMAGETASK_FIRST] = {
    "Image Event Unkown Event",
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

/********************************** Local Functions  *************************************/

/**
 * This is the local callback that executes when the timer expires
 *
 * It calls the function that was registered in the initialisation process.
 *
 */
static void capture_timer_callback(TimerHandle_t xTimer) {
	APP_MSG_T send_msg;

	// Send back to our own queue
	send_msg.msg_data = 0;
	send_msg.msg_parameter = 0;
	send_msg.msg_event = APP_MSG_IMAGETASK_RECAPTURE;

	if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("send_msg=0x%x fail\r\n", send_msg.msg_event);
	}
}

/**
 * Sets the fileOp pointers for the data retrieved from cisdp_get_jpginfo()
 *
 * NOTE: this doesn't belong here, it belongs coupled with the _get func in cisdp_sensor.c,
 * but since there are so many duplicate files for each sensor, it is easier to put it here for now.
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
 *
 */
void os_app_dplib_cb(SENSORDPLIB_STATUS_E event)
{
    APP_MSG_T dp_msg;
    BaseType_t xHigherPriorityTaskWoken;
    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;
    dbg_printf(DBG_LESS_INFO, "os_app_dplib_cb event = %d\n", event);
    switch (event)
    {
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
    dbg_printf(DBG_LESS_INFO, "Send to dp task 0x%x\r\n", dp_msg.msg_event);
    xQueueSendFromISR(xImageTaskQueue, &dp_msg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        taskYIELD();
    }
}

#if 1
/**
 * Implements state machine when in APP_IMAGE_TASK_STATE_INIT
 *
 * This is the state when we are idle and waiting for an instruction.
 *
 * Expected events:
 * 		APP_MSG_IMAGETASK_STARTCAPTURE - from CLI
 *
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
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
		// the CLI task has asked us to start capturing
		requested_captures = (uint16_t) img_recv_msg.msg_data;
		requested_period = (uint16_t) img_recv_msg.msg_parameter;

		// Check parameters are acceptable
		if ((requested_captures < MIN_IMAGE_CAPTURES) || (requested_captures > MAX_IMAGE_CAPTURES) ||
				(requested_period< MIN_IMAGE_INTERVAL) || (requested_period > MAX_IMAGE_INTERVAL) ) {
			xprintf("Invalid parameter values %d or %d\n", requested_captures, requested_period);
		}
		else {
			g_captures_to_take = requested_captures;
			g_timer_period = requested_period;
			XP_LT_GREEN
			xprintf("Captures to take: %d\n", g_captures_to_take);
			xprintf("Timer period: %ds\n", g_timer_period);
			XP_WHITE;

			xLastWakeTime = xTaskGetTickCount();

			// now start the image sensor. The next thing we expect is a 'Image Event Frame Ready'

			app_start_state(APP_STATE_RESTART); // cisdp_sensor_init(false) and cisdp_dp_init()
			cisdp_sensor_start(); // Sets up sensor registers for normal image capture, and starts data path sensor control block

			image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
			// Next:we expect a frame ready message
		}
		break;


	default:
		flagUnexpectedEvent(img_recv_msg);

	}	// switch

	return send_msg;
}

#else
// earlier code

/**
 * Implements state machine when in APP_IMAGE_TASK_STATE_INIT
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForInit(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;

    // first instance for request
    if (g_captures_to_take == 0)  {
        // separates the input parameter into two parts, numbers of captures and timer period
        g_captures_to_take = (uint16_t) img_recv_msg.msg_data;
        g_timer_period = (uint16_t) img_recv_msg.msg_parameter;

        XP_LT_GREEN
        xprintf("Captures to take: %d\n", g_captures_to_take);
        xprintf("Timer period: %ds\n", g_timer_period);
        XP_WHITE;
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        send_msg.message.msg_data = 0;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_STARTCAPTURE;

        xLastWakeTime = xTaskGetTickCount();
    }

    // if input capture parameter is out of range
    if ((g_captures_to_take < MIN_IMAGE_CAPTURES) || (g_captures_to_take > MAX_IMAGE_CAPTURES) ||
    		(g_timer_period < MIN_IMAGE_INTERVAL) || (g_timer_period > MAX_IMAGE_INTERVAL) )
    {
        xprintf("Invalid parameter values %d or %d\n", g_captures_to_take, g_timer_period);
        send_msg = flagUnexpectedEvent(img_recv_msg);
    }

    // Keep capturing while frames captured is less than the total captures to take
    else if (g_cur_jpegenc_frame < g_captures_to_take) {
    	// How about don't delay for the first image...
    	// TODO a value of time in ms would allow fractional second delays.
    	if (g_cur_jpegenc_frame > 0) {
    		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(g_timer_period * 1000)); // Convert g_timer_period to milliseconds
    	}

        switch (event)  {
        case APP_MSG_IMAGETASK_STARTCAPTURE:
        	// the CLI task has asked us to start capturing

            send_msg.destination = xImageTaskQueue;
            image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
            send_msg.message.msg_event = APP_MSG_IMAGETASK_STARTCAPTURE;
            break;

        case APP_MSG_IMAGETASK_STOPCAPTURE:
            send_msg.message.msg_event = APP_MSG_IMAGETASK_STOPCAPTURE;
            break;

        case APP_MSG_IMAGETASK_FRAME_READY:
            send_msg.destination = xImageTaskQueue;
            send_msg.message.msg_event = APP_MSG_IMAGETASK_FRAME_READY;
            image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
            break;

        case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
            if (img_recv_msg.msg_data == 0) {
                send_msg.message.msg_event = APP_MSG_IMAGETASK_DONE;
                image_task_state = APP_IMAGE_TASK_STATE_INIT;
            }
            else {
                dbg_printf(DBG_LESS_INFO, "Image not written, error occured: %d", event);
                flagUnexpectedEvent(img_recv_msg);
            }
            break;

        case APP_MSG_IMAGETASK_DONE:
            send_msg.message.msg_event = APP_MSG_IMAGETASK_DONE;
            image_task_state = APP_IMAGE_TASK_STATE_INIT;
            break;

        default:
            flagUnexpectedEvent(img_recv_msg);
        }
    }
    else  {
        // Current captures sequence completed
        XP_GREEN;
        xprintf("Current captures completed: %d\n", g_captures_to_take);
        xprintf("Total frames captured since last reset: %d\n", g_frames_total);
        XP_WHITE;

        // Experimental - can we send an unsolicited message to the MKL62BA?
        snprintf(msgToMaster, MSGTOMASTERLEN, "Captured %d images. Last is %s",
        		(int) g_captures_to_take, lastImageFileName);
        sendMsgToMaster(msgToMaster);

        // Resets counters
        g_captures_to_take = 0;
        g_cur_jpegenc_frame = 0;
    }
    return send_msg;
}


#endif // 1

/*
 * Handles event path for image capturing
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    uint32_t jpeg_addr;
    uint32_t jpeg_sz;

    event = img_recv_msg.msg_event;

    switch (event)  {

    case APP_MSG_IMAGETASK_FRAME_READY:
    	// frame ready event received from os_app_dplib_cb
        g_cur_jpegenc_frame++;	// The number in this sequence
        g_frames_total++;		// The number since the start of time.

        // run NN processing
        cv_run();

        cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

#ifdef ALT_FILENAMES
        uint16_t imageSeqNum;

        imageSeqNum = fatfs_getImageSequenceNumber();
        // Set the fileOp structure
        setFileOpFromJpeg(jpeg_sz, jpeg_addr, imageSeqNum);
#else
        // Set the fileOp structure
        setFileOpFromJpeg(jpeg_sz, jpeg_addr, g_frames_total);
#endif // ALT_FILENAMES

        dbg_printf(DBG_LESS_INFO, "Writing frame to %s, data size = %d, addr = 0x%x\n",
        		fileOp.fileName, fileOp.length, jpeg_addr);

        // Save the file name as the most recent image
        snprintf(lastImageFileName, IMAGEFILENAMELEN, "%s", fileOp.fileName);

        send_msg.destination = xFatTaskQueue;
        send_msg.message.msg_event = APP_MSG_FATFSTASK_WRITE_FILE;
        send_msg.message.msg_data = (uint32_t)&fileOp;

        image_task_state = APP_IMAGE_TASK_STATE_NN_PROCESSING;
        break;

    case APP_MSG_DPEVENT_EDM_WDT1_TIMEOUT:
    case APP_MSG_DPEVENT_EDM_WDT2_TIMEOUT:
    case APP_MSG_DPEVENT_SENSORCTRL_WDT_OUT:
    	// Unfortunately I see this. EDM WDT2 Timeout = 0x011c
    	// probably if HM0360 is not receiving I2C commands...
    	dbg_printf(DBG_LESS_INFO, "Received a timeout event. TODO - re-initialise HM0360?");
    	// For now, deliberate fall through

    default:
    	flagUnexpectedEvent(img_recv_msg);
    }

    return send_msg;
}

/**
 * Implements state machine for NN processing
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 *
 */
static APP_MSG_DEST_T handleEventForNNProcessing(APP_MSG_T img_recv_msg) {
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;

    send_msg.destination = NULL;
    event = img_recv_msg.msg_event;

    switch (event) {

    case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
        if (img_recv_msg.msg_data == 0) {
#ifdef ALT_FILENAMES
        fatfs_incrementImageSequenceNumber();
#endif // ALT_FILENAMES
        }
        else {
            dbg_printf(DBG_LESS_INFO, "Image not written, error occured: %d", event);
        }

        // This represents the point at which one image has been captured and processed.
        if (g_cur_jpegenc_frame == g_captures_to_take) {
            // Current captures sequence completed
             XP_GREEN;
             xprintf("Current captures completed: %d\n", g_captures_to_take);
             xprintf("Total frames captured since last reset: %d\n", g_frames_total);
             XP_WHITE;

             // Experimental - can we send an unsolicited message to the MKL62BA?
             snprintf(msgToMaster, MSGTOMASTERLEN, "Captured %d images. Last is %s",
             		(int) g_captures_to_take, lastImageFileName);
             sendMsgToMaster(msgToMaster);

             // Resets counters
             g_captures_to_take = 0;
             g_cur_jpegenc_frame = 0;

             image_task_state = APP_IMAGE_TASK_STATE_INIT;
        }
        else {
        	// Start a timer that delays for the defined interval.
        	// When it expires,switch to CAPUTURUNG state and request another image
            if (capture_timer != NULL)  {
                // Change the period and start the timer
            	// The callback issues a APP_MSG_IMAGETASK_RECAPTURE event
                xTimerChangePeriod(capture_timer, pdMS_TO_TICKS(g_timer_period * 1000), 0);
            }
        }

        break;

    case APP_MSG_IMAGETASK_RECAPTURE:
    	// here when the capture_timer expires
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
    send_msg.message.msg_data = img_recv_msg.msg_event;
    send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;


    XP_LT_RED;
    if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST)) {
        xprintf("UNHANDLED event '%s' in '%s'\r\n", imageTaskEventString[event - APP_MSG_IMAGETASK_FIRST], imageTaskStateString[image_task_state]);
    }
    else  {
        xprintf("UNHANDLED event 0x%04x in '%s'\r\n", event, imageTaskStateString[image_task_state]);
    }
    XP_WHITE;

    // If non-null then our task sends another message to another task
    return send_msg;
}

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
    APP_MSG_DEST_T send_msg;
    QueueHandle_t target_queue;
    APP_IMAGE_TASK_STATE_E old_state;
    const char *event_string;
    APP_MSG_EVENT_E event;
    uint32_t recv_data;

    g_frames_total = 0;
    g_cur_jpegenc_frame = 0;
    g_captures_to_take = 0;
    g_timer_period = 0;
    g_img_data = 0;

    // Either way, this will start image capturing
    if (coldBoot == true)  {
        app_start_state(APP_STATE_ALLON);
    }
    else {
        app_start_state(APP_STATE_RESTART);
    }

    // Computer vision init
    if (cv_init(true, true) < 0)  {
        xprintf("cv init fail\n");
        configASSERT(0);
    }
    else {
        xprintf("Initialised neural network.\n");
    }

    // Initial state of the image task (initialized)
    image_task_state = APP_IMAGE_TASK_STATE_INIT;

    for (;;)  {
        // Wait for a message in the queue
        if (xQueueReceive(xImageTaskQueue, &(img_recv_msg), __QueueRecvTicksToWait) == pdTRUE)  {
            event = img_recv_msg.msg_event;
            recv_data = img_recv_msg.msg_data;
            old_state = image_task_state;

            // convert event to a string
            if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST))  {
                event_string = imageTaskEventString[event - APP_MSG_IMAGETASK_FIRST];
            }
            else   {
                event_string = "Unexpected";
            }

            XP_LT_CYAN
            xprintf("\nIMAGE Task event ");
            XP_WHITE;
            xprintf("received '%s' (0x%04x). Received data = 0x%08x\r\n", event_string, event, recv_data);
            xprintf("Image task state = %s (%d)\r\n", imageTaskStateString[image_task_state], image_task_state);

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

            default:
                send_msg = flagUnexpectedEvent(img_recv_msg);
                break;
            }

            if (old_state != image_task_state)   {
                XP_LT_CYAN;
                xprintf("IMAGE Task state changed ");
                XP_WHITE;
                xprintf("from '%s' (%d) to '%s' (%d)\r\n",
                        imageTaskStateString[old_state], old_state,
                        imageTaskStateString[image_task_state], image_task_state);
            }

            // Passes message to other tasks if required (commonly fatfs)
            if (send_msg.destination == NULL)   {
                xprintf("No outgoing messages...\n");
            }
            else  {
                target_queue = send_msg.destination;
                if (xQueueSend(target_queue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
                {
                    xprintf("IMAGE task sending event 0x%x failed\r\n", send_msg.message.msg_event);
                }
                else
                {
                    xprintf("IMAGE task sending event 0x%04x. Value = 0x%08x\r\n", send_msg.message.msg_event, send_msg.message.msg_data);
                }
            }

            xprintf("\r\n");
        }
    }
}

/**
 * Initialises camera capturing
 *
 * @param APP_STATE_E state
 * @return true if initialised. false if no working camera
 */
bool app_start_state(APP_STATE_E state) {

    if (state == APP_STATE_ALLON) {
        if (cisdp_sensor_init(true) < 0) {
            xprintf("\r\nCIS Init fail\r\n");
            return false;
        }
    }
    else if (state == APP_STATE_RESTART)  {
        if (cisdp_sensor_init(false) < 0)  {
            xprintf("\r\nCIS Restart fail\r\n");
            return false;
        }
    }
    else if ( state == APP_STATE_STOP )  {
        xprintf("APP_STATE_STOP\n");
        xprintf("Stopping sensor\n");
        cisdp_sensor_stop();	// run some sensordplib_stop functions then run HM0360_stream_off commands to the HM0360
        return true;
    }

    // if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
    //  Datapath events give callbacks to os_app_dplib_cb() in dp_task
    if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
        xprintf("\r\nDATAPATH Init fail\r\n");
        return false;
    }

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

/********************************** Public Functions  *************************************/

/**
 * Creates the image task.
 *
 * The task itself initialises the Image sensor and then manages requests to access it.
 */
TaskHandle_t image_createTask(int8_t priority, bool coldBootParam) {

    if (priority < 0)  {
        priority = 0;
    }

    //Save this. It may determine what sensor initialisation is required
    coldBoot = coldBootParam;

    image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

    xImageTaskQueue = xQueueCreate(IMAGE_TASK_QUEUE_LEN, sizeof(APP_MSG_T));
    if (xImageTaskQueue == 0)
    {
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

// Temporary until I can make this work through the state machine
// Call back when inactivity is sensed and we want to enter DPD
void image_hackInactive(void) {

	XP_LT_GREEN;
	xprintf("Inactive\n");
	XP_WHITE;

	app_start_state(APP_STATE_STOP); // run some sensordplib_stop functions then run HM0360_stream_off commands to the HM0360
	cisdp_sensor_md_init();

	// Do some configuration of MD parameters
	//hm0360_x_set_threshold(10);

	// Save the state of the imageSequenceNumber
	// TODO this should really be done using the state machine events!
	if (fatfs_getImageSequenceNumber() > 0) {
		fatfs_saveSequenceNumber(fatfs_getImageSequenceNumber());
	}

	xprintf("\nEnter DPD mode!\n\n");
	//app_ledBlue(false);

	app_pmu_enter_dpd(false);	// Does not return
}

