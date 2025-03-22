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

#include "exif_utc.h"
#include "hx_drv_rtc.h"
#include "xprintf.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
//#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/*************************************** Local routine declarations  *************************************/


/**************************************** Local Variables **************************************/


/*************************************** Local function definitions  *************************************/

/*************************************** Global function definitions  *************************************/


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

