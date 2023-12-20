/*
 * hx_drv_watchdog.h
 *
 *  Created on: 2021/4/29
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_WATCHDOG_H_
#define DRIVERS_INC_HX_DRV_WATCHDOG_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	WATCHDOG_DRV	WATCHDOG Driver
 * \ingroup		WATCHDOG_DRV
 * \brief	WATCHDOG Driver Declaration
 * \details Application can use WATCHDOG Structure to control Hardware\n
 * <pre>
 * For Example
 * Ex1: WATCHDOG_CFG_T: All WATCHDOG configuration for application to control WATCHDOG
 * </pre>
 */
/**
 * \defgroup	WATCHDOG_DRV_ENUM	WATCHDOG Driver Enumeration
 * \ingroup	WATCHDOG_DRV
 * \brief	Defines some enumeration of WATCHDOG Driver need.
 * @{
 */
/**
 * \enum WATCHDOG_ERROR_E
 * \brief WATCHDOG Errors Type
 */
typedef enum {
	WATCHDOG_NO_ERROR = 0, /**< No ERROR */
	WATCHDOG_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	WATCHDOG_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} WATCHDOG_ERROR_E;

/**
 * \enum WATCHDOG_ACTION_E
 * \brief ACTION of WATCHDOG
 */
typedef enum {
	WATCHDOG_NO_ACTION = 0, /**< type = 0 : No action */
	WATCHDOG_INT, /**< type = 1 : Interrupt */
	WATCHDOG_RESET, /**< type = 2 : Reset */
} WATCHDOG_ACTION_E;


/**
 * \enum WATCHDOG_ID_E
 * \brief ID of WATCHDOG
 */
typedef enum {
	WATCHDOG_ID_0 = 0, /**< WATCHDOG ID 0 */
	WATCHDOG_ID_1, /**< WATCHDOG ID 1 */
	WATCHDOG_ID_MAX, /**< WATCHDOG MAX */
} WATCHDOG_ID_E;

/**
 * \enum WATCHDOG_CTRL_E
 * \brief WATCHDOG Ctrl in CPU or PMU
 */
typedef enum {
	WATCHDOG_CTRL_CPU = 0, /**< WATCHDOG Control By CPU*/
	WATCHDOG_CTRL_PMU /**< WATCHDOG Control By PMU Case*/
} WATCHDOG_CTRL_E;

/**
 * \enum WATCHDOG_STATE_E
 * \brief WATCHDOG Used in DC or PMU
 */
typedef enum {
	WATCHDOG_STATE_DC = 0, /**< WATCHDOG Used in DC*/
	WATCHDOG_STATE_PMU /**< WATCHDOG Used in PMU*/
} WATCHDOG_STATE_E;

/**
 * \struct WATCHDOG_CFG_T
 * \brief Watchdog configuration
 */
typedef struct {
	uint32_t period;
	WATCHDOG_CTRL_E ctrl;
	WATCHDOG_STATE_E state;
	WATCHDOG_ACTION_E type;
} WATCHDOG_CFG_T;
/** @} */

/**
 * \defgroup	WATCHDOG_DRV_MACRO	WATCHDOG Driver MACRO
 * \ingroup	WATCHDOG_DRV
 * \brief	contains definitions of WATCHDOG Driver MACRO.
 * @{
 */

/** @} */

/**
 * \defgroup	WATCHDOG_DRV_FUNCDLR	Watchdog Driver Function Declaration
 * \ingroup	WATCHDOG_DRV
 * \brief	Contains declarations of Watchdog Driver functions.
 * @{
 */
/**
 * \brief	Watchdog interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*WDG_ISREvent_t) (uint32_t event);

/**
 * \brief WATCHDOG driver init
 *
 * \param[in]	id	id
 * \param[in]	wdt_baseaddr	wdt HW base address
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_init(WATCHDOG_ID_E id, uint32_t wdt_baseaddr);


/**
 * \brief	program WATCHDOG
 *
 *  type = 0 : No action
 *  type = 1 : Interrupt
 *  type = 2 : Reset
 * \param[in]	id	id
 * \param[in]	cfg	watch dog config
 * \param[in]	wdg_cb	watchdog cb
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_start(WATCHDOG_ID_E id, WATCHDOG_CFG_T *cfg, WDG_ISREvent_t wdg_cb);

/**
 * \brief	stop WATCHDOG
 *
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_stop(WATCHDOG_ID_E id);

/**
 * \brief	update watchdog counter
 *
 * \param[in]	ms	millisecond
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_update(WATCHDOG_ID_E id, uint32_t ms);

/**
 * \brief	 clear watchdog interrupt
 *
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_irq_clear(WATCHDOG_ID_E id);

/**
 * \brief	 get watchdog raw interrupt status
 *
 * \return	uint32_t  raw irq status
 */
uint32_t hx_drv_watchdog_irq_raw_status(WATCHDOG_ID_E id);


/**
 * \brief	 get watchdog  interrupt status
 *
 * \return	uint32_t  irq status
 */
uint32_t hx_drv_watchdog_irq_status(WATCHDOG_ID_E id);

/**
 * \brief	 get watchdog  value
 *
 * \return	uint32_t  watchdog value
 */
uint32_t hx_drv_watchdog_value(WATCHDOG_ID_E id);

/**
 * \brief	Set Clock Div
 *
 * \param[in]	id watchdog id
 * \param[in]	clk_div clock div
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_set_clk_div(WATCHDOG_ID_E id, uint32_t clk_div);

/**
 * \brief	Get Watchdog Div
 *
 * \param[in]	id watchdog id
 * \param[out]	clk_div clock div
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_get_clk_div(WATCHDOG_ID_E id, uint32_t *clk_div);

/**
 * \brief	Get Clock
 *
 * \param[in]	id watchdog id
 * \param[out]	clk clock
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_get_clk(WATCHDOG_ID_E id, uint32_t *clk);


/**
 * \brief	program CM55M WATCHDOG
 *
 *  type = 0 : No action
 *  type = 1 : Interrupt
 *  type = 2 : Reset
 * \param[in]	cfg	config
 * \param[in]	wdg_cb	watchdog cb
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_cm55m_start(WATCHDOG_CFG_T *cfg, WDG_ISREvent_t wdg_cb);

/**
 * \brief	stop CM55M WATCHDOG
 *
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_cm55m_stop();

/**
 * \brief	update CM55M watchdog counter
 *
 * \param[in]	ms	millisecond
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_cm55m_update(uint32_t ms);

/**
 * \brief	program CM55S WATCHDOG
 *
 *  type = 0 : No action
 *  type = 1 : Interrupt
 *  type = 2 : Reset
 * \param[in]	cfg	config
 * \param[in]	wdg_cb	watchdog callback
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_cm55s_start(WATCHDOG_CFG_T *cfg, WDG_ISREvent_t wdg_cb);

/**
 * \brief	stop CM55S WATCHDOG
 *
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_cm55s_stop();

/**
 * \brief	update CM55S watchdog counter
 *
 * \param[in]	ms	millisecond
 * \return	WATCHDOG_ERROR_E.
 */
WATCHDOG_ERROR_E hx_drv_watchdog_cm55s_update(uint32_t ms);



#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DRIVERS_INC_HX_DRV_WATCHDOG_H_ */
