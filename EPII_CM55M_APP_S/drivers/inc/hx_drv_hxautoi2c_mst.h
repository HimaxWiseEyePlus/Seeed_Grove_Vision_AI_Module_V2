#ifndef _HX_DRV_HXAUTOI2C_MST_H_
#define _HX_DRV_HXAUTOI2C_MST_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	HXAUTOI2C_MST_DRV	HXAUTOI2C_MST Hardware accelerator Driver
 * \ingroup		HXAUTOI2C_MST_DRV
 * \brief	HXAUTOI2C_MST Hardware accelerator Driver Declaration
 * \details Application can use HXAUTOI2C_MST Structure and library to control Hardware\n
 * <pre>
 * Himax AUTO I2C SPEC:
 * 1. I2C CLOCK: 100K, 400K, 1M
 * 2. Max 4 I2C Command
 * 3. Max 4 data bytes for 1 I2C Command
 * 4. Source Clock 24M
 * 5. Each Command and data with delay: 16 bit counter with 1T is 41.666ns
 * For Example
 *
 * </pre>
 */
/**
 * \defgroup	HXAUTOI2C_MST_DRV_ENUM	HXAUTOI2C_MST Hardware accelerator Driver Enumeration
 * \ingroup	HXAUTOI2C_MST_DRV
 * \brief	Defines some enumeration of HXAUTOI2C_MST Hardware accelerator Driver need.
 * @{
 */
/**
 * \enum HXAUTOI2C_MST_SCL_CLK_DIV_SEL_E
 * \brief HXAUTOI2C_MST SCL Clock Select
 */
typedef enum{
    HXAUTOI2C_SCL_CLK_DIV_240, /**< 1/240 reference clk */
    HXAUTOI2C_SCL_CLK_DIV_60, /**< 1/60 reference clk */
    HXAUTOI2C_SCL_CLK_DIV_24, /**< 1/24 reference clk */
    HXAUTOI2C_SCL_CLK_DIV_4, /**< 1/4 reference clk */
}HXAUTOI2C_MST_CLK_DIV_SEL_E;

/**
 * \enum HXAUTOI2CHC_ERROR_E
 * \brief HXAUTOI2CHC Errors Type
 */
typedef enum
{
    HXAUTOI2CHC_NO_ERROR                           = 0,    /**< No ERROR */
    HXAUTOI2CHC_ERROR_NOT_INIT                     = 1,    /**< ERROR MSG: not init */
    HXAUTOI2CHC_ERROR_ALREADY_INIT                 = 2,    /**< ERROR MSG: already init */
    HXAUTOI2CHC_ERROR_INVALID_PARAMETERS           = 3,    /**< ERROR MSG: Invalid parameters */
    HXAUTOI2CHC_UNKNOWN_ERROR                      = 4,    /**< ERROR MSG: UNKNOWN ERROR*/
    HXAUTOI2CHC_ENTDAA_FAIL                        = 5,    /**< ERROR MSG: ENTDAA FAIL*/
} HXAUTOI2CHC_ERROR_E;

/**
 * \enum HXAUTOI2CHC_CMD_CTRL_E
 * \brief HXAUTOI2CHC COMMAND CONTROL Type
 */
typedef enum
{
    HXAUTOI2CHC_CMD_CTRL_BY_HW                     = 0,    /**< CMD CONTROL by HW */
    HXAUTOI2CHC_CMD_CTRL_BY_SW                     = 1,    /**< CMD CONTROL by SW */
} HXAUTOI2CHC_CMD_CTRL_E;

/**
 * \enum HXAUTOI2CHC_CMD_TYPE_E
 * \brief HXAUTOI2CHC COMMAND Type
 */
typedef enum
{
    HXAUTOI2CHC_CMD_TRIG                     = 0,    /**< CMD only TRIG*/
    HXAUTOI2CHC_CMD_TRIG_STOP                = 1,    /**< CMD TRIG + STOP*/
} HXAUTOI2CHC_CMD_TYPE_E;

/**
 * \enum HXAUTOI2CHC_INT_LOC_E
 * \brief HXAUTOI2CHC COMMAND Type
 */
