/*
 * hx_drv_pmu.h
 *
 *  Created on: 2019¦~1¤ë18¤é
 *      Author: 902452
 */

#ifndef PLATFORM_INC_HX_DRV_PMU_H_
#define PLATFORM_INC_HX_DRV_PMU_H_

#include "hx_drv_pmu_export.h"
/**
 * \defgroup	PMU_DRV	Power Management Driver
 * \ingroup		PMU_DRV
 * \brief	Power Management Driver Declaration
 * \details Application can use PMU driver Data Structure and Power Management library to control WE-1 Power Management\n
 */
/**
 * \defgroup	PMU_DRV_MACRO	Power Management Driver MACRO
 * \ingroup	PMU_DRV
 * \brief	contains definitions of Power Management Driver MACRO.
 * @{
 */
/** @} */

/**
 * \defgroup	PMU_DRV_ENUM	PMU Driver Enumeration
 * \ingroup	PMU_DRV
 * \brief	Defines some enumeration of PMU Driver need.
 * @{
 */


/**
 * \enum PMU_CTRL_TYPE_E
 * \brief PMU Power Driver Contrl Type
 */
typedef enum PMU_CTRL_TYPE_S
{
	/* Register 10h */
	PMU_REG_SIMO_VOUT1_SEL_N_R         		= 0x1000,
	PMU_REG_SIMO_VOUT1_SEL_N         				,
	PMU_REG_SIMO_VOUT1_SEL_R         				,
	/* Register 14h */
	PMU_REG_SIMO_VOUT2_SEL_N_R        		= 0x1400,
	PMU_REG_SIMO_VOUT2_SEL_N						,
	PMU_REG_SIMO_VOUT2_SEL_R						,
	/* Register 20h */
	PMU_REG_SIMO_CTRL		        		= 0x2000,
	PMU_REG_SIMO_BYPASS_EN_HV						,
	PMU_REG_SIMO_DUAL_EN_HV							,
	PMU_REG_SIMO_ULPSIMO_EN_HV						,
	PMU_REG_SIMO_BIAS_SEL							,
	PMU_REG_SIMO_OCP_EN								,
	PMU_REG_SIMO_OCP_SEL							,
	PMU_REG_SIMO_OSC_SEL							,
	PMU_REG_SIMO_STARTUP_PWD						,
	PMU_REG_SIMO_TEST_EN							,
	PMU_REG_SIMO_TEST_SEL							,
	PMU_REG_SIMO_TSC_PWD							,
	PMU_REG_SIMO_ULPSIMO_EN							,
	PMU_REG_SIMO_UVLO_EN							,
	PMU_REG_SIMO_UVLO_SEL							,
	PMU_REG_SIMO_ZCD_EN								,
	PMU_REG_SIMO_MODE_SEL							,
	PMU_REG_SIMO_AREG1								,
	/* Register 24h */
	PMU_REG_SIMO_VOUT12_SEL		        	= 0x2400,
	PMU_REG_SIMO_VOUT1_SEL							,
	PMU_REG_SIMO_VOUT2_SEL							,

	/* Register 30h */
	PMU_REG_ALDO_SLDO_SIMO_CTRL_SW			= 0x3000,
	PMU_REG_ALDO_ULPLDO_EN_HV_SW					,
	PMU_REG_ALDO_NLDO_EN_SW							,
	PMU_REG_ALDO_RLDO_EN_SW							,
	PMU_REG_ALDO_STARTUP_PWD_SW						,
	PMU_REG_ALDO_VOUT_SEL_SW						,
	PMU_REG_ALDO_ULPLDO_EN_SW						,
	PMU_REG_SLDO_ULPLDO_EN_HV_SW					,
	PMU_REG_SLDO_NLDO_EN_SW							,
	PMU_REG_SLDO_RLDO_EN_SW							,
	PMU_REG_SLDO_STARTUP_PWD_SW						,
	PMU_REG_SLDO_VOUT_SEL_SW						,
	PMU_REG_SLDO_ULPLDO_EN_SW						,
	PMU_REG_SIMO_ULPSIMO_EN_HV_SW					,
	PMU_REG_SIMO_MODE_SEL_SW						,
	PMU_REG_SIMO_STARTUP_PWD_SW						,
	PMU_REG_SIMO_OSC_SEL_SW							,
	PMU_REG_SIMO_VOUT1_SEL_SW						,
	PMU_REG_SIMO_VOUT2_SEL_SW						,

	/* Register 40h */
	PMU_REG_SIMO_WAKEUP_TIME_CTRL			= 0x4000,
	PMU_REG_TWAKEUP_SIMO							,
	PMU_REG_TVREF2N_SIMO							,
	PMU_REG_TSTABLE_N2R_SIMO						,
	PMU_REG_TSTABLE_R2N_SIMO						,

	/* Register 90h */
	PMU_REG_PMU_TRIG_FEATURE				= 0x9000,
	PMU_REG_CPU_EVENT_TRIGGER						,
	PMU_REG_CPU_SCENARIO							,
	PMU_REG_PRE_CAPTURED_ON							,
	PMU_REG_SC_CTL_FRAMEEND_SEL						,
	PMU_REG_SC_CTL_TRIGGER_SEL						,
	PMU_REG_RX_CTL_SEL_SW							,
	PMU_REG_RX_LPRX_EN_LN							,
	PMU_REG_RX_LN_NB								,
	PMU_REG_FASTVPR									,
	PMU_REG_SIMO_PD_OFF								,

	/* Register 94h */
	PMU_REG_PMU_FSM							= 0x9400,
	PMU_REG_FSM_ACT_DC_KEEP							,
	PMU_REG_FSM_DPD_KEEP							,
	PMU_REG_FSM_PD_KEEP								,
	PMU_REG_FSM_ACT_SCL_KEEP						,
	PMU_REG_FSM_ACT_SCH_KEEP						,
	PMU_REG_FORCE_EVENT_trig						,
	PMU_REG_FORCE_EVENT_NSTATE						,
	PMU_REG_SB_TIMER6_8_INT_MASK					,
	PMU_REG_SB_TIMER6_8_SCA_INT_MASK				,
	/* Register 98h */
	PMU_REG_PMU_EVENT_CLEAR_MASK			= 0x9800,
	PMU_REG_WAKEUP_CPU_EVENT_CLR_TRIG				,
	PMU_REG_PMU_MIPI_I2C_MST_noack_int_MASK			,
	PMU_REG_PMU_SEN_RSTN_INT_CLR					,
	PMU_REG_RTC_timer_int_MASK						,
	PMU_REG_anti_tamp_int_MASK						,
	PMU_REG_SB_timer_int_MASK						,
	PMU_REG_ADCC_int_MASK							,
	PMU_REG_TS_int_MASK								,
	PMU_REG_I2C_W_int_MASK							,
	PMU_REG_SCA_SB_timer_int_MASK					,
	PMU_REG_SCA_RTC_timer_int_MASK					,
	PMU_REG_SCA_ADCC_int_MASK						,
	PMU_REG_SCA_TS_int_MASK							,
	PMU_REG_SCA_I2C_W_int_MASK						,
	PMU_REG_SCA_ext_int_nxt_dc_MASK					,
	PMU_REG_SCA_Sensor_EXT_INT_MASK					,

	/* Register 9Ch */
	PMU_REG_PMU_MASK						= 0x9C00,
	PMU_REG_PMU_INT_MASK							,
	PMU_REG_PMU_DMA_AB_MASK							,
	PMU_REG_PMU_DMA_CFG								,
	PMU_REG_PMU_DMA_CFG_MASK						,
	PMU_REG_PMU_SEN_INT_MASK_SC						,
	PMU_REG_PMU_SEN_INT_MASK_FSM					,
	PMU_REG_PMU_GPIO_EDGE_TRIG						,
	PMU_REG_PMU_GPIO_POL							,
	PMU_PAD_AON_GPIO_0_MASK							,
	PMU_PAD_VMUTE_MASK								,
	/* Register A0h */
	PMU_REG_PMU_DELAY_TIME1					= 0xA000,
	PMU_REG_TSB_PWSW								,
	PMU_REG_TLSC_PWSW								,
	PMU_REG_THSC_PWSW								,
	PMU_REG_TM55B_PWSW								,

	/* Register A4h */
	PMU_REG_TLSC_IO_PIF_PWSW					= 0xA400,
	PMU_REG_TLSC_IO_PIFA_PWSW_N2R					,
	PMU_REG_TLSC_IO_PIFA_PWSW_R2N					,
	PMU_REG_TLSC_IO_PIFB_PWSW_N2R					,
	PMU_REG_TLSC_IO_PIFB_PWSW_R2N					,


	/* Register A8h */
	PMU_REG_PMU_WDG							= 0xA800,
	PMU_REG_LOAD_COUNTER							,
	PMU_REG_PRE_SCALE								,
	PMU_REG_LOAD_EN_1T								,
	PMU_REG_WDG_ENABLE								,
	PMU_REG_WDG_INT_MASK							,
	PMU_REG_WDG_CHIPRESET_EN						,
#if(IC_VERSION <=20)
	/* Register ACh */
	PMU_REG_PMU_ADC_WDG						= 0xAC00,
	PMU_REG_ADC_LOAD_COUNTER						,
	PMU_REG_ADC_PRE_SCALE							,
	PMU_REG_ADC_LOAD_EN_1T							,
	PMU_REG_ADC_WDG_ENABLE							,
	PMU_REG_ADC_WDG_INT_MASK						,
#else
	/* Register ACh */
	PMU_REG_TLSC_IO_SIF_PWSW					= 0xAC00,
	PMU_REG_TLSC_IO_SIFA_PWSW_N2R					,
	PMU_REG_TLSC_IO_SIFA_PWSW_R2N					,
	PMU_REG_TLSC_IO_SIFB_PWSW_N2R					,
	PMU_REG_TLSC_IO_SIFB_PWSW_R2N					,
#endif
	/* Register B0h */
	PMU_REG_PMU_SYSMEM_PGEN_RET				= 0xB000,
	PMU_REG_HSC_SYSMEM_PGEN							,
	PMU_REG_HSC_SYSMEM_RET1N						,
	PMU_REG_HSC_OMEM_PGEN							,
	PMU_REG_HSC_OMEM_RET1N							,
	PMU_REG_M55B_CMEM_PGEN							,
	PMU_REG_M55B_CMEM_RET1N							,
	PMU_REG_M55B_TMEM_PGEN							,
	PMU_REG_M55B_TMEM_RET1N							,
	PMU_REG_M55B_TMEM_RET2N							,
	PMU_REG_M55L_CMEM_PGEN							,
	PMU_REG_M55L_CMEM_RET1N							,
	PMU_REG_LSC_SYSMEM_PGEN							,
	PMU_REG_LSC_SYSMEM_RET1N						,

#if(IC_VERSION >= 30)
	/* Register B4h */
	PMU_REG_PD_LSC_IO_RET					= 0xB400,
	PMU_REG_PD_LSC_IO_PIFA_RETON					,
	PMU_REG_PD_LSC_IO_PIFA_RETOFF					,
	PMU_REG_PD_LSC_IO_PIFB_RETON					,
	PMU_REG_PD_LSC_IO_PIFB_RETOFF					,
	PMU_REG_PD_LSC_IO_SIFA_RETON					,
	PMU_REG_PD_LSC_IO_SIFA_RETOFF					,
	PMU_REG_PD_LSC_IO_SIFB_RETON					,
	PMU_REG_PD_LSC_IO_SIFB_RETOFF					,
	PMU_REG_PD_LSC_IO_PIFA_HWSW_EN					,
	PMU_REG_PD_LSC_IO_PIFB_HWSW_EN					,
	PMU_REG_PD_LSC_IO_SIFA_HWSW_EN					,
	PMU_REG_PD_LSC_IO_SIFB_HWSW_EN					,
	PMU_REG_PD_LSC_IO_PIFA_EN_SW					,
	PMU_REG_PD_LSC_IO_PIFB_EN_SW					,
	PMU_REG_PD_LSC_IO_SIFA_EN_SW					,
	PMU_REG_PD_LSC_IO_SIFB_EN_SW					,
#endif
	/* Register B8h */
	PMU_REG_PMU_SYSMEM_SW					= 0xB800,
	PMU_REG_HSC_SYSMEM_EN_SW						,
	PMU_REG_HSC_OMEM_EN_SW							,
	PMU_REG_M55B_CMEM_EN_SW							,
	PMU_REG_M55B_TMEM_EN_SW							,
	PMU_REG_M55L_CMEM_EN_SW							,
	PMU_REG_LSC_SYSMEM_EN_SW						,
	PMU_REG_HSC_SMEM_PD_RET_SEL_SW					,
	PMU_REG_LSC_SMEM_PD_RET_SEL_SW					,
	PMU_REG_M55B_TMEM_PD_RET_SEL_SW					,
#if(IC_VERSION >= 30)
	/* Register BCh */
	PMU_REG_AIP_LV_OFF_HV					= 0xBC00,
	PMU_REG_AIP_LV_OFF_HV_PMU_SW					,
	PMU_REG_AIP_LV_OFF_HV_AON_SW					,
	PMU_REG_HSC_RSTN								,
	PMU_REG_HSC_EN_SW								,
#endif
	/* Register C0h */
	PMU_REG_PMU_CLK_CTRL					= 0xC000,
	PMU_REG_PMU_CTRL_DCDC_EN						,
	PMU_REG_DCDC_EN_SW								,
	PMU_REG_DCDC_EN									,
	PMU_REG_PMU_DCDC_POL							,
	PMU_REG_OSC96M_EN_SW							,
	PMU_REG_OSC96M_EN								,
	PMU_REG_OSC24M_EN_SW							,
	PMU_REG_OSC24M_EN								,
	PMU_REG_XTAL24M_EN_SW							,
	PMU_REG_XTAL24M_EN								,
	PMU_REG_OSC32K_EN_SW							,
	PMU_REG_OSC32K_EN								,
	PMU_REG_XTAL32K_EN_SW							,
	PMU_REG_XTAL32K_EN								,

	/* Register E0h */
	PMU_pmu_wdg_cur_count					= 0xE000,

	/* Register E8h */
	PMU_pmu_status							= 0xE800,
	PMU_AON_GPIO									,
	PMU_SB_GPIO										,
	PMU_WDOGINT0									,
	PMU_WDOGINT1									,
	PMU_TIMERINT0									,
	PMU_PAD_AON_GPIO_0_STATUS						,
	PMU_PAD_VMUTE_STATUS							,
	PMU_TS_CTRL_INT									,
	PMU_DB_STATUS									,
	/* Register ECh */
	PMU_pmu_db_sel							= 0xEC00,
	/* Register F0h */
	PMU_pmu_wakeup_EVT						= 0xF000,
	/* Register F4h */
	PMU_pmu_wakeup_EVT1						= 0xF400,
} PMU_CTRL_TYPE_E;


