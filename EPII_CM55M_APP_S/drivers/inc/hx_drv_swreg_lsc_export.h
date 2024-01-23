/*
 * hx_drv_swreg_lsc_export.h
 *
 *  Created on: 2023¦~12¤ë13¤é
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SWREG_LSC_EXPORT_H_
#define DRIVERS_INC_HX_DRV_SWREG_LSC_EXPORT_H_

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
 * \enum SW_FORCE_WAKEUPEVENT_E
 * \brief SW Trigger wakeup to all on event
 */
typedef enum SW_FORCE_WAKEUPEVENT_S
{
	PMU_SW_WAKEUP_NONE 					= 0x0,
	PMU_SW_WAKEUP_SCL_AUDONLY_SW_TRIG	= 0x1,	//[0]: PS_SCL wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)
	PMU_SW_WAKEUP_SCL_SENEXTINT			= 0x2,	//[1]: PS_SCL wakeup by (~pmu_sc_active && Sensor_EXT_INT)
	PMU_SW_WAKEUP_SCL_EXTDC				= 0x4,	//[2]: PS_SCL wakeup by (ext_int_nxt_dc)
	PMU_SW_WAKEUP_SCL_RTC_TIMER_INT		= 0x8,   //[3]: PS_SCL wakeup by (RTC_timer_int)
	PMU_SW_WAKEUP_SCL_SB_TIMER_INT		= 0x10,	//[4]: PS_SCL wakeup by (SB_timer_int[5:1](VerA))(|SB_timer_int[8:1](VerB))
	PMU_SW_WAKEUP_SCL_ADCC_int			= 0x20,	//[5]: PS_SCL wakeup by (ADCC_int)
	PMU_SW_WAKEUP_SCL_TS_INT			= 0x40,	//[6]: PS_SCL wakeup by (TS_int)
	PMU_SW_WAKEUP_SCL_I2C_INT				= 0x80, 	//[7]: PS_SCL wakeup by (I2C_W_int)

	PMU_SW_WAKEUP_SCH_VIDAUD_SW_TRIG 		= 0x100,  //[8]: PS_SCH wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)
	PMU_SW_WAKEUP_SCH_ext_int_nxt_dc 		= 0x200,  //[9]: PS_SCH wakeup by (ext_int_nxt_dc)
	PMU_SW_WAKEUP_SCH_RTC_TIMER_INT 		= 0x400,  //[10]: PS_SCH wakeup by (RTC_timer_int)
	PMU_SW_WAKEUP_SCH_SB_TIMER_INT 			= 0x800,  //[11]: PS_SCH wakeup by (|SB_timer_int[5:1](VerA))(|SB_timer_int[8:1](VerB))
	PMU_SW_WAKEUP_SCH_ADCC_int				= 0x1000,	//[12]: PS_SCH wakeup by (ADCC_int)
	PMU_SW_WAKEUP_SCH_TS_INT				= 0x2000,	//[13]: PS_SCH wakeup by (TS_int)
	PMU_SW_WAKEUP_SCH_I2C_INT				= 0x4000, 	//[14]: PS_SCH wakeup by (I2C_W_int)
	PMU_SW_WAKEUP_SCH_PMUWDG_TIMEOUT		= 0x8000,  //[!5]: PS_SCH wakeup by (pmu_wdg_timeout_int)
	PMU_SW_WAKEUP_SCH_XDMA_ABNORMAL			= 0x10000,  //[16]: PS_SCH wakeup by (WRDMAx_abnormal_int)
	PMU_SW_WAKEUP_SCH_DP_ABNORMAL			= 0x20000,  //[!7]: PS_SCH wakeup by (DP_ABNORMAL_INT)
	PMU_SW_WAKEUP_SCH_CDM					= 0x40000,  //[18]: PS_SCH wakeup by (SC_sen_nframe_end && all_cfg_xDMAx_int && DP_CDM_MOTION_INT)
	PMU_SW_WAKEUP_SCL_ANTI_TAMP_INT 		= 0x80000,  	//[20]: PS_SCL wakeup by (anti_tamp_int)
	PMU_SW_WAKEUP_SCH_I2CNOACK 				= 0x100000,  	//[21]: PS_SCH wakeup by (SC_sen_noack)
	PMU_SW_WAKEUP_SCH_ANTI_TAMP_INT 		= 0x200000,  	//[22]: PS_SCH wakeup by (anti_tamp_int)
}SW_FORCE_WAKEUPEVENT_E;
/** @} */

/**
 * \defgroup	SWREG_LSC_DRV_MACRO	SW REG LSC Driver MACRO
 * \ingroup	SWREG_LSC_DRV
 * \brief	contains definitions of SWREG LSC Driver MACRO.
 * @{
 */

/** @} */

#endif /* DRIVERS_INC_HX_DRV_SWREG_LSC_EXPORT_H_ */
