/**
 * \defgroup	DEVICE_HAL_COMMON	Common Device Layer Definitions
 * \ingroup	DEVICE_HAL_DEF
 * \brief	common definitions for device layer (\ref dev_common.h)
 *
 * @{
 *
 * \file
 * \brief	header file to define common definitions for device layer
 * \details	Here in this file provide definitions that need by other
 * 	devices in device layer
 */

#ifndef _DEVICE_HAL_COMMON_H_
#define _DEVICE_HAL_COMMON_H_

#include <stdint.h>
#include <limits.h>	/* C99 standard lib */
#include <stddef.h>	/* C99 standard lib */
#include <stdbool.h> 	/* C99 standard lib */
#include "WE2_device.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef TRUSTZONE_SEC
#define FLASH1_BASE        		BASE_ADDR_FLASH1_R_ALIAS
#define SRAM2_BASE        			BASE_ADDR_SRAM2_ALIAS
#define SRAM1_BASE        			BASE_ADDR_SRAM1_ALIAS
#define SRAM0_BASE        			BASE_ADDR_SRAM0_ALIAS
#define DTCM_BASE					BASE_ADDR_DTCM_ALIAS
#define ITCM_BASE					BASE_ADDR_ITCM_ALIAS
#else
#ifndef TRUSTZONE
#define FLASH1_BASE        		BASE_ADDR_FLASH1_R_ALIAS
#define SRAM2_BASE        			BASE_ADDR_SRAM2_ALIAS
#define SRAM1_BASE        			BASE_ADDR_SRAM1_ALIAS
#define SRAM0_BASE        			BASE_ADDR_SRAM0_ALIAS
#define DTCM_BASE					BASE_ADDR_DTCM_ALIAS
#define ITCM_BASE					BASE_ADDR_ITCM_ALIAS
#else
#define FLASH1_BASE        		BASE_ADDR_FLASH1_R
#define SRAM2_BASE        			BASE_ADDR_SRAM2
#define SRAM1_BASE        			BASE_ADDR_SRAM1
#define SRAM0_BASE        			BASE_ADDR_SRAM0
#define DTCM_BASE					BASE_ADDR_DTCM
#define ITCM_BASE					BASE_ADDR_ITCM
#endif
#endif

//#define SRAM0_SIZE					(0x00100000)
//#define SRAM1_SIZE					(0x00180000)
//#define SRAM2_SIZE					(0x00200000)
#define DTCM_SIZE					(0x00040000)
#define ITCM_SIZE					(0x00040000)
#define FLASH1_SIZE                 (0x01000000)

/*
 * macros for device open and close state
 */
#define DEV_CLOSED              (0) /*!< Indicate that device was closed */
#define DEV_OPENED              (1) /*!< Indicate that the device was opened */

/*
 * macros for device good and error state
 */
#define DEV_GOOD                (0) /*!< Indicate device is good */
#define DEV_ERROR               (1) /*!< Indicate device error */
/** @} */

/*
 * macros for device working method
 */
#define DEV_POLL_METHOD         (0) /*!< Indicate that the device running in poll method */
#define DEV_INTERRUPT_METHOD    (1) /*!< Indicate that the device running in interrupt method */
#define DEV_DMA_METHOD          (2) /*!< Indicate that the device running in DMA method */
/** @} */
/**
 * \name	Main Error Code Definitions
 * @{
 */
#define E_OK                   (0)         /*!< ok */
#define E_SYS                  (-5)        /*!< system error */
#define E_OPNED                (-6)        /*!< device is opened */
#define E_CLSED                (-7)        /*!< device is closed */
#define E_NOSPT                (-9)        /*!< unsupported features */
#define E_RSFN                 (-10)       /*!< reserved function code */
#define E_RSATR                (-11)       /*!< reserved attribute */
#define E_PAR                  (-17)       /*!< parameter error */
#define E_ID                   (-18)       /*!< invalid ID number */
#define E_CTX                  (-25)       /*!< context error */
#define E_MACV                 (-26)       /*!< memory access violation */
#define E_OACV                 (-27)       /*!< object access violation */
#define E_ILUSE                (-28)       /*!< illegal service call use */
#define E_NOMEM                (-33)       /*!< insufficient memory */
#define E_NOID                 (-34)       /*!< no ID number available */
#define E_NORES                (-35)       /*!< no resource available */
#define E_OBJ                  (-41)       /*!< object state error */
#define E_NOEXS                (-42)       /*!< non-existent object */
#define E_QOVR                 (-43)       /*!< queue overflow */
#define E_RLWAI                (-49)       /*!< forced release from waiting */
#define E_TMOUT                (-50)       /*!< polling failure or timeout */
#define E_DLT                  (-51)       /*!< waiting object deleted */
#define E_CLS                  (-52)       /*!< waiting object state changed */
#define E_WBLK                 (-57)       /*!< non-blocking accepted */
#define E_BOVR                 (-58)       /*!< buffer overflow */
#define E_DEV_I2C_ADDR_NO_ACK  (-60)       /*!< Slave address is sent but not addressed by any slave devices */
#define E_DEV_I2C_DATA_NO_ACK  (-61)       /*!< Data in transfer is not acked when it should be acked */
#define E_DEV_I2C_TIMEOUT      (-62)       /*!< Timeout, no more data is received or sent */
#define E_DEV_I2C_MSTSTOP      (-63)       /*!< Slave received a STOP condition from master device */
#define E_DEV_I2C_LOST_BUS     (-64)       /*!< Master or Slave-Transmitter lost arbitration */

/** @} end of name */

#define Inline	static inline

/**
 * DesignWare UART driver macros used in uart driver
 */