typedef enum
{
    HXAUTOI2CHC_INT_LOC_BEF_DELAY01          = 0,    /**< INT before delay01 */
    HXAUTOI2CHC_INT_LOC_AFT_DELAY01          = 1,    /**< INT after delay01 */
    HXAUTOI2CHC_INT_LOC_AFT_CMD1             = 2,    /**< INT after command1 */
    HXAUTOI2CHC_INT_LOC_AFT_DELAY12          = 3,    /**< INT after delay12 */
    HXAUTOI2CHC_INT_LOC_AFT_CMD2             = 4,    /**< INT after command2 */
    HXAUTOI2CHC_INT_LOC_AFT_DELAY23          = 5,    /**< INT after delay23 */
    HXAUTOI2CHC_INT_LOC_AFT_CMD3             = 6,    /**< INT after command3 */
    HXAUTOI2CHC_INT_LOC_AFT_DELAY34          = 7,    /**< INT after delay34 */
    HXAUTOI2CHC_INT_LOC_AFT_CMD4             = 8,    /**< INT after command4 */
} HXAUTOI2CHC_INT_LOC_E;

/**
 * \enum HXAUTOI2CHC_CMD_NUM_E
 * \brief HXAUTOI2CHC COMMAND number
 */
typedef enum
{
    HXAUTOI2CHC_CMD_NUM_1          = 0,    /**< Command number 1 */
    HXAUTOI2CHC_CMD_NUM_2          = 1,    /**< Command number 2 */
    HXAUTOI2CHC_CMD_NUM_3          = 2,    /**< Command number 3 */
    HXAUTOI2CHC_CMD_NUM_4          = 3,    /**< Command number 4 */
} HXAUTOI2CHC_CMD_NUM_E;

/**
 * \enum HXAUTOI2CHC_BYTE_NUM_E
 * \brief HXAUTOI2CHC BYTE number
 */
typedef enum
{
    HXAUTOI2CHC_BYTE_NUM_1          = 0,    /**< Command number 1 */
    HXAUTOI2CHC_BYTE_NUM_2          = 1,    /**< Command number 2 */
    HXAUTOI2CHC_BYTE_NUM_3          = 2,    /**< Command number 3 */
    HXAUTOI2CHC_BYTE_NUM_4          = 3,    /**< Command number 4 */
} HXAUTOI2CHC_BYTE_NUM_E;

/**
 * \enum HXAUTOI2CHC_INT_MASK_E
 * \brief HXAUTOI2CHC INT MASK enable/disable
 */
typedef enum
{
    HXAUTOI2CHC_INT_MASK_DISEN          = 0,    /**< INT MASK disable */
    HXAUTOI2CHC_INT_MASK_EN             = 1,    /**< INT MASK enable */
} HXAUTOI2CHC_INT_MASK_E;

/**
 * \enum HXAUTOI2CHC_INTSTATUS_E
 * \brief HXAUTOI2CHC Interrupt Status
 */
typedef enum
{
    HXAUTOI2CHC_INTSTATUS_NO             = 0x0,  /**< No Interrupt Status */
    HXAUTOI2CHC_INTSTATUS_DATA_NO_ACK    = 0x1,  /**< [0] reg data no ack */
    HXAUTOI2CHC_INTSTATUS_ADDR_NO_ACK    = 0x2,  /**< [1] reg addr no ack */
} HXAUTOI2CHC_INTSTATUS_E;

/** @} */

/**
 * \defgroup    HXAUTOI2C_MST_REGSTRUCT   HXAUTOI2C_MST Driver Structure
 * \ingroup HXAUTOI2C_MST_DRV
 * \brief   contains definitions of HXAUTOI2C_MST Driver structure.
 * @{
 */
/**
 * \struct HXAUTOI2CHC_STATIC_CFG_T
 * \brief HXAUTOI2CHC static configuration by HXAUTOI2C_MST Driver
 */
typedef struct
{
    uint8_t slaveid;      /**< SLAVE ID for HXAUTOI2C*/
    HXAUTOI2C_MST_CLK_DIV_SEL_E clkdiv;  /**< CLOCK DIV for HXAUTOI2C*/
    HXAUTOI2CHC_CMD_TYPE_E cmdtype;     /**< COMMAND type*/
    HXAUTOI2CHC_CMD_CTRL_E trig_ctrl_sw;/**< TRIG CMD control switch*/
    HXAUTOI2CHC_CMD_CTRL_E stop_ctrl_sw;/**< STOP CMD control switch*/
} HXAUTOI2CHC_STATIC_CFG_T;

