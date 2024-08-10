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

#include "WE2_device.h"
#include "WE2_core.h"
#include "WE2_debug.h"
#include "board.h"
#include "xprintf.h"
#include "printf_x.h"	// Print colours
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
#include "common_config.h"
#include "app_msg.h"
#include "app_state.h"
#include "xprintf.h"
#include "board.h"
#include "app_msg.h"
#include "app_state.h"
#include "dp_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "ww130_test.h"
#include "cvapp.h"
#include "cisdp_sensor.h"
#include "cisdp_cfg.h"
#include "spi_master_protocol.h"
#ifdef IP_timer
#include "timer_interface.h"
#endif

#if ( SUPPORT_FATFS == 1 )
#include "ff.h"
#include "spi_fatfs.h"
static bool ffsInitialised = false;
#endif //( SUPPORT_FATFS == 1 )

extern QueueHandle_t     xMainTaskQueue;
extern QueueHandle_t     xAlgoTaskQueue;
extern volatile APP_ALGO_TASK_STATE_E g_algotask_state;

#if ( SUPPORT_FATFS == 1 )
static uint32_t g_save_jpg_cnt = 0;
#endif	// ( SUPPORT_FATFS == 1 )

#define SPI_SEN_PIC_CLK				(10000000)
#define WE2_CHIP_VERSION_C		    0x8538000c

