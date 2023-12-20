/**
 ********************************************************************************************
 *  @file      event_handler.c
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

/********************************************************************************************
 *  History :
 *  2019.07.17 - Initial version
 *
 *******************************************************************************************/
#include "stdint.h"
#include "hxevent.h"
#include "event_handler.h"
#include "event_handler_cfg.h"
#include "event_handler_evt.h"
#include "hxevent_debug.h"
#include "hx_drv_timer.h"
#include "WE2_core.h"

#ifdef EVT_I2CS_0_CMD
#include "i2c_comm.h"
#include "evt_i2ccomm.h"

#endif

#ifdef EVT_I2CS_1_CMD
#include "i2c_comm.h"
#include "evt_i2ccomm.h"
#endif

#ifdef EVT_DATAPATH
#include "evt_datapath.h"
#endif

#ifdef EVT_SPIMCOMM
#include "evt_spimcomm.h"
#endif

#ifdef EVT_SPISCOMM
#include "evt_spiscomm.h"
#endif

#ifdef EVT_AUDIO
#include "evt_audio.h"
#endif

#ifdef EVT_ISPCOMM
#include "isp_comm.h"
#include "evt_ispcomm.h"
#endif

#ifdef EVT_UARTCOMM
#include "evt_uartcomm/evt_uartcomm.h"
#endif

#ifdef EVT_CM55STIMER
#include "evt_cm55stimer.h"
#endif

#ifdef EVT_CM55MTIMER
#include "evt_cm55mtimer.h"
#endif

#ifdef EVT_CM55MMB
#include "evt_cm55mmb.h"
#endif

#ifdef EVT_CM55MMB_NBAPP
#include "evt_cm55mmb_nbapp.h"
#endif
/****************************************************
 * Constant Definition                              *
 ***************************************************/
// EVT Priority : lower value with higher priority
#define EVT_PRIORITY_HIGHEST	(0)
#define EVT_PRIORITY_L2			(64)
#define EVT_PRIORITY_L3			(128)
#define EVT_PRIORITY_LOWEST		(255)


/***************************************************
 * Macro Definition
 **************************************************/


/***************************************************
 * Static Function Declaration
 **************************************************/
static void idle(void);		// event idle function which use to override the built-in function in event library



/****************************************************
 * Variable Declaration                             *
 ***************************************************/
hx_event_t g_event[HX_EVENTQUE_MAXSIZE];



/***************************************************
 * Function Implementation
 **************************************************/
