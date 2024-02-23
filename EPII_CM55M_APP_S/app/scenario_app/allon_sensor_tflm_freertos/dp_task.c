/*
 * dp_task.c
 *
 *  Created on: 2022/2/22
 *      Author: 902447
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#include "WE2_device.h"
#include "WE2_core.h"
#include "WE2_debug.h"
#include "board.h"
#include "xprintf.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#ifdef IP_sensorctrl
#include "hx_drv_sensorctrl.h"
#endif
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#include "sensor_dp_lib.h"
#endif
#ifdef IP_cdm
#include "hx_drv_cdm.h"
#endif
#ifdef IP_edm
#include "hx_drv_edm.h"
#endif
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#ifdef IP_swreg_aon
#include "hx_drv_swreg_aon.h"
#endif
#include "hx_drv_pmu.h"
#include "powermode.h"
#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
#include "hx_drv_CIS_common.h"
#ifdef TRUSTZONE_SEC
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
#ifdef NSC
#include "veneer_table.h"
#endif
/* Trustzone config. */

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif
#endif
#define CIS_XSHUT_SGPIO0
#ifdef CIS_XSHUT_SGPIO0
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
#else
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
#endif
#include "xprintf.h"
#include "board.h"
#include "app_msg.h"
#include "app_state.h"
#include "xprintf.h"
#include "board.h"
#include "app_msg.h"
#include "app_state.h"
#include "dp_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "allon_sensor_tflm.h"
#include "powermode_export.h"
#include "common_config.h"
#include "person_detect_model_data_vela.h"
#include "cvapp.h"
#include "cisdp_sensor.h"
#include "cisdp_cfg.h"
#include "spi_master_protocol.h"
#define FRAME_CHECK_DEBUG 1
#define MAX_STRING  100
#define DEBUG_SPIMST_SENDPICS		(0x01) //0x00: off/ 0x01: JPEG/0x02: YUV422/0x03: YUV420/0x04: YUV400/0x05: RGB

extern QueueHandle_t     xMainTaskQueue;
extern QueueHandle_t     xDPTaskQueue;
extern volatile APP_DP_TASK_STATE_E g_dptask_state;

static uint8_t g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t g_spi_master_initial_status;
uint32_t jpeg_addr, jpeg_sz;
uint32_t g_img_data = 0;

