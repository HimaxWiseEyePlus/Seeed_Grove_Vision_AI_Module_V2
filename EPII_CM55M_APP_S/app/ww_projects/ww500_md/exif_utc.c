/*
 * exif_utc.c
 *
 * Supports processing of UTC time for inclusion in EXIF metadata
 *
 *  Created on: 21 Mar 2025
 *      Author: charl
 */


/********************************** Includes ******************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "exif_utc.h"
#include "hx_drv_rtc.h"
#include "xprintf.h"

#include "time.h"
#include "hx_drv_pmu_export.h"
#include "powermode.h"
#include "ff.h"  // FatFs types

#include "hx_drv_rtc.h"

/**************************************** Local routine declarations  *************************************/

static int is_leap_year(int year);
static int days_in_month(int mon, int year);

/**************************************** Local Variables **************************************/

static bool timeHasBeenSet = false;

/**************************************** Local function definitions  *************************************/

/**
 * Check if a year is a leap year
 * Helper function for exif_utc_add_seconds_to_tm()
 *
 * @param year rtc_time.tm_year is years since 1900
 * @return 1 if a leap year
 */
static int is_leap_year(int year) {
    year += 1900;  // rtc_time.tm_year is years since 1900
    return ((year % 4 == 0) && (year % 100 != 0 || year % 400 == 0));
}


/**
 * Get number of days in a given month/year
 * Helper function for exif_utc_add_seconds_to_tm()
 *
 * @param mon = month (0-11)
 * @param year = year
 * @return the number of days in the month
 */
static int days_in_month(int mon, int year) {
    static const int days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (mon == 1 && is_leap_year(year)) // February in a leap year
        return 29;
    return days[mon];
}

/**************************************** Global function definitions  *************************************/


/**
 * Initialises clocks and sets the time
 *
 * @param str = ISO srting e.g. 2025-01-01T00:00:00Z"
 * @return status
 */
RTC_ERROR_E exif_utc_init(const char *str) {
	RTC_ERROR_E ret;
	rtc_time tm;

	// Required to get the RTCworking
	exif_utc_clk_enable();

	ret = exif_utc_utc_string_to_time(str, &tm);

	if (ret != RTC_NO_ERROR) {
		return ret;
	}

	ret = exif_utc_set_rtc_from_time(&tm);

	//xprintf("DEBUG: set RTC to %s and get_fattime() as 0x%04x\n", str, get_fattime());

	return ret;
}

/**
 * TODO - implement this!
 *
 * In the event that neither the app nor the LoRaWAN network have provided the device with its time,
 * we should probably not try to add a timestamp in the EXIF. I shouldimplemnt this function that returns a boolean.
 */
bool exif_utc_has_time(void) {
	return timeHasBeenSet;
}

/**
 * Reads the RTC hardware to produce a rtc_time object
 *
 * Slower: uses RTC_TIME_AFTER_DPD_1ST_READ_YES -  It need to sync new counter in DPD period
 * It appears to take 961ms - though I am sure that previously it has taken a variable amount of time...
 *
 * @param - pointer to a rtc_time object
 * @return error code
 */
RTC_ERROR_E exif_utc_get_rtc_as_time_dpd(rtc_time * tm) {
	return hx_drv_rtc_read_time(RTC_ID_0, tm, RTC_TIME_AFTER_DPD_1ST_READ_YES);
}

/**
 * Reads the RTC hardware to produce a rtc_time object
 *
 * @param - pointer to a rtc_time object
 * @return error code
 */
RTC_ERROR_E exif_utc_get_rtc_as_time(rtc_time * tm) {
	return hx_drv_rtc_read_time(RTC_ID_0, tm, RTC_TIME_AFTER_DPD_1ST_READ_NO);
}

/**
 * Sets the RTC hardware from a rtc_time object
 *
 * This function appears to take 1-2s
 *
 * @param - pointer to a rtc_time object
 * @return error code
 */
RTC_ERROR_E exif_utc_set_rtc_from_time(rtc_time * tm) {
	return hx_drv_rtc_set_time(RTC_ID_0, tm);
}

/**
 * Converts an ISO UTC string to a rtc_time object
 *
 * The ISO 8601 format "YYYY-MM-DDTHH:MM:SSZ"
 * e.g. '2025-03-21T09:05:00Z'
 *
 * @param - pointer to a string containing the UTC time
 * @param - pointer to a rtc_time object
 * @return error code
 */
