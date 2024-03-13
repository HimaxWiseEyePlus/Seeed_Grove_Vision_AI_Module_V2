/**
 ********************************************************************************************
 *  @file      evt_reboot_api.c
 *  @details   This file contains all evt reboot related function
 *  @author    himax/903935
 *  @version   V1.0.0
 *  @date      26-Oct-2023
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

/********************************************************************************************
 *  History :
 *  2023.10.26 - Initial version
 *
 *******************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "WE2_core.h"
#include "xprintf.h"
#include "WE2_debug.h"

#include "hx_drv_swreg_aon_export.h"
#include "hx_drv_pmu_export.h"
#include "hx_drv_swreg_aon.h"

#include "powermode.h"
#include "evt_reboot_api.h"

/****************************************************
 * Constant Definition                              *
 ***************************************************/


/***************************************************
 * Function Declaration
 **************************************************/


/***************************************************
 * Static Function Declaration
 **************************************************/


/****************************************************
 * Variable Declaration                             *
 ***************************************************/

/***************************************************
 * Function Implementation
 **************************************************/
void setTimer0AlarmPMU() {
//
	TIMER_ID_E id;
	TIMER_ERROR_E ret;
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = 1;

	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_PMU;
	timer_cfg.state = TIMER_STATE_PMU;
	id = TIMER_ID_0;

	ret = hx_drv_timer_hw_start(id, &timer_cfg, NULL);

}

