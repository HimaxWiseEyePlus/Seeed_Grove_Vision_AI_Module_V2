/*
 * hx_drv_swreg_aon_export.h
 *
 *  Created on: 2021/12/10
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SWREG_AON_EXPORT_H_
#define DRIVERS_INC_HX_DRV_SWREG_AON_EXPORT_H_

/**
 * \defgroup	SWREG_AON_DRV	SWREG AON Driver
 * \ingroup		SWREG_AON_DRV
 * \brief	SWREG AON Driver Declaration
 * \details Application can use SW Reg AON Structure to control Hardware\n
 */

#define SW_BOOT_OPT		0x48494D58

/**
 * \defgroup	SWREG_AON_DRV_ENUM	SWREG AON Driver Enumeration
 * \ingroup	SWREG_AON_DRV
 * \brief	Defines some enumeration of SWREG_AON Driver need.
 * @{
 */
/**
 * \enum SWREG_AON_RETENTION_E
 * \brief SWREG AON Retention
 */
typedef enum {
	SWREG_AON_NO_RETENTION = 0, /**< No Retention */
	SWREG_AON_RETENTION, /**< Retention */
} SWREG_AON_RETENTION_E;

/**
 * \enum SWREG_AON_NOTIFY_OTHERCORE_E
 * \brief SWREG AON Notify
 */
typedef enum {
	SWREG_AON_NO_NOTIFY_OTHER = 0, /**< No Notify Other */
	SWREG_AON_NOTIFY_OTHER, /**< Notify Other */
} SWREG_AON_NOTIFY_OTHERCORE_E;

/**
 * \enum SWREG_AON_PMUWAKE_CM55S_RERESET_E
 * \brief SWREG AON ReReset
 */
typedef enum {
	SWREG_AON_PMUWAKE_CM55S_RERESET_NO = 0, /**< No Reset After PMU Wakeup */
	SWREG_AON_PMUWAKE_CM55S_RERESET_YES, /**< Reset After PMU Wakeup */
} SWREG_AON_PMUWAKE_CM55S_RERESET_E;

/**
 * \enum SWREG_AON_OTA_E
 * \brief SWREG AON OTA
 */
typedef enum {
	SWREG_AON_OTA_NO_FLAG = 0, /**< Not OTA flag */
	SWREG_AON_OTA_YES_FLAG, /**< OTA flag */
} SWREG_AON_OTA_E;

/**
 * \enum SWREG_AON_MODEMOTA_E
 * \brief SWREG AON Modem OTA
 */
typedef enum {
	SWREG_AON_MODEMOTA_NO_FLAG = 0, /**< Not MODEM OTA flag */
	SWREG_AON_MODEMOTA_YES_FLAG, /**< MODEM OTA flag */
} SWREG_AON_MODEMOTA_E;

/**
 * \enum SWREG_AON_LSCCLKCHANGE_REF_E
 * \brief SWREG AON PMU Wake up LSC Clock change reference CM55S Reset flag or SW control flag
 */
typedef enum {
	SWREG_AON_LSCCLKCHANGE_REF_CM55SRESET = 0, /**< PMU Wake up LSC CLK change reference CM55S Reset Flag */
	SWREG_AON_LSCCLKCHANGE_REF_SW, /**< PMU Wake up LSC CLK change reference SW Control Flag */
} SWREG_AON_LSCCLKCHANGE_REF_E;

/**
 * \enum SWREG_AON_LSCCLKCHANGE_E
 * \brief SWREG AON PMU Wake up LSC Clock change or not
 */
typedef enum {
	SWREG_AON_LSCCLKCHANGE_YES = 0, /**< PMU Wake up LSC CLK change*/
	SWREG_AON_LSCCLKCHANGE_NO, /**< PMU Wake up LSC CLK don't change*/
} SWREG_AON_LSCCLKCHANGE_E;

/**
 * \enum SWREG_AON_SENSORINIT_E
 * \brief SWREG AON Sensor init or not
 */
typedef enum {
	SWREG_AON_SENSOR_INIT_NO = 0, /**< Sensor Setting don't init*/
	SWREG_AON_SENSOR_INIT_YES, /**< Sensor Setting init done*/
} SWREG_AON_SENSORINIT_E;

/**
 * \enum SWREG_AON_WARMBOOTDISPLL_BLCHG_E
 * \brief SWREG WarmBoot if BootROM DisPLL, BL Change frequency or not
 */
typedef enum {
	SWREG_AON_WARMBOOTDISPLL_BLCHG_NO = 0, /**< BL Use Warm Boot BOOTROM Freq*/
	SWREG_AON_WARMBOOTDISPLL_BLCHG_YES, /**< BL Use Cold boot Freq if Warm Boot BOOTROM Displl*/
} SWREG_AON_WARMBOOTDISPLL_BLCHG_E;

/**
 * \enum SWREG_AON_COREVOLT_E
 * \brief SWREG Core Voltage
 */
