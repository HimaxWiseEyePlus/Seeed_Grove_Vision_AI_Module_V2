/*
 * imgcap_task.c
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
#include "cisdp_sensor.h"
#include "board.h"
#include "xprintf.h"
#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif

#include "allon_sensor_tflm.h"
#include "uln2003.h"
#include "app_msg.h"
#include "app_state.h"
#include "spi_fatfs.h"

extern QueueHandle_t xImgCapTaskQueue;
extern volatile APP_IMGCAP_TASK_STATE_E g_imgcaptask_state;

#define STEP_SIZE 400

void imgcap_task(void *pvParameters)
{
    APP_MSG_T imgcap_recv_msg;
    APP_MSG_T send_msg;

    if (fatfs_init() != 0)
    {
        xprintf("imgcap init fail\r\n");
        return;
    }

    g_imgcaptask_state = APP_IMGCAP_TASK_STATE_INIT;
	char filename[20];
    uint32_t g_frame_cnt = 0;
    /*volatile*/ uint32_t jpeg_addr, jpeg_sz;

    for (;;)
    {
        if (xQueueReceive(xImgCapTaskQueue, &(imgcap_recv_msg), __QueueRecvTicksToWait) == pdTRUE)
        {
            dbg_printf(DBG_LESS_INFO, "======== imgcap_recv_msg=0x%x\r\n", imgcap_recv_msg.msg_event);
            switch (imgcap_recv_msg.msg_event)
            {
            case APP_MSG_IMGCAP_CAPTURE:
		        cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);
                xsprintf(filename, "image%04d.jpg", g_frame_cnt);
                dbg_printf(DBG_LESS_INFO, "write frame to %s, data size=%d,addr=0x%x\n", filename, jpeg_sz, jpeg_addr);
                int write_res = fastfs_write_image(jpeg_addr, jpeg_sz, filename);

                if (write_res)
                {
		            dbg_printf(DBG_LESS_INFO, "ERROR - Unable to write file %s - %i!.\r\n",filename,write_res);
                }
                break;
            default:
                g_imgcaptask_state = APP_IMGCAP_TASK_STATE_ERROR;
                break;
            }
        }
    }
}
