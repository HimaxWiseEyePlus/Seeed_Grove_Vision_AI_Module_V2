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

/* Task priorities. */
#define if_task_PRIORITY	(configMAX_PRIORITIES - 2)

// The states for the if_task
typedef enum {
	APP_IF_STATE_UNINIT						=0x0000,
	APP_IF_STATE_IDLE						=0x0001,
	APP_IF_STATE_I2C						=0x0002,
	APP_IF_STATE_PA0						=0x0003,
	APP_IF_STATE_ERROR						,
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
extern const char* ifTaskStateString[APP_IF_STATE_ERROR + 1];

// Create the task and all its support pieces
void ifTask_createTask(void);

// Return the internal state (as a number)
uint16_t ifTask_getTaskState(void);

// Return the internal state (as a string)
const char * ifTask_getTaskStateString(void);


#endif /* APP_WW_PROJECTS_WW130_CLI_IF_H_ */
