/*
 * hx_drv_timer.h
 *
 *  Created on: 2021/4/7
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_TIMER_H_
#define DRIVERS_INC_HX_DRV_TIMER_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	TIMER_DRV	TIMER Driver
 * \ingroup		TIMER_DRV
 * \brief	TIMER Driver Declaration
 * \details Application can use TIMER Structure to control Hardware\n
 * <pre>
 * For Example
 * Ex1: TIMER_CFG_T: All TIMER configuration for application to control TIMER
 * 
 * hx_drv_timer_sensor_start
 * hx_drv_timer_sensor_stop()
 * hx_drv_timer_sensor_clkdiv
 * - uses TIMER_ID_0
 * 
 * hx_drv_timer_cm55m_ns_delay_ms
 * hx_drv_timer_cm55m_ns_delay_us
 * - uses TIMER_ID_1
 * 
 * hx_drv_timer_cm55m_start
 * hx_drv_timer_cm55m_stop
 * hx_drv_timer_cm55m_clkdiv
 * - uses TIMER_ID_2
 * 
 * hx_drv_timer_cm55m_sec_delay_ms
 * hx_drv_timer_cm55m_sec_delay_us
 * hx_drv_timer_cm55m_sec_delay_clkdiv
 * - uses TIMER_ID_3
 * 
 * hx_drv_timer_cm55m_ns_start
 * hx_drv_timer_cm55m_ns_stop
 * hx_drv_timer_cm55m_ns_delay_clkdiv
 * - uses TIMER_ID_6
 * 
 * hx_drv_timer_cm55s_start
 * hx_drv_timer_cm55s_stop
 * hx_drv_timer_cm55s_clkdiv
 * - uses TIMER_ID_4
 * 
 * hx_drv_timer_cm55s_ns_start
 * hx_drv_timer_cm55s_ns_stop
 * hx_drv_timer_cm55s_ns_clkdiv
 * - uses TIMER_ID_7
 * 
 * hx_drv_timer_cm55s_delay_ms
 * hx_drv_timer_cm55s_delay_us
 * hx_drv_timer_cm55s_delay_clkdiv
 * - uses TIMER_ID_5
 * 
 * hx_drv_timer_cm55s_ns_delay_ms
 * hx_drv_timer_cm55s_ns_delay_us
 * hx_drv_timer_cm55s_ns_delay_clkdiv
 * - uses TIMER_ID_8
 * 
 * </pre>
 */
/**
 * \defgroup	TIMER_DRV_ENUM	TIMER Driver Enumeration
 * \ingroup	TIMER_DRV
 * \brief	Defines some enumeration of TIMER Driver need.
 * @{
 */
/**
 * \enum TIMER_ERROR_E
 * \brief TIMER Errors Type
 */
typedef enum {
	TIMER_NO_ERROR = 0, /**< No ERROR */
	TIMER_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	TIMER_ERROR_ALREADY_USED, /**< ERROR MSG: already used */
	TIMER_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} TIMER_ERROR_E;

/**
 * \enum TIMER_ID_E
 * \brief ID of TIMER
 */
typedef enum {
	TIMER_ID_0 = 0, /**< Timer ID 0 */
	TIMER_ID_1, /**< Timer ID 1 */
	TIMER_ID_2, /**< Timer ID 2 */
	TIMER_ID_3, /**< Timer ID 3 */
	TIMER_ID_4, /**< Timer ID 4 */
	TIMER_ID_5, /**< Timer ID 5 */
	TIMER_ID_6, /**< Timer ID 6 */
	TIMER_ID_7, /**< Timer ID 7 */
	TIMER_ID_8, /**< Timer ID 8 */
	TIMER_ID_MAX, /**< Timer MAX */
} TIMER_ID_E;

/**
 * \enum TIMER_MODE_E
 * \brief TIMER MODE Type
 */
typedef enum {
	TIMER_MODE_ONESHOT = 0, /**< ONE Shot mode */
	TIMER_MODE_PERIODICAL, /**< Periodical mode */
	TIMER_MODE_DELAY /**< Delay mode */
} TIMER_MODE_E;

