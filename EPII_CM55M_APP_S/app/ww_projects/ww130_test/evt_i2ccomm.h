/**
 ********************************************************************************************
 *  @file      evt_i2ccomm.h
 *  @details   This file contains all event handler related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
/**
 * \defgroup    EVT_I2CCOMM I2CCOMM Event
 * \ingroup EVENT_HANDLER
 * \brief   I2CCOMM EVENT declaration
 */

#ifndef EVENT_HANDLER_EVT_I2CCOMM_EVT_I2CCOMM_H_
#define EVENT_HANDLER_EVT_I2CCOMM_EVT_I2CCOMM_H_

#include "hx_drv_iic.h"

/****************************************************
 * Constant Definition                              *
 ***************************************************/

/* I2CCOMM_FEATURE_SYS : Payload Length Definition of RX buffer
 * I2CCOMM_SYS_CMD_E Command Payload Size for i2c master read
 */
#define I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP     4 // I2CCOMM_CMD_SYS_GET_VER_BSP
#define I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C     4 // I2CCOMM_CMD_SYS_GET_VER_I2C

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \defgroup    I2C_COMM_ENUM   I2C Communication Library Enumeration
 * \ingroup I2C_COMM
 * \brief   Defines the required enumeration of I2C communication library.
 * @{
 */

/**
 * \enum EVT_IICCOM_ERR_E
 * \brief this enumeration use in i2c communication library, define the status of i2c communication process.
 */
typedef enum
{
    EVT_IICCOMM_NO_ERROR                 = 0,   /**< STATUS: No Error*/
    EVT_IICCOMM_ERR_DRVFAIL                     /**< STATUS: Driver Not Initialize Correctly */
} EVT_IICCOM_ERR_E;

/**
 * \enum I2CCOMM_FEATURE_E
 * \brief this enumeration use in i2c communication library, define the supported feature.
 */
typedef enum
{
    I2CCOMM_FEATURE_SYS                 = 0x00, /**< SUPPORT FEATURE: system information*/
    I2CCOMM_FEATURE_REG_ACCES           = 0x10, /**< SUPPORT FEATURE: WE-I Register read/write*/
    I2CCOMM_FEATURE_GPIO_CTRL           = 0x11,
    I2CCOMM_FEATURE_SENSOR_REG_ACCESS   = 0x20,
    I2CCOMM_FEATURE_QUANTA_ISH          = 0x30, /**< SUPPORT FEATURE: Quanta NB Product - ISH communication*/
    I2CCOMM_FEATURE_SPI                 = 0x40,
    I2CCOMM_FEATURE_OTA_RESERVED        = 0x50,
    I2CCOMM_FEATURE_OTA_ALL             = 0x51,
    I2CCOMM_FEATURE_OTA_MEM_DSP         = 0x52,
    I2CCOMM_FEATURE_OTA_APP             = 0x53,
    I2CCOMM_FEATURE_OTA_APP_CONFIG      = 0x54,
    I2CCOMM_FEATURE_OTA_EMZA_CONFIG     = 0x55,
    I2CCOMM_FEATURE_OTA_CNNLUT          = 0x56,
    I2CCOMM_FEATURE_AUDIO_TEST          = 0x57,
    I2CCOMM_FEATURE_CSTM_SECURE_DGB     = 0x70,

    I2CCOMM_FEATURE_CUSTOMER_MIN        = 0x80,
    I2CCOMM_FEATURE_CUSTOMER_MAX        = 0x8F,
    I2CCOMM_FEATURE_MAX
} I2CCOMM_FEATURE_E;

/**
 * \enum I2CCOMM_SYS_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported system command.
 */
typedef enum
{
    I2CCOMM_CMD_SYS_GET_VER_BSP = 0, /**< System CMD : get version number of BSP */
    I2CCOMM_CMD_SYS_GET_VER_I2C,     /**< System CMD : get version number of I2C communication protocol */
    I2CCOMM_CMD_SYS_GET_VER_FORMAL,  /**< System CMD : get formal version number of BSP */
    I2CCOMM_CMD_SYS_MAX
} I2CCOMM_SYS_CMD_E;

/**
 * \enum I2CCOMM_REG_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported WE-I register read/write command.
 */
typedef enum
{
    I2CCOMM_CMD_AHB_REG_GET = 0, /**< Register CMD : get AHB register */
    I2CCOMM_CMD_AHB_REG_SET,     /**< Register CMD : set AHB register */
    I2CCOMM_CMD_AUX_REG_GET,     /**< Register CMD : get AUX register */
    I2CCOMM_CMD_AUX_REG_SET,     /**< Register CMD : set AUX register */
    I2CCOMM_CMD_REG_MAX
} I2CCOMM_REG_CMD_E;

/**
 * \enum I2CCOMM_GPIO_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported GPIO control command.
 */
typedef enum
{
    I2CCOMM_CMD_GPIO_MUX = 0, /**< GPIO CMD : set io mux */
    I2CCOMM_CMD_GPIO_SET,     /**< GPIO CMD : set value */
    I2CCOMM_CMD_GPIO_GET,     /**< GPIO CMD : get value */
    I2CCOMM_CMD_IO_MAX
} I2CCOMM_GPIO_CMD_E;

/**
 * \enum I2CCOMM_SENSOR_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported sensor register r/w command.
 */
typedef enum
{
    I2CCOMM_CMD_SENSOR_REG_SET = 0, /**< Sensor Register CMD: set register */
    I2CCOMM_CMD_SENSOR_REG_GET,     /**< Sensor Register CMD: get value */
    I2CCOMM_CMD_SENSOR_MAX
} I2CCOMM_SENSOR_CMD_E;

/**
 * \enum I2CCOMM_OTA_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported OTA command.
 */
typedef enum
{
    I2CCOMM_CMD_OTA_GET_VER                         = 0x0,
    I2CCOMM_CMD_OTA_GET_PROJID                      = 0x1,
    I2CCOMM_CMD_OTA_START                           = 0x2,
    I2CCOMM_CMD_OTA_DATA                            = 0x3,
    I2CCOMM_CMD_OTA_END                             = 0x4,
    I2CCOMM_CMD_OTA_SENT_RESULT                     = 0x5,
    I2CCOMM_CMD_OTA_ABORT                           = 0x6,
    I2CCOMM_CMD_OTA_JUMP2UPG                        = 0x7,
    I2CCOMM_CMD_OTA_SEND_CHIP_ID                    = 0x8,
    I2CCOMM_CMD_OTA_TURN_ON_ECI                     = 0x9,
    I2CCOMM_CMD_OTA_TURN_OFF_ECI                    = 0xA,
    I2CCOMM_CMD_OTA_TURN_OFF_PDM                    = 0xB,
    I2CCOMM_CMD_OTA_AUDIO_RECOGNITION_INIT          = 0xC,
    I2CCOMM_CMD_OTA_AUDIO_RECOGNITION_RECV_DATA     = 0xD,
    I2CCOMM_CMD_OTA_AUDIO_RECOGNITION               = 0xE,
    I2CCOMM_CMD_OTA_GET_AUDIO_RESULT                = 0xF,
    I2CCOMM_CMD_OTA_GET_ALGO_ID                     = 0x14,
    I2CCOMM_CMD_OTA_MAX
} I2CCOMM_OTA_CMD_E;

/**
 * \enum I2CCOMM_AUDIO_TEST_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported OTA command.
 */
typedef enum
{
    I2CCOMM_CMD_AUDIO_INIT_PDM,
    I2CCOMM_CMD_AUDIO_TURN_OFF_PDM,
    I2CCOMM_CMD_AUDIO_PDM_RECORD,
    I2CCOMM_CMD_AUDIO_PDM_GETBUF,
    I2CCOMM_CMD_AUDIO_TEST_RECV_DATA_SIZE,
    I2CCOMM_CMD_AUDIO_TEST_RECV_DATA,
    I2CCOMM_CMD_AUDIO_TEST_RECOGNITION,
    I2CCOMM_CMD_AUDIO_GET_RESULT,
    I2CCOMM_CMD_AUDIO_ALANGO_PROCESS,
    I2CCOMM_CMD_AUDIO_ALANGO_DATA,
    I2CCOMM_CMD_AUDIO_SEND_DATA,
    I2CCOMM_CMD_AUDIO_OPEN_Measure,
    I2CCOMM_CMD_AUDIO_OFF_Measure,
    I2CCOMM_CMD_AUDIO_Reset,
} I2CCOMM_AUDIO_CMD_E;

/**
 * \brief this enumeration use in i2c communication library, define the supported SPI feature.
 */
#define SPI_DISABLE_ALGO     (1<<7)
#define SPI_ENABLE_PDM       (1<<5)
#define SPI_ENABLE_RAWIMG    (1<<4)
#define SPI_ALL_ON           (1<<3)
#define SPI_ENABLE_JPEG      (1<<2)
#define SPI_ENABLE_METADATA  (1<<1)
#define SPI_ENABLE_VOICE     (1<<0)
#define SPI_ENABLE_ALL       (SPI_ALL_ON | SPI_ENABLE_JPEG | SPI_ENABLE_METADATA | SPI_ENABLE_VOICE | SPI_ENABLE_RAWIMG | SPI_ENABLE_PDM)


/** @} */   //EVT_I2CCOMM_FUNCDLR

#endif /* EVENT_HANDLER_EVT_I2CCOMM_EVT_I2CCOMM_H_ */
