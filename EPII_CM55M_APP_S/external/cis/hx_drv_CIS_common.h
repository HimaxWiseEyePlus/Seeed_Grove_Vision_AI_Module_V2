/*
 * hx_drv_CIS_common.h
 *
 *  Created on: 2018/10/2
 *      Author: 902447
 */

#ifndef HX_DRV_CIS_COMMON_H_
#define HX_DRV_CIS_COMMON_H_

#include <stdint.h>
#include "WE2_device.h"
#include "board.h"
#include "BITOPS.h"
#include "hx_drv_dp.h"
#include "hx_drv_sensorctrl.h"
#include "hx_drv_iic.h"

/**
 * \defgroup	COMMON_SENSOR_DRV	Common Sensor Driver
 * \ingroup	COMMON_SENSOR_DRV
 * \brief	Common Sensor Driver Declaration
 * \details Application can use common sensor driver API to control Sensor by application\n
 * <pre>
 * Step1: Setting WE-1 xShutdown pin and MCLK Frequency for CIS to initial CIS
 * 		hx_drv_cis_init(CIS_XHSHUTDOWN_INDEX_E xshutdown_pin, SENSORCTRL_MCLK_E mclk_div);
 * Step2: Setting CIS any configuration by I2C Master
 * 		hx_drv_cis_setRegTable(HX_CIS_SensorSetting_t *pSensorSetting, uint16_t Length);
 * </pre>
 */
/**
 * \defgroup	COMMON_SENSOR_DRV_ENUM	Common Sensor Driver Enumeration
 * \ingroup	COMMON_SENSOR_DRV
 * \brief	Defines some enumeration of Common Sensor Driver need.
 * @{
 */
/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \enum HX_CIS_ERROR_E
 * \brief HX CIS Errors Type
 */
typedef enum {
	HX_CIS_NO_ERROR = 0, /**< No ERROR */
	HX_CIS_ERROR_INVALID_PARAMETERS = 1, /**< ERROR MSG: Invalid parameters */
	HX_CIS_ERROR_I2C = 2, /**< ERROR MSG: I2C Fail */
	HX_CIS_ERROR_NOT_SUPPORT = 3, /**< ERROR MSG: Not Support */
	HX_CIS_UNKNOWN_ERROR = 4, /**< ERROR MSG: UNKNOWN ERROR*/
	HX_CIS_I2C_ADDR_NO_ACK = -60, /**< ERROR MSG: I2C ADDRESS NO ACK */
	HX_CIS_I2C_DATA_NO_ACK = -61, /**< ERROR MSG: I2C DATA NO ACK */
	HX_CIS_I2C_LOST_BUS = -64 /**< ERROR MSG: I2C LOST BUS */
} HX_CIS_ERROR_E;

/**
 * \enum HX_CIS_I2C_ActionType_e
 * \brief CIS Sensor I2C Master Action Write, Read, Delay
 */
typedef enum EMBARC_PACKED {
	HX_CIS_I2C_Action_W = 0, /**< action I2C Master Write */
	HX_CIS_I2C_Action_R, /**< action I2C Master Read */
	HX_CIS_I2C_Action_S, /**< action CPU Delay ms, wait */
	HX_CIS_I2C_Action_W_1Byte_Reg, /**< action I2C Master Write (1 Byte Register) */
	HX_CIS_I2C_Action_R_1Byte_Reg, /**< action I2C Master Read (1 Byte Register) */
} HX_CIS_I2C_ActionType_e;

