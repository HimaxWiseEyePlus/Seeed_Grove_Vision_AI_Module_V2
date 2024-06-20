/*
 * comm_task.c
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
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif

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
#include "app_msg.h"
#include "app_state.h"
#include "xprintf.h"
#include "board.h"
#include "i2c_comm.h"
#include "evt_i2ccomm.h"
#include "app_msg.h"
#include "app_state.h"
#include "dp_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "allon_sensor_tflm.h"

#define DBG_EVT_IICS_CMD_LOG             (1)
#define DBG_EVT_IICS_CALLBACK_LOG        (0)

#if DBG_EVT_IICS_CMD_LOG
    #define dbg_evt_iics_cmd(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cmd(fmt, ...)
#endif

#if DBG_EVT_IICS_CALLBACK_LOG
    #define dbg_evt_iics_cb(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cb(fmt, ...)
#endif

/****************************************************
 * Constant Definition                              *
 ***************************************************/
#define DATA_SFT_OFFSET_0           0
#define DATA_SFT_OFFSET_8           8
#define DATA_SFT_OFFSET_16          16
#define DATA_SFT_OFFSET_24          24

#define EVT_I2CS_0_SLV_ADDR         0x62
//#define EVT_I2CS_1_SLV_ADDR         0x64
unsigned char gWrite_buf[1][I2CCOMM_MAX_WBUF_SIZE];
unsigned char gRead_buf[1][I2CCOMM_MAX_RBUF_SIZE];

extern QueueHandle_t     xMainTaskQueue;
extern QueueHandle_t     xCommTaskQueue;
extern volatile APP_COMM_TASK_STATE_E g_commtask_state;

void app_i2ccomm_init();
void aon_gpio0_cb(uint8_t group, uint8_t aIndex);
void aon_gpio1_cb(uint8_t group, uint8_t aIndex);
void i2cs_cb_tx(void *param);
void i2cs_cb_rx(void *param);
void i2cs_cb_err(void *param);
uint8_t evt_i2ccomm_0_rx_cb(void);
void evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_E iic_id);

typedef void (*i2ccomm_customer)(void);

i2ccomm_customer i2ccomm_cmd_customer_process = NULL;

I2CCOMM_CFG_T gI2CCOMM_cfg[1] = {
    {
        EVT_I2CS_0_SLV_ADDR,
        i2cs_cb_tx,
        i2cs_cb_rx,
        i2cs_cb_err}
};

/***************************************************
 * Function Implementation
 **************************************************/
static void prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_E iic_id)
{
    memset((void *)&gRead_buf[iic_id], 0xFF, 4);
    hx_CleanDCache_by_Addr((void *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
}

void app_i2ccomm_init()
{
    USE_DW_IIC_SLV_E iic_id = USE_DW_IIC_SLV_0;

    hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg[iic_id]);

    prv_evt_i2ccomm_clear_read_buf_header(iic_id);
    hx_lib_i2ccomm_start(iic_id, (unsigned char *)&gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);

}

void i2ccomm_cmd_customer_register_cb(void *cb_func)
{
    i2ccomm_cmd_customer_process = cb_func;
}

void i2cs_cb_tx(void *param)
{
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:0) \n", __FUNCTION__);
        comm_send_msg.msg_data = iic_info_ptr->slv_addr;
        comm_send_msg.msg_event = APP_MSG_COMMEVENT_I2CCOMM_TX;
        dbg_printf(DBG_LESS_INFO, "send to comm task 0x%x\r\n", comm_send_msg.msg_event);
    	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken )
        {
        	taskYIELD();
        }
    }else{
        dbg_evt_iics_cb("%s(iic_id:1) txcb not support\n", __FUNCTION__);
    }
}

void i2cs_cb_rx(void *param)
{
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:0) \n", __FUNCTION__);
        comm_send_msg.msg_data = iic_info_ptr->slv_addr;
        comm_send_msg.msg_event = APP_MSG_COMMEVENT_I2CCOMM_RX;
        dbg_printf(DBG_LESS_INFO, "send to comm task 0x%x\r\n", comm_send_msg.msg_event);
    	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken )
        {
        	taskYIELD();
        }
    }else{
        dbg_evt_iics_cb("%s(iic_id:1) rxcb not support\n", __FUNCTION__);
    }
}

void i2cs_cb_err(void *param)
{
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:0) \n", __FUNCTION__);
        comm_send_msg.msg_data = iic_info_ptr->slv_addr;
        comm_send_msg.msg_event = APP_MSG_COMMEVENT_I2CCOMM_ERR;
        dbg_printf(DBG_LESS_INFO, "send to comm task 0x%x\r\n", comm_send_msg.msg_event);
    	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken )
        {
        	taskYIELD();
        }
    }else{
        dbg_evt_iics_cb("%s(iic_id:1) errcb not support\n", __FUNCTION__);
    }
}

