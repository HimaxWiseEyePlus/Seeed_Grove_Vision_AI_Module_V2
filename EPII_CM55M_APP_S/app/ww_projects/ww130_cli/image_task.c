
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "WE2_device.h"
#include "WE2_debug.h"
#include "WE2_core.h"
#include "board.h"
#include "printf_x.h"

// FreeRTOS kernel includes.
#ifdef FREERTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
#include "semphr.h"

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif

#include "image_task.h"
#include "fatfs_task.h"
#include "app_msg.h"
#include "CLI-commands.h"
#include "ww130_cli.h"
#include "ff.h"
#include "cisdp_sensor.h"
#include "app_msg.h"
#include "hx_drv_pmu.h"
#include "sleep_mode.h"
#include <sys/time.h>
#include <time.h>

#include "driver_interface.h"
#include "cvapp.h"
#include "common_config.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define image_task_PRIORITY (configMAX_PRIORITIES - 2)

#define IMAGE_TASK_QUEUE_LEN 10
#define VAD_BUFF_SIZE 2048




/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vImageTask(void *pvParameters);

// static FRESULT imageInit(void);

// These are separate event handlers, one for each of the possible state machine state
// static APP_MSG_DEST_T handleEventForIdle(APP_MSG_T rxMessage);
// static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T rxMessage);
void app_start_state(APP_STATE_E state);

/*************************************** External variables *******************************************/

extern SemaphoreHandle_t xI2CTxSemaphore;
extern QueueHandle_t xFatTaskQueue;
fileOperation_t *fileOp;

/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t image_task_id;
QueueHandle_t xImageTaskQueue;

volatile APP_IMAGE_TASK_STATE_E image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

static bool coldBoot;
// For the current request increments
static uint32_t g_cur_jpegenc_frame;
// For current request captures to take
static uint32_t g_captures_to_take;
// For the accumulative total captures
static uint32_t g_frames_total;
static uint32_t timer_period;
//uint16_t g_captures_min;
//uint16_t g_captures_max;
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

/********************************** Local Functions  *************************************/

/**
 * Initialises image processing variables
 */
static void image_var_int(void)
{
    g_frames_total = 0;
    g_cur_jpegenc_frame = 0;
    g_captures_to_take = 0;
    timer_period = 0;
    g_img_data = 0;
    //g_captures_min = MIN_IMAGE_CAPTURES;
    //g_captures_max = MAX_IMAGE_CAPTURES;
}

/**
 * Sets the fileOp pointers for the data retrieved from cisdp_get_jpginfo()
 *
 * NOTE: this doesn't belong here, it belongs coupled with the _get func in cisdp_sensor.c,
 * but since there are so many duplicate files for each sensor, it is easier to put it here for now.
 *
 * Parameters: uint32_t - jpeg_sz, jpeg_addr, frame_num
 */
