/*
 * main_task.c
 *
 *  Created on: 31 Jul 2024
 *      Author: CGP
 *
 *      FreeRTOS main_task
 *
 */


/*********************************** Includes ************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "hx_drv_pmu.h"
#include "powermode.h"
#include "sleep_mode.h"

#include "driver_interface.h"
#include "xprintf.h"
#include "printf_x.h"

#include "WE2_debug.h"

#include "comm_task.h"
#include "main_task.h"
#include "ww130_test.h"
#include "dp_task.h"
#include "cisdp_sensor.h"

#include "app_state.h"
#include "app_msg.h"

#include "cvapp.h"
#include "common_config.h"

/*********************************** Definitions ************************************************/

// TODO - is this the best way of managing errors?

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

// I think that task state change should only happen in the task's loop
// and not in main_task. If this is enabled then it does this by sending a new message to the tasks
// to change state.
#define XINTASK

/*********************************** Local function declarations ************************************************/

static void stopCapture(void);

/*********************************** Local Variables ************************************************/

static uint32_t g_algo_done_frame = 0;
static uint32_t g_enter_pmu_frame_cnt = 0;

static APP_MSG_T main_recv_msg;
static APP_MSG_T algo_send_msg;
static APP_MSG_T dp_send_msg;
static uint8_t main_waitstart_cap = 0;	// Seems to be set if there needs to be a delay before starting capture

#ifdef TODO
// to stop compiler warnings
static uint8_t main_motion_detect = 0;
#endif	// TODO

/*********************************** External Variables ************************************************/

extern QueueHandle_t     xMainTaskQueue;
extern QueueHandle_t     xCommTaskQueue;
extern QueueHandle_t     xDPTaskQueue;
extern QueueHandle_t     xAlgoTaskQueue;

extern volatile APP_MAIN_TASK_STATE_E g_maintask_state;
extern volatile APP_ALGO_TASK_STATE_E g_algotask_state;
extern volatile APP_DP_TASK_STATE_E g_dptask_state;
extern volatile APP_COMM_TASK_STATE_E g_commtask_state;

extern uint32_t wakeup_event;
extern uint32_t wakeup_event1;
extern uint32_t g_img_data;

/*********************************** Local function Definitions ************************************************/

/**
 * Sends messages to start capture.
 *
 * Typically called when a button is pressed?
 */
void main_startCapture(void) {

	if (((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END)
			&& (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)) || \
			((g_dptask_state == APP_DP_TASK_STATE_ERROR)
					&& (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))) {
		main_waitstart_cap = 0;
		dp_send_msg.msg_data = 0;
		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;

		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
			dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
		}
	}
	else {
		dbg_printf(DBG_LESS_INFO, "Not starting: g_dptask_state = %d, g_algotask_state = %d\n", g_dptask_state, g_algotask_state);
		if ((g_dptask_state == APP_DP_TASK_STATE_SETUP_CAP_START) || (g_dptask_state == APP_DP_TASK_STATE_SETUP_CAP_END)) {
			dbg_printf(DBG_LESS_INFO, "already in start cap g_dptask_state=%d\r\n", g_dptask_state);
		}
		else {
			main_waitstart_cap = 1;
		}
	}
}

/**
 * Placeholder for code to send EXIF data to the WW130
 */
void main_sendExif(void) {

}

/**
 * Sends messages to stop capture.
 *
 * Typically called when a button is pressed?
 */
static void stopCapture(void) {
	if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_START) || (g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END)) {
		dbg_printf(DBG_LESS_INFO, "g_dptask_state=0x%x no send STOPCAPTURE\r\n", g_dptask_state);
	}
	else {
		dp_send_msg.msg_data = 0;
		dp_send_msg.msg_event = APP_MSG_DPEVENT_STOPCAPTURE;
		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
			dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
		}
	}

	if((g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO)
			|| (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)) {
		dbg_printf(DBG_LESS_INFO, "g_algotask_state = %d no send STOP_ALGO\r\n", g_algotask_state);
	}
	else {
		algo_send_msg.msg_data = 0;
		algo_send_msg.msg_event = APP_MSG_VISIONALGOEVENT_STOP_ALGO;
		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE) {
			dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
		}
	}
}

/*********************************** Exported Function Definitions ************************************************/

/**
 * Initialises sensor capturing
 * TODO - should be in dp_task.c
 */
