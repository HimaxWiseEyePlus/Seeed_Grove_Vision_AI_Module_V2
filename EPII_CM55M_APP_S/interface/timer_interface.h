/*
 * timer_interface.h
 *
 *  Created on: 2023¦~6¤ë13¤é
 *      Author: 902447
 */

#ifndef INTERFACE_TIMER_INTERFACE_H_
#define INTERFACE_TIMER_INTERFACE_H_

#ifdef IP_timer
#include "hx_drv_timer.h"

/**
 * \brief	 delay millisecond
 *
 * \param[in]	id  Timer ID
 * \param[in]	ms  millisecond
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55x_delay_ms(uint32_t ms, TIMER_STATE_E state);

/**
 * \brief	delay microsecond
 *
 * \param[in]	id  Timer ID
 * \param[in]	us  micro second
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55x_delay_us(uint32_t us, TIMER_STATE_E state);

#endif
#endif /* INTERFACE_TIMER_INTERFACE_H_ */