void algo_task(void *pvParameters) {
	APP_ALGO_TASK_STATE_E old_state;

	// this is used so verose initialisation only happens at cold boot.
	bool coldBoot = *((bool *)pvParameters);

    //xprintf("Coldboot is %s\r\n", coldBoot?"true":"false");

    APP_MSG_T algo_recv_msg;
    APP_MSG_T main_send_msg;
	// CGP unused
	//uint32_t chipid, version;
	//char filename[20];


#if ( SUPPORT_FATFS == 1 )
	char filename[20];
	FRESULT res;
	res = fatfs_init(coldBoot);
    if ( res == FR_OK ) {
    	ffsInitialised = true;
    }
    else {
    	ffsInitialised = false;
        xprintf("Fat FS init fail (reason %d)\r\n", res);
        // Don't return or this task won't operate
        //return;
    }
#else
    if ( hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0 )  {
        xprintf("SPI master init fail\r\n");
        return;
    }
#endif

    g_algotask_state = APP_ALGO_TASK_STATE_INIT;

    for (;;)  {

    	if (xQueueReceive ( xAlgoTaskQueue , &(algo_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE ) {
    		XP_GREEN
			dbg_printf(DBG_LESS_INFO, "\nalgo_task");
    		XP_WHITE;
    		dbg_printf(DBG_LESS_INFO, " received message = 0x%x (data 0x%04x)\r\n",
    				algo_recv_msg.msg_event, algo_recv_msg.msg_data);

    		old_state = g_algotask_state;

    		switch(algo_recv_msg.msg_event)   {

    		case APP_MSG_ALGOEVENT_CHANGE_STATE:
    			// main_task changes our state
    			if (algo_recv_msg.msg_data <= APP_ALGO_TASK_STATE_ERROR) {
    				g_algotask_state = algo_recv_msg.msg_data;
    			}
			break;

    		case APP_MSG_VISIONALGOEVENT_START_ALGO:
    			// Main task says"start processing the frame"
    			g_algotask_state = APP_ALGO_TASK_STATE_DOALGO;

#ifdef CIS_IMX
    			uint32_t chipid;
    			uint32_t version;
    			hx_drv_scu_get_version(&chipid, &version);
    			if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
    			{
    				cisdp_stream_off();
    				set_mipi_csirx_disable();
    			}
#endif

#if 1    // send JPG image
    			uint32_t jpeg_addr, jpeg_sz;
    			int32_t write_status;
    			XP_LT_GREY
				cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
    			XP_WHITE

#if ( SUPPORT_FATFS == 1 )
				if (ffsInitialised) {
					xsprintf(filename, "image%04d.jpg", g_save_jpg_cnt++);
					dbg_printf(DBG_LESS_INFO, "SD card writing %s, data size=%d, addr=0x%x\n", filename, jpeg_sz, jpeg_addr);
					write_status = fastfs_write_image(jpeg_addr, jpeg_sz, (uint8_t *) filename);
					if (write_status != 0) {
						dbg_printf(DBG_LESS_INFO, "  SD card error %d \n", write_status);
					}
				}
				else {
					dbg_printf(DBG_LESS_INFO, "  (Won't attempt fatfs write)\n");
				}
#else
    			write_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
    			xprintf("SPI write frame result %d, data size=%d, addr=0x%x\r\n", write_status, jpeg_sz, jpeg_addr);
#endif  // end SUPPORT_FATFS

#else   // send BGR image
    			SPI_CMD_DATA_TYPE image_type;
    			uint32_t wdam3_addr = app_get_raw_addr();
    			uint32_t data_size = app_get_raw_sz();
    			uint8_t imagesize_header[4];
    			int32_t write_status;
    			image_type = DATA_TYPE_RAW_HEADER_IMG_YUV420_8U3C;
    			imagesize_header[0] = app_get_raw_width() & 0xFF;
    			imagesize_header[1] = (app_get_raw_width()>>8) & 0xFF;
    			imagesize_header[2] = app_get_raw_height() & 0xFF;
    			imagesize_header[3] = (app_get_raw_height()>>8) & 0xFF;
    			write_status =  hx_drv_spi_mst_protocol_write_ex(wdam3_addr, data_size, image_type, imagesize_header, 4);
    			xprintf("addr=0x%x, YUV write frame result %d, data size %d\n", wdam3_addr, write_status, data_size);
#endif

    			cv_run();

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
    			// TODO - I think algo_state should change here...
    			if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    				dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    			}
    			break;

    		case APP_MSG_VISIONALGOEVENT_STOP_ALGO:
    			// Sent by stopCapture()
    			g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO;

    			//hx_drv_timer_cm55x_delay_ms(50, TIMER_STATE_DC);//TODO replace as algo stop

    			main_send_msg.msg_data = 0;
    			main_send_msg.msg_event = APP_MSG_MAINEVENT_VISIONALGO_STOPDONE;
    			if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    				dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    			}
    			break;

    		case APP_MSG_AUDIOALGOEVENT_START_ALGO:
    			// Not used in this app, but presumably a request to process audio
    			g_algotask_state = APP_ALGO_TASK_STATE_DOALGO;
    			dbg_printf(DBG_LESS_INFO, "AUDIOALGOEVENT_START_ALGO size=0x%x\r\n", main_send_msg.msg_data);
    			//hx_drv_timer_cm55x_delay_ms(500, TIMER_STATE_DC);//TODO replace as algo start

    			main_send_msg.msg_data = 0;
    			main_send_msg.msg_event = APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE;
    			if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    				dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    			}
    			break;

    		case APP_MSG_AUDIOALGOEVENT_STOP_ALGO:
    			// Not used in this app, but presumably a request to stop processing audio
    			g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO;
    			dbg_printf(DBG_LESS_INFO, "  AUDIOALGOEVENT_STOP_ALGO\r\n");
    			//hx_drv_timer_cm55x_delay_ms(50, TIMER_STATE_DC);//TODO replace as algo stop

    			main_send_msg.msg_data = 0;
    			main_send_msg.msg_event = APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE;
    			if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    				dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    			}
    			break;

    		default:
    			// CGP - why change state if we get an unrecognised command?
    			g_algotask_state = APP_ALGO_TASK_STATE_ERROR;
    			break;
    		} // switch

    		if (old_state != g_algotask_state) {
    			// state has changed
        		XP_GREEN;
        		dbg_printf(DBG_LESS_INFO, "  Algo_state ");
        		XP_WHITE;
        		dbg_printf(DBG_LESS_INFO, "changed from %d to %d\r\n", old_state, g_algotask_state);
    		}
    	}
    }
}

