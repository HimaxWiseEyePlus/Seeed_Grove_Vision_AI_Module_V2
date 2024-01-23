/*
 * hx_drv_rtc.h
 *
 *  Created on: 2021嚙羯5嚙踝蕭2嚙踝蕭
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_RTC_H_
#define DRIVERS_INC_HX_DRV_RTC_H_
#include <stdio.h>
#include "WE2_device.h"

/**
 * \defgroup RTC
 * \brief  Real time clock
 * \details WE2 has three RTC clocks for use 
 * <pre>
 *      
 *  ��	hx_drv_rtc_cm55m_xxx uses RTC_ID_0
 *  ��	hx_drv_rtc_cm55s_xxx uses RTC_ID_1
 *  currently no dedicated driver API uses RTC_ID_2
 * 
 * </pre>
 * @}
 */
 
/**
 * \enum RTC_ERROR_E
 * \brief RTC Errors Type
 */
typedef enum {
	RTC_NO_ERROR = 0, /**< No ERROR */
	RTC_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	RTC_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} RTC_ERROR_E;

/**
 * \enum RTC_AIE_E
 * \brief Alarm Int enable on/off
 */
typedef enum {
	RTC_AIE_ON = 0, /**< Alarm Int. Enable On */
	RTC_AIE_OFF, /**< Alarm Int. enable OFF */
} RTC_AIE_E;

/**
 * \enum RTC_ID_E
 * \brief ID of RTC
 */
typedef enum {
	RTC_ID_0 = 0, /**< Timer ID 0 */
	RTC_ID_1, /**< Timer ID 1 */
	RTC_ID_2, /**< Timer ID 2 */
	RTC_ID_MAX, /**< Timer MAX */
} RTC_ID_E;

/**
 * \enum RTC_TIME_AFTER_DPD_1ST_READ_E
 * \brief First Read RTC time after DPD wakeup. If Yes, it will wait Time sync then read time
 */
typedef enum {
	RTC_TIME_AFTER_DPD_1ST_READ_NO = 0, /**< Not First Read RTC time after DPD */
	RTC_TIME_AFTER_DPD_1ST_READ_YES, /**<  First Read RTC time after DPD. It need to sync new counter in DPD period */
} RTC_TIME_AFTER_DPD_1ST_READ_E;

/*
 * time structure, base unit is 1 second. rtc time start count from Jan/01/1970 00:00:00 with 2^32 unsigned long counter, each count represent 1 second. it will be able to count 136 years of time value.
 */
typedef struct {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
//	int tm_isdst;
}rtc_time;

/*
 * alarm API.
 */
typedef struct {
	unsigned char enabled;	/* 0 = alarm disabled, alarm callback will not trigger. 1 = alarm enabled, alarm callback will trigger when RTC time value equals to alarm */
	unsigned char pending;  /* 0 = RTC time is smaller than alarm time. 1 = RTC time equals or larger than alarm time */
	rtc_time time;	/* time the alarm is set to */
}rtc_wkalrm;

