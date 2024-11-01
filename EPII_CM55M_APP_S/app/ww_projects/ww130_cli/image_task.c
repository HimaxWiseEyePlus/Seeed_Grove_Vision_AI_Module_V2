
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
#include "app_msg.h"
#include "CLI-commands.h"
#include "ww130_cli.h"
#include "ff.h"
#include "cisdp_sensor.h"
#include "app_msg.h"
#include "hx_drv_pmu.h"
#include "sleep_mode.h"

#include "driver_interface.h"
#include "cvapp.h"
#include "common_config.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define image_task_PRIORITY (configMAX_PRIORITIES - 3)

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

/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t image_task_id;
QueueHandle_t xImageTaskQueue;

volatile APP_IMAGE_TASK_STATE_E image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

static bool coldBoot;
static uint8_t g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint32_t g_enter_pmu_frame_cnt = 0;
static uint8_t g_spi_master_initial_status;
uint32_t jpeg_addr, jpeg_sz;
uint32_t g_img_data = 0;
uint32_t wakeup_event;
uint32_t wakeup_event1;

// Strings for each of these states. Values must match APP_IMAGE_TASK_STATE_E in image_task.h
const char *imageTaskStateString[APP_IMAGE_TASK_STATE_NUMSTATES] = {
    "Uninitialised",
    "Init",
    "Setup Capture Start",
    "Setup Capture End",
    "Recapture Frame",
    "Capture Frame Ready",
    "Stop Capture Start",
    "Stop Capture End"};

// Strings for expected messages. Values must match messages directed to image Task in app_msg.h
const char *imageTaskEventString[APP_MSG_IMAGETASK_LAST - APP_MSG_IMAGETASK_FIRST] = {
    "Image Event Unkown Event",
    "Image Event Start Capture",
    "Image Event Stop Capture",
    "Image Event ReCapture"};

/********************************** Local Functions  *************************************/

/**
 * Initialises image processing variables
 */