/**
 * \struct HXAUTOI2CHC_INT_CFG_T
 * \brief HXAUTOI2CHC interrupt configuration by HXAUTOI2C_MST Driver
 */
typedef struct
{
    HXAUTOI2CHC_INT_LOC_E trig_int_loc;      /**< TRIG INT LOC for HXAUTOI2C*/
    HXAUTOI2CHC_INT_LOC_E stop_int_loc;      /**< STOP INT LOC for HXAUTOI2C*/
    uint16_t trig_delay;    /**< TRIG DELAY for HXAUTOI2C*/
    uint16_t stop_delay;    /**< STOP DELAY for HXAUTOI2C*/
} HXAUTOI2CHC_INT_CFG_T;

/**
 * \struct HXAUTOI2CHC_CMD_CFG_T
 * \brief HXAUTOI2CHC command configuration by HXAUTOI2C_MST Driver
 */
typedef struct
{
    HXAUTOI2CHC_CMD_NUM_E cmd_num;      /**< Command bumber for HXAUTOI2C*/
    HXAUTOI2CHC_BYTE_NUM_E cmd1_byte_num;      /**< Byte1 number for HXAUTOI2C*/
    HXAUTOI2CHC_BYTE_NUM_E cmd2_byte_num;      /**< Byte1 number for HXAUTOI2C*/
    HXAUTOI2CHC_BYTE_NUM_E cmd3_byte_num;      /**< Byte1 number for HXAUTOI2C*/
    HXAUTOI2CHC_BYTE_NUM_E cmd4_byte_num;      /**< Byte1 number for HXAUTOI2C*/
    uint16_t delay01;    /**< DELAY before cmd1 for HXAUTOI2C*/
    uint16_t delay12;    /**< DELAY before cmd2 for HXAUTOI2C*/
    uint16_t delay23;    /**< DELAY before cmd3 for HXAUTOI2C*/
    uint16_t delay34;    /**< DELAY before cmd4 for HXAUTOI2C*/
} HXAUTOI2CHC_CMD_CFG_T;

/**
 * \struct HXAUTOI2CHC_CMD_T
 * \brief HXAUTOI2CHC command configuration by HXAUTOI2C_MST Driver
 */
typedef struct
{
    uint8_t byte1;    /**< Command byte1 for HXAUTOI2C*/
    uint8_t byte2;    /**< Command byte2 for HXAUTOI2C*/
    uint8_t byte3;    /**< Command byte3 for HXAUTOI2C*/
    uint8_t byte4;    /**< Command byte4 for HXAUTOI2C*/
} HXAUTOI2CHC_CMD_T;

/** @} */

/**
 * \defgroup    HXAUTOI2C_MST_DRV_FUNCDLR HXAUTOI2C_MST Driver Function Declaration
 * \ingroup HXAUTOI2C_MST_DRV
 * \brief   Contains declarations of HXAUTOI2C_MST Driver functions.
 * @{
 */
/**
 * \brief   himax auto i2c interrupt status callback function(debug purpose)
 *
 * \param[in]   event    interrupt event
 * \return  void.
 */
typedef void (*HXAUTOI2CHC_ISREvent_t) (uint8_t event);

