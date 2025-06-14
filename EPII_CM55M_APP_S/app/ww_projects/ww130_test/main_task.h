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
}CAMERA_CONFIG_E;

void main_task(void *pvParameters);

// TODO should be in dp_task.c?
void app_start_state(CAMERA_CONFIG_E state);

// fucntions called by the command processor in ww130_cmd.c
void main_startCapture(void);
void main_sendExif(void);
void main_sendInt(uint16_t duration);

#endif /* APP_SCENARIO_APP_WW130_TEST_MAIN_TASK_H_ */