/**
 * \brief	Alarm interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*RTC_ISREvent_t) (uint32_t event);

/**
 * \brief RTC init, base address is rtc hardware register which is located in the WE2_device_addr.h
 *				find HX_RTC0_BASE/HX_RTC1_BASE/HX_RTC2_BASE related to RTC0/RTC1/RTC2
 *
 * \param[in]	id	RTC ID
 * \param[in]	rtc_baseaddr	RTC HW base address, fill "HX_RTC0_BASE" if target is RTC0
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_init(RTC_ID_E id, uint32_t rtc_baseaddr);

/**
 * \brief RTC get clock, it return rrc reference clock
 *
 * \param[in]	id	RTC ID
 * \param[out]	freq	RTC Frequency in Hz.
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_clk(RTC_ID_E id, uint32_t *freq);

/**
 * \brief	Enable/Disable RTC
 *
 * \param[in]	id	RTC ID
 * \param[in]	enable	RTC enable state, 0: disable. 1:enable
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_enable(RTC_ID_E id, uint32_t enable);

/**
 * \brief	read RTC Time and return time data with rtc_time structure.
 *
 * \param[in]	id	RTC ID
 * \param[out]	tm	read RTC time in rtc_time structure
 * \param[in]	read_sync	read sync DPD cycle count or not. when RTC_TIME_AFTER_DPD_1ST_READ_YES, rtc driver will check this read back time value synchronized to real RTC information.
 *            when DPD, there is a RTC counter inside which still works but may not update to rtc data register synchronically.
 *            it will be better when back from Deep power down state, first time to read real time value to set RTC_TIME_AFTER_DPD_1ST_READ_E = RTC_TIME_AFTER_DPD_1ST_READ_YES.
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_read_time(RTC_ID_E id, rtc_time *tm, RTC_TIME_AFTER_DPD_1ST_READ_E read_sync);

/**
 * \brief	set RTC Time
 *
 * \param[in]	id	RTC ID
 * \param[in]	tm	set RTC time with rtc_time time structure
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_time(RTC_ID_E id, rtc_time *tm);

/**
 * \brief	read alarm Time
 *
 * \param[in]	id	RTC ID
 * \param[out]	alarm	read Alarm RTC time and enable status
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_read_alarm(RTC_ID_E id, rtc_wkalrm *alarm);

/**
 * \brief	set alarm Time
 *
 * \param[in]	id	RTC ID
 * \param[in]	alarm	set Alarm RTC time
 * \param[in]	cb Alarm RTC Callback
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_alarm(RTC_ID_E id, rtc_wkalrm *alarm, RTC_ISREvent_t cb);
/**
 * \brief	set alarm interrupt enable/disable status
 *
 * \param[in]	id	RTC ID
 * \param[in]	inten	interrupt enable/disable
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_alarm_int(RTC_ID_E id, RTC_AIE_E inten);
/**
 * \brief	get alarm interrupt enable status
 *
 * \param[in]	id	RTC ID
 * \param[out]	inten	interrupt enable status
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_int(RTC_ID_E id, RTC_AIE_E *inten);
/**
 * \brief	clear alarm interrupt status on dedicated rtc
 *
 * \param[in]	id	RTC ID
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_clear_alarm_int_status(RTC_ID_E id);
/**
 * \brief	get alarm raw interrupt status
 *
 * \param[in]	id	RTC ID
 * \param[out]	status	interrupt status
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_rawint_status(RTC_ID_E id, uint32_t *status);

/**
 * \brief	get alarm mask interrupt status
 *
 * \param[in]	id	RTC ID
 * \param[out]	status	interrupt status
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_maskint_status(RTC_ID_E id, uint32_t *status);

/**
 * \brief	read rtc val, similar to "hx_drv_rtc_read_time" but return with "val" represent in counter value. The value baase unit is 1 second.
 *
 * \param[in]	id	RTC ID
 * \param[out]	val	read rtc value
 * \param[in]	read_sync	read sync DPD cycle count or not, please reference "hx_drv_rtc_read_time" for more detail
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_read_val(RTC_ID_E id, uint32_t *val, RTC_TIME_AFTER_DPD_1ST_READ_E read_sync);
/**
 * \brief	set rtc Time by cycle, this API co-work with "hx_drv_rtc_set_time_cycle".
 *				"hx_drv_rtc_set_time_cycle"	actually will use "cycle" value as a time value set to RTC, RTC time will start count from this value
 *        and "hx_drv_rtc_set_time_cycle" is used to trigger the period you need.
 *				for ex. we want to trigger a 5 second timout with callback
 *				//we start RTC_2 with time value = 1000, it will start to count from 1000 with 1Hz incremental 1  
 *				hx_drv_rtc_set_time_cycle(RTC_ID_2, 1000);
 *				//set alarm time value to 1005, which is 5 second later, alarm_en=1 and cb = cb_ptr, which the cb_ptr will be called after 5 seconds
 *				hx_drv_rtc_set_alarm_cycle(RTC_ID_2, 1, 1005, cb_ptr);
 *
 * \param[in]	id	RTC ID
 * \param[in]	cycle	rtc cycle (1hz)
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_time_cycle(RTC_ID_E id, uint32_t cycle);
/**
 * \brief	get rtc Time by cycle, get back the time value we have set, this is the original time value we set before
 *				for ex. we set a time cycle and read the time cycle value back after 2 second
 *				//we start RTC_2 with time value = 1000, it will start to count from 1000 with 1Hz incremental 1  
 *				uint32_t cycle_readback = 0;
 *				hx_drv_rtc_set_time_cycle(RTC_ID_2, 1000);
 *				//...2 seconds after
 *				hx_drv_rtc_get_time_cycle(RTC_ID_2, &cycle_readback);
 *				//the cycle_readback value will be 1000
 *
 * \param[in]	id	RTC ID
 * \param[out]	cycle	rtc cycle
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_time_cycle(RTC_ID_E id, uint32_t *cycle);

/**
 * \brief	set rtc alarm Time by cycle, this API co-work with "hx_drv_rtc_set_time_cycle". check there for more deatil.
 *
 * \param[in]	id	RTC ID
 * \param[in]	alarm_en	set alarm enable/disable status. 0:disable, will not trigger callback when alarm time match. 1:enable, callback will be trigger when RTC match alarm value
 * \param[in]	cycle	rtc alarm cycle time
 * \param[in]	cb	callback function when alarm match with RTC
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_alarm_cycle(RTC_ID_E id, uint8_t alarm_en, uint32_t cycle, RTC_ISREvent_t cb);

/**
 * \brief	get rtc alarm Time by cycle, get the alarm value been set before.
 *
 * \param[in]	id	RTC ID
 * \param[out]	cycle	rtc alarm cycle time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_cycle(RTC_ID_E id, uint32_t *cycle);



/**
 * \brief	read CM55M RTC Time, uses RTC_ID_0  
 *
 * \param[out]	tm read RTC time back with rtc_time structure
 * \param[in]	read_sync	read sync DPD cycle count or not, check "hx_drv_rtc_read_time" for more detail.
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_read_time(rtc_time *tm, RTC_TIME_AFTER_DPD_1ST_READ_E read_sync);

/**
 * \brief	set CM55M RTC Time, uses RTC_ID_0
 *
 * \param[in]	tm read RTC time in rtc_time structure
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_set_time(rtc_time *tm);

/**
 * \brief	read CM55M alarm Time, uses RTC_ID_0
 *
 * \param[out]	alarm read Alarm RTC time in rtc_wkalrm structure
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_read_alarm(rtc_wkalrm *alarm);

/**
 * \brief	set CM55M alarm Time, uses RTC_ID_0
 *
 * \param[in]	alarm set Alarm RTC time in rtc_wkalrm structure
 * \param[in]	cb Alarm RTC Callback
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_set_alarm(rtc_wkalrm *alarm, RTC_ISREvent_t cb);

/**
 * \brief	read CM55S RTC Time, uses RTC_ID_1
 *
 * \param[out]	rtc_time	read RTC time
 * \param[in]	read_sync	read sync DPD cycle count or not, check "hx_drv_rtc_read_time" for more detail.
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_read_time(rtc_time *tm, RTC_TIME_AFTER_DPD_1ST_READ_E read_sync);

/**
 * \brief	set CM55S RTC Time, uses RTC_ID_1
 *
 * \param[in]	tm read RTC time in rtc_time structure
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_set_time(rtc_time *tm);

/**
 * \brief	read CM55S alarm Time, uses RTC_ID_1
 *
 * \param[out]	alarm read Alarm RTC time in rtc_wkalrm structure
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_read_alarm(rtc_wkalrm *alarm);

/**
 * \brief	set CM55S alarm Time, uses RTC_ID_1
 *
 * \param[in]	alarm set Alarm RTC time in rtc_wkalrm structure
 * \param[in]	cb Alarm RTC Callback
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_set_alarm(rtc_wkalrm *alarm, RTC_ISREvent_t cb);


/**
 * \brief	set rtc Time by cycle with Clock Configuration
 *				"hx_drv_rtc_set_time_cycle_with_clkcfg"	actually will use "cycle" value as a time value set to RTC, RTC time will start count from this value
 *        and "hx_drv_rtc_set_time_cycle" is used to trigger the period you need.
 *				for ex. we want to trigger a 5 clkhz timout with callback
 *				//we start RTC_2 with time value = 1000, it will start to count from 1000 with 1Hz incremental 1
 *				hx_drv_rtc_set_time_cycle_with_clkcfg(RTC_ID_2, 1000, 1);
 *				//set alarm time value to 1005, which is 5 Clkhz later, alarm_en=1 and cb = cb_ptr, which the cb_ptr will be called after 5 clkHz
 *				hx_drv_rtc_set_alarm_cycle(RTC_ID_2, 1, 1005, cb_ptr);
 *
 * \param[in]	id	RTC ID
 * \param[in]	cycle	rtc cycle
 * \param[in]	clk_hz	rtc clock Frequency(Max: 32Khz. Clock Source: 32Khz)
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_time_cycle_with_clkcfg(RTC_ID_E id, uint32_t cycle, uint32_t clk_hz);

#endif /* DRIVERS_INC_HX_DRV_RTC_H_ */