typedef enum {
	CIS_XHSUTDOWN_IOMUX_SGPIO0,
	CIS_XHSUTDOWN_IOMUX_SGPIO1,
	CIS_XHSUTDOWN_IOMUX_SGPIO2,
	CIS_XHSUTDOWN_IOMUX_SGPIO3,
	CIS_XHSUTDOWN_IOMUX_NONE = 0xFF
} CIS_XHSHUTDOWN_INDEX_E;
/** @} */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup	COMMON_SENSOR_DRV_REGSTRUCT		Common Sensor Driver Structure
 * \ingroup	COMMON_SENSOR_DRV
 * \brief	contains definitions of Common Sensor Driver structure.
 * @{
 */
/**
 * \struct HX_CIS_SensorSetting_t
 * \brief CIS Sensor I2C Master Setting by Common Sensor Driver. You can use this structure to create table for all Sensor configuration
 */
typedef __PACKED_STRUCT {
	HX_CIS_I2C_ActionType_e I2C_ActionType;/**< CIS Sensor I2C Master Action Write, Read, Delay */
	uint16_t RegAddree; /**< register address for I2C Master if I2C_ActionType == HX_CIS_I2C_Action_S, RegAddree = delay msec */
	uint8_t Value;/**< register value for I2C Master if I2C_ActionType == HX_CIS_I2C_Action_S, don't (care fill 0) */
} HX_CIS_SensorSetting_t;
/** @} */

/**
 * \defgroup	COMMON_SENSOR_DRV_MACRO	Common Sensor Driver MACRO
 * \ingroup	COMMON_SENSOR_DRV
 * \brief	contains definitions of Common Sensor Driver MACRO.
 * @{
 */
#define HX_CIS_SIZE_N(p,type) sizeof(p) / sizeof(type)  /**< Sensor configuration table count */

#define HX_CIS_I2C_RETRY_TIME  3  /**< If I2C Master set fail, maximum retry time for sensor setting*/
#define HX_CIS_I2C_ADDR_BYTE   2  /**< I2C Master Register address length*/
#define HX_CIS_I2C_DATA_BYTE   1  /**< I2C Master Register value length*/
#define HX_CIS_SLAVE_ID        0x24	/**< Default CIS Common Driver I2C-M Slave ID*/
#define HX_CIS_IIC_M_ID        USE_DW_IIC_1	/**< WE-1 I2C Master ID for CIS*/
#define HX_CIS_CMU_REG_ADDR    0x0104	/**< CMU update register address for HII Sensor*/
/** @} */


/**
 * \brief Common Sensor Driver Initialize. xShutdown 0--> 1 and MCLK output if sensor needs WE-1 output MCLK
 *
 * \param[in] xShutdown 0--> 1 and MCLK output if sensor needs WE-1 output MCLK
 * \param[in] mclk_div (deprecated), to change mclk divider, use API hx_drv_scu_set_pdlsc_dpclk_cfg() instead
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_init(CIS_XHSHUTDOWN_INDEX_E xshutdown_pin,
		uint8_t mclk_div);

/**
 * \brief Set Slave ID for common CIS Sensor. If slave id is not default, you can use this API to change.
 *
 * \param[in] slave_id I2C Master Slave ID for CIS
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_set_slaveID(uint8_t slave_id);
/**
 * \brief Get Current Slave ID for common CIS Sensor.
 *
 * \param[in] slave_id I2C Master Slave ID for CIS
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_get_slaveID(uint8_t *slave_id);
/**
 * \brief Set Configuration to CIS Sensor by I2C Master.
 *
 * \param[in] pSensorSetting Configuration table for CIS Sensor
 * \param[in] Length Configuration table Length for CIS Sensor
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_setRegTable(HX_CIS_SensorSetting_t *pSensorSetting,
		uint16_t Length);
/**
 * \brief Get Configuration to CIS Sensor by I2C Master.
 *
 * \param[in] pSensorSetting Configuration table for CIS Sensor
 * \param[in] Length Configuration table Length for CIS Sensor
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_getRegTable(HX_CIS_SensorSetting_t *pSensorSetting,
		uint16_t Length);
/**
 * \brief Set Register value to CIS Sensor by I2C Master.
 *
 * \param[in] addr Register Address for CIS Sensor
 * \param[in] val Register value for CIS Sensor
 * \param[in] cmu_update After register setting, it need CMU update to Sensor or not
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_set_reg(uint16_t addr, uint8_t val,
		uint8_t cmu_update);
/**
 * \brief Set Register value to CIS Sensor by I2C Master.
 *
 * \param[in] addr Register Address for CIS Sensor (1 Byte Reg Addr)
 * \param[in] val Register value for CIS Sensor
 * \param[in] cmu_update After register setting, it need CMU update to Sensor or not
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_set_reg_1byte(uint8_t addr, uint8_t val,
		uint8_t cmu_update);
/**
 * \brief Get Current Register value from CIS Sensor by I2C Master.
 *
 * \param[in] addr Register Address for CIS Sensor
 * \param[out] val Current Register value from CIS Sensor
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_get_reg(uint16_t addr, uint8_t *val);
/**
 * \brief Get Current Register value from CIS Sensor by I2C Master.
 *
 * \param[in] addr Register Address for CIS Sensor (1 Byte Reg Addr)
 * \param[out] val Current Register value from CIS Sensor
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_get_reg_1byte(uint8_t addr, uint8_t *val);
/**
 * \brief Set CMU update to CIS Sensor by I2C Master.
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_CMU_Update(void);
/**
 * \brief Toggled XSHUTDOWN to Reset SENSOR.
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_xshutdown_rst(void);
/**
 * \brief Toggled XSHUTDOWN to High.
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_xshutdown_high(void);
/**
 * \brief Toggled XSHUTDOWN to Low.
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_xshutdown_low(void);
/**
 * \brief Enable SOI sensor init flow
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_enable_soi_init_flow(void);
/**
 * \brief Disable SOI sensor init flow
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_disable_soi_init_flow(void);
/**
 * \brief Change Xsleep default value
 *
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_set_xsleep_def_val(uint8_t val);
/** @} */
#ifdef __cplusplus
}
#endif
#endif /* HX_DRV_CIS_COMMON_H_ */
