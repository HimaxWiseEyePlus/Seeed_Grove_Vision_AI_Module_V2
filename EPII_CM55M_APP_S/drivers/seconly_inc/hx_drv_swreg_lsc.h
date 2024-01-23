/*
 * hx_drv_swreg_lsc.h
 *
 *  Created on: 2021�~9��5��
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SWREG_LSC_H_
#define DRIVERS_INC_HX_DRV_SWREG_LSC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "hx_drv_swreg_lsc_export.h"
/**
 * \defgroup	SWREG_LSC_DRV	SWREG LSC Driver
 * \ingroup		SWREG_LSC_DRV
 * \brief	SWREG LSC Driver Declaration
 * \details Application can use SW Reg LSC Structure to control Hardware\n
 */
/**
 * \defgroup	SWREG_LSC_DRV_ENUM	SWREG LSC Driver Enumeration
 * \ingroup	SWREG_LSC_DRV
 * \brief	Defines some enumeration of SWREG_LSC Driver need.
 * @{
 */
/**
 * \enum SWREG_LSC_ERROR_E
 * \brief SWREG_LSC Errors Type
 */
typedef enum {
	SWREG_LSC_NO_ERROR = 0, /**< No ERROR */
	SWREG_LSC_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	SWREG_LSC_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} SWREG_LSC_ERROR_E;
/** @} */

/**
 * \defgroup	SWREG_LSC_DRV_MACRO	SW REG LSC Driver MACRO
 * \ingroup	SWREG_LSC_DRV
 * \brief	contains definitions of SWREG LSC Driver MACRO.
 * @{
 */
/**
 * \struct SWREG_LSC_RETENTION_CFG_T
 * \brief Retention flag Configuration
 */
/** @} */

/**
 * \defgroup	SWREG_LSC_DRV_FUNCDLR	SWREG LSC Driver Function Declaration
 * \ingroup	SWREG_LSC_DRV
 * \brief	Contains declarations of SWREG LSC  Driver functions.
 * @{
 */
/**
 * \brief	Set SW REG LSC
 *
 * \param[in]	offset	 offset of SW reg LSC
 * \return	SWREG_LSC_ERROR_E.
 */
SWREG_LSC_ERROR_E hx_drv_swreg_lsc_set_reg(unsigned int offset, unsigned int val);

/**
 * \brief	Set SW REG LSC
 *
 * \param[in]	offset	 offset of SW reg LSC
 * \param[out]	*val	 value
 * \return	SWREG_LSC_ERROR_E.
 */
SWREG_LSC_ERROR_E hx_drv_swreg_lsc_get_reg(unsigned int offset, unsigned int *val);


/**
 * \brief	get BL Version
 *
 * \param[out]	*bl_version	 BL Version
 * \return	SWREG_LSC_ERROR_E.
 */
SWREG_LSC_ERROR_E hx_drv_swreg_lsc_get_bl_version(unsigned int *bl_version);

/**
 * \brief	Set SW Wakeup Event
 *
 * \param[in]	event	 SW Wakeup Event
 * \return	SWREG_LSC_ERROR_E.
 */
SWREG_LSC_ERROR_E hx_drv_swreg_lsc_set_sw_wakeupevent(SW_FORCE_WAKEUPEVENT_E event);

/**
* \brief	Get SW Wakeup Event
*
* \param[in]	event	 SW Wakeup Event
* \return	SWREG_LSC_ERROR_E.
*/
SWREG_LSC_ERROR_E hx_drv_swreg_lsc_get_sw_wakeupevent(SW_FORCE_WAKEUPEVENT_E *event);

/** @} */

#endif /* DRIVERS_INC_HX_DRV_SWREG_LSC_H_ */