typedef enum {
	SWREG_AON_COREVOLT_0P9V = 0, /**< 0p9V CoreVoltage for CLK tree*/
	SWREG_AON_COREVOLT_0P8V, /**< 0p8V CoreVoltage for CLK tree*/
} SWREG_AON_COREVOLT_E;

/**
 * \enum SWREG_AON_LSCPLL_E
 * \brief SWREG LSC PLL
 */
typedef enum {
	SWREG_AON_LSCPLL_YES = 0, /**< LSC PLL if it has PLL*/
	SWREG_AON_LSCPLL_NO, /**< LSC no PLL if it has PLL*/
} SWREG_AON_LSCPLL_E;

/**
 * \enum SWREG_AON_RCTRIM_TYPE_E
 * \brief SWREG RC TRIM Type
 */
typedef enum {
	SWREG_AON_RCTRIM_RC32K = 0, /**< RC32K Trim*/
	SWREG_AON_RCTRIM_RC24M, /**< RC24M Trim*/
	SWREG_AON_RCTRIM_RC96M, /**< RC96M Trim*/
	SWREG_AON_RCTRIM_RC1K, /**< RC1K Trim*/
	SWREG_AON_RCTRIM_RC1M, /**< RC1M Trim*/
	SWREG_AON_RCTRIM_RC48M, /**< RC48M Trim*/
	SWREG_AON_RCTRIM_RC1M_P2, /**< RC1M Trim Part2*/
	SWREG_AON_RCTRIM_RC1M_P3 /**< RC1M Trim Part3*/
} SWREG_AON_RCTRIM_TYPE_E;

/**
 * \enum SWREG_AON_WARMBOOT_RELOAD_TRIM_DCVOLT_E
 * \brief SWREG Reload Trim DC Voltage or not when warm boot
 */
typedef enum {
	SWREG_AON_WARMBOOT_RELOAD_TRIM_DCVOLT_NO = 0, /**< No need Reload Trim DC Volt when WARM BOOT*/
	SWREG_AON_WARMBOOT_RELOAD_TRIM_DCVOLT_YES, /**< LSC no PLL if it has PLL*/
} SWREG_AON_WARMBOOT_RELOAD_TRIM_DCVOLT_E;
/** @} */

/**
 * \defgroup	SWREG_AON_DRV_MACRO	SW REG AON Driver MACRO
 * \ingroup	SWREG_AON_DRV
 * \brief	contains definitions of SWREG AON Driver MACRO.
 * @{
 */
/**
 * \struct SWREG_AON_RETENTION_CFG_T
 * \brief Retention flag Configuration
 */
typedef struct {
	SWREG_AON_RETENTION_E sec_mem_flag; /*!< Secure Mem Layout Retention */
	SWREG_AON_RETENTION_E first_bl_flag; /*!< First BL Retention */
	SWREG_AON_RETENTION_E cm55m_s_app_flag; /*!< cm55m_s_app RO Retention */
	SWREG_AON_RETENTION_E cm55m_ns_app_flag; /*!< cm55m_ns_app RO Retention */
	SWREG_AON_RETENTION_E cm55s_s_app_flag; /*!< cm55s_s_app RO Retention */
	SWREG_AON_RETENTION_E cm55s_ns_app_flag; /*!< cm55s_ns_app RO Retention */
	SWREG_AON_RETENTION_E cm55m_model_flag; /*!< cm55m model Retention */
	SWREG_AON_RETENTION_E cm55s_model_flag; /*!< cm55s model Retention */
	SWREG_AON_RETENTION_E cm55m_appcfg_flag; /*!< cm55m appcfg Retention */
	SWREG_AON_RETENTION_E cm55s_appcfg_flag; /*!< cm55s appcfg Retention */
	SWREG_AON_RETENTION_E cm55m_s_app_rwdata_flag;/*!< cm55m_s_app RW Data Retention */
	SWREG_AON_RETENTION_E cm55m_ns_app_rwdata_flag;/*!< cm55m_ns_app RW Data Retention */
	SWREG_AON_RETENTION_E cm55s_s_app_rwdata_flag;/*!< cm55s_s_app RW Data Retention */
	SWREG_AON_RETENTION_E cm55s_ns_app_rwdata_flag;/*!< cm55s_ns_app RW Data Retention */
	SWREG_AON_RETENTION_E secure_debug_flag;/*!< Secure Debug Retention */
	SWREG_AON_RETENTION_E others_model_flag;/*!< others_model Data Retention */
} SWREG_AON_RETENTION_CFG_T;

/**
 * \struct SWREG_AON_PMUWake_Notify_CFG_T
 * \brief PMU Wake Configuration
 */
typedef struct {
	SWREG_AON_NOTIFY_OTHERCORE_E notify_cm55s_flag; /*!< Notify CM55S */
	SWREG_AON_NOTIFY_OTHERCORE_E notify_cm55m_flag; /*!< Notify CM55M */
} SWREG_AON_PMUWake_Notify_CFG_T;
/** @} */

#endif /* DRIVERS_INC_HX_DRV_SWREG_AON_EXPORT_H_ */