/** @} */

/**
 * \defgroup	PMU_DRV_FUNCDLR	Power Management Driver Function Declaration
 * \ingroup	PMU_DRV
 * \brief	Contains declarations of Power Management Driver functions.
 * @{
 */

/**
 * \brief   Power Management Driver set control function
 *
 * \param[in]	aCtrl	 Control type
 * \param[in]	aValue	 Control value
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_set_ctrl(PMU_CTRL_TYPE_E aCtrl, uint32_t aValue);
/**
 * \brief   Power Management Driver get control function
 *
 * \param[in]	aCtrl	 Control type
 * \param[out]	param	 Control value
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_get_ctrl(PMU_CTRL_TYPE_E aCtrl, void* param);


#ifndef BOOTROM
/**
 * \brief   Power Management Driver Timeout Set function
 *
 * \param[in]	aType	 Timeout type
 * \param[in]	aTime	 Timeout value
 * \param[in]	aEnable	 Enable Start
 * \param[in]	chipreset_en	 Chip Reset enable
 * \param[in]	cb_fun	 Callbcak function to upper layer when interrupt occurs
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_wdt_start(PMU_TIMEOUT_E aType, uint32_t aTime, uint8_t aEnable, pmu_cb_t  cb_fun, uint8_t chipreset_en);
/**
 * \brief   Power Management Driver Timeout Stop function
 *
 * \param[in]	aType	 Timeout type
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_wdt_stop(PMU_TIMEOUT_E aType);
#endif
#ifndef BOOT_USED
/**
 * \brief   Power Management Driver after PMU wake up to all on, check PMU already done Sensor path function
 *
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_set_SenResetNClr();

/**
 * \brief   Power Management Driver query sensor control status
 *
 * \param[in]	pmu_sc_active	 sensor control active or not
 * \param[in]	pmu_sc_rstn	 sensor control resetn status
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_get_sensorctrl_status(uint8_t *pmu_sc_active, uint8_t *pmu_sc_rstn);
#endif


#if(IC_VERSION >= 30)
/**
 * \brief   Power Management Driver LSC IO Retention Configuration
 *
 * \param[in]	aGroup	 IO Group
 * \param[in]	aPolicy	 LSC IO Retention Policy
 * \param[in]	aMode	 IO Mode
 * \return	PMU_ERROR_E.
 */
PMU_ERROR_E hx_drv_pmu_set_lsc_ioret(PMU_LSC_IORET_GROUP_E aGroup, PMU_LSC_IORET_POLICY_E aPolicy, PMU_LSC_IORET_MODE_E aMode);
#endif

/** @} */

#endif /* PLATFORM_INC_HX_DRV_PMU_H_ */