void set_jpeginfo(uint32_t jpeg_sz, uint32_t jpeg_addr, uint32_t frame_num)
{
    time_t current_time;
    struct tm *time_info;
    char timestamp[20];

    time(&current_time);
    time_info = localtime(&current_time);
    // format timestamp
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d", time_info);

    // Allocate and set fileName
    fileOp->fileName = (char *)pvPortMalloc(24);
    if (fileOp->fileName == NULL)
    {
        printf("Memory allocation for fileName failed.\n");
        return;
    }
    // TODO fix compiler warning. Replace '24' with a defined value
    //  warning: '%04ld' directive output may be truncated writing between 4 and 11 bytes into a region of size between 0 and 19
    snprintf(fileOp->fileName, 24, "%simage%04ld.jpg", timestamp, g_cur_jpegenc_frame);

    fileOp->buffer = (uint8_t *)jpeg_addr;
    fileOp->length = jpeg_sz;
    fileOp->senderQueue = xImageTaskQueue;
    fileOp->closeWhenDone = true;
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

/**
 * Implements state machine when in APP_IMAGE_TASK_STATE_INIT
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForInit(APP_MSG_T img_recv_msg)
{
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    event = img_recv_msg.msg_event;
    send_msg.destination = NULL;

    // first instance for request
    if (g_captures_to_take == 0)
    {
        // seperates the input parameter into two parts, numbers of captures and timer period
        g_captures_to_take = (uint16_t) img_recv_msg.msg_data;
        timer_period = (uint16_t) img_recv_msg.msg_parameter;

        XP_LT_GREEN
        xprintf("Captures to take: %d\n", g_captures_to_take);
        xprintf("Timer period: %ds\n", timer_period);
        XP_WHITE;
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        send_msg.message.msg_data = 0;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_STARTCAPTURE;
    }

    // if input capture parameter is out of range
    if ((g_captures_to_take < MIN_IMAGE_CAPTURES) || (g_captures_to_take > MAX_IMAGE_CAPTURES) ||
    		(timer_period < MIN_IMAGE_INTERVAL) || (timer_period > MAX_IMAGE_INTERVAL) )
    {
        xprintf("Invalid parameter values %d or %d\n", g_captures_to_take, timer_period);
        send_msg = flagUnexpectedEvent(img_recv_msg);
    }
    // keep capturing while frames captured is less than the total captures to take
    else if (g_cur_jpegenc_frame < g_captures_to_take)
    {
    	// How about don't delay for the first image...
    	// TODO a value of time in ms would allow fractional second delays.
    	if (g_cur_jpegenc_frame > 0) {
    		vTaskDelay(pdMS_TO_TICKS(timer_period * 1000)); // Convert timer_period to milliseconds
    	}

        switch (event)
        {
        case APP_MSG_IMAGETASK_STARTCAPTURE:
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
            if (img_recv_msg.msg_data == 0)
            {
                send_msg.message.msg_event = APP_MSG_IMAGETASK_DONE;
                image_task_state = APP_IMAGE_TASK_STATE_INIT;
            }
            else
            {
                dbg_printf(DBG_LESS_INFO, "Image not written, error occured: %d", event);
                flagUnexpectedEvent(img_recv_msg);
            }
            break;

        case APP_MSG_IMAGETASK_DONE:
            send_msg.message.msg_event = APP_MSG_IMAGETASK_DONE;
            image_task_state = APP_IMAGE_TASK_STATE_INIT;
            break;

        default:
            dbg_printf(DBG_LESS_INFO, "Unexpected event: %d", event);
            flagUnexpectedEvent(img_recv_msg);
        }
    }
    else
    {
        // Current request completed
        XP_GREEN
        xprintf("Current captures completed: %d\n", g_captures_to_take);
        xprintf("Total frames captured since last reset: %d\n", g_frames_total);
        XP_WHITE;
        // Resets counters
        g_captures_to_take = 0;
        g_cur_jpegenc_frame = 0;
    }
    return send_msg;
}

/*
 * Handles event path for image capturing
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T img_recv_msg)
{
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    uint32_t jpeg_addr, jpeg_sz;
    event = img_recv_msg.msg_event;

    switch (event)
    {
    // starts sensor and image capturing
    case APP_MSG_IMAGETASK_STARTCAPTURE:
        app_start_state(APP_STATE_RESTART);
        cisdp_sensor_start();
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        send_msg.destination = xImageTaskQueue;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_STOPCAPTURE;
        send_msg.message.msg_data = 0;
        break;

    // resets queue and stops capture
    case APP_MSG_IMAGETASK_STOPCAPTURE:
        xQueueReset(xImageTaskQueue);
        image_task_state = APP_IMAGE_TASK_STATE_BUSY;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_STOPCAPTURE;
        send_msg.message.msg_data = img_recv_msg.msg_event;
        break;

    // recaptures image, not implemented currently
    case APP_MSG_IMAGETASK_RECAPTURE:
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        send_msg.destination = xImageTaskQueue;
        sensordplib_retrigger_capture();
        break;

    // frame ready event received from os_app_dplib_cb
    case APP_MSG_IMAGETASK_FRAME_READY:
        send_msg.destination = xImageTaskQueue;
        image_task_state = APP_IMAGE_TASK_STATE_NN_PROCESSING;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_DONE;
        break;

    // get, set and send info to fatfs task
    case APP_MSG_IMAGETASK_DONE:
        g_cur_jpegenc_frame++;
        g_frames_total++;
        cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
        set_jpeginfo(jpeg_sz, jpeg_addr, g_cur_jpegenc_frame);
        dbg_printf(DBG_LESS_INFO, "write frame to %s, data size=%d,addr=0x%x\n", fileOp->fileName, fileOp->length, jpeg_addr);
        send_msg.destination = xFatTaskQueue;
        send_msg.message.msg_event = APP_MSG_FATFSTASK_WRITE_FILE;
        send_msg.message.msg_data = (uint32_t)&fileOp;
        break;

    // returned from fatfs task
    case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
        send_msg.destination = xImageTaskQueue;
        image_task_state = APP_IMAGE_TASK_STATE_INIT;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_STARTCAPTURE;
        // TODO add error handling for deallocating
        vPortFree(fileOp->fileName);
        fileOp->fileName = NULL;
        fileOp = NULL;
        break;

    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL1:
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL1:
        break;

    default:
        dbg_printf(DBG_LESS_INFO, "Unexpected event: %d", event);
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
static APP_MSG_DEST_T handleEventForNNProcessing(APP_MSG_T img_recv_msg)
{
    APP_MSG_DEST_T send_msg;
    APP_MSG_EVENT_E event;
    send_msg.destination = NULL;
    event = img_recv_msg.msg_event;

    switch (event)
    {
    case APP_MSG_IMAGETASK_DONE:
        // run NN processing
        cv_run();
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        send_msg.destination = xImageTaskQueue;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_DONE;
        send_msg.message.msg_data = 0;
        break;

    case APP_MSG_IMAGETASK_STOPCAPTURE:
        break;

    default:
        flagUnexpectedEvent(img_recv_msg);
        break;
    }

    return send_msg;
}

/**
 * Implements state machine when in APP_FATFS_STATE_BUSY
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 *
 */
static APP_MSG_DEST_T handleEventForBusy(APP_MSG_T img_recv_msg)
{
    APP_MSG_EVENT_E event;
    APP_MSG_DEST_T send_msg;
    send_msg.destination = NULL;
    event = img_recv_msg.msg_event;

    switch (event)
    {
    case APP_MSG_IMAGETASK_STARTCAPTURE:
        send_msg.destination = xImageTaskQueue;
        send_msg.message.msg_event = img_recv_msg.msg_event;
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        break;

    case APP_MSG_IMAGETASK_STOPCAPTURE:
        break;

    case APP_MSG_IMAGETASK_FRAME_READY:
        send_msg.destination = xImageTaskQueue;
        send_msg.message.msg_event = APP_MSG_IMAGETASK_FRAME_READY;
        image_task_state = APP_IMAGE_TASK_STATE_CAPTURING;
        break;

    case APP_MSG_IMAGETASK_DONE:
        break;

    case APP_MSG_IMAGETASK_DISK_WRITE_COMPLETE:
        break;

    default:
        // Here for events that are not expected in this state.
        flagUnexpectedEvent(img_recv_msg);
        break;
    }

    return send_msg;
}

/**
 * For state machine: Print a red message to see if there are unhandled events we should manage
 * Parameters: APP_MSG_T img_recv_msg
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T img_recv_msg)
{
    APP_MSG_EVENT_E event;
    APP_MSG_DEST_T send_msg;
    send_msg.destination = NULL;
    event = img_recv_msg.msg_event;
    send_msg.message.msg_data = img_recv_msg.msg_event;
    send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;

    XP_LT_RED;
    if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST))
    {
        xprintf("UNHANDLED event '%s' in '%s'\r\n", imageTaskEventString[event - APP_MSG_IMAGETASK_FIRST], imageTaskStateString[image_task_state]);
    }
    else
    {
        xprintf("UNHANDLED event 0x%04x in '%s'\r\n", event, imageTaskStateString[image_task_state]);
    }
    XP_WHITE;

    // If non-null then our task sends another message to another task
    return send_msg;
}

/*
 * The main image task
 * Parameters: void *pvParameters
 */
static void vImageTask(void *pvParameters)
{
    APP_MSG_T img_recv_msg;
    APP_MSG_DEST_T send_msg;
    QueueHandle_t target_queue;
    APP_IMAGE_TASK_STATE_E old_state;
    const char *event_string;
    APP_MSG_EVENT_E event;
    uint32_t recv_data;

    // sensor & counter init
    image_var_int();
    app_start_state(APP_STATE_ALLON);

// Currently set to 0
#if (FLASH_XIP_MODEL == 1)
    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
#endif

    // Computer vision init
    if (cv_init(true, true) < 0)
    {
        xprintf("cv init fail\n");
        configASSERT(0);
    }

    // Initial state of the image task (initialized)
    image_task_state = APP_IMAGE_TASK_STATE_INIT;

    for (;;)
    {
        // Wait for a message in the queue
        if (xQueueReceive(xImageTaskQueue, &(img_recv_msg), __QueueRecvTicksToWait) == pdTRUE)
        {
            event = img_recv_msg.msg_event;
            recv_data = img_recv_msg.msg_data;
            old_state = image_task_state;

            // convert event to a string
            if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST))
            {
                event_string = imageTaskEventString[event - APP_MSG_IMAGETASK_FIRST];
            }
            else
            {
                event_string = "Unexpected";
            }

            XP_LT_CYAN
            xprintf("\nIMAGE Task");
            XP_WHITE;
            xprintf(" received event '%s' (0x%04x). Received data = 0x%08x\r\n", event_string, event, recv_data);
            xprintf("Image task state = %s (%d)\r\n", imageTaskStateString[image_task_state], image_task_state);
            switch (image_task_state)
            {
            case APP_IMAGE_TASK_STATE_UNINIT:
                send_msg = flagUnexpectedEvent(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_INIT:
                send_msg = handleEventForInit(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_CAPTURING:
                send_msg = handleEventForCapturing(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_BUSY:
                send_msg = handleEventForBusy(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_NN_PROCESSING:
                send_msg = handleEventForNNProcessing(img_recv_msg);
                break;

            default:
                send_msg = flagUnexpectedEvent(img_recv_msg);
                break;
            }

            if (old_state != image_task_state)
            {
                XP_LT_CYAN;
                xprintf("IMAGE Task state changed ");
                XP_WHITE;
                xprintf("from '%s' (%d) to '%s' (%d)\r\n",
                        imageTaskStateString[old_state], old_state,
                        imageTaskStateString[image_task_state], image_task_state);
            }

            // Passes message to other tasks if required (commonly fatfs)
            if (send_msg.destination == NULL)
            {
                xprintf("No outgoing messages...\n");
            }
            else
            {
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
 * Parameters: APP_STATE_E state
 * Returns: void
 */
void app_start_state(APP_STATE_E state)
{
    if (state == APP_STATE_ALLON)
    {
        if (cisdp_sensor_init(true) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
    }
    else if (state == APP_STATE_RESTART)
    {
        if (cisdp_sensor_init(false) < 0)
        {
            xprintf("\r\nCIS Restart fail\r\n");
            APP_BLOCK_FUNC();
        }
    }

    // if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
    //  Datapath events give callbacks to os_app_dplib_cb() in dp_task
    if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0)
    {
        xprintf("\r\nDATAPATH Init fail\r\n");
        APP_BLOCK_FUNC();
    }
}

/********************************** Public Functions  *************************************/

/**
 * Creates the image task.
 *
 * The task itself initialises the Image sensor and then manages requests to access it.
 */
TaskHandle_t image_createTask(int8_t priority)
{
    if (priority < 0)
    {
        priority = 0;
    }

    hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
    hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

    if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
    {
        coldBoot = true;
    }
    else
    {
        coldBoot = false;
    }

    image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

    xImageTaskQueue = xQueueCreate(IMAGE_TASK_QUEUE_LEN, sizeof(APP_MSG_T));
    if (xImageTaskQueue == 0)
    {
        xprintf("Failed to create xImageTaskQueue\n");
        configASSERT(0); // TODO add debug messages?
    }

    if (xTaskCreate(vImageTask, /*(const char *)*/ "IMAGE",
                    3 * configMINIMAL_STACK_SIZE,
                    &coldBoot, priority,
                    &image_task_id) != pdPASS)
    {
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
const char *image_getStateString(void)
{
    return *&imageTaskStateString[image_task_state];
}