/**
 * \enum TIMER_CTRL_E
 * \brief TIMER Control, this enumeration represent TIMER start by whom. When "TIMER_CTRL_CPU", timer start by cpu which will start immediately. 
 *	When "TIMER_CTRL_PMU", timer will start when PMU case
 */
typedef enum {
	TIMER_CTRL_CPU = 0, /**< Timer enable by CPU */
	TIMER_CTRL_PMU, /**< Timer enable by PMU */
} TIMER_CTRL_E;

/**
 * \enum TIMER_STATE_E
 * \brief TIMER STATE, which represent the reference clock timer will use, when timer used in ALLON case, select "TIMER_STATE_DC"
 *				"TIMER_STATE_PMU" means this timer will be used when PMU case, "TIMER_STATE_PMU" is usually co-work with TIMER_CTRL_E=TIMER_CTRL_PMU
 */
typedef enum {
	TIMER_STATE_DC = 0, /**< In DC State */
	TIMER_STATE_PMU, /**< In PMU State */
} TIMER_STATE_E;
/** @} */

/**
 * \defgroup	TIMER_DRV_MACRO	TIMER Driver MACRO
 * \ingroup	TIMER_DRV
 * \brief	contains definitions of TIMER Driver MACRO.
 * @{
 */
/**
 * \struct TIMER_CFG_T
 * \brief Timer configuration
 */
typedef struct {
	uint32_t period;  //milli-second value
	TIMER_MODE_E mode;
	TIMER_CTRL_E ctrl;
	TIMER_STATE_E state;
} TIMER_CFG_T;
/** @} */

/**
 * \defgroup	TIMER_DRV_FUNCDLR	MailBox Driver Function Declaration
 * \ingroup	TIMER_DRV
 * \brief	Contains declarations of MailBox Driver functions.
 * @{
 */

/**
 * \brief	Timer interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*Timer_ISREvent_t) (uint32_t event);
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief	initial TIMER Driver and clock
 *
 * \param[in]	id	TIMER id
 * \param[in]	timer_baseaddr	TIMER base address, base address is rtc hardware register which is located in the WE2_device_addr.h
 *						use HX_TIMER0_BASE~HX_TIMER8_BASE related to timer0~8
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_init(TIMER_ID_E id, uint32_t timer_baseaddr);

/**
 * \brief	start HW TIMER
 *
 * \param[in]	id	TIMER id
 * \param[in]	cfg	TIMER config
 * \param[in]	cb_event	callback function pointer when config time trigger
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_hw_start(TIMER_ID_E id, TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);

/**
 * \brief	stop HW TIMER
 *
 * \param[in]	id	TIMER ID
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_hw_stop(TIMER_ID_E id);


/**
 * \brief	delay millisecond, use state=TIMER_STATE_DC when ALLON reference clock
 *
 * \param[in]	id  Timer ID
 * \param[in]	ms  millisecond
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_delay_ms(TIMER_ID_E id,uint32_t ms, TIMER_STATE_E state);

/**
 * \brief	delay microsecond, use state=TIMER_STATE_DC when ALLON reference clock
 *
 * \param[in]	id  Timer ID
 * \param[in]	us  micro second
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_delay_us(TIMER_ID_E id, uint32_t us, TIMER_STATE_E state);

/**
 * \brief	de-initial TIMER Driver
 *
 * \param[in]	id  Timer ID
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_deinit(TIMER_ID_E id);

/**
 * \brief	Get Timer used or not, when used_timer = 0, which means this id timer is not yet used. When used_time=1, this id timer is used.
 *				"hx_drv_timer_hw_stop" and "hx_drv_timer_deinit" sets timer back to used_time=0
 *				"hx_drv_timer_delay_ms" and "hx_drv_timer_delay_us" use case will set used_time=0 when function end 
 *				if timer been set with TIMER_MODE_E=TIMER_MODE_DELAY or TIMER_MODE_E=TIMER_MODE_ONESHOT in TIMER_CFG_T, it will set used_time=0 when function finish trigger callback function.
 *
 * \param[in]	id timer ID
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_get_used(TIMER_ID_E id, uint8_t *used_timer);

/**
 * \brief	Get Timer not used, it start check from timer0 ti timer8 to find used_time=0 timer 
 *
 * \param[out]	id timer ID
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_get_available(TIMER_ID_E *id);

/**
 * \brief	Set Clock Divider, use state=TIMER_STATE_DC when ALLON reference clock. This will divide the input clock from reference clock.
 *
 * \param[in]	id timer id
 * \param[in]	clk_div clock div
 * \param[in]	state timer use in which state
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_set_clk_div(TIMER_ID_E id, uint32_t clk_div, TIMER_STATE_E state);

/**
 * \brief	Get Clock Divider, use state=TIMER_STATE_DC when ALLON reference clock. This will get the divide value back.
 *
 * \param[in]	id timer id
 * \param[out]	clk_div clock div
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_get_clk_div(TIMER_ID_E id, uint32_t *clk_div);

/**
 * \brief	Get Clock, get the reference clock for timer, co-work with "hx_drv_timer_get_clk_div", we can know the time period of each time tick.
 *
 * \param[in]	id timer id
 * \param[out]	clk clock
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_get_clk(TIMER_ID_E id, uint32_t *clk);
/**
 * @brief Timer interrupt clear
 * \param[in]	id timer id
 */