RTC_ERROR_E exif_utc_utc_string_to_time(const char *str, rtc_time * tm) {
	int charsRead;

	if ((sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2dZ%n",
			&tm->tm_year, &tm->tm_mon, &tm->tm_mday,
			&tm->tm_hour, &tm->tm_min, &tm->tm_sec, &charsRead) != 6)
	|| (charsRead != strlen(str))){
		return RTC_ERROR_INVALID_PARAMETERS;
	}
	else {
		return RTC_NO_ERROR;
	}
}

/**
 * Converts a rtc_time object to an ISO UTC string
 *
 * The ISO 8601 format is "YYYY-MM-DDTHH:MM:SSZ"
 * e.g. '2025-03-21T09:05:00Z'
 * Needs a 21 character buffer (including the trailing '\0')
 *
 * @param - pointer to a rtc_time object
 * @param - pointer to a string containing the UTC time
 * @param - length = length of string
 * @return error code
 */
RTC_ERROR_E exif_utc_time_to_utc_string(rtc_time * tm, char *str, uint8_t length) {

	if (length < UTCSTRINGLENGTH) {
		return RTC_ERROR_INVALID_PARAMETERS;
	}

	snprintf(str, length, "%04d-%02d-%02dT%02d:%02d:%02dZ",
			tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	return RTC_NO_ERROR;
}

/**
 * Converts a rtc_time object to an EXIF timestamp string
 *
 * The EXIF timestamp format is "YYYY:MM:DD HH:MM:SS",
 * e.g. '2025:03:21 09:05:00'
 * Needs a 20 character buffer (including the trailing '\0')
 *
 * @param - pointer to a rtc_time object
 * @param - pointer to a string containing the UTC time
 * @param - length = length of string
 * @return error code
 */
RTC_ERROR_E exif_utc_time_to_exif_string(rtc_time * tm, char *str, uint8_t length) {

	if (length < EXIFSTRINGLENGTH) {
		return RTC_ERROR_INVALID_PARAMETERS;
	}

	snprintf(str, length, "%04d:%02d:%02d %02d:%02d:%02d",
			tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	return RTC_NO_ERROR;
}

// reads the RTC hardware to produce an EXIF timestamp string
RTC_ERROR_E exif_utc_get_rtc_as_exif_string(char *str, uint8_t length) {
	rtc_time tm;
	RTC_ERROR_E ret;

	if (length < EXIFSTRINGLENGTH) {
		return RTC_ERROR_INVALID_PARAMETERS;
	}

	ret = hx_drv_rtc_read_time(RTC_ID_0, &tm, RTC_TIME_AFTER_DPD_1ST_READ_NO);

	if (ret == RTC_NO_ERROR) {
		snprintf(str, length, "%04d:%02d:%02d %02d:%02d:%02d",
			tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}

	return ret;
}


/**
 * This is to be called when exiting DPD
 *
 * It is moved from sleep_mode.c where it was called app_clk_enable()
 */
void exif_utc_clk_enable(void) {
	SCU_PDAON_CLKEN_CFG_T aonclken;

	aonclken.rtc0_clk_en = 1;/*!< RTC0 Clock enable */
	aonclken.rtc1_clk_en = 1;/*!< RTC1 Clock enable */
	aonclken.rtc2_clk_en = 1;/*!< RTC2 Clock enable */
	aonclken.pmu_clk_en = 1;/*!< PMU Clock enable */
	aonclken.aon_gpio_clk_en = 1;/*!< AON GPIO Clock enable */
	aonclken.aon_swreg_clk_en = 1;/*!< AON SW REG Clock enable */
	aonclken.antitamper_clk_en = 1;/*!< ANTI TAMPER Clock enable */
	hx_drv_scu_set_pdaon_clken_cfg(aonclken);
}

/**
 * This is to be called when entering DPD
 *
 * It is moved from sleep_mode.c where it was called app_clk_disable()
 */
void exif_utc_clk_disable(void) {
	SCU_PDAON_CLKEN_CFG_T aonclken;

	aonclken.rtc0_clk_en = 1;/*!< RTC0 Clock enable */
	aonclken.rtc1_clk_en = 0;/*!< RTC1 Clock enable */
	aonclken.rtc2_clk_en = 0;/*!< RTC2 Clock enable */
	aonclken.pmu_clk_en = 1;/*!< PMU Clock enable */
	aonclken.aon_gpio_clk_en = 0;/*!< AON GPIO Clock enable */
	aonclken.aon_swreg_clk_en = 1;/*!< AON SW REG Clock enable */
	aonclken.antitamper_clk_en = 0;/*!< ANTI TAMPER Clock enable */
	hx_drv_scu_set_pdaon_clken_cfg(aonclken);
}

/**
 * Function used by fatfs to add timestamps.
 *
 * bit31:25 - Year from 1980 (0..127)
 * bit24:21 - Month (1..12)
 * bit20:16 - Day (1..31)
 * bit15:11 - Hour (0..23)
 * bit10:5  - Minute (0..59)
 * bit4:0   - Second / 2 (0..29, means 0 to 58 seconds)
 */
DWORD get_fattime(void) {
	RTC_ERROR_E ret;
    rtc_time tm;

    ret = exif_utc_get_rtc_as_time(&tm);

    if (ret == 0) {
    return  ((DWORD)(tm.tm_year - 1980) << 25)  // Years since 1980
          | ((DWORD)(tm.tm_mon) << 21)    		// Month
          | ((DWORD)tm.tm_mday << 16)         	// Day
          | ((DWORD)tm.tm_hour << 11)         	// Hour
          | ((DWORD)tm.tm_min << 5)           	// Minute
          | ((DWORD)(tm.tm_sec / 2));         	// Seconds / 2
    }
    else {
    	return ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16);
    }
}

/**
 * Tests the get RTC functions
 *
 * Reports the delay in reading the registers
 */
void exif_utc_test_get_rtc(void) {
	rtc_time time;
	RTC_ERROR_E ret;

	TickType_t startTime;
	TickType_t elapsedTime;
	uint32_t elapsedMs;

	// Read it 3 times
	// (1)with RTC_TIME_AFTER_DPD_1ST_READ_YES
    startTime = xTaskGetTickCount();

    ret = exif_utc_get_rtc_as_time_dpd(&time);

    if (ret != 0) {
    	xprintf("Error %d\n", ret);
    	return;
    }

    elapsedTime = xTaskGetTickCount() - startTime;
    elapsedMs = (elapsedTime * 1000) / configTICK_RATE_HZ;

	// "YYYY-MM-DDTHH:MM:SSZ"
	xprintf("%04d-%02d-%02dT%02d:%02d:%02dZ\n",
			time.tm_year, time.tm_mon, time.tm_mday,
			time.tm_hour, time.tm_min, time.tm_sec);
	xprintf("exif_utc_get_rtc_as_time_dpd() took %d ticks = %dms\n", elapsedTime,  elapsedMs);

	// (2) with RTC_TIME_AFTER_DPD_1ST_READ_NO
    startTime = xTaskGetTickCount();

    ret = exif_utc_get_rtc_as_time(&time);

    if (ret != 0) {
    	xprintf("Error %d\n", ret);
    	return;
    }

    elapsedTime = xTaskGetTickCount() - startTime;
    elapsedMs = (elapsedTime * 1000) / configTICK_RATE_HZ;

	// "YYYY-MM-DDTHH:MM:SSZ"
	xprintf("%04d-%02d-%02dT%02d:%02d:%02dZ\n",
			time.tm_year, time.tm_mon, time.tm_mday,
			time.tm_hour, time.tm_min, time.tm_sec);
	xprintf("exif_utc_get_rtc_as_time() took %d ticks = %dms\n", elapsedTime,  elapsedMs);

	// (3) with hx_drv_rtc_cm55m_read_time
    startTime = xTaskGetTickCount();

    ret = hx_drv_rtc_cm55m_read_time(&time, RTC_TIME_AFTER_DPD_1ST_READ_NO);

    if (ret!= 0) {
    	xprintf("Error %d\n", ret);
    	return;
    }

    elapsedTime = xTaskGetTickCount() - startTime;
    elapsedMs = (elapsedTime * 1000) / configTICK_RATE_HZ;

	// "YYYY-MM-DDTHH:MM:SSZ"
	xprintf("%04d-%02d-%02dT%02d:%02d:%02dZ\n",
			time.tm_year, time.tm_mon, time.tm_mday,
			time.tm_hour, time.tm_min, time.tm_sec);
	xprintf("hx_drv_rtc_cm55m_read_time() took %d ticks = %dms\n", elapsedTime,  elapsedMs);
}

/**
 * Tests the string conversion functions
 *
 */
void exif_utc_test_set_rtc(char *str) {
	rtc_time time;
	RTC_ERROR_E ret;
	char timeString[UTCSTRINGLENGTH];

	xprintf("Trying to set RTC from '%s'\n", str);

	// Convert the string to a time
	ret = exif_utc_utc_string_to_time(str, &time);

    if (ret != RTC_NO_ERROR) {
    	xprintf("Error %d\n", ret);
    	return;
    }

	// Set the RTC from that time
	ret = exif_utc_set_rtc_from_time(&time);

    if (ret == RTC_NO_ERROR) {
    	xprintf("Success (RTC is set)\n");
    }
    else {
    	xprintf("Error %d\n", ret);
    	return;
    }

	// Read it back
    ret = exif_utc_get_rtc_as_time(&time);

    if (ret != RTC_NO_ERROR) {
    	xprintf("Error %d\n", ret);
    }

    // Make a change
    time.tm_year++;

    // print this as a UTC string
    ret = exif_utc_time_to_utc_string(&time, timeString, sizeof(timeString));
    if (ret == RTC_NO_ERROR) {
    	xprintf("Next year will be '%s' (UTC format)", timeString);
    }
    else {
    	xprintf("Error %d\n", ret);
    	return;
    }

    // print this as a EXIF string
    ret = exif_utc_time_to_exif_string(&time, timeString, sizeof(timeString));
    if (ret == RTC_NO_ERROR) {
    	xprintf(" or '%s' (EXIF format)\n", timeString);
    }
    else {
    	xprintf("Error %d\n", ret);
    	return;
    }

    // Get the time directly from the RTC and print it
    ret = exif_utc_get_rtc_as_exif_string(timeString, sizeof(timeString));
    if (ret == RTC_NO_ERROR) {

    	xprintf("Now the RTC is '%s' (EXIF format)\n", timeString);
    }
    else {
    	xprintf("Error %d\n", ret);
    }
}

/*
// Adds timelapsePeriod seconds to a struct tm.
// Returns the result as a new struct tm (in local time).
rtc_time exif_utc_add_seconds_to_tm(rtc_time input_rtc, time_t seconds_to_add) {
	    struct tm temp_tm;
	    rtc_time result;

	   // Convert rtc_time to struct tm
	    temp_tm.tm_sec  = input_rtc.tm_sec;
	    temp_tm.tm_min  = input_rtc.tm_min;
	    temp_tm.tm_hour = input_rtc.tm_hour;
	    temp_tm.tm_mday = input_rtc.tm_mday;
	    temp_tm.tm_mon  = input_rtc.tm_mon;
	    temp_tm.tm_year = input_rtc.tm_year;
	    temp_tm.tm_wday = input_rtc.tm_wday;  // Optional; mktime will recompute
	    temp_tm.tm_yday = input_rtc.tm_yday;  // Optional
	    temp_tm.tm_isdst = -1; // Let system determine daylight saving

	    // Normalize and add seconds
	    time_t base = mktime(&temp_tm);
	    time_t new_time = base + seconds_to_add;

	    struct tm *new_tm = gmtime(&new_time);

	    xprintf("DEBUG: base is %d new_time is %d - added %d\n",
	    		(int) base, (int) new_tm, (int) seconds_to_add);
	    // Convert back to rtc_time
	    result.tm_sec  = new_tm->tm_sec;
	    result.tm_min  = new_tm->tm_min;
	    result.tm_hour = new_tm->tm_hour;
	    result.tm_mday = new_tm->tm_mday;
	    result.tm_mon  = new_tm->tm_mon;
	    result.tm_year = new_tm->tm_year;
	    result.tm_wday = new_tm->tm_wday;
	    result.tm_yday = new_tm->tm_yday;

	    return result;
}
*/

/**
 * Adjusts a time by adding a number of seconds.
 * Can be used to create an rtc_time object in the future - e.g. for setting an alarm.
 *
 * @param input_rtc = a time/date
 * @param seconds_to_add = the number of seconds to add
 * @return a time/date object which is later
 */
rtc_time exif_utc_add_seconds_to_tm(rtc_time input_rtc, time_t seconds_to_add) {
    rtc_time t = input_rtc;

    // Add seconds
    t.tm_sec += seconds_to_add;

    // Normalize seconds to minutes
    if (t.tm_sec >= 60) {
        t.tm_min += t.tm_sec / 60;
        t.tm_sec = t.tm_sec % 60;
    }

    // Normalize minutes to hours
    if (t.tm_min >= 60) {
        t.tm_hour += t.tm_min / 60;
        t.tm_min = t.tm_min % 60;
    }

    // Normalize hours to days
    if (t.tm_hour >= 24) {
        t.tm_mday += t.tm_hour / 24;
        t.tm_hour = t.tm_hour % 24;
    }

    // Normalize days to months/years
    while (1) {
        int dim = days_in_month(t.tm_mon, t.tm_year);

        if (t.tm_mday <= dim) {
        	break;
        }

        t.tm_mday -= dim;
        t.tm_mon += 1;

        if (t.tm_mon >= 12) {
            t.tm_mon = 0;
            t.tm_year += 1;
        }
    }

    // Optional: zero or recompute tm_yday and tm_wday if required
    // For now, we leave them unchanged
    return t;
}

