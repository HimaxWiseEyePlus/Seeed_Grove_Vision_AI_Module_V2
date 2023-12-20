/**
 ********************************************************************************************
 *  @file      evt_CM55MTIMER.h
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef EVENT_HANDLER_EVT_CM55MTIMER_EVT_CM55MTIMER_H_
#define EVENT_HANDLER_EVT_CM55MTIMER_EVT_CM55MTIMER_H_
#include "stdint.h"

/**
 * \defgroup	EVT_CM55MTIMER	CM55MTIMER Event
 * \ingroup	EVENT_HANDLER
 * \brief	CM55MTIMER EVENT declaration
 */


/****************************************************
 * Constant Definition                              *
 ***************************************************/




/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \defgroup	CM55MTIMER_ENUM	CM55MTIMER Communication Library Enumeration
 * \ingroup	CM55MTIMER
 * \brief	Defines the required enumeration of CM55MTIMER library.
 * @{
 */
/**
 * \enum EVT_CM55MTIMER_ERR_E
 * \brief this enumeration use in CM55MTIMER library, define the status of CM55MTIMER process.
 */
typedef enum
{
	EVT_CM55MTIMER_NO_ERROR	             = 0,	/**< STATUS: No Error*/
	EVT_CM55MTIMER_ERR_DRVFAIL	           	    	/**< STATUS: Driver Not Initialize Correctly */
} EVT_CM55MTIMER_ERR_E;
/** @} */


/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \defgroup	EVT_I2CCOMM_FUNCDLR	I2C Communication Event Function Declaration
 * \ingroup	EVENT_HANDLER
 * \brief	Contains declarations of I2C Communication Event functions.
 * @{
 */

/**
 * \brief CM55MTIMER user define callback function type definition
 *
 * \retval void.
 */
typedef void (*evthandlercm55mtimer_CBEvent_t) (void);

/**
 * \brief   Init CM55m Timer
 *
 * Init CM55MTIMER process
 * \retval  EVT_CM55MTIMER_NO_ERROR    no error
 * \retval  others  error
 */
EVT_CM55MTIMER_ERR_E evt_cm55mtimer_init(void);


/**
 * \brief   Callback function for CM55MTIMER process
 *
 * Callback function for CM55MTIMER process
 * \retval  HX_EVENT_RTN_NONE  to keep event alive
 * \retval  HX_EVENT_RTN_CLEAR to clear event
 */
uint8_t evt_cm55mtimer_cb(void);

/**
 * \brief   Trig for CM55MTIMER process
 *
 * Callback function for CM55MTIMER process
 */
void cm55mtimer_trig_oneshot(uint32_t delayms, evthandlercm55mtimer_CBEvent_t cb);

/**
 * \brief   Stop CM55MTIMER process
 *
 * Callback function for CM55MTIMER process
 */
void cm55mtimer_stop_oneshot();


/** @} */	//EVT_CM55MTIMER_FUNCDLR

#endif /* EVENT_HANDLER_EVT_I2CCOMM_EVT_I2CCOMM_H_ */