void hx_drv_timer_ClearIRQ(TIMER_ID_E id);

/**
 * @brief Returns timer IRQ status
 * \param[in]	id timer id
 */
uint32_t hx_drv_timer_StatusIRQ(TIMER_ID_E id);

/**
 * @brief Returns Timer Reload value.
 * \param[in]	id timer id
 */

uint32_t hx_drv_timer_GetReload(TIMER_ID_E id);

/**
 * @brief Sets Timer Reload value.
 * \param[in]	id timer id
 * \param[in]	value Reload value to set
 */
void hx_drv_timer_SetReload(TIMER_ID_E id, uint32_t value);

/**
 * @brief Returns Timer current value.
 * \param[in]	id timer id
 */
uint32_t hx_drv_timer_GetValue(TIMER_ID_E id);

/**
 * @brief Sets Timer current value.
 * \param[in]	id timer id
 * \param[in]	value time value to set
 */
void hx_drv_timer_SetValue(TIMER_ID_E id, uint32_t value);

/**
 * @brief Get Timer Ctrl. return timer control register value.
 *				bit[0]: enable status of timer
 *				bit[1]: enable by external input, use a high state to trigger timer
 *				bit[2]: enable by external input, use a clock with hihg/low state to trigger timer
 *				bit[3]: timer interrupt enable/disable status.
 */
uint32_t hx_drv_timer_GetCtrl(TIMER_ID_E id);

/**
 * @brief Timer IRQ Handler, isr of timer, which usually not need by application. For application user, use "hx_drv_timer_hw_start" with "Timer_ISREvent_t" callback
 */
void hx_drv_timer_irq_handler(TIMER_ID_E id);