// Function used to initialize event handler
void event_handler_init(void)
{

    // init event index
    hx_event_init();

    // Initialize all of supported modules(event) for event handler

#ifdef EVT_I2CS_0_CMD
    evt_i2ccomm_init(USE_DW_IIC_SLV_0);
    hx_event_create(&g_event[EVT_INDEX_I2CS_0_RX]);

    if(g_event[EVT_INDEX_I2CS_0_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_I2CS_0_RX\n");
    }else{
        hx_event_set_callback(g_event[EVT_INDEX_I2CS_0_RX], evt_i2ccomm_0_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_I2CS_0_RX], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_I2CS_0_TX]);
    if(g_event[EVT_INDEX_I2CS_0_TX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_I2CS_0_TX\n");
    }else{
        hx_event_set_callback(g_event[EVT_INDEX_I2CS_0_TX], evt_i2ccomm_0_tx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_I2CS_0_TX], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_I2CS_0_ERR]);
    if(g_event[EVT_INDEX_I2CS_0_ERR] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_I2CS_0_ERR\n");
    }else{
        hx_event_set_callback(g_event[EVT_INDEX_I2CS_0_ERR], evt_i2ccomm_0_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_I2CS_0_ERR], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_I2CS_1_CMD
    evt_i2ccomm_init(USE_DW_IIC_SLV_1);

    hx_event_create(&g_event[EVT_INDEX_I2CS_1_RX]);
    if(g_event[EVT_INDEX_I2CS_1_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_I2CS_1_RX\n");
    }else{
        hx_event_set_callback(g_event[EVT_INDEX_I2CS_1_RX], evt_i2ccomm_1_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_I2CS_1_RX], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_I2CS_1_TX]);
    if(g_event[EVT_INDEX_I2CS_1_TX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_I2CS_1_TX\n");
    }else{
        hx_event_set_callback(g_event[EVT_INDEX_I2CS_1_TX], evt_i2ccomm_1_tx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_I2CS_1_TX], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_I2CS_1_ERR]);
    if(g_event[EVT_INDEX_I2CS_1_ERR] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_I2CS_1_ERR\n");
    }else{
        hx_event_set_callback(g_event[EVT_INDEX_I2CS_1_ERR], evt_i2ccomm_1_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_I2CS_1_ERR], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_DATAPATH
    // Create and register callback for EVT_INDEX_1BITPARSER_ERR
    hx_event_create(&g_event[EVT_INDEX_1BITPARSER_ERR]);
    if(g_event[EVT_INDEX_1BITPARSER_ERR] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_1BITPARSER_ERR\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_1BITPARSER_ERR], evt_1bitParser_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_1BITPARSER_ERR], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_CDM_FIFO_ERR
    hx_event_create(&g_event[EVT_INDEX_CDM_FIFO_ERR]);
    if(g_event[EVT_INDEX_CDM_FIFO_ERR] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CDM_FIFO_ERR\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_CDM_FIFO_ERR], evt_CDM_fifo_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CDM_FIFO_ERR], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_XDMA_WDMA1_ABNORMAL
    hx_event_create(&g_event[EVT_INDEX_XDMA_WDMA1_ABNORMAL]);
    if(g_event[EVT_INDEX_XDMA_WDMA1_ABNORMAL] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_XDMA_WDMA1_ABNORMAL\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_XDMA_WDMA1_ABNORMAL], evt_WDMA1_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_XDMA_WDMA1_ABNORMAL], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_XDMA_WDMA2_ABNORMAL
    hx_event_create(&g_event[EVT_INDEX_XDMA_WDMA2_ABNORMAL]);
    if(g_event[EVT_INDEX_XDMA_WDMA2_ABNORMAL] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_XDMA_WDMA2_ABNORMAL\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_XDMA_WDMA2_ABNORMAL], evt_WDMA2_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_XDMA_WDMA2_ABNORMAL], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_XDMA_WDMA3_ABNORMAL
    hx_event_create(&g_event[EVT_INDEX_XDMA_WDMA3_ABNORMAL]);
    if(g_event[EVT_INDEX_XDMA_WDMA3_ABNORMAL] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_XDMA_WDMA3_ABNORMAL\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_XDMA_WDMA3_ABNORMAL], evt_WDMA3_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_XDMA_WDMA3_ABNORMAL], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_XDMA_RDMA_ABNORMAL
    hx_event_create(&g_event[EVT_INDEX_XDMA_RDMA_ABNORMAL]);
    if(g_event[EVT_INDEX_XDMA_RDMA_ABNORMAL] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_XDMA_RDMA_ABNORMAL\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_XDMA_RDMA_ABNORMAL], evt_RDMA_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_XDMA_RDMA_ABNORMAL], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_EDM_WDT1_TIMEOUT
    hx_event_create(&g_event[EVT_INDEX_EDM_WDT1_TIMEOUT]);
    if(g_event[EVT_INDEX_EDM_WDT1_TIMEOUT] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_EDM_WDT1_TIMEOUT\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_EDM_WDT1_TIMEOUT], evt_EDM_WDT1_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_EDM_WDT1_TIMEOUT], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_EDM_WDT2_TIMEOUT
    hx_event_create(&g_event[EVT_INDEX_EDM_WDT2_TIMEOUT]);
    if(g_event[EVT_INDEX_EDM_WDT2_TIMEOUT] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_EDM_WDT2_TIMEOUT\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_EDM_WDT2_TIMEOUT], evt_EDM_WDT2_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_EDM_WDT2_TIMEOUT], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_EDM_WDT3_TIMEOUT
    hx_event_create(&g_event[EVT_INDEX_EDM_WDT3_TIMEOUT]);
    if(g_event[EVT_INDEX_EDM_WDT3_TIMEOUT] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_EDM_WDT3_TIMEOUT\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_EDM_WDT3_TIMEOUT], evt_EDM_WDT3_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_EDM_WDT3_TIMEOUT], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_CDM_MOTION
    hx_event_create(&g_event[EVT_INDEX_CDM_MOTION]);
    if(g_event[EVT_INDEX_CDM_MOTION] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CDM_MOTION\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_CDM_MOTION], evt_CDM_Motion_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CDM_MOTION], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_XDMA_FRAME_READY
    hx_event_create(&g_event[EVT_INDEX_XDMA_FRAME_READY]);
    if(g_event[EVT_INDEX_XDMA_FRAME_READY] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_XDMA_FRAME_READY\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_XDMA_FRAME_READY], evt_xDMA_FrameReady_cb);
        hx_event_set_priority(g_event[EVT_INDEX_XDMA_FRAME_READY], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_SENSOR_RTC_FIRE
    hx_event_create(&g_event[EVT_INDEX_SENSOR_RTC_FIRE]);
    if(g_event[EVT_INDEX_SENSOR_RTC_FIRE] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_SENSOR_RTC_FIRE\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_SENSOR_RTC_FIRE], evt_Sensor_RTC_Fire_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SENSOR_RTC_FIRE], EVT_PRIORITY_HIGHEST);
    }