void app_start_state(APP_STATE_E state) {
	// TODO - this seems to initialise dp_task variables. Should it be called from here?
    dp_var_int();

    if (state == APP_STATE_ALLON)  {
        xprintf("APP_STATE_ALLON\n");
        if (cisdp_sensor_init(true) < 0)  {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
    }
    else if ( state == APP_STATE_RESTART) {
        xprintf("APP_STATE_RESTART\n");
        if (cisdp_sensor_init(false) < 0)   {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
    }
    else if ( state == APP_STATE_STOP )  {
        xprintf("APP_STATE_STOP\n");
        cisdp_sensor_stop();
        return;
    }

    //if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
    // Datapath events give callbacks to os_app_dplib_cb() in dp_task
    if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, os_app_dplib_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0) {
        xprintf("\r\nDATAPATH Init fail\r\n");
        APP_BLOCK_FUNC();
    }

    cisdp_sensor_start();
}


/**
 * FreeRTOS main_task
 */
void main_task(void *pvParameters) {
    uint8_t gpioValue;
	APP_MAIN_TASK_STATE_E old_state;

#if (FLASH_XIP_MODEL == 1)
    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
#endif

    if(cv_init(true, true)<0) {
    	xprintf("cv init fail\n");
        while (1) ;
    }

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		/*Cold Boot*/

		XP_LT_RED;
		xprintf("### Cold Boot ###\n\n");
		XP_WHITE;

		// shows each of the colours
		printf_x_test();

		g_enter_pmu_frame_cnt = SENSOR_AE_STABLE_CNT;

//#define INHIBIT_CAPTURE
#ifdef INHIBIT_CAPTURE
		//hack inhibit image capture
		XP_LT_GREEN;
		xprintf("INHIBITING IMAGE CAPTURE\n");
		XP_WHITE;
#else
    	app_start_state(APP_STATE_ALLON);	// prints the state and initiliases the image sensor
#endif	// INHIBIT_CAPTURE
	}
	else {
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

	XP_LT_GREY;
	xprintf("\nmain_task initialisation finished.\n\n");
	XP_WHITE;

#ifdef SUPPORT_DUAL_CORE
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
	hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);
#endif

	g_maintask_state = APP_MAIN_TASK_STATE_INIT;

	for (;;)  {

    	if (xQueueReceive ( xMainTaskQueue , &(main_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE ) {
    		XP_LT_GREEN
    		dbg_printf(DBG_LESS_INFO, "\nmain_task");
       		XP_WHITE;
       		dbg_printf(DBG_LESS_INFO, " received message = 0x%x\r\n", main_recv_msg.msg_event);

    		old_state = g_maintask_state;

    	   	switch(main_recv_msg.msg_event)  	{

    	   	case APP_MSG_MAINEVENT_CAP_FRAME_DONE:
    	   		// dp_task says "capture done"
    	   		algo_send_msg.msg_data = 0;
    	   		algo_send_msg.msg_event = APP_MSG_VISIONALGOEVENT_START_ALGO;
    	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
    	   		}
    	   		break;

    	   	case APP_MSG_MAINEVENT_VISIONALGO_STARTDONE:
    	   		// Sent by algo_task when it has finished NN processing.
    	   		// Increment a counter and sleep when it reaches a limit
    	   		dbg_printf(DBG_LESS_INFO, "  APP_MSG_MAINEVENT_VISIONALGO_STARTDONE\n");
#ifdef XINTASK
       	   		//algo_send_msg.msg_data = APP_ALGO_TASK_STATE_DOALGO_DONE;	// new state
       	   		algo_send_msg.msg_data = APP_ALGO_TASK_STATE_STOPALGO_DONE;	// new state
        	   		algo_send_msg.msg_event = APP_MSG_ALGOEVENT_CHANGE_STATE;
    	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
    	   		}
#else
    	   		g_algotask_state = APP_ALGO_TASK_STATE_DOALGO_DONE;
#endif	// XINTASK
				g_algo_done_frame++;
				dbg_printf(DBG_LESS_INFO, "g_algo_done_frame = %d/%d\n", g_algo_done_frame, g_enter_pmu_frame_cnt);

#if ( ENTER_SLEEP_MODE == 1 )
				if ( g_algo_done_frame == g_enter_pmu_frame_cnt ) {
					dbg_printf(DBG_LESS_INFO, "\nEnter Sleep for %dms:\n", APP_SLEEP_INTERVAL);
					app_start_state(APP_STATE_STOP);
					// This never returns. After sleeping the processor resets and goes through a warm boot sequence.
					app_pmu_enter_sleep(APP_SLEEP_INTERVAL, 0xFF, 0);	// 1 second or AON_GPIO0 wake up, memory no retention
				}
#endif	// ENTER_SLEEP_MODE

				if ( g_algo_done_frame < g_enter_pmu_frame_cnt ) {
					dbg_printf(DBG_LESS_INFO, "Now capture another image:\n");
					dp_send_msg.msg_data = 0;
					dp_send_msg.msg_event = APP_MSG_DPEVENT_RECAPTURE;
					if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
						dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
					}
				}
				else {
					// We have captured as many images as determined by the settings in common_config.h
					// we will NOT enter the sleep mode invoked by app_pmu_enter_sleep()
					// Instead, we will rely in the FreeRTOS tickless idle mechanism to put us in a low power state
					// until the next interrupt arrives
					XP_GREEN;
					dbg_printf(DBG_LESS_INFO, "Waiting for an event:\n");
					XP_WHITE;

					// CGP this is rather a hack, but main_startCapture expects g_dptask_state = 7
#ifdef XINTASK
					dp_send_msg.msg_data = APP_DP_TASK_STATE_STOP_CAP_END;	// new state
					dp_send_msg.msg_event = APP_MSG_DPEVENT_CHANGE_STATE;
					if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
						dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
					}
#else
					g_dptask_state = APP_DP_TASK_STATE_STOP_CAP_END;
#endif	// XINTASK
				}

				break;

    	   	case APP_MSG_MAINEVENT_DP_ERROR:
#ifdef XINTASK
    	   		dp_send_msg.msg_data = APP_DP_TASK_STATE_ERROR;	// new state
    	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_CHANGE_STATE;
    	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
    	   		}
#else
    	   		g_dptask_state = APP_DP_TASK_STATE_ERROR;
#endif	// XINTASK

    	   	    dbg_printf(DBG_LESS_INFO, "main_recv_msg.msg_data=0x%x fail\r\n", main_recv_msg.msg_data);
#ifdef TODO
    	   	    app_dump_dplib_edminfo();
    	   		main_motion_detect = 0;
    	   	    app_setup_dplib();
#endif
    	   	    break;

    	   	case APP_MSG_MAINEVENT_SENSOR_DP_ERROR:
#ifdef XINTASK
    	   		dp_send_msg.msg_data = APP_DP_TASK_STATE_ERROR;	// new state
    	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_CHANGE_STATE;
    	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
    	   		}
