/**
 ********************************************************************************************
 *  @file      evt_cm55mtimer.c
 *  @details   This file contains all i2c communication event related function
 *  @author    himax/901912
 *  @version   V1.0.0
 *  @date      3-Jan-2022
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

/********************************************************************************************
 *  History :
 *  2022.01.03 - Initial version
 *
 *******************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "hxevent.h"
#include "event_handler_evt.h"
#include "evt_cm55mtimer.h"
#include "hx_drv_timer.h"
#include "hxevent_debug.h"
#ifdef AIOT_NB_EXAMPLE_TZ_S_ONLY
#include "sensor_dp_lib.h"
#endif


/****************************************************
 * Constant Definition                              *
 ***************************************************/


/***************************************************
 * Static Function Declaration
 **************************************************/
static void cm55mtimer_callback_fun(void);

/****************************************************
 * Variable Declaration                             *
 ***************************************************/
extern hx_event_t g_event[];
static evthandlercm55mtimer_CBEvent_t g_timerevt_hdl = NULL;


/***************************************************
 * Function Implementation
 **************************************************/
EVT_CM55MTIMER_ERR_E evt_cm55mtimer_init(void)
{
#ifdef AIOT_NB_EXAMPLE_TZ_S_ONLY
	/*
	 * use for oneshot mode, for Triger SENSOR
	 */
#else
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = 5000;
	timer_cfg.mode = TIMER_MODE_PERIODICAL;
	timer_cfg.ctrl = TIMER_CTRL_CPU;
	timer_cfg.state = TIMER_STATE_DC;

    hx_drv_timer_cm55m_start(&timer_cfg, cm55mtimer_callback_fun);
#endif


    return EVT_CM55MTIMER_NO_ERROR;
}

// raise timer event
static void cm55mtimer_callback_fun(void)
{
    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MTIMER]);

//    dbg_printf(DBG_LESS_INFO, "cm55mtimer_callback_fun \n");
}

uint8_t evt_cm55mtimer_cb(void)
{
#ifdef AIOT_NB_EXAMPLE_TZ_S_ONLY
	/*
	 * use for oneshot mode, for Triger SENSOR
	 */
	if(g_timerevt_hdl != NULL)
	{
		g_timerevt_hdl();
	}
	else
	{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CM55MTIMER g_event_cb == NULL\n");
	}
#else
    static uint32_t loop_cnt = 0;

    loop_cnt++;

    dbg_printf(DBG_LESS_INFO, "evt_cm55mtimer_cb trigggggger %d times.............\n", loop_cnt);
#endif

    return HX_EVENT_RETURN_DONE;
}

void cm55mtimer_trig_oneshot(uint32_t delayms, evthandlercm55mtimer_CBEvent_t cb)
{
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = delayms;
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_CPU;
	timer_cfg.state = TIMER_STATE_DC;

	g_timerevt_hdl = cb;

    hx_drv_timer_cm55m_start(&timer_cfg, (Timer_ISREvent_t)cm55mtimer_callback_fun);
}

void cm55mtimer_stop_oneshot()
{
	hx_drv_timer_cm55m_stop();
}