uint8_t evt_i2ccomm_0_tx_cb(void)
{
    dbg_evt_iics_cmd("%s \n", __FUNCTION__);
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
    return 0;
}

uint8_t evt_i2ccomm_0_err_cb(void)
{
    dbg_evt_iics_cmd("%s \n", __FUNCTION__);
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
    return 0;
}

uint8_t evt_i2ccomm_0_rx_cb(void)
{
    //xprintf("0x%02x 0x%02x 0x%02x 0x%02x (rx.s)\n", gRead_buf[USE_DW_IIC_SLV_0][0], gRead_buf[USE_DW_IIC_SLV_0][1], gRead_buf[USE_DW_IIC_SLV_0][2], gRead_buf[USE_DW_IIC_SLV_0][3]);
    //hx_InvalidateDCache_by_Addr((uint32_t) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
    //xprintf("0x%02x 0x%02x 0x%02x 0x%02x (rx.e)\n", gRead_buf[USE_DW_IIC_SLV_0][0], gRead_buf[USE_DW_IIC_SLV_0][1], gRead_buf[USE_DW_IIC_SLV_0][2], gRead_buf[USE_DW_IIC_SLV_0][3]);
    uint8_t ret = 0;
    unsigned char feature = gRead_buf[USE_DW_IIC_SLV_0][I2CFMT_FEATURE_OFFSET];
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s(feature:0x%02x) \n", __FUNCTION__, feature);

    switch (feature)
    {
        case I2CCOMM_FEATURE_SYS:
            evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_0);
            prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
            break;

        case I2CCOMM_FEATURE_CUSTOMER_MIN ... I2CCOMM_FEATURE_CUSTOMER_MAX:
            i2ccomm_cmd_customer_process();
            break;

        case I2CCOMM_FEATURE_MAX:
            ret = -1;
            break;

        default:
            prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
            hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
            ret = -1;
            break;
    }
    return ret;
}

