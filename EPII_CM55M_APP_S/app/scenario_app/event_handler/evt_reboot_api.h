/**
 ********************************************************************************************
 *  @file      evt_reboot_api.h
 *  @details   This file contains all evt reboot related function
 *  @author    himax/903935
 *  @version   V1.0.0
 *  @date      26-Oct-2023
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
/**
 * \defgroup	EVENT_HANDLER_REBOOT_API	Event Handler Framework
 * \ingroup	EVENT_HANDLER
 * \brief	Event Handler Framework declaration
 */
 
#ifndef EVENT_HANDLER_EVT_REBOOT_API_H_
#define EVENT_HANDLER_EVT_REBOOT_API_H_

/****************************************************
 * Type Definition                                 *
 ***************************************************/

/****************************************************
 * Constant Definition                              *
 ***************************************************/

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/


/** @} */
/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \defgroup    EVENT_HANDLER_REBOOT_API evt reboot api Function Declaration
 * \ingroup EVENT_HANDLER
 * \brief   Contains declarations of event reboot api functions.
 * @{
 */

/**
 * \brief   The function use to set PMU reboot timer
 *
 * \retval  no return no error
 */
void setTimer0AlarmPMU();

/**
 * \brief   The function use to invoke PMU reboot 
 *
 * \retval  no return no error
 */
void setPS_PDNoVid();
/** @} */   //EVENT_HANDLER_REBOOT_API

#endif /* EVENT_HANDLER_EVT_REBOOT_API_H_ */

