/*
 * ww130_cmd.h
 *
 *  Created on: 30 Jul 2024
 *      Author: CGP
 *
 * Processes incoming commands received from I2C master (WW130)
 *
 */

#ifndef APP_SCENARIO_APP_WW130_TEST_WW130_CMD_H_
#define APP_SCENARIO_APP_WW130_TEST_WW130_CMD_H_


/*********************************** Includes ************************************************/

#include "evt_i2ccomm.h"

/*********************************** Exported Definitions **************************************/

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

/*********************************** Exported Function Declarations **************************/

// Called once by comm_task
void ww130_cmd_init(void);


#endif /* APP_SCENARIO_APP_WW130_TEST_WW130_CMD_H_ */