#define _epii_rarely(a)				__builtin_expect((int)(a), 0)
/**
 * \brief   check an expression to see if it is right, and when error
 * 	set the ercd, and goto exit_label
 * \param   EXPR	the expression that need to be checked (==0 failed)
 * \param   ERCD	MUST pass a variable to here to get the error code
 * \param   ERROR_CODE error code that pass to ERCD
 * \param   EXIT_LABEL a label to go when error happens
 */
#define CHECK_EXP(EXPR, ERCD, ERROR_CODE, EXIT_LABEL) 	{	\
		if (_epii_rarely(!(EXPR))) { 			\
			ERCD = (ERROR_CODE);			\
			goto EXIT_LABEL;			\
		}						\
	}

/** check cnt bytes align, 1 for aligned, 0 for not-aligned */
#define CHECK_ALIGN_BYTES(pointer, cnt)		((((uint32_t)(pointer)) & (cnt-1)) == 0)
/** check 2 bytes align, 1 for aligned, 0 for not-aligned */
#define CHECK_ALIGN_2BYTES(pointer)		((((uint32_t)(pointer)) & 0x1) == 0)
/** check 4 bytes align, 1 for aligned, 0 for not-aligned */
#define CHECK_ALIGN_4BYTES(pointer)		((((uint32_t)(pointer)) & 0x3) == 0)
/** check 8 bytes align, 1 for aligned, 0 for not-aligned */
#define CHECK_ALIGN_8BYTES(pointer)		((((uint32_t)(pointer)) & 0x7) == 0)

/**
 * \defgroup	DEVICE_HAL_COMMON_DEVMODE	Common Device Working Mode
 * \ingroup	DEVICE_HAL_COMMON
 * \brief	definitions for device working mode(master or slave)
 * \details	here defines macros for working mode,
 * 	Master or Slave mode,used in
 * 	\ref DEV_HAL_IIC, \ref DEV_HAL_SPI.
 * @{
 */
/*
 * macros for device working mode
 */
#define DEV_MASTER_MODE				(0)	/*!< Indicate that the device working as master */
#define DEV_SLAVE_MODE				(1)	/*!< Indicate that the device working as slave */
/** @} */

/**
 * \defgroup	DEVICE_HAL_COMMON_DEVSTATUS	Common Device Status
 * \ingroup	DEVICE_HAL_COMMON
 * \brief	definitions for device status, 1 bit for 1 function
 * @{
 */
#define DEV_DISABLED				(0)		/*!< Bit 0 for device enabled state, disabled */
#define DEV_ENABLED					(1<<0)	/*!< Bit 0 for device enabled state, enabled */
#define DEV_IN_TX					(1<<1)	/*!< Bit 1 for device in transmit state */
#define DEV_IN_RX					(1<<2)	/*!< Bit 2 for device in receive state */
#define DEV_IN_XFER					(1<<3)	/*!< Bit 3 for device in transfer state */
#define DEV_IN_TX_ABRT				(1<<4)	/*!< Bit 4 for device in transmit abort state */
#define DEV_IN_RX_ABRT				(1<<5)	/*!< Bit 5 for device in receive abort state */
#define DEV_IN_XFER_ABRT			(1<<6)	/*!< Bit 6 for device in transfer abort state */

#define DEV_INTNO_INVALID		(0xFFFFFFFF)

/**
 * \defgroup	DEVICE_HAL_COMMON_DEFCMD	Common Device Defining Command
 * \ingroup	DEVICE_HAL_COMMON
 * \brief	definitions for defining command code
 * \details	here defines macros to define command code,
 * 	in system code, use \ref DEV_SET_SYSCMD to define command code.
 * 	in user code, use \ref DEV_SET_USRCMD to define command code.
 * 	So that there will be no conflicts in system and user defined command code.
 * 	this used used in
 * 	\ref DEVICE_HAL_UART, \ref DEVICE_HAL_SPI,
 * 	\ref DEVICE_HAL_IIC, \ref DEVICE_HAL_GPIO,
 *     	and in user code
 * @{
 */
/*
 * macros for control command base
 */
#define DEV_SYS_CMDBSE				(0x00000000)		/*!< default system device control command base(defined by embARC) */
#define DEV_USR_CMDBSE				(0x80000000)		/*!< default user device control command base(defined by user) in user implementing */
#define DEV_SET_SYSCMD(cmd)			(DEV_SYS_CMDBSE|(cmd))	/*!< set device system control command */
#define DEV_SET_USRCMD(cmd)			(DEV_USR_CMDBSE|(cmd))	/*!< set device user control command */

#define CONV2VOID(param)			((void *)(param))	/*!< convert param into void * type */
/**
 * Common Device Buffer Structure
 */
typedef struct dev_buffer {
	void *buf;	/*!< buffer pointer */
	uint32_t len;	/*!< buffer length in bytes */
	uint32_t ofs;	/*!< current offset in buffer */
} DEV_BUFFER;

/** Init device buffer */
#define DEV_BUFFER_INIT(devbuf, buffer, size)   {       \
                    (devbuf)->buf = (void *)(buffer);   \
                    (devbuf)->len = (uint32_t)(size);   \
                    (devbuf)->ofs = (uint32_t)(0);      \
                }
				
/**
 *   Device callback function typedef.
 * This is usually used in device callback settings,
 * and \ptr should be the device object pointer,
 * such as DEV_IIC * */
typedef void (*DEV_CALLBACK) (void *ptr);

typedef void (*INT_HANDLER) (void *ptr);

#ifdef __cplusplus
}
#endif
#endif /* _DEVICE_HAL_COMMON_H_ */
