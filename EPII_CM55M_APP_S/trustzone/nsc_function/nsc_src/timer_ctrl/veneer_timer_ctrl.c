/*
 * veneer_timer_ctrl.c
 *
 *  Created on: 2021¦~12¤ë10¤é
 *      Author: 902447
 */
#if !defined(HX_TFM)

#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include <stdio.h>
#include "stdint.h"
#include "arm_cmse.h"
#include "hx_drv_scu.h"
#ifdef IP_timer
#include "veneer_timer_ctrl.h"

/**
 * \brief Timer Callback
 *
 * \retval void.
 */
#if defined(__IAR_SYSTEMS_ICC__)
typedef __cmse_nonsecure_call void (*Timer_ISREvent_NS_t) (uint32_t event);
#else
typedef void (*Timer_ISREvent_NS_t)(uint32_t event) __attribute__((cmse_nonsecure_call));
#endif

Timer_ISREvent_NS_t g_sentimerISR_NS_CB = NULL;
#ifdef CM55_BIG
Timer_ISREvent_NS_t g_cm55mtimerISR_NS_CB = NULL;
#else
Timer_ISREvent_NS_t g_cm55stimerISR_NS_CB = NULL;
#endif
//Periodic Timer Callback
void sensor_timer_fire(uint32_t event)
{
    //trigger capture frame
	if(g_sentimerISR_NS_CB != NULL)
		g_sentimerISR_NS_CB(event);
}

#ifdef CM55_BIG
//Periodic Timer Callback
void cm55m_timer_fire(uint32_t event)
{
    //trigger capture frame
	if(g_cm55mtimerISR_NS_CB != NULL)
		g_cm55mtimerISR_NS_CB(event);
}
#else
//Periodic Timer Callback
void cm55s_timer_fire(uint32_t event)
{
    //trigger capture frame
	if(g_cm55stimerISR_NS_CB != NULL)
		g_cm55stimerISR_NS_CB(event);
}
#endif
/**
 * \brief	start Sensor TIMER
 *
 * \param[in]	cfg	timer config
 * \return	XDMA_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  TIMER_ERROR_E veneer_timer_sensor_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event)
{
	g_sentimerISR_NS_CB = (Timer_ISREvent_NS_t) cmse_nsfptr_create(cb_event);
#ifdef IP_timer
	return hx_drv_timer_sensor_start(cfg, sensor_timer_fire);
#else
	return TIMER_UNKNOWN_ERROR;
#endif
}

/**
 * \brief	Stop Sensor Timer
 *
 * \param[in]	id	TIMER ID
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  TIMER_ERROR_E veneer_timer_sensor_stop()
{
	g_sentimerISR_NS_CB = NULL;
#ifdef IP_timer
	return hx_drv_timer_sensor_stop();
#else
	return TIMER_UNKNOWN_ERROR;
#endif
}

#ifdef CM55_BIG
/**
 * \brief	start CM55M TIMER
 *
 * \param[in]	timer_ms	millisecond
 * \param[in]	mode	timer mode
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  TIMER_ERROR_E veneer_timer_cm55m_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event)
{
	g_cm55mtimerISR_NS_CB = (Timer_ISREvent_NS_t) cmse_nsfptr_create(cb_event);
#ifdef IP_timer
	return hx_drv_timer_cm55m_start(cfg, cm55m_timer_fire);
#else
	return TIMER_UNKNOWN_ERROR;
#endif
}
/**
 * \brief	Stop CM55M Timer
 *
 * \param[in]	id	TIMER ID
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  TIMER_ERROR_E veneer_timer_cm55m_stop()
{
#ifdef IP_timer
	return hx_drv_timer_cm55m_stop();
#else
	return TIMER_UNKNOWN_ERROR;
#endif
}

#else
/**
 * \brief	start CM55S TIMER
 *
 * \param[in]	timer_ms	millisecond
 * \param[in]	mode	timer mode
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  TIMER_ERROR_E veneer_timer_cm55s_start(TIMER_CFG_T *cfg, Timer_ISREvent_t  cb_event)
{
	g_cm55stimerISR_NS_CB = (Timer_ISREvent_NS_t) cmse_nsfptr_create(cb_event);
#ifdef IP_timer
	return hx_drv_timer_cm55s_start(cfg, cm55s_timer_fire);
#else
	return TIMER_UNKNOWN_ERROR;
#endif
}
/**
 * \brief	Stop CM55S Timer
 *
 * \param[in]	id	TIMER ID
 * \param[in]	cb_event	TIMER Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  TIMER_ERROR_E veneer_timer_cm55s_stop()
{
#ifdef IP_timer
	return hx_drv_timer_cm55s_stop();
#else
	return TIMER_UNKNOWN_ERROR;
#endif
}

#endif
#endif

#endif //#if !defined(HX_TFM)
