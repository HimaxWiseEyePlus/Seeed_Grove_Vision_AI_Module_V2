/**
 ********************************************************************************************
 *  @file      evt_cm55mmb.c
 *  @details   This file contains all cm55m mailbox event related function
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

#include "WE2_device.h"
#include "board.h"
#include "hxevent.h"
#include "event_handler_evt.h"
#include "evt_cm55mmb_nbapp.h"
#include "hxevent_debug.h"
#include "hx_drv_mb.h"
//#include "aiot_nb_example_tz_s_only.h"



/****************************************************
 * Constant Definition                              *
 ***************************************************/


/***************************************************
 * Static Function Declaration
 **************************************************/
static void cm55mmb_nbapp_callback_fun(uint32_t event);

/****************************************************
 * Variable Declaration                             *
 ***************************************************/
extern hx_event_t g_event[];
static evthandlermbox_CBEvent_t g_mbevt_hdl = NULL;

/***************************************************
 * Function Implementation
 **************************************************/
EVT_CM55MMB_NBAPP_ERR_E evt_cm55mmb_nbapp_init(void)
{
	hx_drv_mb_init(MB_CORE_CM55M, HX_MAILBOX_INT_BASE);

	hx_drv_mb_register_cm55m_cb(cm55mmb_nbapp_callback_fun);

	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_nbapp_init\n");

    return EVT_CM55MMB_NBAPP_NO_ERROR;
}

// raise MB event
static void cm55mmb_nbapp_callback_fun(uint32_t event)
{
//	dbg_printf(DBG_LESS_INFO, "cm55mmb_callback_fun: 0x%08X.....trigger........\n", event);

	//undefine event
	if(event == 0x00)
		return;

	hx_drv_mb_clear_cm55m_irq(event);

	switch (event)
	{
	case MB_CM55BIG_EVT_CM55M_PREROLL:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_PREROLL]);
	    break;
	case MB_CM55BIG_EVT_CM55M_STOP_PREROLL:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_STOP_PREROLL]);
	    break;
	case MB_CM55BIG_EVT_CM55M_ALLON_VID:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_ALLON_VID]);
	    break;
	case MB_CM55BIG_EVT_CM55M_STOP_ALLON_VID:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_STOP_ALLON_VID]);
	    break;
	case MB_CM55BIG_EVT_CM55M_CM55S_READY:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_CM55S_RDY]);
	    break;
	case MB_CM55BIG_EVT_CM55M_SPION:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_SPION]);
	    break;
	case MB_CM55BIG_EVT_CM55M_SPIOFF:
	    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB_SPIOFF]);
	    break;
	default:
		break;
	}

}

uint8_t evt_cm55mmb_preroll_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_PREROLL);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_cm55mmb_stop_preroll_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_STOP_PREROLL);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_cm55mmb_allon_vid_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_ALLON_VID);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_cm55mmb_stop_allon_vid_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_STOP_ALLON_VID);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_cm55mmb_cm55s_rdy_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_CM55S_READY);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_cm55mmb_spion_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_SPION);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_cm55mmb_spioff_cb(void)
{
    if(g_mbevt_hdl != NULL)
    {
        g_mbevt_hdl(MB_CM55BIG_EVT_CM55M_SPIOFF);
    }else{
        dbg_printf(DBG_LESS_INFO,"EVT_INDEX_CDM_FIFO_ERR g_event_cb == NULL\n");
    }

    return HX_EVENT_RETURN_DONE;
}

void evt_cm55mmb_setup_cb(evthandlermbox_CBEvent_t cb)
{
    g_mbevt_hdl = cb;
}
