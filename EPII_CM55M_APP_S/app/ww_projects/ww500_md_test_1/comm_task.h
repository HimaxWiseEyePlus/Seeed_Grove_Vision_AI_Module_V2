/*
 * comm_task.h
 *
 *  Created on: 2022¦~2¤ë22¤é
 *      Author: 902447
 */

#ifndef APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_COMM_TASK_H_
#define APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_COMM_TASK_H_



void aon_gpio0_interupt_reg();
void aon_gpio1_interupt_reg();

void comm_task(void *pvParameters);


#endif /* APP_SCENARIO_APP_FREERTOS_TZ_S_DP_SAMPLE_COMM_TASK_H_ */
