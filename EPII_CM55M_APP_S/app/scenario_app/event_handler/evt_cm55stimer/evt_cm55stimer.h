/**
 ********************************************************************************************
 *  @file      evt_cm55stimer.h
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
/**
 * \defgroup	EVT_CM55STIMER	CM55STIMER Event
 * \ingroup	EVENT_HANDLER
 * \brief	CM55STIMER EVENT declaration
 */

#ifndef EVENT_HANDLER_EVT_CM55STIMER_EVT_CM55STIMER_H_
#define EVENT_HANDLER_EVT_CM55STIMER_EVT_CM55STIMER_H_

/****************************************************
 * Constant Definition                              *
 ***************************************************/




/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \defgroup	CM55STIMER_ENUM	CM55STIMER Communication Library Enumeration
 * \ingroup	CM55STIMER
 * \brief	Defines the required enumeration of CM55STIMER library.
 * @{
 */
/**
 * \enum EVT_CM55STIMER_ERR_E
 * \brief this enumeration use in CM55STIMER library, define the status of CM55STIMER process.
 */
typedef enum
{
	EVT_CM55STIMER_NO_ERROR	             = 0,	/**< STATUS: No Error*/
	EVT_CM55STIMER_ERR_DRVFAIL	           	    	/**< STATUS: Driver Not Initialize Correctly */
} EVT_CM55STIMER_ERR_E;
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
 * \brief   Init CM55s Timer
 *
 * Init CM55STIMER process
 * \retval  EVT_ISPCOMM_NO_ERROR    no error
 * \retval  others  error
 */
EVT_CM55STIMER_ERR_E evt_cm55stimer_init(void);


/**
 * \brief   Callback function for cm55stimer process
 *
 * Callback function for CM55STIMER process
 * \retval  INFRA_EVT_RTN_NONE  to keep event alive
 * \retval  INFRA_EVT_RTN_CLEAR to clear event
 */
uint8_t evt_cm55stimer_cb(void);  // isp rx event handler


/** @} */	//EVT_CM55STIMER_FUNCDLR

#endif /* EVENT_HANDLER_EVT_I2CCOMM_EVT_I2CCOMM_H_ */
