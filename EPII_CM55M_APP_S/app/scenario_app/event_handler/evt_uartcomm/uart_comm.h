/**
 ********************************************************************************************
 *  @file      uart_comm.h
 *  @details   This file contains all uart communication protocol related function
 *  @author    himax/903870
 *  @version   V1.0.0
 *  @date      27-Sep-2021
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef LIBRARY_UART_COMM_H_
#define LIBRARY_UART_COMM_H_


/****************************************************
 * Constant Definition                              *
 ***************************************************/
	
#define UARTCOMM_HEADER_SIZE     5
#define UARTCOMM_COMMAND_DATA_SIZE	4
#define UARTCOMM_PAYLOAD_SIZE    (5*1024)
#define UARTCOMM_CHECKSUM_SIZE   2
#define UARTCOMM_MAX_WBUF_SIZE   (UARTCOMM_HEADER_SIZE + UARTCOMM_PAYLOAD_SIZE + UARTCOMM_CHECKSUM_SIZE)
#define UARTCOMM_MAX_RBUF_SIZE   (UARTCOMM_HEADER_SIZE + UARTCOMM_PAYLOAD_SIZE + UARTCOMM_CHECKSUM_SIZE)
#define UARTCOMM_CMD_SIZE       (UARTCOMM_HEADER_SIZE + UARTCOMM_CHECKSUM_SIZE)

	//OTA UART PROTOCOL
	// Flag Code
#define UART_OTA_FLAG_OFFSET   0
#define UART_OTA_FLAG_SIZE     1
	
	// Feature Code
#define UART_OTA_FEATURE_OFFSET   1
#define UART_OTA_FEATURE_SIZE     1
	// Command Code
#define UART_OTA_COMMAND_OFFSET   2
#define UART_OTA_COMMAND_SIZE     1
	// Payload Length
#define UART_OTA_PAYLOADLEN_LSB_OFFSET    3
#define UART_OTA_PAYLOADLEN_MSB_OFFSET    4
#define UART_OTA_PAYLOADLEN_SIZE          2
	
	// Payload buffer
#define UART_OTA_PAYLOAD_OFFSET          5
#define UART_OTA_PAYLOAD_SIZE             UARTCOMM_PAYLOAD_SIZE


#define UART_OTA_CHECHSUM_OFFSET	8

#define UART_OTA_SYS_CMD_PAYLOAD_VER_BSP		4		// I2CCOMM_CMD_SYS_GET_VER_BSP
#define UART_OTA_SYS_CMD_PAYLOAD_VER_I2C		4		// I2CCOMM_CMD_SYS_GET_VER_I2C	

#define POLY 0x8408 /* 1021H bit reversed */

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/

/**
 * \enum UART_COMM_ERROR_E
 * \brief this enumeration use in i2c communication process, define return code of the provided function
 */
typedef enum
{
    UARTCOMM_NO_ERROR = 0,             /**< No ERROR */
    UARTCOMM_ERROR_DRIVER_FAIL,        /**< ERROR MSG: i2c driver fail */
    UARTCOMM_ERROR_NOT_INIT,           /**< ERROR MSG: not init */
    UARTCOMM_ERROR_ALREADY_INIT,       /**< ERROR MSG: already init */
    UARTCOMM_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
    UARTCOMM_ERROR_BUFFER_NOT_ASSIGN,  /**< ERROR MSG: Without assign read/write buffer */
    UARTCOMM_ERROR_DRVIER_WRITE_FAIL,  /**< ERROR MSG: I2C Slave Fail to execute i2c write */
    UARTCOMM_ERROR_DRVIER_READ_FAIL,   /**< ERROR MSG: I2C Slave Fail to execute i2c read */
    UARTCOMM_ERROR_CHECKSUM,           /**< ERROR MSG: I2C Read/Write checksum error */
    UARTCOMM_ERROR_UNKNOWN             /**< ERROR MSG: UNKNOWN ERROR*/
} UARTCOMM_ERROR_E;



/**
 * \enum UART_COMM_STATUS_E
 * \brief this enumeration use in uart communication library, define the status of uart communication process.
 */
typedef enum
{
	UARTCOMM_STATUS_UNINIT	             = 0,	/**< STATUS: Close state - the process is closed*/
	UARTCOMM_STATUS_INIT		             	    ,	/**< STATUS: Init state - the library is initialized */
	UARTCOMM_STTAUS_IDLE		                	,	/**< STATUS: Idle state - wait uart write command to process */
	UARTCOMM_STATUS_BUSY				     		,	/**< STATUS: Busy state - library is processing write or read command */
	UARTOMM_STATUS_SUSPEND     					,	/**< STATUS: Suspend state - process is been suspend*/
	UARTCOMM_STATUS_WAIT							,   /**< STATUS: Wait state - the process is waiting for read command due to previous W/R cmd */
	UARTCOMM_STATUS_UNKNOWN							/**< STATUS: Unknown status - abnormal state */
} UARTCOMM_STATUS_E;


/**
 * \enum UARTCOMM_FEATURE_E
 * \brief this enumeration use in uart communication library, define the supported feature.
 */
typedef enum
{
	UARTCOMM_FEATURE_OTA_OP	     	  	= 0x50,
	UARTCOMM_FEATURE_OTA_UPG     	  	= 0x51,	
	UARTCOMM_FEATURE_MAX
} UARTCOMM_FEATURE_E;


/**
 * \enum UARTCOMM_OTA_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported OTA command.
 */
typedef enum
{
    UARTCOMM_CMD_OTA_GET_VER         = 0x01,
    UARTCOMM_CMD_OTA_READ_CHIP_ID    = 0x02,
    UARTCOMM_CMD_OTA_READ_STATUS     = 0x03, 
    UARTCOMM_CMD_OTA_REBOOT          = 0x04,
    UARTCOMM_CMD_OTA_BUSY_PIN_SELECT = 0x05,
    UARTCOMM_CMD_OTA_READ_BUSY_PIN   = 0x06,
    UARTCOMM_CMD_OTA_JUMP2UPG        = 0x08,
    UARTCOMM_CMD_OTA_START           = 0x09,
    UARTCOMM_CMD_OTA_DATA            = 0x0A,
    UARTCOMM_CMD_OTA_END             = 0x0B,
    UARTCOMM_CMD_OTA_MAX,
} UARTCOMM_OTA_CMD_E;

#if 0
typedef enum
{
	DATA_STATUS               = 0x01,
	DATA_CHIP_ID                    ,
	DATA_PROJID					,
	DATA_VER 				,
	DATA_FLASH_DUMP 				,

}UART_TRANSFER_TYPE;
#endif

#endif /* LIBRARY_UART_COMM_H_ */
