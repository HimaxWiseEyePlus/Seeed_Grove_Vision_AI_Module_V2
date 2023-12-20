/**
 ********************************************************************************************
 *  @file      evt_cm55mmb.h
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef EVENT_HANDLER_EVT_CM55MMB_H_
#define EVENT_HANDLER_EVT_CM55MMB_H_
#include <stdint.h>

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
 * \defgroup	CM55MMB_ENUM	CM55MMB Communication Library Enumeration
 * \ingroup	CM55MMB
 * \brief	Defines the required enumeration of CM55MMB library.
 * @{
 */
/**
 * \enum EVT_CM55MMB_ERR_E
 * \brief this enumeration use in CM55MMB library, define the status of CM55MMB process.
 */
typedef enum
{
	EVT_CM55MMB_NO_ERROR	             = 0,	/**< STATUS: No Error*/
	EVT_CM55MMB_ERR_DRVFAIL				 = 1,	/**< STATUS: Driver Not Initialize Correctly */
} EVT_CM55MMB_ERR_E;
/** @} */


/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \defgroup	EVT_CM55MMB_FUNCDLR	CM55M MAILBOX Event Function Declaration
 * \ingroup	EVENT_HANDLER
 * \brief	Contains declarations of CM55M MAILBOX Event functions.
 * @{
 */

/**
 * \brief   Init CM55M MB
 *
 * Init CM55MMB process
 * \retval  EVT_CM55MMB_NO_ERROR    no error
 * \retval  others  error
 */
EVT_CM55MMB_ERR_E evt_cm55mmb_init(void);


/**
 * \brief   Callback function for CM55MMB process
 *
 * Callback function for CM55MMB process
 * \retval  HX_EVENT_RTN_NONE  to keep event alive
 * \retval  HX_EVENT_RTN_CLEAR to clear event
 */
uint8_t evt_cm55mmb_cb(void);


/** @} */	//EVT_CM55MMB_FUNCDLR

#endif /* EVENT_HANDLER_EVT_CM55MMB_H_ */