static void image_var_int(void)
{
    g_frame_ready = 0;
    g_cur_jpegenc_frame = 0;
    g_spi_master_initial_status = 0;
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
        dp_msg.msg_event = APP_MSG_DPEVENT_XDMA_FRAME_READY;
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

static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T img_recv_msg)
{
    APP_MSG_EVENT_E event;
    // uint32_t data;
    APP_MSG_DEST_T send_msg;
    send_msg.destination = NULL;
    event = img_recv_msg.msg_event;
    // data = img_recv_msg.msg_data;

    switch (event)
    {
        // DP Event Start
    case APP_MSG_DPEVENT_ERR_FS_HVSIZE: /*!< [8] reg_inpparser_fs_hsize_vsize_error_int_en*/
    case APP_MSG_DPEVENT_ERR_FE_TOGGLE: /*!< [7] reg_inpparser_wait_fe_toggle_error_int_en*/
    case APP_MSG_DPEVENT_ERR_FD_TOGGLE: /*!< [6] reg_inpparser_wait_fd_toggle_error_int_en*/
    case APP_MSG_DPEVENT_ERR_FS_TOGGLE: /*!< [5] reg_inpparser_wait_fs_toggle_error_int_en*/
    case APP_MSG_DPEVENT_ERR_BLANK_ERR: /*!< [4] reg_inpparser_blank_toggle_error_int_en*/
    case APP_MSG_DPEVENT_ERR_CRC_ERR:   /*!< [3] reg_inpparser_crc_error_int_en*/
    case APP_MSG_DPEVENT_ERR_FE_ERR:    /*!< [2] reg_inpparser_fe_cycle_error_int_en*/
    case APP_MSG_DPEVENT_ERR_HSIZE_ERR: /*!< [1] reg_inpparser_hsize_error_int_en*/
    case APP_MSG_DPEVENT_ERR_FS_ERR:    /*!< [0] reg_inpparser_fs_cycle_error_int_en*/
        send_msg.message.msg_data = img_recv_msg.msg_event;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", img_recv_msg.msg_event);
        }
        break;

    case APP_MSG_DPEVENT_VSYNC:            /*!< EDM [0]:vsync_active error*/
    case APP_MSG_DPEVENT_HSYNC:            /*!< EDM [1]:hsync_active error*/
    case APP_MSG_DPEVENT_OVERLAP_VH:       /*!< EDM [2]:overlap_vh error*/
    case APP_MSG_DPEVENT_OVERLAP_HD:       /*!< EDM [3]:overlap_hd error*/
    case APP_MSG_DPEVENT_OVERLAP_DF:       /*!< EDM [4]:overlap_df error*/
    case APP_MSG_DPEVENT_HSYNC_FP:         /*!< EDM [5]:hsync_front_porch error*/
    case APP_MSG_DPEVENT_HSYNC_BP:         /*!< EDM [6]:hsync_back_porch error*/
    case APP_MSG_DPEVENT_FE:               /*!< EDM [7]:frame_end_timing error*/
    case APP_MSG_DPEVENT_CON_VSYNC:        /*!< EDM [8]:con_vsync_active error*/
    case APP_MSG_DPEVENT_CON_HSYNC:        /*!< EDM [9]:con_hsync_active error*/
    case APP_MSG_DPEVENT_CH0_DE_LESS:      /*!< EDM [10]:ch0_de_less error*/
    case APP_MSG_DPEVENT_CH0_DE_MORE:      /*!< EDM [11]:ch0_de_more error*/
    case APP_MSG_DPEVENT_CH1_DE_LESS:      /*!< EDM [12]:ch1_de_less error*/
    case APP_MSG_DPEVENT_CH1_DE_MORE:      /*!< EDM [13]:ch1_de_more error*/
    case APP_MSG_DPEVENT_CH2_DE_LESS:      /*!< EDM [14]:ch2_de_less error*/
    case APP_MSG_DPEVENT_CH2_DE_MORE:      /*!< EDM [15]:ch2_de_more error*/
    case APP_MSG_DPEVENT_CONV_DE_LESS:     /*!< EDM [16]:con_de_less error*/
    case APP_MSG_DPEVENT_CONV_DE_MORE:     /*!< EDM [17]:con_de_more error*/
    case APP_MSG_DPEVENT_EDM_WDT3_TIMEOUT: /*!< EDM WDT3 Timeout*/
    case APP_MSG_DPEVENT_EDM_WDT2_TIMEOUT: /*!< EDM WDT2 Timeout*/
    case APP_MSG_DPEVENT_EDM_WDT1_TIMEOUT: /*!< EDM WDT1 Timeout*/
        send_msg.message.msg_data = img_recv_msg.msg_event;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_SENSOR_DP_ERROR;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_DPEVENT_SENSORCTRL_WDT_OUT: /*!< Sensor Control Timeout (not used in current code)*/
        send_msg.message.msg_data = img_recv_msg.msg_event;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_SENSOR_DP_ERROR;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;
    case APP_MSG_DPEVENT_CDM_FIFO_OVERFLOW:  /*!< CDM Abnormal OVERFLOW*/
    case APP_MSG_DPEVENT_CDM_FIFO_UNDERFLOW: /*!< CDM Abnormal UnderFLOW */
        send_msg.message.msg_data = img_recv_msg.msg_event;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL1: /*!< XDMA_WDMA1STATUS_ERR_FE_COUNT_NOT_REACH */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL2: /*!< XDMA_WDMA1STATUS_ERR_DIS_BEFORE_FINISH */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL3: /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH1_MISMATCH */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL4: /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH2_MISMATCH */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL5: /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH3_MISMATCH*/
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL6: /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH1_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL7: /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH2_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL8: /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH3_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL9: /*!< XDMA_WDMA1STATUS_ERR_BUS */

    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL1: /*!< XDMA_WDMA2STATUS_ERR_FE_COUNT_NOT_REACH */
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL2: /*!< XDMA_WDMA2STATUS_ERR_DIS_BEFORE_FINISH */
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL3: /*!< XDMA_WDMA2STATUS_ERR_FIFO_MISMATCH */
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL4: /*!< XDMA_WDMA2STATUS_ERR_FIFO_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL5: /*!< XDMA_WDMA2STATUS_ERR_BUS */
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL6: /*!< WDMA2 Abnormal Case6 */
    case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL7: /*!< WDMA2 Abnormal Case7 */

    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL1: /*!< XDMA_WDMA3STATUS_ERR_FE_COUNT_NOT_REACH */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL2: /*!< XDMA_WDMA3STATUS_ERR_DIS_BEFORE_FINISH */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL3: /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH1_MISMATCH */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL4: /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH2_MISMATCH */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL5: /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH3_MISMATCH */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL6: /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH1_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL7: /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH2_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL8: /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH3_OVERFLOW */
    case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL9: /*!< XDMA_WDMA3STATUS_ERR_BUS */

    case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL1: /*!< XDMA_RDMASTATUS_ERR_DIS_BEFORE_FINISH */
    case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL2: /*!< XDMA_RDMASTATUS_ERR_BUS */
    case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL3: /*!< RDMA Abnormal Case3 */
    case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL4: /*!< RDMA Abnormal Case4 */
    case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL5: /*!< RDMA Abnormal Case5 */
        send_msg.message.msg_data = img_recv_msg.msg_event;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_DPEVENT_CDM_MOTION_DETECT:
        send_msg.message.msg_data = 0;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_MOTION_DETECT;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_DPEVENT_XDMA_WDMA1_FINISH: /*!< xDMA1 WDMA1 FINISH */
    case APP_MSG_DPEVENT_XDMA_WDMA2_FINISH: /*!< xDMA1 WDMA2 FINISH */
    case APP_MSG_DPEVENT_XDMA_WDMA3_FINISH: /*!< xDMA1 WDMA3 FINISH */
    case APP_MSG_DPEVENT_XDMA_RDMA_FINISH:  /*!< xDMA1 RDMA FINISH */
        break;

    case APP_MSG_DPEVENT_XDMA_FRAME_READY:
        // Data path says frame ready
        image_task_state = APP_IMAGE_TASK_STATE_CAP_FRAMERDY;
        g_cur_jpegenc_frame++;
        g_frame_ready = 1;
        dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
        send_msg.message.msg_event = APP_MSG_MAINEVENT_CAP_FRAME_DONE;
        send_msg.message.msg_data = 0;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_DPEVENT_TIMER_FIRE_APP_NOTREADY: /*!< Timer Fire but app not ready for frame */
        break;

    case APP_MSG_DPEVENT_TIMER_FIRE_APP_READY: /*!< Timer Fire and app ready for frame */
        break;

    case APP_MSG_DPEVENT_UNKOWN: /*!< DP Unknown */
        // error
        send_msg.message.msg_data = 0;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_IMAGETASK_STARTCAPTURE:
        // Start Capture Event
        image_task_state = APP_IMAGE_TASK_STATE_SETUP_CAP_START;
        // TP set msg_data to 0 or msg_event?
        send_msg.message.msg_data = img_recv_msg.msg_event;
        // send_msg.message.msg_data = 0;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_START_CAPTURE;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send img_recv_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_IMAGETASK_STOPCAPTURE:
        image_task_state = APP_IMAGE_TASK_STATE_STOP_CAP_START;
        cisdp_sensor_stop();
        app_start_state(APP_STATE_STOP);
        send_msg.message.msg_data = img_recv_msg.msg_event;
        send_msg.message.msg_event = APP_MSG_MAINEVENT_STOP_CAPTURE;
        if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "send send_msg=0x%x fail\r\n", send_msg.message.msg_event);
        }
        break;

    case APP_MSG_IMAGETASK_RECAPTURE:
        image_task_state = APP_IMAGE_TASK_STATE_RECAP_FRAME;
        sensordplib_retrigger_capture();
        break;

    default:
        // Unexpected event
        flagUnexpectedEvent(img_recv_msg);
        break;
    }
    return send_msg;
}

