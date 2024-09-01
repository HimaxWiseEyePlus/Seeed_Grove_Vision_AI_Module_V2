/*
 * sleep_mode.h
 *
 *  Created on: 2023/10/24
 *      Author: Himax
 */

#ifndef APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_
#define APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_

static void print_wakeup_event(uint32_t event, uint32_t event1);
void setCM55MTimerAlarmPMU(uint32_t timer_ms);
void app_pmu_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention);

#endif  /* APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_ */