#ifdef EVT_DATAPATH_AUTOI2C
    // Create and register callback for EVT_INDEX_HXAUTOI2C_ERR
    hx_event_create(&g_event[EVT_INDEX_HXAUTOI2C_ERR]);
    if(g_event[EVT_INDEX_HXAUTOI2C_ERR] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_HXAUTOI2C_ERR\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_HXAUTOI2C_ERR], evt_hxautoi2c_err_cb);
        hx_event_set_priority(g_event[EVT_INDEX_HXAUTOI2C_ERR], EVT_PRIORITY_HIGHEST);
    }
#endif
#endif

#ifdef EVT_DATAPATH_EXT
    // Create and register callback for EVT_INDEX_PGPIO0
    hx_event_create(&g_event[EVT_INDEX_PGPIO0]);
    if(g_event[EVT_INDEX_PGPIO0] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO0\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO0], evt_PGPIO0_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO0], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_PGPIO1
    hx_event_create(&g_event[EVT_INDEX_PGPIO1]);
    if(g_event[EVT_INDEX_PGPIO1] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO1\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO1], evt_PGPIO1_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO1], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_PGPIO2
    hx_event_create(&g_event[EVT_INDEX_PGPIO2]);
    if(g_event[EVT_INDEX_PGPIO2] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO2\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO2], evt_PGPIO2_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO2], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO3]);
    if(g_event[EVT_INDEX_PGPIO3] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO3\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO3], evt_PGPIO3_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO3], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO4]);
    if(g_event[EVT_INDEX_PGPIO4] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO4\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO4], evt_PGPIO4_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO4], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO5]);
    if(g_event[EVT_INDEX_PGPIO5] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO5\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO5], evt_PGPIO5_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO5], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO6]);
    if(g_event[EVT_INDEX_PGPIO6] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO6\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO6], evt_PGPIO6_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO6], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO7]);
    if(g_event[EVT_INDEX_PGPIO7] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO7\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO7], evt_PGPIO7_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO7], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO8]);
    if(g_event[EVT_INDEX_PGPIO8] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO8\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO8], evt_PGPIO8_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO8], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO9]);
    if(g_event[EVT_INDEX_PGPIO9] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO9\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO9], evt_PGPIO9_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO9], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO10]);
    if(g_event[EVT_INDEX_PGPIO10] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO10\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO10], evt_PGPIO10_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO10], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO11]);
    if(g_event[EVT_INDEX_PGPIO11] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO11\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO11], evt_PGPIO11_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO11], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO12]);
    if(g_event[EVT_INDEX_PGPIO12] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO12\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO12], evt_PGPIO12_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO12], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO13]);
    if(g_event[EVT_INDEX_PGPIO13] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO13\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO13], evt_PGPIO13_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO13], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_PGPIO14]);
    if(g_event[EVT_INDEX_PGPIO14] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_PGPIO14\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_PGPIO14], evt_PGPIO14_cb);
        hx_event_set_priority(g_event[EVT_INDEX_PGPIO14], EVT_PRIORITY_HIGHEST);
    }


    hx_event_create(&g_event[EVT_INDEX_SGPIO0]);
    if(g_event[EVT_INDEX_SGPIO0] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_SGPIO0\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_SGPIO0], evt_SGPIO0_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SGPIO0], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_SGPIO1]);
    if(g_event[EVT_INDEX_SGPIO1] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_SGPIO1\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_SGPIO1], evt_SGPIO1_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SGPIO1], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_ADC_CH0
    hx_event_create(&g_event[EVT_INDEX_ADC_CH0]);
    if(g_event[EVT_INDEX_ADC_CH0] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_ADC_CH0\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ADC_CH0], evt_ADC_Ch0_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ADC_CH0], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_ADC_CH1
    hx_event_create(&g_event[EVT_INDEX_ADC_CH1]);
    if(g_event[EVT_INDEX_ADC_CH1] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_ADC_CH1\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ADC_CH1], evt_ADC_Ch1_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ADC_CH1], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_ADC_CH2
    hx_event_create(&g_event[EVT_INDEX_ADC_CH2]);
    if(g_event[EVT_INDEX_ADC_CH2] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_ADC_CH2\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ADC_CH2], evt_ADC_Ch2_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ADC_CH2], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_ADC_CH3
    hx_event_create(&g_event[EVT_INDEX_ADC_CH3]);
    if(g_event[EVT_INDEX_ADC_CH3] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_ADC_CH3\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ADC_CH3], evt_ADC_Ch3_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ADC_CH3], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_ADC_CFG_INT
    hx_event_create(&g_event[EVT_INDEX_ADC_CFG_INT]);
    if(g_event[EVT_INDEX_ADC_CFG_INT] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_ADC_CFG_INT\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ADC_CFG_INT], evt_ADC_CFG_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ADC_CFG_INT], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_SENSOR_RTC
    hx_event_create(&g_event[EVT_INDEX_SENSOR_RTC]);
    if(g_event[EVT_INDEX_SENSOR_RTC] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_SENSOR_RTC\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_SENSOR_RTC], evt_SensorRTC_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SENSOR_RTC], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for EVT_INDEX_ADC_RTC
    hx_event_create(&g_event[EVT_INDEX_ADC_RTC]);
    if(g_event[EVT_INDEX_ADC_RTC] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_ADC_RTC\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ADC_RTC], evt_ADCRTC_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ADC_RTC], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_SUPPORT_OTA
    // TODO: Initialize OTA Module

