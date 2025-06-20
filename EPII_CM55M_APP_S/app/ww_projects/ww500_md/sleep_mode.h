/*
 * sleep_mode.h
 *
 *  Created on: 2023/10/24
 *      Author: Himax
 */

#ifndef APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_
#define APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_

// Options for wake from DPD - each one bit
typedef enum {
	SLEEPMODE_WAKE_SOURCE_WAKE_PIN = 0x1,
	SLEEPMODE_WAKE_SOURCE_RTC = 0x2
} SLEEPMODE_WAKE_SOURCE_E;

void sleep_mode_print_event(uint32_t event, uint32_t event1);

void sleep_mode_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention);

void sleep_mode_enter_dpd(SLEEPMODE_WAKE_SOURCE_E wakeSource, uint16_t timelapsePeriod, bool verbose);

// Moved to exif_utc.c
//RTC_ERROR_E RTC_GetTimeDPD(rtc_time *tm);
//RTC_ERROR_E RTC_GetTime(rtc_time *tm);
//RTC_ERROR_E RTC_SetTime(rtc_time *tm);
//void app_clk_enable(void);
//void app_clk_disable(void);

#endif  /* APP_SCENARIO_APP_FREERTOS_SLEEP_MODE_H_ */
