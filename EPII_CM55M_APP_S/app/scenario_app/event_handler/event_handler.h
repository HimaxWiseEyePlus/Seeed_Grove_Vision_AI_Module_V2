/**
 ********************************************************************************************
 *  @file      event_handler.h
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

/**
 * \defgroup	EVENT_HANDLER	Event Handler Framework
 * \ingroup	EVENT_HANDLER
 * \brief	Event Handler Framework declaration
 */
#include "hxevent.h"
#include "event_handler_cfg.h"


/****************************************************
 * Constant Definition                              *
 ***************************************************/


/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \defgroup	EVENT_HANDLER_ENUM	Event Handler Framework Enumeration
 * \ingroup	EVENT_HANDLER
 * \brief	Defines the required enumeration of Event Handler Framework.
 * @{
 */

/** @} */

/****************************************************
 * Type Definition                                 *
 ***************************************************/

/****************************************************
 * Structure Definition                             *
 ***************************************************/
/**
 * \defgroup	EVENT_HANDLER_STRUCT		Event Handler Framework Structure
 * \ingroup	EVENT_HANDLER
 * \brief	Defines the required structure of Event Handler Framework library.
 * @{
 */

/** @} */



/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \defgroup	EVENT_HANDLER_FUNCDLR	Event Handler Framework Function Declaration
 * \ingroup	EVENT_HANDLER
 * \brief	Contains declarations of Event Handler Framework functions.
 * @{
 */

/**
 * \brief	The function use to initialize the Event Handler Framework
 *
 * Initialize the event handler framework
 * \retval	void
 */
void event_handler_init(void);


/**
 * \brief	The function use to de-initialize the Event Handler Framework
 *
 * Initialize the event handler framework
 * \retval	void
 */
void event_handler_deinit(void);


/**
 * \brief	The function use to start the Event Handler Framework
 *
 * Start the event handler framework
 * \retval	void
 */
void event_handler_start(void);


/**
 * \brief	The function use to stop the Event Handler Framework
 *
 * Stop the event handler framework
 * \retval	void
 */
void event_handler_stop(void);

/** @} */

#endif /* _EVENT_HANDLER_H_ */
