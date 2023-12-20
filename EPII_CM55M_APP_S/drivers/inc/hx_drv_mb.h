/*
 * hx_drv_mailbox.h
 *
 *  Created on: 2021�~4��23��
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_MB_H_
#define DRIVERS_INC_HX_DRV_MB_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	MAILBOX_DRV	MAILBOX Driver
 * \ingroup		MAILBOX_DRV
 * \brief	MAILBOX Driver Declaration
 * \details Application can use MAILBOX Structure to control Hardware\n
 * <pre>
 * For Example
 * Ex1: MAILBOX_CFG_T: All MAILBOX configuration for application to control MAILBOX
 * </pre>
 */
/**
 * \defgroup	MAILBOX_DRV_ENUM	MAILBOX Driver Enumeration
 * \ingroup	MAILBOX_DRV
 * \brief	Defines some enumeration of MAILBOX Driver need.
 * @{
 */
/**
 * \enum MB_ERROR_E
 * \brief MAILBOX Errors Type
 */
typedef enum {
	MB_NO_ERROR = 0, /**< No ERROR */
	MB_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	MB_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} MB_ERROR_E;

/**
 * \enum MB_CORE_E
 * \brief Core of MAILBOX
 */
typedef enum {
	MB_CORE_CM55M = 0, /**< Core is CM55M  */
	MB_CORE_CM55S, /**< Core is CM55S */
} MB_CORE_E;

/**
 * \enum MB_MUTEX_STATUS_E
 * \brief Core of MAILBOX
 */
typedef enum {
	MB_MUTEX_STATUS_NOT_GET = 0, /**< Not require Mutex */
	MB_MUTEX_STATUS_GET, /**< Require Mutex */
} MB_MUTEX_STATUS_E;
/** @} */

/**
 * \defgroup	MAILBOX_DRV_MACRO	MAILBOX Driver MACRO
 * \ingroup	MAILBOX_DRV
 * \brief	contains definitions of MAILBOX Driver MACRO.
 * @{
 */

/** @} */

/**
 * \defgroup	MAILBOX_DRV_FUNCDLR	MailBox Driver Function Declaration
 * \ingroup	MAILBOX_DRV
 * \brief	Contains declarations of MailBox Driver functions.
 * @{
 */

/**
 * \brief	CM55M MB interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*CM55M_MB_ISREvent_t) (uint32_t event);


/**
 * \brief	CM55S MB interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*CM55S_MB_ISREvent_t) (uint32_t event);

/**
 * \brief	initial MAILBOX Driver
 *
 * \param[in]	mb_baseaddr	CM55M MB base address
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_init(MB_CORE_E core, uint32_t mb_baseaddr);

/**
 * \brief	register CM55M MB Interrupt Status callback
 *
 * \param[in]	cb_event	CM55M MB Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_register_cm55m_cb(CM55M_MB_ISREvent_t  cb_event);

/**
 * \brief	trigger CM55M IRQ
 *
 * \param[in]	irq_status	 CM55M IRQ interrupt
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_trigger_cm55m_irq(uint32_t irq_status);

/**
 * \brief	clear CM55M IRQ
 *
 * \param[in]	irq_status	 CM55M IRQ interrupt
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_clear_cm55m_irq(uint32_t irq_status);

/**
 * \brief	get CM55M IRQ status
 *
 * \param[out]	irq_status	 CM55M IRQ Status
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_get_cm55m_irq_status(uint32_t *irq_status);

/**
 * \brief	register CM55S MB Interrupt Status callback
 *
 * \param[in]	cb_event	CM55S MB Interrupt callback function
 * \return	XDMA_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_register_cm55s_cb(CM55S_MB_ISREvent_t  cb_event);


/**
 * \brief	trigger CM55S IRQ
 *
 * \param[in]	irq_status	 CM55S IRQ interrupt
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_trigger_cm55s_irq(uint32_t irq_status);

/**
 * \brief	clear CM55S IRQ
 *
 * \param[in]	irq_status	 CM55S IRQ interrupt
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_clear_cm55s_irq(uint32_t irq_status);

/**
 * \brief	get CM55S IRQ status
 *
 * \param[out]	irq_status	 CM55S IRQ Status
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_get_cm55s_irq_status(uint32_t *irq_status);


/**
 * \brief	set share information data N content
 *
 * \param[in]	number  number of share information
 * \param[in]	share_data  share_data of share information
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_set_sharedata(uint8_t number, uint32_t share_data);

/**
 * \brief	get share information data N content
 *
 * \param[in]	number  number of share information
 * \param[out]	share_data  share_data of share information
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_get_sharedata(uint8_t number, uint32_t *share_data);

/**
 * \brief	release number of N mutex
 *
 * \param[in]	number  number of Mutex
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_release_mutex(uint8_t number);

/**
 * \brief	require  number of N mutex
 *
 * \param[in]	number  number of Mutex
 * \param[out]	mutex_status  mutex  status (1: get mutex success, 0: get mutex fail)
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_require_mutex(uint8_t number, MB_MUTEX_STATUS_E *mutex_status);

/**
 * \brief	de-initial MAILBOX Driver
 *
 * \param[in]	core  Current Core
 * \return	MB_ERROR_E.
 */
MB_ERROR_E hx_drv_mb_deinit(MB_CORE_E core);
/** @} */

#endif /* DRIVERS_INC_HX_DRV_MB_H_ */
