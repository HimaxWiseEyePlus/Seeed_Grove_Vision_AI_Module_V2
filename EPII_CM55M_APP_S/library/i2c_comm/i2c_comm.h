/**
 ********************************************************************************************
 *  @file      i2c_comm.h
 *  @details   This file contains all i2c communication protocol related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef LIBRARY_I2C_COMM_I2C_COMM_H_
#define LIBRARY_I2C_COMM_I2C_COMM_H_
/**
 * \defgroup    I2C_COMM    I2C Communication Library
 * \ingroup I2C_COMM
 * \brief   I2C communication library declaration
 */

#include "hx_drv_iic.h"
#include "i2c_comm_cfg.h"

/****************************************************
 * Support Feature List                             *
 ***************************************************/

/****************************************************
 * Constant Definition                              *
 ***************************************************/

/* I2C Communication Protocol : Format / Field Definition
 * [Feature] [CMD] [Payload Len] [Payload] [checksum]
 * [Feature] - one byte
 * [CMD]     - one byte
 * [PL_LEN ] - two bytes
 * [Payload] - 256bytes (MAX)
 * [checksum]- two bytes (ccitt-crc16)
 */
#define I2CCOMM_HEADER_SIZE         4       // Feature(1 bytes) + CMD(1 bytes) + PL_LEN(2 bytes)
#define I2CCOMM_MAX_PAYLOAD_SIZE    256
#define I2CCOMM_CHECKSUM_SIZE       2
#define PADDING_ALIGN_SCB_DCACHE_LINE_SIZE 26
#define I2CCOMM_MAX_WBUF_SIZE   (I2CCOMM_HEADER_SIZE + I2CCOMM_MAX_PAYLOAD_SIZE + I2CCOMM_CHECKSUM_SIZE + PADDING_ALIGN_SCB_DCACHE_LINE_SIZE)
#define I2CCOMM_MAX_RBUF_SIZE   (I2CCOMM_HEADER_SIZE + I2CCOMM_MAX_PAYLOAD_SIZE + I2CCOMM_CHECKSUM_SIZE + PADDING_ALIGN_SCB_DCACHE_LINE_SIZE)

// Feature Code
#define I2CFMT_FEATURE_OFFSET   0
#define I2CFMT_FEATURE_SIZE     1

// Command Code
#define I2CFMT_COMMAND_OFFSET   1
#define I2CFMT_COMMAND_SIZE     1

// Payload Length
#define I2CFMT_PAYLOADLEN_LSB_OFFSET    2
#define I2CFMT_PAYLOADLEN_MSB_OFFSET    3
#define I2CFMT_PAYLOADLEN_SIZE          2

// Payload buffer
#define I2CFMT_PAYLOAD_OFFSET           4

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
 * \enum I2C_COMM_ERROR_E
 * \brief this enumeration use in i2c communication process, define return code of the provided function
 */
typedef enum
{
    I2CCOMM_NO_ERROR = 0,             /**< No ERROR */
    I2CCOMM_ERROR_DRIVER_FAIL,        /**< ERROR MSG: i2c driver fail */
    I2CCOMM_ERROR_NOT_INIT,           /**< ERROR MSG: not init */
    I2CCOMM_ERROR_ALREADY_INIT,       /**< ERROR MSG: already init */
    I2CCOMM_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
    I2CCOMM_ERROR_BUFFER_NOT_ASSIGN,  /**< ERROR MSG: Without assign read/write buffer */
    I2CCOMM_ERROR_DRVIER_WRITE_FAIL,  /**< ERROR MSG: I2C Slave Fail to execute i2c write */
    I2CCOMM_ERROR_DRVIER_READ_FAIL,   /**< ERROR MSG: I2C Slave Fail to execute i2c read */
    I2CCOMM_ERROR_CHECKSUM,           /**< ERROR MSG: I2C Read/Write checksum error */
    I2CCOMM_ERROR_UNKNOWN             /**< ERROR MSG: UNKNOWN ERROR*/
} I2CCOMM_ERROR_E;

/**
 * \enum I2C_COMM_STATUS_E
 * \brief this enumeration use in i2c communication library, define the status of i2c communication process.
 */
typedef enum
{
    I2CCOMM_STATUS_CLOSE = 0, /**< STATUS: Close state - the process is closed (disconnect to i2c driver)*/
    I2CCOMM_STATUS_OPEN,      /**< STATUS: Open state - the library is opened (connect to i2c driver) */
    I2CCOMM_STATUS_INIT,      /**< STATUS: Init state - the library is initialized */
    I2CCOMM_STATUS_DEINIT,    /**< STATUS: De-init state - the library is de-initialized */
#if 0                         // [Option] - Currently, not support
    I2CCOMM_STTAUS_IDLE                         ,   /**< STATUS: Idle state - wait i2c write command to process */
    I2CCOMM_STATUS_BUSY                         ,   /**< STATUS: Busy state - library is processing write or read command */
    I2CCOMM_STATUS_SUSPEND                      ,   /**< STATUS: Suspend state - process is been suspend*/
    I2CCOMM_STATUS_WAIT                         ,   /**< STATUS: Wait state - the process is waiting for read command due to previous W/R cmd */
#endif
    I2CCOMM_STATUS_UNKNOWN /**< STATUS: Unknown status - abnormal state */
} I2CCOMM_STATUS_E;
/** @} */