/**
 * \brief   enable HXAUTOI2C_MST Driver
 *
 * \param[in]   reg_baseaddr    HXAUTOI2C_MST reg base address
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_init(uint32_t reg_baseaddr);

/**
 * \brief   enable HXAUTOI2C_MST Driver
 *
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_enable();

/**
 * \brief   disable HXAUTOI2C_MST Driver
 *
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_disable();

/**
 * \brief   SW TRIG start
 *
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_sw_trig_start();

/**
 * \brief   SW STOP start
 *
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_sw_stop_start();

/**
 * \brief   set HXAUTOI2C_MST static Setting
 *
 * \param[in]   cfg    HXAUTOI2C_MST static configuration
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_static_cfg(HXAUTOI2CHC_STATIC_CFG_T cfg);

/**
 * \brief   set HXAUTOI2C_MST interrupt Setting
 *
 * \param[in]   cfg    HXAUTOI2C_MST interrupt configuration
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_int_cfg(HXAUTOI2CHC_INT_CFG_T cfg);

/**
 * \brief   set HXAUTOI2C_MST TRIG Setting
 *
 * \param[in]   cfg    HXAUTOI2C_MST TRIG configuration
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_trig_cfg(HXAUTOI2CHC_CMD_CFG_T cfg);

/**
 * \brief   set HXAUTOI2C_MST STOP Setting
 *
 * \param[in]   cfg    HXAUTOI2C_MST STOP configuration
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_stop_cfg(HXAUTOI2CHC_CMD_CFG_T cfg);

/**
 * \brief   set HXAUTOI2C_MST TRIG Command 1
 *
 * \param[in]   cmd    HXAUTOI2C_MST TRIG command 1
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_trig_cmd1(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST TRIG Command 2
 *
 * \param[in]   cmd    HXAUTOI2C_MST TRIG command 2
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_trig_cmd2(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST TRIG Command 3
 *
 * \param[in]   cmd    HXAUTOI2C_MST TRIG command 3
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_trig_cmd3(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST TRIG Command 4
 *
 * \param[in]   cmd    HXAUTOI2C_MST TRIG command 4
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_trig_cmd4(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST STOP Command 1
 *
 * \param[in]   cmd    HXAUTOI2C_MST STOP command 1
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_stop_cmd1(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST STOP Command 2
 *
 * \param[in]   cmd    HXAUTOI2C_MST STOP command 2
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_stop_cmd2(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST STOP Command 3
 *
 * \param[in]   cmd    HXAUTOI2C_MST STOP command 3
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_stop_cmd3(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST STOP Command 4
 *
 * \param[in]   cmd    HXAUTOI2C_MST STOP command 4
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_stop_cmd4(HXAUTOI2CHC_CMD_T cmd);

/**
 * \brief   set HXAUTOI2C_MST TRIG interrupt mask
 *
 * \param[in]   setting    HXAUTOI2C_MST TRIG interrupt mask
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_trig_intmask(HXAUTOI2CHC_INT_MASK_E setting);

/**
 * \brief   set HXAUTOI2C_MST STOP interrupt mask
 *
 * \param[in]   setting    HXAUTOI2C_MST STOP interrupt mask
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_set_stop_intmask(HXAUTOI2CHC_INT_MASK_E setting);

/**
 * \brief   get HXAUTOI2C_MST interrupt masked status
 *
 * \return  interrupt masked status
 */
uint8_t hx_drv_hxautoi2c_get_maskedstatus();

/**
 * \brief   get HXAUTOI2C_MST interrupt raw status
 *
 * \return  interrupt raw status
 */
uint8_t hx_drv_hxautoi2c_get_rawstatus();

/**
 * \brief   get HXAUTOI2C_MST FSM status
 *
 * \return  FSM status
 */
uint8_t hx_drv_hxautoi2c_get_fsm_status();

/**
 * \brief   get HXAUTOI2C_MST noack FSM status
 *
 * \return  interrupt raw status
 */
uint8_t hx_drv_hxautoi2c_get_noackstatus();

/**
 * \brief   set HXAUTOI2C_MST interrupt callback
 *
 * \param[in]   cb_event    HXAUTOI2C_MST callback function
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_register_cb(HXAUTOI2CHC_ISREvent_t cb_event);

/**
 * \brief   clear HXAUTOI2C_MST interrupt status
 *
 * \param[in]   intval    HXAUTOI2C_MST interrrupt status
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_clr_noack_int(HXAUTOI2CHC_INTSTATUS_E intval);

/**
 * \brief   set HXAUTOI2C_MST Trigger CMD Done interrupt callback
 *
 * \param[in]   cb_event    HXAUTOI2C_MST callback function
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_trigcmd_done_register_cb(HXAUTOI2CHC_ISREvent_t cb_event);

/**
 * \brief   set HXAUTOI2C_MST Stop CMD Done interrupt callback
 *
 * \param[in]   cb_event    HXAUTOI2C_MST callback function
 * \return  HXAUTOI2CHC_ERROR_E
 */
HXAUTOI2CHC_ERROR_E hx_drv_hxautoi2c_stopcmd_done_register_cb(HXAUTOI2CHC_ISREvent_t cb_event);

/** @} */
#endif