// Command process for FEATURE:SYSTEM_INFO
void evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_E iic_id)
{
    int retval = 0;
    unsigned int data;
    unsigned char cmd;
    unsigned short checksum;

    retval = hx_lib_i2ccomm_validate_checksum((unsigned char *) &gRead_buf[iic_id]);
    if (retval != I2CCOMM_NO_ERROR)
    {
        dbg_evt_iics_cmd("%s - checksum validation : FAIL\n", __FUNCTION__);
        prv_evt_i2ccomm_clear_read_buf_header(iic_id);
        hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
        return;
    }

    cmd = gRead_buf[iic_id][I2CFMT_COMMAND_OFFSET];
    dbg_evt_iics_cmd("%s(iic_id:%d, cmd:0x%02x) \n", __FUNCTION__, iic_id, cmd);

    switch (cmd)
    {
        case I2CCOMM_CMD_SYS_GET_VER_BSP:
            // prepare write buffer for write process
            gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
            gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SYS_GET_VER_BSP;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP >> 8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP & 0xFF;

            data = 0xA1B2C3D4;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET] = data & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 1] = (data >> DATA_SFT_OFFSET_8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 2] = (data >> DATA_SFT_OFFSET_16) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 3] = (data >> DATA_SFT_OFFSET_24) & 0xFF;

            hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP, &checksum);

            if (retval == I2CCOMM_NO_ERROR)
            {
                dbg_evt_iics_cmd("checksum generation : 0x%04x \n", checksum);
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = checksum & 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else
            {
                dbg_evt_iics_cmd("[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = 0xFF;
            }

            hx_CleanDCache_by_Addr((void *) &gWrite_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
            break;

        case I2CCOMM_CMD_SYS_GET_VER_I2C:
            // prepare write buffer for write process
            gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
            gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SYS_GET_VER_I2C;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C >> 8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C & 0xFF;

            data = hx_lib_i2ccomm_version();
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET] = data & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 1] = (data >> DATA_SFT_OFFSET_8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 2] = (data >> DATA_SFT_OFFSET_16) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 3] = (data >> DATA_SFT_OFFSET_24) & 0xFF;
            retval = hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C, &checksum);

            if (retval == I2CCOMM_NO_ERROR)
            {
                dbg_evt_iics_cmd("checksum generation : 0x%04x \n", checksum);
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = checksum & 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else
            {
                dbg_evt_iics_cmd("[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = 0xFF;
            }

            hx_CleanDCache_by_Addr((void *) &gWrite_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
            break;

        default:
            /* only get command without write data or information to external I2C master
               or
               undefine command */
            prv_evt_i2ccomm_clear_read_buf_header(iic_id);
            hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            break;
    }
}

void aon_gpio0_cb(uint8_t group, uint8_t aIndex)
{
    uint8_t value;
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    hx_drv_gpio_get_in_value(AON_GPIO0, &value);
    dbg_printf(DBG_LESS_INFO, "aon_gpio0_cb(%d, %d, %d)\n", group, aIndex, value);

    comm_send_msg.msg_data = value;
    comm_send_msg.msg_event = APP_MSG_COMMEVENT_AON_GPIO0_INT;
    dbg_printf(DBG_LESS_INFO, "send to comm task 0x%x\r\n", comm_send_msg.msg_event);
	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	if( xHigherPriorityTaskWoken )
    {
    	taskYIELD();
    }
}

void aon_gpio1_cb(uint8_t group, uint8_t aIndex)
{
    uint8_t value;
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    hx_drv_gpio_get_in_value(AON_GPIO1, &value);
    dbg_printf(DBG_LESS_INFO, "aon_gpio1_cb(%d, %d, %d)\n", group, aIndex, value);

    comm_send_msg.msg_data = value;
    comm_send_msg.msg_event = APP_MSG_COMMEVENT_AON_GPIO1_INT;
    dbg_printf(DBG_LESS_INFO, "send to comm task 0x%x\r\n", comm_send_msg.msg_event);
	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
	hx_drv_gpio_clr_int_status(AON_GPIO1);
	if( xHigherPriorityTaskWoken )
    {
    	taskYIELD();
    }
}

void aon_gpio0_interupt_init()
{
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
    hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
    hx_drv_gpio_cb_register(AON_GPIO0, aon_gpio0_cb);
    hx_drv_gpio_set_input(AON_GPIO0);
    hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
}

void aon_gpio1_interupt_init()
{
    hx_drv_scu_set_PA1_pinmux(SCU_PA1_PINMUX_AON_GPIO1, 1);
    hx_drv_gpio_set_int_type(AON_GPIO1, GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
    hx_drv_gpio_cb_register(AON_GPIO1, aon_gpio1_cb);
    hx_drv_gpio_set_input(AON_GPIO1);
    hx_drv_gpio_set_int_enable(AON_GPIO1, 1);

}


/*!
 * @brief Task responsible for task communication
 */
void comm_task(void *pvParameters)
{
    APP_MSG_T comm_recv_msg;
    APP_MSG_T main_send_msg;

    //init GPIO 0/1, if enable GPIO0/1, please make sure these GPIO are not floating
    //aon_gpio0_interupt_init();
    //aon_gpio1_interupt_init();

    app_i2ccomm_init();

    g_commtask_state = APP_COMM_TASK_STATE_INIT;

    for (;;)
    {
    	if (xQueueReceive ( xCommTaskQueue , &(comm_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE )
        {
    		dbg_printf(DBG_LESS_INFO, "comm_recv_msg=0x%x\r\n", comm_recv_msg.msg_event);
    		switch(comm_recv_msg.msg_event)
            {
    			case APP_MSG_COMMEVENT_AON_GPIO0_INT:
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = comm_recv_msg.msg_data;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_AON_GPIO0_INT;
					if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
					{
						dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
					}
    	   		break;

    			case APP_MSG_COMMEVENT_AON_GPIO1_INT:
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = comm_recv_msg.msg_data;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_AON_GPIO1_INT;
					if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
					{
						dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
					}
    				break;

    			case APP_MSG_COMMEVENT_SB_GPIO0_INT:
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = 0;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_SB_GPIO0_INT;
    				if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    				{
    					dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    				}
                   break;

    			case APP_MSG_COMMEVENT_SB_GPIO1_INT:
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = 0;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_SB_GPIO1_INT;
    				if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE)
    				{
    					dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    				}
    				break;

    			case APP_MSG_COMMEVENT_I2CCOMM_RX:
    				evt_i2ccomm_0_rx_cb();
    				break;

    			case APP_MSG_COMMEVENT_I2CCOMM_TX:
    				evt_i2ccomm_0_tx_cb();
    				break;

    			case APP_MSG_COMMEVENT_I2CCOMM_ERR:
    				evt_i2ccomm_0_err_cb();
    				break;

    			case APP_MSG_COMMEVENT_SPISEND_PIC:
    				dbg_printf(DBG_LESS_INFO, "APP_MSG_COMMEVENT_SPISEND_PIC\r\n");
    				break;

    			default:
    				//TODO error
    				g_commtask_state = APP_COMM_TASK_STATE_ERROR;
    				break;
            }
        }
	}
}