static APP_MSG_DEST_T handleEventForStopCapture(APP_MSG_T img_recv_msg)
{
    // TODO: Implement this function
}

/**
 * Implements state machine when in APP_FATFS_STATE_BUSY
 *
 */
static APP_MSG_DEST_T handleEventForBusy(APP_MSG_T rxMessage)
{
    APP_MSG_EVENT_E event;
    // uint32_t data;
    APP_MSG_DEST_T sendMsg;
    sendMsg.destination = NULL;

    event = rxMessage.msg_event;
    // data = rxMessage.msg_data;

    switch (event)
    {

    case APP_MSG_IMAGETASK_STOPCAPTURE:
        image_task_state = APP_IMAGE_TASK_STATE_STOP_CAP_START;
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
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T img_recv_msg)
{
    APP_MSG_EVENT_E event;
    APP_MSG_DEST_T send_msg;
    send_msg.destination = NULL;
    event = img_recv_msg.msg_event;
    send_msg.message.msg_data = img_recv_msg.msg_event;
    send_msg.message.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
    // image_task_state = APP_IMAGE_CAPTURE_STATE_ERROR;

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

static void vImageTask(void *pvParameters)
{
    APP_MSG_T img_recv_msg;
    APP_MSG_DEST_T send_msg;
    // TP Is target_queue needed?
    // QueueHandle_t target_queue;
    APP_IMAGE_TASK_STATE_E old_state;
    const char *event_string;
    APP_MSG_EVENT_E event;
    uint32_t rxData;

    hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
    hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

// Currently set to 0
#if (FLASH_XIP_MODEL == 1)
    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
#endif

    if (cv_init(true, true) < 0)
    {
        xprintf("cv init fail\n");
        while (1)
            ;
    }

    // Init camera on task creation
    image_task_state = APP_IMAGE_TASK_STATE_INIT;

    if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
    {
        /*Cold Boot*/
        XP_LT_RED;
        xprintf("### Cold Boot ###\n\n");
        XP_WHITE;
        // shows each of the colours
        printf_x_test();
        g_enter_pmu_frame_cnt = SENSOR_AE_STABLE_CNT;

// #define INHIBIT_CAPTURE
#ifdef INHIBIT_CAPTURE
        // hack inhibit image capture
        XP_LT_GREEN;
        xprintf("INHIBITING IMAGE CAPTURE\n");
        XP_WHITE;
#else
        app_start_state(APP_STATE_ALLON); // prints the state and initiliases the image sensor
#endif // INHIBIT_CAPTURE
    }
    else
    {
        /*Warm Boot*/
        XP_LT_RED;
        xprintf("### Warm Boot ###\n\n");
        XP_WHITE;
        g_enter_pmu_frame_cnt = ENTER_PMU_MODE_FRAME_CNT;
        xprintf("drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_CPU)\n");
        drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_CPU);
        sensordplib_csirx_disable();
        app_start_state(APP_STATE_RESTART);
    }

    for (;;)
    {
        // Wait for a message in the queue
        if (xQueueReceive(xImageTaskQueue, &(img_recv_msg), __QueueRecvTicksToWait) == pdTRUE)
        {
            event = img_recv_msg.msg_event;
            rxData = img_recv_msg.msg_data;
            old_state = image_task_state;

            // convert event to a string
            if ((event >= APP_MSG_IMAGETASK_FIRST) && (event < APP_MSG_IMAGETASK_LAST))
            {
                event_string = imageTaskStateString[event - APP_MSG_IMAGETASK_FIRST];
            }
            else
            {
                event_string = "Unexpected";
            }

            XP_LT_CYAN
            xprintf("\nIMAGE Task");
            XP_WHITE;
            xprintf(" received event '%s' (0x%04x). Value = 0x%08x\r\n", event_string, event, rxData);
            // switch on state - needs to be reviewed as all events are redirected to the "capturing" event handler
            switch (image_task_state)
            {

            case APP_IMAGE_TASK_STATE_UNINIT:
                send_msg = flagUnexpectedEvent(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_SETUP_CAP_START:
                send_msg = handleEventForCapturing(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_SETUP_CAP_END:
                send_msg = handleEventForCapturing(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_RECAP_FRAME:
                send_msg = handleEventForCapturing(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_CAP_FRAMERDY:
                send_msg = handleEventForCapturing(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_STOP_CAP_START:
                send_msg = handleEventForStopCapture(img_recv_msg);
                break;

            case APP_IMAGE_TASK_STATE_STOP_CAP_END:
                send_msg = handleEventForStopCapture(img_recv_msg);
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
        }
    }
}

/**
 * Initialises camera capturing
 */
void app_start_state(APP_STATE_E state)
{
    image_var_int();

    if (state == APP_STATE_ALLON)
    {
        xprintf("APP_STATE_ALLON\n");
        if (cisdp_sensor_init(true) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
        else
        {
            xprintf("CISDP Sensor Start\n");
            cisdp_sensor_start();
        }
    }
    else if (state == APP_STATE_RESTART)
    {
        xprintf("APP_STATE_RESTART\n");
        if (cisdp_sensor_init(false) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
        else
        {
            xprintf("CISDP Sensor Restarted\n");
            cisdp_sensor_start();
        }
    }
    else if (state == APP_STATE_STOP)
    {
        xprintf("APP_STATE_STOP\n");
        cisdp_sensor_stop();
        return;
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
                    3 * configMINIMAL_STACK_SIZE + CLI_CMD_LINE_BUF_SIZE + CLI_OUTPUT_BUF_SIZE,
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