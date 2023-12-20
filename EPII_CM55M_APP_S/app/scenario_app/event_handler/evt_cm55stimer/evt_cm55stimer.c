/**
 ********************************************************************************************
 *  @file      evt_cm55stimer.c
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
#include "evt_cm55stimer.h"
#include "hx_drv_timer.h"
#include "hxevent_debug.h"


/****************************************************
 * Constant Definition                              *
 ***************************************************/


/***************************************************
 * Static Function Declaration
 **************************************************/
static void cm55stimer_callback_fun(void);

/****************************************************
 * Variable Declaration                             *
 ***************************************************/
extern hx_event_t g_event[];


/***************************************************
 * Function Implementation
 **************************************************/
EVT_CM55STIMER_ERR_E evt_cm55stimer_init(void)
{
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = 1000;
	timer_cfg.mode = TIMER_MODE_PERIODICAL;
	timer_cfg.ctrl = TIMER_CTRL_CPU;
	timer_cfg.state = TIMER_STATE_DC;

    hx_drv_timer_cm55s_start(&timer_cfg, cm55stimer_callback_fun);


    return EVT_CM55STIMER_NO_ERROR;
}

// raise timer event
static void cm55stimer_callback_fun(void)
{
    hx_event_activate_ISR(g_event[EVT_INDEX_CM55STIMER]);
}

uint8_t evt_cm55stimer_cb(void)
{
    static uint32_t loop_cnt = 0;

    loop_cnt++;

    dbg_printf(DBG_LESS_INFO, "evt_cm55stimer_cb trigggggger %d times.............\n", loop_cnt);

    return HX_EVENT_RETURN_DONE;
}

