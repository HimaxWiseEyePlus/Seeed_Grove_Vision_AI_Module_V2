/*
 * dp_task.h
 *
 *  Created on: 2022�~2��22��
 *      Author: 902447
 */

#ifndef APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_DP_TASK_H_
#define APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_DP_TASK_H_

#include "sensor_dp_lib.h"

void app_setup_dplib();
void app_dump_dplib_edminfo();
void app_stop_dplib();
void app_dump_jpeginfo();

void dp_task(void *pvParameters);
void dp_var_int(void);
void os_app_dplib_cb(SENSORDPLIB_STATUS_E event);

#endif /* APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_DP_TASK_H_ */
