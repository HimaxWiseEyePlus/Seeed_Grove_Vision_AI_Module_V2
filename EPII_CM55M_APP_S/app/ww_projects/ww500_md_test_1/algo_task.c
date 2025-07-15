/*
 * visionalgo_task.c
 *
 *  Created on: 2022/02/22
 *      Author: 902447
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "WE2_debug.h"

#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"
#include "xprintf.h"
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
#include "hx_drv_pmu.h"
#include "powermode.h"
#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
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
#include "../ww500_md_test_1/common_config.h"
#include "../ww500_md_test_1/app_msg.h"
#include "../ww500_md_test_1/app_state.h"
#include "xprintf.h"
#include "board.h"
#include "../ww500_md_test_1/app_msg.h"
#include "../ww500_md_test_1/app_state.h"
#include "../ww500_md_test_1/dp_task.h"
#include "../ww500_md_test_1/comm_task.h"
#include "../ww500_md_test_1/algo_task.h"
#include "ww500_md_test_1.h"
#include "../ww500_md_test_1/cvapp.h"
#include "../ww500_md_test_1/cis_sensor/cis_hm0360/cisdp_sensor.h"
#include "../ww500_md_test_1/cis_sensor/cis_hm0360/cisdp_cfg.h"
#include "spi_master_protocol.h"
#ifdef IP_timer
#include "timer_interface.h"
#endif

#include "../ww500_md_test_1/sleep_mode.h"
#include "../ww500_md_test_1/spi_fatfs.h"

extern QueueHandle_t     xMainTaskQueue;
extern QueueHandle_t     xAlgoTaskQueue;
extern volatile APP_ALGO_TASK_STATE_E g_algotask_state;
//static uint32_t g_save_jpg_cnt = 0;

extern uint16_t imageSequenceNumber;

#define SPI_SEN_PIC_CLK				(10000000)
#define WE2_CHIP_VERSION_C		    0x8538000c

void algo_task(void *pvParameters)
{
    APP_MSG_T algo_recv_msg;
    APP_MSG_T main_send_msg;
	// CGP unused uint32_t chipid, version;
	char filename[20];
	TickType_t startTime;
	TickType_t endTime;

#if ( SUPPORT_FATFS == 0 )
    if ( hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0 )
    {
        xprintf("SPI master init fail\r\n");
        return;
    }
#endif

    g_algotask_state = APP_ALGO_TASK_STATE_INIT;

    for (;;)
    {
    	if (xQueueReceive ( xAlgoTaskQueue , &(algo_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE )
        {
            dbg_printf(DBG_LESS_INFO, "algo_recv_msg=0x%x\r\n", algo_recv_msg.msg_event);
            switch(algo_recv_msg.msg_event)
            {
            case APP_MSG_VISIONALGOEVENT_START_ALGO:
                g_algotask_state = APP_ALGO_TASK_STATE_DOALGO;

                #ifdef CIS_IMX
                hx_drv_scu_get_version(&chipid, &version);
                if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
                {
                    cisdp_stream_off();
                    set_mipi_csirx_disable();
                }
                #endif

#if 1    // send JPG image
                uint32_t jpeg_addr, jpeg_sz;
                // Not used int32_t read_status;
                cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
#if ( SUPPORT_FATFS == 1 )
                // Investigate if RTC is working
                rtc_time tm;
                RTC_GetTime(&tm);
                dbg_printf(DBG_LESS_INFO, "At %04d/%02d/%02d %02d:%02d:%02d\n", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                if (imageSequenceNumber > 0) {
                	//xsprintf(filename, "image%04d.jpg", g_save_jpg_cnt++);
                	xsprintf(filename, "image%04d.jpg", imageSequenceNumber++);
                	//dbg_printf(DBG_LESS_INFO, "Write frame to %s, data size=%d, addr=0x%x\n", filename, jpeg_sz, jpeg_addr);
                	dbg_printf(DBG_LESS_INFO, "Write %d bytes to %s\n", jpeg_sz, filename);
                	// read_status = fastfs_write_image(jpeg_addr, jpeg_sz, filename);
                	fastfs_write_image(jpeg_addr, jpeg_sz, (uint8_t *) filename);
                }
#else
				read_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
				//xprintf("write frame result %d, data size=%d,addr=0x%x\r\n", read_status, jpeg_sz, jpeg_addr);
 #endif  // end SUPPORT_FATFS

#else   // send BGR image
				SPI_CMD_DATA_TYPE image_type;
				uint32_t wdam3_addr = app_get_raw_addr();
				uint32_t data_size = app_get_raw_sz();
				uint8_t imagesize_header[4];
                int32_t read_status;
				image_type = DATA_TYPE_RAW_HEADER_IMG_YUV420_8U3C;
				imagesize_header[0] = app_get_raw_width() & 0xFF;
				imagesize_header[1] = (app_get_raw_width()>>8) & 0xFF;
				imagesize_header[2] = app_get_raw_height() & 0xFF;
				imagesize_header[3] = (app_get_raw_height()>>8) & 0xFF;
				read_status =  hx_drv_spi_mst_protocol_write_ex(wdam3_addr, data_size, image_type, imagesize_header, 4);
				xprintf("addr=0x%x, YUV write frame result %d, data size %d\n", wdam3_addr, read_status, data_size);
#endif

				xprintf("Running Neural Network\n");

				startTime = xTaskGetTickCount();  // Get start time

				cv_run();

				endTime = xTaskGetTickCount();  // Get end time

				// Convert ticks to milliseconds
				uint32_t elapsedMs = (endTime - startTime) * portTICK_PERIOD_MS;
				printf("NN took %lums\n", elapsedMs);

				// Finished the capture and processing sequence
				app_ledGreen(false);

                #ifdef CIS_IMX
                hx_drv_scu_get_version(&chipid, &version);
                if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
                {
                    set_mipi_csirx_enable();
                    cisdp_stream_on();
                }
                #endif

                main_send_msg.msg_data = 0;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_VISIONALGO_STARTDONE;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;
            case APP_MSG_VISIONALGOEVENT_STOP_ALGO:
                g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO;
            
                //hx_drv_timer_cm55x_delay_ms(50, TIMER_STATE_DC);//TODO replace as algo stop
            
                main_send_msg.msg_data = 0;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_VISIONALGO_STOPDONE;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;
            case APP_MSG_AUDIOALGOEVENT_START_ALGO:
                g_algotask_state = APP_ALGO_TASK_STATE_DOALGO;
                dbg_printf(DBG_LESS_INFO, "AUDIOALGOEVENT_START_ALGO size=0x%x\r\n", main_send_msg.msg_data);
                //hx_drv_timer_cm55x_delay_ms(500, TIMER_STATE_DC);//TODO replace as algo start
            
                main_send_msg.msg_data = 0;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;
            case APP_MSG_AUDIOALGOEVENT_STOP_ALGO:
                g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO;
                dbg_printf(DBG_LESS_INFO, "AUDIOALGOEVENT_STOP_ALGO\r\n");
                //hx_drv_timer_cm55x_delay_ms(50, TIMER_STATE_DC);//TODO replace as algo stop
            
                main_send_msg.msg_data = 0;
                main_send_msg.msg_event = APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE;
    	   		if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
                {
                    dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
                }
                break;
            default:
                g_algotask_state = APP_ALGO_TASK_STATE_ERROR;
                break;
            }
        }
    }
}