#endif
#ifdef EVT_SPIMCOMM
    evt_spimcomm_init();

    // Create and register callback for SPIM_RX
    hx_event_create(&g_event[EVT_INDEX_SPIM1_RX]);
    if(g_event[EVT_INDEX_SPIM1_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for INFRA_EVT_SPIM1_RX\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_SPIM1_RX], evt_spimcomm_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SPIM1_RX], EVT_PRIORITY_HIGHEST);
    }

    // Create and register callback for SPIM_TX
    hx_event_create(&g_event[EVT_INDEX_SPIM1_TX]);
    if(g_event[EVT_INDEX_SPIM1_TX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for INFRA_EVT_SPIM1_TX\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_SPIM1_TX], evt_spimcomm_tx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SPIM1_TX], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_SPISCOMM
    evt_spiscomm_init();

    // Create and register callback for SPIS_RX
    hx_event_create(&g_event[EVT_INDEX_SPIS_RX]);
    if(g_event[EVT_INDEX_SPIS_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for INFRA_EVT_SPIS_RX \n");
    }
    else{
#ifdef DUT
        dbg_printf(DBG_LESS_INFO, "re-register SPIS callback for DUT test \n");
        //re-register SPIS callback for DUT test
        hx_event_set_callback(g_event[EVT_INDEX_SPIS_RX], evt_spiscomm_rx_cb_swtich_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SPIS_RX], 0);      //EVT_PRIORITY_HIGHEST
#else
        hx_event_set_callback(g_event[EVT_INDEX_SPIS_RX], evt_spiscomm_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_SPIS_RX], EVT_PRIORITY_HIGHEST);
#endif
    }
#endif

#ifdef EVT_AUDIO
    evt_audio_init();

    // Create and register callback for AUDIO_RX
    hx_event_create(&g_event[EVT_INDEX_AUDIO_RX]);
    if(g_event[EVT_INDEX_AUDIO_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for INFRA_EVT_AUDIO_RX\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_AUDIO_RX], evt_audio_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_AUDIO_RX], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_ISPCOMM
    evt_ispcomm_init();

    // Create and register callback for ISP_RX
    hx_event_create(&g_event[EVT_INDEX_ISP_RX]);
    if(g_event[EVT_INDEX_ISP_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for INFRA_EVT_ISP_RX\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_ISP_RX], evt_ispcomm_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_ISP_RX], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_UARTCOMM
    evt_uartcomm_init();

    // Create and register callback for UART_RX
    hx_event_create(&g_event[EVT_INDEX_UART_RX]);
    if(g_event[EVT_INDEX_UART_RX] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for INFRA_EVT_UART_RX \n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_UART_RX], evt_uartcomm_rx_cb);
        hx_event_set_priority(g_event[EVT_INDEX_UART_RX], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_CM55STIMER
    evt_cm55stimer_init();

    hx_event_create(&g_event[EVT_INDEX_CM55STIMER]);
    if(g_event[EVT_INDEX_CM55STIMER] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55STIMER \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55STIMER], evt_cm55stimer_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55STIMER], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_CM55MTIMER
    evt_cm55mtimer_init();

    hx_event_create(&g_event[EVT_INDEX_CM55MTIMER]);
    if(g_event[EVT_INDEX_CM55MTIMER] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MTIMER \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MTIMER], evt_cm55mtimer_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MTIMER], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_CM55MMB
    evt_cm55mmb_init();

    hx_event_create(&g_event[EVT_INDEX_CM55MMB]);
    if(g_event[EVT_INDEX_CM55MMB] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB], evt_cm55mmb_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB], EVT_PRIORITY_HIGHEST);
    }