#else
    	   		g_dptask_state = APP_DP_TASK_STATE_ERROR;
#endif	// XINTASK

    	   	    dbg_printf(DBG_LESS_INFO, "main_recv_msg.msg_data=0x%x fail\r\n", main_recv_msg.msg_data);
#ifdef TODO
    	   	    app_dump_dplib_edminfo();
    	   		main_motion_detect = 0;
    	   	    app_setup_dplib();
#endif
    	   	    break;

    	   	case APP_MSG_MAINEVENT_START_CAPTURE:
	   			dbg_printf(DBG_LESS_INFO, "  APP_MSG_MAINEVENT_START_CAPTURE\n");
	   			main_waitstart_cap = 0;
#ifdef XINTASK
    	   		dp_send_msg.msg_data = APP_DP_TASK_STATE_SETUP_CAP_END;	// new state
    	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_CHANGE_STATE;
    	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
    	   		}
#else
    	   		g_dptask_state = APP_DP_TASK_STATE_SETUP_CAP_END;
#endif	// XINTASK

    	   		break;

    	   	case APP_MSG_MAINEVENT_STOP_CAPTURE:
    	   		// Sent by dp_task, in response to stopCapture() request. dp_task has stopped the sensor.
	   			dbg_printf(DBG_LESS_INFO, "  APP_MSG_MAINEVENT_STOP_CAPTURE main_waitstart_cap=%d\n", main_waitstart_cap);
    	   		xQueueReset(xDPTaskQueue);
#ifdef XINTASK
    	   		dp_send_msg.msg_data = APP_DP_TASK_STATE_STOP_CAP_END;	// new state
    	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_CHANGE_STATE;
    	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
    	   		}
#else
    	   		g_dptask_state = APP_DP_TASK_STATE_STOP_CAP_END;
