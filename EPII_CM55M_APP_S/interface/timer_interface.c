/*
 * timer_interface.c
 *
 *  Created on: 2023¦~6¤ë13¤é
 *      Author: 902447
 */

#include <stdio.h>
#include "WE2_device.h"
#include "WE2_core.h"

#ifdef IP_timer
#include "timer_interface.h"
#endif




#ifdef IP_timer
/**
 * \brief	 delay millisecond
 *
 * \param[in]	id  Timer ID
 * \param[in]	ms  millisecond
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55x_delay_ms(uint32_t ms, TIMER_STATE_E state)
{
#ifdef CM55_BIG
#ifdef TRUSTZONE_NS
	return hx_drv_timer_cm55m_ns_delay_ms(ms, state);
#else
	return hx_drv_timer_cm55m_sec_delay_ms(ms, state);
#endif//CM55M TRUSTZONE_NS
#else//CM55_BIG
#ifdef TRUSTZONE_NS
	return hx_drv_timer_cm55s_ns_delay_ms(ms, state);
#else
	return hx_drv_timer_cm55s_delay_ms(ms, state);
#endif//CM55S TRUSTZONE_NS
#endif//CM55_BIG

}

/**
 * \brief	delay microsecond
 *
 * \param[in]	id  Timer ID
 * \param[in]	us  micro second
 * \return	TIMER_ERROR_E.
 */
TIMER_ERROR_E hx_drv_timer_cm55x_delay_us(uint32_t us, TIMER_STATE_E state)
{
#ifdef CM55_BIG
#ifdef TRUSTZONE_NS
	return hx_drv_timer_cm55m_ns_delay_us(us, state);
#else
	return hx_drv_timer_cm55m_sec_delay_us(us, state);
#endif
#else
#ifdef TRUSTZONE_NS
	return hx_drv_timer_cm55s_ns_delay_us(us, state);
#else
	return hx_drv_timer_cm55s_delay_us(us, state);
#endif
#endif
}
#endif
