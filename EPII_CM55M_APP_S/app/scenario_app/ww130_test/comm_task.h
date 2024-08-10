/*
 * comm_task.h
 *
 *  Created on: 2022�~2��22��
 *      Author: 902447
 */

#ifndef APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_COMM_TASK_H_
#define APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_COMM_TASK_H_

#include "ww130_cmd.h"

// Although Himax allows 256, this gives a total lenght of 256 + 32
// The WW130 can't handle more than 255
// But choose a number which is a multiple of 32
#define WW130_MAX_PAYLOAD_SIZE			128

void comm_task(void *pvParameters);

void comm_task_customer_register_cb(void *cb_func);

void comm_task_customer_response(uint8_t * message, aiProcessor_msg_type_t messageType, uint16_t length);

#endif /* APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_COMM_TASK_H_ */
