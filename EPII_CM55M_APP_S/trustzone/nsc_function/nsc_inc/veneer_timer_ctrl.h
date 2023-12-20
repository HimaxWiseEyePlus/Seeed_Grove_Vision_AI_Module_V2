/*
 * veneer_timer_ctrl.h
 *
 *  Created on: 2021¦~12¤ë10¤é
 *      Author: 902447
 */

#ifndef TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_TIMER_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_TIMER_CTRL_H_

#include "hx_drv_timer.h"
#ifdef IP_timer
#include "timer_interface.h"
#endif
#ifdef IP_timer
/**
 * \brief	start Sensor TIMER
 *
 * \param[in]	cfg	timer cfg
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E veneer_timer_sensor_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);

/**
 * \brief	Stop Sensor Timer
 *
 * \param[in]	id	TIMER ID
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E veneer_timer_sensor_stop();

#ifdef CM55_BIG
/**
 * \brief	start CM55M TIMER
 *
 * \param[in]	cfg	timer config
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E veneer_timer_cm55m_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop CM55M Timer
 *
 * \param[in]	id	TIMER ID
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E veneer_timer_cm55m_stop();
#else
/**
 * \brief	start CM55S TIMER
 *
 * \param[in]	cfg	timer cfg
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E veneer_timer_cm55s_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event);
/**
 * \brief	Stop CM55S Timer
 *
 * \param[in]	id	TIMER ID
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
TIMER_ERROR_E veneer_timer_cm55s_stop();

#endif
#endif
#endif /* TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_TIMER_CTRL_H_ */
