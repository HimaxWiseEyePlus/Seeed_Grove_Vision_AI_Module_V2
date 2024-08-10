/*
 * main_task.h
 *
 *  Created on: 31 Jul 2024
 *      Author: CGP
 *
 * FreeRTOS main_task
 */

#ifndef APP_SCENARIO_APP_WW130_TEST_MAIN_TASK_H_
#define APP_SCENARIO_APP_WW130_TEST_MAIN_TASK_H_


typedef enum
{
	APP_STATE_ALLON,
    APP_STATE_RESTART,
    APP_STATE_STOP,
}APP_STATE_E;

void main_task(void *pvParameters);

// TODO should be in dp_task.c?
void app_start_state(APP_STATE_E state);

// fucntions called by the command processor in ww130_cmd.c
void main_startCapture(void);
void main_sendExif(void);

#endif /* APP_SCENARIO_APP_WW130_TEST_MAIN_TASK_H_ */
