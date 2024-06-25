/*
 * motor_task.c
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



extern QueueHandle_t     xMotorTaskQueue;
extern volatile APP_MOTOR_TASK_STATE_E g_motortask_state;

#define STEP_SIZE 400 

void motor_task(void *pvParameters)
{
    APP_MSG_T motor_recv_msg;
    APP_MSG_T send_msg;

    if (init_motors() != 0 )
    {
        xprintf("motor init fail\r\n");
        return;
    }

    g_motortask_state = APP_MOTOR_TASK_STATE_INIT;
    uint8_t step_idx = 0;

    for (;;)
    {
    	if (xQueueReceive ( xMotorTaskQueue , &(motor_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE )
        {
            dbg_printf(DBG_LESS_INFO, "======== motor_recv_msg=0x%x\r\n", motor_recv_msg.msg_event);
            switch(motor_recv_msg.msg_event)
            {
            case APP_MSG_MOTOR_M0_CLOCKWISE:
                step_idx = step_some(step_idx,0,1,STEP_SIZE);
                break;
            case APP_MSG_MOTOR_M0_ANTICLOCKWISE:
                step_idx = step_some(step_idx,0,0,STEP_SIZE);
                break;
            case APP_MSG_MOTOR_M1_CLOCKWISE:
                step_idx = step_some(step_idx,1,1,STEP_SIZE);
                break;
            case APP_MSG_MOTOR_M1_ANTICLOCKWISE:
                step_idx = step_some(step_idx,1,0,STEP_SIZE);
                break;
            default:
                g_motortask_state = APP_MOTOR_TASK_STATE_ERROR;
                break;
            }
        }
    }
}