void setPS_PDNoVid()
{
	PM_PD_NOVIDPRE_CFG_T cfg;
	uint32_t freq;
	SCU_LSC_CLK_CFG_T lsc_cfg;
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
	PM_CFG_PWR_MODE_E mode;
	SCU_PLL_FREQ_E pmuwakeup_pll_freq;
	SCU_HSCCLKDIV_E pmuwakeup_cm55m_div;
	SCU_LSCCLKDIV_E pmuwakeup_cm55s_div;
	SCU_PLL_CLK_CFG_T pll_cfg;

	/*Get System Current Clock*/
	hx_drv_swreg_aon_get_pmuwakeup_freq(&pmuwakeup_pll_freq, &pmuwakeup_cm55m_div, &pmuwakeup_cm55s_div);
	hx_drv_swreg_aon_get_pllfreq(&freq);
	dbg_printf(DBG_MORE_INFO,"pmuwakeup_freq_type=%d, pmuwakeup_cm55m_div=%d, pmuwakeup_cm55s_div=%d\n", 
                              pmuwakeup_pll_freq, pmuwakeup_cm55m_div, pmuwakeup_cm55s_div);
	dbg_printf(DBG_MORE_INFO,"pmuwakeup_run_freq=%d\n", freq);

#ifdef SUPPORT_PRECAP
	/*Setup Sensor and Datapath*/
	app_setup_dplib();
#endif

	mode = PM_MODE_PS_NOVID_PREROLLING;
	hx_lib_pm_get_defcfg_bymode(&cfg, mode);

	/*Setup bootrom clock speed when PMU Warm boot wakeup*/
	cfg.bootromspeed.bootromclkfreq = pmuwakeup_pll_freq;
	cfg.bootromspeed.pll_freq = freq;
	cfg.bootromspeed.cm55m_div = pmuwakeup_cm55m_div;
	cfg.bootromspeed.cm55s_div = pmuwakeup_cm55s_div;

	/*Setup CM55 Small can be reset*/
	cfg.cm55s_reset = SWREG_AON_PMUWAKE_CM55S_RERESET_YES;
	/*UnMask PA01 Interrupt for PMU*/
	cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK_ALL_UNMASK;
	/*UnMask RTC Interrupt for PMU*/
	cfg.pmu_rtc_mask = PM_RTC_INT_MASK_ALLUNMASK;
	/*UnMask PA23 Interrupt for PMU*/
	cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK_ALL_UNMASK;
	/*Mask I2CWakeup Interrupt for PMU*/
	cfg.pmu_i2cw_mask = PM_IP_INT_MASK;
	/*UnMask Timer0 Interrupt others timer interrupt are mask for PMU*/
	cfg.pmu_timer_mask = 0x1FE;
	/*Mask CMP Interrupt for PMU*/
	cfg.pmu_cmp_mask = PM_IP_INT_MASK;
	/*Mask TS Interrupt for PMU*/
	cfg.pmu_ts_mask = PM_IP_INT_MASK;
	/*Mask ANTI TAMPER Interrupt for PMU*/
	cfg.pmu_anti_mask = PM_IP_INT_MASK;
	/*No Debug Dump message*/
	cfg.support_debugdump = 0;
#ifdef SUPPORT_PRECAP
#if defined(CIS_HM0360) || defined(CIS_HM11B1)
	/**< HW pin control to capture frame **/
	cfg.nframeend_ctrl = PMU_NFRAMEEND_CTRL_SC;
#endif
#if defined(CIS_HM2170)
	/**< Auto I2C control to capture frame **/
	cfg.nframeend_ctrl = PMU_NFRAMEEND_CTRL_I2C;
#endif
#endif
#ifdef SUPPORT_MEM_RETENTION
	/*Setup Memory retention*/
	cfg.tcm_retention = PM_MEM_RET_YES;			/**< CM55M TCM Retention**/
	cfg.hscsram_retention[0] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[1] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[2] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[3] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.lscsram_retention = PM_MEM_RET_YES;		/**< LSC SRAM Retention**/
	cfg.skip_bootflow.sec_mem_flag = SWREG_AON_RETENTION;			/**< Skip Boot Flow**/
	cfg.skip_bootflow.first_bl_flag = SWREG_AON_RETENTION; /*!< First BL Retention */
	cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_RETENTION; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_RETENTION; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_RETENTION; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_RETENTION; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_RETENTION; /*!< cm55m model Retention */
	cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_RETENTION; /*!< cm55s model Retention */
	cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_RETENTION; /*!< cm55m appcfg Retention */
	cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_RETENTION; /*!< cm55s appcfg Retention */
	cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_s_app RW Data Retention */
	cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_ns_app RW Data Retention */
	cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_s_app RW Data Retention */
	cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_ns_app RW Data Retention */
	cfg.skip_bootflow.secure_debug_flag = SWREG_AON_RETENTION;
#else
	/*Setup Memory no retention*/
	cfg.tcm_retention = PM_MEM_RET_NO;			/**< CM55M TCM Retention**/
	cfg.hscsram_retention[0] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[1] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[2] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[3] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.lscsram_retention = PM_MEM_RET_NO;		/**< LSC SRAM Retention**/
	cfg.skip_bootflow.sec_mem_flag = SWREG_AON_NO_RETENTION;			/**< Skip Boot Flow**/
	cfg.skip_bootflow.first_bl_flag = SWREG_AON_NO_RETENTION; /*!< First BL Retention */
	cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_NO_RETENTION; /*!< cm55m model Retention */
	cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_NO_RETENTION; /*!< cm55s model Retention */
	cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_NO_RETENTION; /*!< cm55m appcfg Retention */
	cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_NO_RETENTION; /*!< cm55s appcfg Retention */
	cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_s_app RW Data Retention */
	cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_ns_app RW Data Retention */
	cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_s_app RW Data Retention */
	cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_ns_app RW Data Retention */
	cfg.skip_bootflow.secure_debug_flag = SWREG_AON_NO_RETENTION;
#endif
#ifdef SUPPORT_PRECAP
	/**Pre-capture when boot up**/
	cfg.support_bootwithcap = PM_BOOTWITHCAP_YES;
#else
	/**No Pre-capture when boot up**/
	cfg.support_bootwithcap = PM_BOOTWITHCAP_NO;
#endif
	/*Not DCDC pin output*/
	cfg.pmu_dcdc_outpin = PM_CFG_DCDC_MODE_OFF;
	/** No Pre-capture when boot up**/
	cfg.ioret = PM_CFG_PD_IORET_ON;
#ifdef SUPPORT_PRECAP
#if defined(CIS_HM0360) || defined(CIS_HM11B1)
	/*No MIPI*/
	cfg.mipi_lane_en = PMU_MIPI_LANE_ALL_DISABLE;
#endif
#if defined(CIS_HM2170)
	/*MIPI 2lanes*/
	cfg.mipi_lane_en = PMU_MIPI_LANE_ALL_LANE_EN;
#endif
#endif
	cfg.sensor_type = PM_SENSOR_TIMING_FVLDLVLD_CON;
	/*SIMO on in PD*/
	cfg.simo_pd_onoff = PM_SIMO_PD_ONOFF_ON;

	dbg_printf(DBG_MORE_INFO,"speed=%d,reset=%d,nframeend_ctrl=%d,retention=%d,precap=%d\n", 
                             cfg.bootromspeed.bootromclkfreq, cfg.cm55s_reset, 
                             cfg.nframeend_ctrl,cfg.tcm_retention ,cfg.support_bootwithcap);
	dbg_printf(DBG_MORE_INFO,"pa01_mask=0x%x,rtc_mask=0x%x,pa23_mask=0x%x,i2cw_mask=0x%x,timer_mask=0x%x,cmp_mask=0x%x,ts_mask=0x%x\n",
                             cfg.pmu_pad_pa01_mask,cfg.pmu_rtc_mask,cfg.pmu_pad_pa23_mask,cfg.pmu_i2cw_mask,
                             cfg.pmu_timer_mask,cfg.pmu_cmp_mask,cfg.pmu_ts_mask);
	dbg_printf(DBG_MORE_INFO,"dcdcpin=%d, pmu_anti_mask=0x%x\n", cfg.pmu_dcdc_outpin, cfg.pmu_anti_mask);
	dbg_printf(DBG_MORE_INFO,"freq=%d, cm55mdiv=%d,cm55sdiv=%d\n", 
                             cfg.bootromspeed.pll_freq, cfg.bootromspeed.cm55m_div, cfg.bootromspeed.cm55s_div);

	hx_lib_pm_cfg_set(&cfg, NULL, mode);
	/*Setup Timer0 Wakeup*/
	setTimer0AlarmPMU();

	/*Use PMU lib to control HSC_CLK and LSC_CLK so set thoes parameter to 0*/
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));
	/*Trigger to PMU mode*/
	hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
}



