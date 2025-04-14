/*
 * sleep_mode.h
 *
 *  Created on: 2023/10/24
 *      Author: Himax
 */

#ifndef APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_
#define APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_

void print_wakeup_event(uint32_t event, uint32_t event1);
void setCM55MTimerAlarmPMU(uint32_t timer_ms);
void app_pmu_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention);

void app_pmu_enter_dpd(bool verbose);

// Moved to exif_utc.c
//RTC_ERROR_E RTC_GetTimeDPD(rtc_time *tm);
//RTC_ERROR_E RTC_GetTime(rtc_time *tm);
//RTC_ERROR_E RTC_SetTime(rtc_time *tm);
//void app_clk_enable(void);
//void app_clk_disable(void);

#endif  /* APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_ */