void os_app_dplib_cb(SENSORDPLIB_STATUS_E event)
{
    APP_MSG_T dp_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;
    dbg_printf(DBG_LESS_INFO, "os_app_dplib_cb event=%d\n",event);
    switch(event)
    {
	case SENSORDPLIB_STATUS_ERR_FS_HVSIZE:
	case SENSORDPLIB_STATUS_ERR_FE_TOGGLE:
	case SENSORDPLIB_STATUS_ERR_FD_TOGGLE:
	case SENSORDPLIB_STATUS_ERR_FS_TOGGLE:
	case SENSORDPLIB_STATUS_ERR_BLANK_ERR:/*reg_inpparser_stall_error*/
	case SENSORDPLIB_STATUS_ERR_CRC_ERR:  /*reg_inpparser_crc_error*/
	case SENSORDPLIB_STATUS_ERR_FE_ERR:  /*reg_inpparser_fe_cycle_error*/
	case SENSORDPLIB_STATUS_ERR_HSIZE_ERR:  /*reg_inpparser_hsize_error*/
	case SENSORDPLIB_STATUS_ERR_FS_ERR:  /*reg_inpparser_fs_cycle_error*/
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
    dbg_printf(DBG_LESS_INFO, "send to dp task 0x%x\r\n", dp_msg.msg_event);
	xQueueSendFromISR( xDPTaskQueue, &dp_msg, &xHigherPriorityTaskWoken );
    if( xHigherPriorityTaskWoken )
    {
    	taskYIELD();
    }
}


static void dp_var_int()
{
    g_frame_ready = 0;
    g_cur_jpegenc_frame = 0;
    g_spi_master_initial_status = 0;
}


void app_start_state(APP_STATE_E state)
{
    dp_var_int();

    if (state == APP_STATE_ALLON)
    {
        xprintf("APP_STATE_ALLON\n");
        if (cisdp_sensor_init(true) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
    }
    else if ( state == APP_STATE_RESTART)
    {
        xprintf("APP_STATE_RESTART\n");
        if (cisdp_sensor_init(false) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
    }
    else if ( state == APP_STATE_STOP )
    {
        xprintf("APP_STATE_STOP\n");
        cisdp_sensor_stop();
        return;
    }

    //if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
    if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0)
    {
        xprintf("\r\nDATAPATH Init fail\r\n");
        APP_BLOCK_FUNC();
    }

    cisdp_sensor_start();
}


/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
void dp_task(void *pvParameters)
{
    APP_MSG_T dp_recv_msg;
    APP_MSG_T main_send_msg;

    g_dptask_state = APP_DP_TASK_STATE_INIT;

    for (;;)
    {
    	if (xQueueReceive ( xDPTaskQueue , &(dp_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE )
        {
            dbg_printf(DBG_LESS_INFO, "dp_recv_msg_event=0x%x\r\n", dp_recv_msg.msg_event);
            switch(dp_recv_msg.msg_event)
            {
                //DP Event Start
            case APP_MSG_DPEVENT_ERR_FS_HVSIZE:  /*!< [8] reg_inpparser_fs_hsize_vsize_error_int_en*/
            case APP_MSG_DPEVENT_ERR_FE_TOGGLE:  /*!< [7] reg_inpparser_wait_fe_toggle_error_int_en*/
            case APP_MSG_DPEVENT_ERR_FD_TOGGLE:     /*!< [6] reg_inpparser_wait_fd_toggle_error_int_en*/
            case APP_MSG_DPEVENT_ERR_FS_TOGGLE:     /*!< [5] reg_inpparser_wait_fs_toggle_error_int_en*/
            case APP_MSG_DPEVENT_ERR_BLANK_ERR:  /*!< [4] reg_inpparser_blank_toggle_error_int_en*/
            case APP_MSG_DPEVENT_ERR_CRC_ERR:  /*!< [3] reg_inpparser_crc_error_int_en*/
            case APP_MSG_DPEVENT_ERR_FE_ERR:  /*!< [2] reg_inpparser_fe_cycle_error_int_en*/
            case APP_MSG_DPEVENT_ERR_HSIZE_ERR:  /*!< [1] reg_inpparser_hsize_error_int_en*/
            case APP_MSG_DPEVENT_ERR_FS_ERR:  /*!< [0] reg_inpparser_fs_cycle_error_int_en*/
                main_send_msg.msg_data = dp_recv_msg.msg_event;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;

            case APP_MSG_DPEVENT_VSYNC:    /*!< EDM [0]:vsync_active error*/
            case APP_MSG_DPEVENT_HSYNC:    /*!< EDM [1]:hsync_active error*/
            case APP_MSG_DPEVENT_OVERLAP_VH:    /*!< EDM [2]:overlap_vh error*/
            case APP_MSG_DPEVENT_OVERLAP_HD:    /*!< EDM [3]:overlap_hd error*/
            case APP_MSG_DPEVENT_OVERLAP_DF:    /*!< EDM [4]:overlap_df error*/
            case APP_MSG_DPEVENT_HSYNC_FP:    /*!< EDM [5]:hsync_front_porch error*/
            case APP_MSG_DPEVENT_HSYNC_BP:    /*!< EDM [6]:hsync_back_porch error*/
            case APP_MSG_DPEVENT_FE:    /*!< EDM [7]:frame_end_timing error*/
            case APP_MSG_DPEVENT_CON_VSYNC:    /*!< EDM [8]:con_vsync_active error*/
            case APP_MSG_DPEVENT_CON_HSYNC:    /*!< EDM [9]:con_hsync_active error*/
            case APP_MSG_DPEVENT_CH0_DE_LESS:    /*!< EDM [10]:ch0_de_less error*/
            case APP_MSG_DPEVENT_CH0_DE_MORE:    /*!< EDM [11]:ch0_de_more error*/
            case APP_MSG_DPEVENT_CH1_DE_LESS:    /*!< EDM [12]:ch1_de_less error*/
            case APP_MSG_DPEVENT_CH1_DE_MORE:    /*!< EDM [13]:ch1_de_more error*/
            case APP_MSG_DPEVENT_CH2_DE_LESS:    /*!< EDM [14]:ch2_de_less error*/
            case APP_MSG_DPEVENT_CH2_DE_MORE:    /*!< EDM [15]:ch2_de_more error*/
            case APP_MSG_DPEVENT_CONV_DE_LESS:    /*!< EDM [16]:con_de_less error*/
            case APP_MSG_DPEVENT_CONV_DE_MORE:    /*!< EDM [17]:con_de_more error*/
            case APP_MSG_DPEVENT_EDM_WDT3_TIMEOUT:    /*!< EDM WDT3 Timeout*/
            case APP_MSG_DPEVENT_EDM_WDT2_TIMEOUT:    /*!< EDM WDT2 Timeout*/
            case APP_MSG_DPEVENT_EDM_WDT1_TIMEOUT:    /*!< EDM WDT1 Timeout*/
                main_send_msg.msg_data = dp_recv_msg.msg_event;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_SENSOR_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    	   		}
    	   		break;

    	   	case APP_MSG_DPEVENT_SENSORCTRL_WDT_OUT:	/*!< Sensor Control Timeout (not used in current code)*/
    	   		main_send_msg.msg_data = dp_recv_msg.msg_event;
    	   		main_send_msg.msg_event = APP_MSG_MAINEVENT_SENSOR_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    	   		}
    	   		break;
    	   	case APP_MSG_DPEVENT_CDM_FIFO_OVERFLOW:	/*!< CDM Abnormal OVERFLOW*/
    	   	case APP_MSG_DPEVENT_CDM_FIFO_UNDERFLOW:	/*!< CDM Abnormal UnderFLOW */
    	   		main_send_msg.msg_data = dp_recv_msg.msg_event;
    	   		main_send_msg.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    	   		}
    	   		break;
            
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL1:    /*!< XDMA_WDMA1STATUS_ERR_FE_COUNT_NOT_REACH */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL2:    /*!< XDMA_WDMA1STATUS_ERR_DIS_BEFORE_FINISH */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL3:    /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH1_MISMATCH */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL4:    /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH2_MISMATCH */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL5:    /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH3_MISMATCH*/
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL6:    /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH1_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL7:    /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH2_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL8:    /*!< XDMA_WDMA1STATUS_ERR_FIFO_CH3_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA1_ABNORMAL9:    /*!< XDMA_WDMA1STATUS_ERR_BUS */
            
            
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL1:    /*!< XDMA_WDMA2STATUS_ERR_FE_COUNT_NOT_REACH */
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL2:    /*!< XDMA_WDMA2STATUS_ERR_DIS_BEFORE_FINISH */
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL3:    /*!< XDMA_WDMA2STATUS_ERR_FIFO_MISMATCH */
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL4:    /*!< XDMA_WDMA2STATUS_ERR_FIFO_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL5:    /*!< XDMA_WDMA2STATUS_ERR_BUS */
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL6:    /*!< WDMA2 Abnormal Case6 */
            case APP_MSG_DPEVENT_XDMA_WDMA2_ABNORMAL7:    /*!< WDMA2 Abnormal Case7 */
            
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL1:    /*!< XDMA_WDMA3STATUS_ERR_FE_COUNT_NOT_REACH */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL2:    /*!< XDMA_WDMA3STATUS_ERR_DIS_BEFORE_FINISH */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL3:    /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH1_MISMATCH */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL4:    /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH2_MISMATCH */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL5:    /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH3_MISMATCH */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL6:    /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH1_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL7:    /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH2_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL8:    /*!< XDMA_WDMA3STATUS_ERR_FIFO_CH3_OVERFLOW */
            case APP_MSG_DPEVENT_XDMA_WDMA3_ABNORMAL9:    /*!< XDMA_WDMA3STATUS_ERR_BUS */
            
            case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL1:    /*!< XDMA_RDMASTATUS_ERR_DIS_BEFORE_FINISH */
            case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL2:    /*!< XDMA_RDMASTATUS_ERR_BUS */
            case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL3:    /*!< RDMA Abnormal Case3 */
            case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL4:    /*!< RDMA Abnormal Case4 */
            case APP_MSG_DPEVENT_XDMA_RDMA_ABNORMAL5:    /*!< RDMA Abnormal Case5 */
                main_send_msg.msg_data = dp_recv_msg.msg_event;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    	   		}
    	   		break;

    	   	case APP_MSG_DPEVENT_CDM_MOTION_DETECT:
    	   		main_send_msg.msg_data = 0;
    	   		main_send_msg.msg_event = APP_MSG_MAINEVENT_MOTION_DETECT;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;

            case APP_MSG_DPEVENT_XDMA_WDMA1_FINISH:    /*!< xDMA1 WDMA1 FINISH */
            case APP_MSG_DPEVENT_XDMA_WDMA2_FINISH:    /*!< xDMA1 WDMA2 FINISH */
            case APP_MSG_DPEVENT_XDMA_WDMA3_FINISH:    /*!< xDMA1 WDMA3 FINISH */
            case APP_MSG_DPEVENT_XDMA_RDMA_FINISH:    /*!< xDMA1 RDMA FINISH */
                break;

            case APP_MSG_DPEVENT_XDMA_FRAME_READY:
                g_dptask_state = APP_DP_TASK_STATE_CAP_FRAMERDY;
                //app_dump_jpeginfo();
                g_cur_jpegenc_frame++;
                g_frame_ready = 1;
		        dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
                main_send_msg.msg_event = APP_MSG_MAINEVENT_CAP_FRAME_DONE;
                main_send_msg.msg_data = 0;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;

            case APP_MSG_DPEVENT_TIMER_FIRE_APP_NOTREADY:    /*!< Timer Fire but app not ready for frame */
                break;

            case APP_MSG_DPEVENT_TIMER_FIRE_APP_READY:    /*!< Timer Fire and app ready for frame */
                break;

            case APP_MSG_DPEVENT_UNKOWN:    /*!< DP Unknown */
                //error
                main_send_msg.msg_data = 0;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;

            case APP_MSG_DPEVENT_STARTCAPTURE:
                g_dptask_state = APP_DP_TASK_STATE_SETUP_CAP_START;
                app_start_state(APP_STATE_ALLON);
                main_send_msg.msg_data = dp_recv_msg.msg_event;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_START_CAPTURE;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;

            case APP_MSG_DPEVENT_STOPCAPTURE:
                g_dptask_state = APP_DP_TASK_STATE_STOP_CAP_START;
        		cisdp_sensor_stop();
                main_send_msg.msg_data = dp_recv_msg.msg_event;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_STOP_CAPTURE;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;

            case APP_MSG_DPEVENT_RECAPTURE:
                g_dptask_state = APP_DP_TASK_STATE_RECAP_FRAME;
                sensordplib_retrigger_capture();
                break;

    	   	case APP_MSG_DPEVENT_1BITPARSER_ERR:
    	   		main_send_msg.msg_data = dp_recv_msg.msg_event;
    	   		main_send_msg.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
				uint32_t err;
                hx_drv_inp1bitparser_get_errstatus((uint16_t *)&err);
                dbg_printf(DBG_LESS_INFO, "DPEVENT_1BITPARSER_ERR err=0x%x\r\n",err);
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    	   		}
    	   		break;

            default:
                g_dptask_state = APP_DP_TASK_STATE_ERROR;
                main_send_msg.msg_data = dp_recv_msg.msg_event;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_DP_ERROR;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;
            }
        }
    }
}