/****************************************************
 * Type Definition                                 *
 ***************************************************/
/**
 * \brief I2C Communication callback function type definition
 *
 * \param[in] void * pointer to read/write header
 * \param[in] void * pointer to read/write buffer
 * \param[in] uint8_t feature code
 * \param[in] uint8_t command type
 * \param[in] uint8_t checksum value
 * \retval void.
 */
//typedef void (*i2ccomm_write_cb_t) (I2CCOMM_HEADER_T *, uint8_t*, uint8_t);
typedef void (*i2ccomm_cb_t)(void *);

/****************************************************
 * Structure Definition                             *
 ***************************************************/
/**
 * \defgroup    I2C_COMM_STRUCT     I2C Communication Library Structure
 * \ingroup I2C_COMM
 * \brief   Defines the required structure of I2C Communication library.
 * @{
 */

/**
 * \struct I2CCOMM_HEADER_T
 * \brief I2C Command Header
 */
typedef struct
{
    uint8_t fcode;       /*!< I2CCOMM Format : feature code */
    uint8_t cmd_type;    /*!< I2CCOMM Format : command type */
    uint8_t payload_len; /*!< I2CCOMM Format : payload length */
} I2CCOMM_HEADER_T;

/**
 * \struct I2CCOMM_FMT_T
 * \brief I2C Command Format
 */
typedef struct
{
    uint8_t fcode;       /*!< I2CCOMM Format : feature code */
    uint8_t cmd_type;    /*!< I2CCOMM Format : command type */
    uint8_t payload_len; /*!< I2CCOMM Format : payload length */
    uint8_t *payload;    /*!< I2CCOMM Format : payload buffer */
    uint8_t checksum;    /*!< I2CCOMM Format : checksum value */
} I2CCOMM_FMT_T;

/**
 * \struct I2CCOMM_CFG_T
 * \brief I2C Communication Configuration
 */
typedef struct
{
    unsigned int slv_addr; /*!< I2CCOMM Format : feature code */
    i2ccomm_cb_t write_cb;
    i2ccomm_cb_t read_cb;
    i2ccomm_cb_t err_cb;
    i2ccomm_cb_t sta_cb;
} I2CCOMM_CFG_T;

/** @} */ //I2C_COMM_STRUCT

/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \brief   The function use to initial the i2c communication protocol
 *
 * Initialize the i2c communication library.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_init(USE_DW_IIC_SLV_E iic_id, I2CCOMM_CFG_T aCfg);

/**
 * \brief   The function use to deinitialize the i2c communication protocol
 *
 * Deinitialize the i2c communication library.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_deinit(USE_DW_IIC_SLV_E iic_id);

/**
 * \brief   The function use to register callback function of IIC communication process.
 *
 * Register callback function.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_register_cb(USE_DW_IIC_SLV_E iic_id, i2ccomm_cb_t aReadcb, i2ccomm_cb_t aRWritecb);

/**
 * \brief   The function use to enable i2c slave read process to monitor the i2c master's write command
 *
 * Start to execute the i2c communication read process : enable to monitor the i2c write command from i2c master.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_E iic_id, unsigned char *rbuf, uint32_t size);

/**
 * \brief   The function use to enable i2c slave write process to monitor the i2c master's read command
 *
 * Start to execute the i2c communication write process : enable to monitor the i2c read command from i2c master.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_enable_write(USE_DW_IIC_SLV_E iic_id, unsigned char *wbuf);

/**
 * \brief   The function use to enable i2c slave write ALGO Result process
 *
 * Start to execute the i2c communication write algo result process
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_enable_algores_write(USE_DW_IIC_SLV_E iic_id, unsigned char *wbuf, unsigned int size);

/**
 * \brief   The function use to start the i2c communication process
 *
 * Stop the i2c communication process : start to monitor the i2c write command on i2c bus.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_start(USE_DW_IIC_SLV_E iic_id, unsigned char *rbuf, uint32_t size);

/**
 * \brief   The function use to stop the i2c communication process
 *
 * Stop the i2c communication process : suspend to monitor the i2c command on i2c bus.
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_stop(USE_DW_IIC_SLV_E iic_id);

/**
 * \brief   The function use to get the status of i2c communication process
 *
 * Get the status of i2c communication process
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_status(USE_DW_IIC_SLV_E iic_id, I2CCOMM_STATUS_E *aStatus);

/**
 * \brief   The function use to get the version of i2c communication process
 *
 * Get the version of i2c communication process
 * \retval  uint32_t version number
 */
uint32_t hx_lib_i2ccomm_version(void);

/**
 * \brief   The function use to validate that whether the checksum is correct or not.
 *
 * Process validate checksum field is correct or not
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_validate_checksum(unsigned char *aBuf);

/**
 * \brief   The function use to generate checksum with the associated buffer data.
 *
 * Process generate checksum value using ccitt_crc16
 * \retval  I2CCOMM_NO_ERROR    no error
 * \retval  others  error
 */
I2CCOMM_ERROR_E hx_lib_i2ccomm_generate_checksum(unsigned char *aBuf, unsigned int aSize, unsigned short *aCRC);

/** @} */ //I2C_COMM_FUNCDLR

#endif /* LIBRARY_I2C_COMM_I2C_COMM_H_ */