#endif

#ifdef EVT_CM55MMB_NBAPP
    evt_cm55mmb_nbapp_init();

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_PREROLL]);
    if(g_event[EVT_INDEX_CM55MMB_PREROLL] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_PREROLL \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_PREROLL], evt_cm55mmb_preroll_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_PREROLL], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_STOP_PREROLL]);
    if(g_event[EVT_INDEX_CM55MMB_STOP_PREROLL] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_STOP_PREROLL \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_STOP_PREROLL], evt_cm55mmb_stop_preroll_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_STOP_PREROLL], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_ALLON_VID]);
    if(g_event[EVT_INDEX_CM55MMB_ALLON_VID] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_ALLON_VID \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_ALLON_VID], evt_cm55mmb_allon_vid_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_ALLON_VID], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_STOP_ALLON_VID]);
    if(g_event[EVT_INDEX_CM55MMB_STOP_ALLON_VID] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_STOP_ALLON_VID \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_STOP_ALLON_VID], evt_cm55mmb_stop_allon_vid_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_STOP_ALLON_VID], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_CM55S_RDY]);
    if(g_event[EVT_INDEX_CM55MMB_CM55S_RDY] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_CM55S_RDY \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_CM55S_RDY], evt_cm55mmb_cm55s_rdy_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_CM55S_RDY], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_SPION]);
    if(g_event[EVT_INDEX_CM55MMB_SPION] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_SPION \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_SPION], evt_cm55mmb_spion_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_SPION], EVT_PRIORITY_HIGHEST);
    }

    hx_event_create(&g_event[EVT_INDEX_CM55MMB_SPIOFF]);
    if(g_event[EVT_INDEX_CM55MMB_SPIOFF] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_CM55MMB_SPIOFF \n");
    }
    else
    {
        hx_event_set_callback(g_event[EVT_INDEX_CM55MMB_SPIOFF], evt_cm55mmb_spioff_cb);
        hx_event_set_priority(g_event[EVT_INDEX_CM55MMB_SPIOFF], EVT_PRIORITY_HIGHEST);
    }
#endif

    /*
     * if enable WFI must disable systick
     */
#ifdef ENABLE_EVENT_IDLE_WFI
    EPII_Set_Systick_enable(0);
#endif

    // Override Idle event callback function
    hx_event_set_idlecb(idle);

    dbg_printf(DBG_MORE_INFO,"Event loop init done \n");

    return;
}


// Function used to de-initialize event handler
void event_handler_deinit(void)
{
    // TODO: delete and clear all of created event

    return;
}


// Function used to start the event handler process
void event_handler_start(void)
{

    dbg_printf(DBG_MORE_INFO,"Event loop start \n");

    hx_eventloop_start();
    return;
}

// Function used to stop the event handler process
void event_handler_stop(void)
{
    hx_eventloop_stop();
    return;
}



// Callback function for idle event
static void idle(void)
{
    //_sleep(0);
//    dbg_printf(DBG_MORE_INFO, "Idle now can do low priority task\n");
//    hx_drv_timer_cm55x_delay_ms(300, TIMER_STATE_DC);
    //    uint8_t input = console_getchar();
    //    xprintf("Input %d\n", input);

//	__disable_irq(); //disable cpu all irq
//	dbg_printf(DBG_MORE_INFO,"Enter WFI \n");
//	__WFI();
//	dbg_printf(DBG_MORE_INFO,"leave WFI \n");
//	__enable_irq(); //disable cpu all irq
}