/**
 * \brief	start Sensor TIMER, uses TIMER_ID_0
 *
 * \param[in]	cfg timer configuration 
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_sensor_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop Sensor Timer, uses TIMER_ID_0
 *
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_sensor_stop();

/**
 * \brief	Set Sensor timer Clock Div, uses TIMER_ID_0
 *
 * \param[in]	clk_div	clk divider used to divide reference clock.
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_sensor_clkdiv(uint32_t clk_div, TIMER_STATE_E state);



/**
 * \brief	start CM55M TIMER, uses TIMER_ID_2
 *
 * \param[in]	cfg	timer config
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop CM55M Timer
 *
 * \param[in]	id	TIMER ID, uses TIMER_ID_2
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_stop();
/**
 * \brief	Set CM55M timer Clock Div
 *
 * \param[in]	clk_div	clk div, uses TIMER_ID_2
 * \param[in]	clk_div	clk divider used to divide reference clock.
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_clkdiv(uint32_t clk_div, TIMER_STATE_E state);
/**
 * \brief	CM55M delay millisecond, uses TIMER_ID_3
 *
 * \param[in]	ms  millisecond
  * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_sec_delay_ms(uint32_t ms, TIMER_STATE_E state);

/**
 * \brief	CM55M delay microsecond, uses TIMER_ID_3
 *
 * \param[in]	us  micro second
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_sec_delay_us(uint32_t us, TIMER_STATE_E state);
/**
 * \brief	Set CM55M Sec delay timer Clock Divider, uses TIMER_ID_3
 *
 * \param[in]	clk_div	clk div
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_sec_delay_clkdiv(uint32_t clk_div, TIMER_STATE_E state);

/**
 * \brief	start CM55M TIMER
 *
 * \param[in]	cfg	timer config
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_ns_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop CM55M NS Timer, uses TIMER_ID_6
 *
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_ns_stop();

/**
 * \brief	CM55 NS delay millisecond, uses TIMER_ID_1
 *
 * \param[in]	ms  millisecond
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock. 
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_ns_delay_ms(uint32_t ms, TIMER_STATE_E state);
/**
 * \brief	CM55 NS delay microsecond, uses TIMER_ID_1
 *
 * \param[in]	us  micro second
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock. 
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_ns_delay_us(uint32_t us, TIMER_STATE_E state);
/**
 * \brief	Set CM55M NS delay timer Clock Div, uses TIMER_ID_6
 *
 * \param[in]	clk_div	clk div
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock. 
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55m_ns_delay_clkdiv(uint32_t clk_div, TIMER_STATE_E state);



/**
 * \brief	start CM55S TIMER, uses TIMER_ID_4
 *
 * \param[in]	cfg	timer config
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop CM55S Timer, uses TIMER_ID_4
 *
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_stop();
/**
 * \brief	Set CM55S timer Clock Div, uses TIMER_ID_4
 *
 * \param[in]	clk_div	clk div
 * \param[in]	state timer use in which state
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_clkdiv(uint32_t clk_div, TIMER_STATE_E state);


/**
 * \brief	start CM55S NS TIMER, uses TIMER_ID_7
 *
 * \param[in]	cfg	timer config
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_ns_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop CM55S NS Timer, uses TIMER_ID_7
 *
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_ns_stop();
/**
 * \brief	Set CM55S NS timer Clock Div, uses TIMER_ID_7
 *
 * \param[in]	clk_div	clk div
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock. 
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_ns_clkdiv(uint32_t clk_div, TIMER_STATE_E state);


/**
 * \brief	CM55S delay millisecond, uses TIMER_ID_5
 *
 * \param[in]	ms  millisecond
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.  
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_delay_ms(uint32_t ms, TIMER_STATE_E state);

/**
 * \brief	CM55S delay microsecond, uses TIMER_ID_5
 *
 * \param[in]	us  micro second
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.  
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_delay_us(uint32_t us, TIMER_STATE_E state);

/**
 * \brief	Set CM55s delay timer Clock Div, uses TIMER_ID_5
 *
 * \param[in]	clk_div	clk div
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock. 
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_delay_clkdiv(uint32_t clk_div, TIMER_STATE_E state);


/**
 * \brief	CM55S NS delay millisecond, uses TIMER_ID_8
 *
 * \param[in]	ms  millisecond
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.  
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_ns_delay_ms(uint32_t ms, TIMER_STATE_E state);

/**
 * \brief	CM55S NS delay microsecond, uses TIMER_ID_8
 *
 * \param[in]	us  micro second
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock.  
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_ns_delay_us(uint32_t us, TIMER_STATE_E state);
/**
 * \brief	Set CM55s delay timer Clock Div, uses TIMER_ID_8
 *
 * \param[in]	clk_div	clk div
 * \param[in]	state timer use in which state, use state=TIMER_STATE_DC when ALLON reference clock. 
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55s_ns_delay_clkdiv(uint32_t clk_div, TIMER_STATE_E state);
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* DRIVERS_INC_HX_DRV_TIMER_H_ */
