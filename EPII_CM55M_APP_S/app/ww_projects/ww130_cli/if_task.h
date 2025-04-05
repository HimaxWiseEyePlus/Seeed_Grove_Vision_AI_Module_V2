/*
 * if_task.h
 *
 *  Created on: 12 Aug 2024
 *      Author: CGP
 *
 * This task handles communications between the Seeed board and the WW130
 */

#ifndef APP_WW_PROJECTS_WW130_CLI_IF_H_
#define APP_WW_PROJECTS_WW130_CLI_IF_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#include "queue.h"
#include "i2c_comm.h"


// Although Himax allows 256, this gives a total length of 256 + 32
// The WW130 can't handle more than 255
// On the WW130 I have chosen 244 which matches the max length for the BLE NUS payload.
// But choose a number which is a multiple of 32 (I don't understand the cache invalidation...)
// TODO check for any connection with FreeRTOS configCOMMAND_INT_MAX_OUTPUT_SIZE (256)
#define WW130_MAX_PAYLOAD_SIZE			244
#define PADDING_ALIGN_SCB_DCACHE (256 - (I2CCOMM_HEADER_SIZE + WW130_MAX_PAYLOAD_SIZE + I2CCOMM_CHECKSUM_SIZE))

#define WW130_MAX_WBUF_SIZE   (I2CCOMM_HEADER_SIZE + WW130_MAX_PAYLOAD_SIZE + I2CCOMM_CHECKSUM_SIZE + PADDING_ALIGN_SCB_DCACHE)
#define WW130_MAX_RBUF_SIZE   (I2CCOMM_HEADER_SIZE + WW130_MAX_PAYLOAD_SIZE + I2CCOMM_CHECKSUM_SIZE + PADDING_ALIGN_SCB_DCACHE)


/* Task priorities. */
//#define if_task_PRIORITY	(configMAX_PRIORITIES - 2)

// The states for the if_task
// Must match the ifTaskStateString[] array in if_task.c
// APP_IF_STATE_NUMSTATES is only used to establish the number of states
typedef enum {
	APP_IF_STATE_UNINIT						=0x0000,	// Uninitialised - before task starts
	APP_IF_STATE_IDLE						=0x0001,	// While nothing else is happening
	APP_IF_STATE_I2C_RX						=0x0002,	// I2C data arrives from master (exchange initiated by master)
	APP_IF_STATE_I2C_TX						=0x0003,	// I2C is transmitting to master (exchange initiated by master)
	APP_IF_STATE_I2C_SLAVE_TX				=0x0004,	// I2C is transmitting to master (exchange initiated by slave)
	APP_IF_STATE_I2C_SLAVE_RX				=0x0005,	// Waiting for I2C data to arrive from master (exchange initiated by slave)
	APP_IF_STATE_PA0						=0x0006,	// Used in testing inter-processor communications interrupt
	APP_IF_STATE_DISK_OP					=0x0007,
	APP_IF_STATE_NUMSTATES					=0x0008
} APP_IF_STATE_E;

/**
 * \enum I2CCOMM_FEATURE_E
 *
 * This is borrowed from evt_i2ccomm.h - we don't need all of the features from that.
 *
 * \brief this enumeration use in i2c communication library, define the supported feature.
 */
typedef enum
{
    I2CCOMM_FEATURE_SYS                 = 0x00, /**< SUPPORT FEATURE: system information*/
    I2CCOMM_FEATURE_REG_ACCES           = 0x10, /**< SUPPORT FEATURE: WE-I Register read/write*/
    I2CCOMM_FEATURE_GPIO_CTRL           = 0x11,
    I2CCOMM_FEATURE_SENSOR_REG_ACCESS   = 0x20,
    I2CCOMM_FEATURE_QUANTA_ISH          = 0x30, /**< SUPPORT FEATURE: Quanta NB Product - ISH communication*/
    I2CCOMM_FEATURE_SPI                 = 0x40,
    I2CCOMM_FEATURE_OTA_RESERVED        = 0x50,
    I2CCOMM_FEATURE_OTA_ALL             = 0x51,
    I2CCOMM_FEATURE_OTA_MEM_DSP         = 0x52,
    I2CCOMM_FEATURE_OTA_APP             = 0x53,
    I2CCOMM_FEATURE_OTA_APP_CONFIG      = 0x54,
    I2CCOMM_FEATURE_OTA_EMZA_CONFIG     = 0x55,
    I2CCOMM_FEATURE_OTA_CNNLUT          = 0x56,
    I2CCOMM_FEATURE_AUDIO_TEST          = 0x57,
    I2CCOMM_FEATURE_CSTM_SECURE_DGB     = 0x70,

    I2CCOMM_FEATURE_CUSTOMER_MIN        = 0x80,
    I2CCOMM_FEATURE_CUSTOMER_MAX        = 0x8F,
    I2CCOMM_FEATURE_MAX
} I2CCOMM_FEATURE_E;

// 1st byte in incoming I2C messages designate where they should be routed
#define WW130_CMD_FEATURE	I2CCOMM_FEATURE_CUSTOMER_MIN

/**
 * List of command types send by the WW130 to the AI board.
 * Both boards must use the same list, in the same sequence!
 */
typedef enum {
    AI_PROCESSOR_MSG_NONE,
    AI_PROCESSOR_MSG_TX_STRING,
    AI_PROCESSOR_MSG_TX_BASE64,
    AI_PROCESSOR_MSG_TX_BINARY,
    AI_PROCESSOR_MSG_RX_STRING,
    AI_PROCESSOR_MSG_RX_BASE64,
    AI_PROCESSOR_MSG_RX_BINARY,

	// This is used just to check on the end of the list
    AI_PROCESSOR_MSG_END
} aiProcessor_msg_type_t;


// Declare strings for each of these states. Values are in if_task.c
//extern const char* ifTaskStateString[APP_IF_STATE_ERROR + 1];

// Create the task and all its support pieces
TaskHandle_t ifTask_createTask(int8_t priority);

// Return the internal state (as a number)
uint16_t ifTask_getState(void);

// Return the internal state (as a string)
const char * ifTask_getStateString(void);


#endif /* APP_WW_PROJECTS_WW130_CLI_IF_H_ */
