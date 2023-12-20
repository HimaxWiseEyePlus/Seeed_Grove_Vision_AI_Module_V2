/**
 ********************************************************************************************
 *  @file      aud_lib.h
 *  @details   This file contains all I2S/PDM audio library related function
 *  @author    himax/902453
 *  @version   V1.0.0
 *  @date      21-Aug-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
/**
 * \defgroup	AUDIO_LIB	Audio Library
 * \ingroup	AUDIO_LIB
 * \brief	I2S/PDM Audio library Declaration
 */

#ifndef LIBRARY_AUDIO_LIB_H_
#define LIBRARY_AUDIO_LIB_H_
#include <stdio.h>
#include <stdint.h>

#ifdef IP_pdm
#include "hx_drv_pdm_rx.h"
#endif

#ifdef IP_i2s
#include "hx_drv_i2s.h"
#endif
/***************************************************
 * Macro Definition
 **************************************************/
/**
 * \defgroup	AUDIO_LIB_ENUM	Audio Library Enumeration
 * \ingroup	AUDIO_LIB
 * \brief	Defines some enumeration of Audio Library need.
 * @{
 */
/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/** AUDIO Errors Type **/
/**
 * \enum AUDIO_ERROR_E
 * \brief AUDIO API ERROR Code
 */
typedef enum AUDIO_ERROR_S
{
    AUDIO_NO_ERROR = 0,                 /**< No ERROR */
    AUDIO_ERROR_NOT_INIT = 1,           /**< ERROR MSG: not init */
    AUDIO_ERROR_DRIVER = 2,             /**< ERROR MSG: audio driver error */
    AUDIO_ERROR_ALREADY_INIT = 3,       /**< ERROR MSG: already init */
    AUDIO_ERROR_INVALID_PARAMETERS = 4, /**< ERROR MSG: Invalid parameters */
    AUDIO_ERROR_NULL_POINTER = 5,       /**< ERROR MSG: NULL pointer */
    AUDIO_UNKNOWN_ERROR = 6,            /**< ERROR MSG: UNKNOWN ERROR*/
} AUDIO_ERROR_E;

typedef enum AUDIO_IF_S
{
    AUDIO_IF_PDM,
    AUDIO_IF_I2S,
} AUDIO_IF_E;

typedef enum AUDIO_RX_MODE_S
{
    AUDIO_INT,
    AUDIO_DMA,
} AUDIO_RX_MODE_E;

typedef struct audio_buffer_config {
    uint32_t *addr;             /* This constant defines the Audio buffer start address */
    uint32_t block_num;         /* This constant defines the total number of Audio buffer block */
    uint32_t block_sz;          /* This constant defines audio block size, max is 8KB for uDMA single transfer */
    uint32_t cb_evt_blk;        /* This constant defines the Audio buffer block fill callback event */
} audio_buffer_config_t;

typedef struct audio_config {
    AUDIO_IF_E                  interface;
    audio_buffer_config_t          buffer;
    AUDIO_RX_MODE_E               rx_mode;  
#ifdef IP_pdm
    PDM_CFG_T                   pdm_config;
#endif
#ifdef IP_i2s
    I2S_CFG_T                   i2s_config;
#endif
} audio_config_t;

typedef void (*AUD_ISR_CB)(uint32_t int_status);

/** @} */

/**
 * \brief	The function is used to get audio block size
 *
 * \retval	bytes	
 */

uint32_t hx_lib_audio_get_block_size(void);
/** @} */

/**
 * \brief	The function is used to init Audio RX
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_init(audio_config_t *aud_cfg);
/** @} */

/**
 * \brief	The function is used to deinit Audio RX
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_deinit(void);
/** @} */

/**
 * \brief	The function is used to register Audio RX evt cb
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_register_evt_cb(AUD_ISR_CB aud_evt_cb);
/** @} */

/**
 * \brief	The function is used to start Audio RX
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_start(void);
/** @} */

/**
 * \brief	The function is used to stop Audio RX
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_stop(void);
/** @} */

/**
 * \brief	The function is used to request Audio RX read index
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_request_read(uint32_t *address, uint32_t *block_num);
/** @} */

/**
 * \brief	The function is used to update Audio RX read index
 *
 * \retval	AUDIO_ERROR_E	success
 * \retval	others			fail
 */
AUDIO_ERROR_E hx_lib_audio_update_idx(uint32_t *block);
/** @} */

#endif /* LIBRARY_AUDIO_LIB_H_ */