#endif	// XINTASK


    	   		if(main_waitstart_cap == 1) {
        	   		if ((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END)
        	   				&& (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)) {
        	   			main_waitstart_cap = 0;
            	   		dp_send_msg.msg_data = 0;
            	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
            	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
            	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
            	   		}
        	   		}
        	   		else {
        	   			dbg_printf(DBG_LESS_INFO, "Not starting: g_dptask_state = %d, g_algotask_state = %d\n", g_dptask_state, g_algotask_state);
        	   		}
    	   		}
    	   		break;

    	   	case APP_MSG_MAINEVENT_AON_GPIO0_INT:
    	   		// Here on interrupt from PA0 pin
    	   	    hx_drv_gpio_get_in_value(AON_GPIO0, &gpioValue);
    	   		dbg_printf(DBG_LESS_INFO, "  APP_MSG_EVENT_MAIN_AON_GPIO0_INT (data %d, pin AON_GPIO0=%d)\n", main_recv_msg.msg_data, gpioValue);

    	   		//stopCapture();
    	   		main_startCapture();

    	   		break;

    	   	case APP_MSG_MAINEVENT_AON_GPIO1_INT:
    	   		// Here on interrupt from PA1 pin
    	   	    hx_drv_gpio_get_in_value(AON_GPIO1, &gpioValue);
    	   		dbg_printf(DBG_LESS_INFO, "  APP_MSG_EVENT_MAIN_AON_GPIO1_INT %d gpioValue=%d\n", main_recv_msg.msg_data, gpioValue);

    	   		// WARNING: With the current hardware this won't happen!
    	   		stopCapture();

    	   		break;

    	   	case APP_MSG_MAINEVENT_SB_GPIO0_INT:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_SB_GPIO0_INT\n");
    	   		break;

    	   	case APP_MSG_MAINEVENT_SB_GPIO1_INT:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_SB_GPIO1_INT\n");
    	   		break;


    	   	case APP_MSG_MAINEVENT_VISIONALGO_STOPDONE:
    	   		// Sent by algo task when it receives APP_MSG_VISIONALGOEVENT_STOP_ALGO
    	   		// which is caused by stopCapture()
    	   		dbg_printf(DBG_LESS_INFO, "  APP_MSG_MAINEVENT_VISIONALGO_STOPDONE\n");
#ifdef XINTASK
    	   		algo_send_msg.msg_data = APP_ALGO_TASK_STATE_STOPALGO_DONE;	// new state
    	   		algo_send_msg.msg_event = APP_MSG_ALGOEVENT_CHANGE_STATE;
    	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
    	   		}
#else
    	   		g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO_DONE;
#endif	// XINTASK

    	   		if (main_waitstart_cap == 1) {
        	   		if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END)
        	   				&& (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)) {
        	   			main_waitstart_cap = 0;
            	   		dp_send_msg.msg_data = 0;
            	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
            	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE) {
            	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
            	   		}
        	   		}
        	   		else {
        	   			dbg_printf(DBG_LESS_INFO, "Not starting: g_dptask_state = %d, g_algotask_state = %d\n", g_dptask_state, g_algotask_state);
        	   		}
    	   		}
    	   		break;

    	   	case APP_MSG_MAINEVENT_MOTION_DETECT:
#ifdef TODO
	// to stop compiler warnings
    	   		main_motion_detect = 1;
#endif	// TODO
    	   		break;

    	   	case APP_MSG_MAINEVENT_I2CCOMM:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_I2CCOMM\n");
    	   		break;

    	   	case APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE\n");
    	   		break;

    	   	case APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE\n");
    	   		break;

    	   	case APP_MSG_MAINEVENT_VADBUF1_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VADBUF1_NOTIFY size=0x%x\n", main_recv_msg.msg_data);
    	   		break;

    	   	case APP_MSG_MAINEVENT_VADBUF2_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VADBUF2_NOTIFY size=0x%x\n", main_recv_msg.msg_data);
    	   		break;

    	   	case APP_MSG_MAINEVENT_CM55SRDY_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_CM55SRDY_NOTIFY\n");
    	   		break;

    	   	case APP_MSG_MAINEVENT_SENSOR_TIMER:
    	   		break;

    	   	default:
    	   		break;
    	   	} // switch

    		if (old_state != g_maintask_state) {
    			// state has changed
        		XP_LT_GREEN;
        		dbg_printf(DBG_LESS_INFO, "  main_state ");
        		XP_WHITE;
        		dbg_printf(DBG_LESS_INFO, "changed from %d to %d\r\n", old_state, g_maintask_state);
    		}
    	}
    }	// for(;;)
}


