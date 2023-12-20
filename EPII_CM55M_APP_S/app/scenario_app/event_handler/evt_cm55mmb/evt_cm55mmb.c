/**
 ********************************************************************************************
 *  @file      evt_cm55mmb.c
 *  @details   This file contains all cm55m mailbox event related function
 *  @author    himax/901912
 *  @version   V1.0.0
 *  @date      3-Jan-2022
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

/********************************************************************************************
 *  History :
 *  2022.01.03 - Initial version
 *
 *******************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "WE2_device.h"
#include "board.h"
#include "hxevent.h"
#include "event_handler_evt.h"
#include "evt_cm55mmb.h"
#include "hxevent_debug.h"
#include "hxmb.h"
#include "powermode.h"

#ifdef TRUSTZONE_SEC
#define HXMB_COPY_DST_ADDR             	BASE_ADDR_SRAM1_ALIAS
#else
#ifndef TRUSTZONE
#define HXMB_COPY_DST_ADDR             	BASE_ADDR_SRAM1_ALIAS
#else
#define HXMB_COPY_DST_ADDR             	BASE_ADDR_SRAM1
#endif
#endif


/****************************************************
 * Constant Definition                              *
 ***************************************************/


/***************************************************
 * Static Function Declaration
 **************************************************/
static void cm55mmb_callback_fun(uint8_t event);

/****************************************************
 * Variable Declaration                             *
 ***************************************************/
extern hx_event_t g_event[];
static MBLIB_CM55BIG_EVT_E g_hxmb_event = 0;


/***************************************************
 * Function Implementation
 **************************************************/
EVT_CM55MMB_ERR_E evt_cm55mmb_init(void)
{
	hx_lib_mb_cm55m_init(cm55mmb_callback_fun);

	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_init\n");

    return EVT_CM55MMB_NO_ERROR;
}

// raise MB event
static void cm55mmb_callback_fun(uint8_t event)
{
//	dbg_printf(DBG_LESS_INFO, "cm55mmb_callback_fun: 0x%08X.....trigger........\n", event);

	//fail event
	if(event == 0)
		return;

	hx_drv_mb_clear_cm55m_irq(event);

	for(uint32_t i=0;i<50;i++)
		__NOP();

	g_hxmb_event = event;

    hx_event_activate_ISR(g_event[EVT_INDEX_CM55MMB]);
}

static void cm55mPSSCLMode()
{
    xprintf("Enter PM_MODE_PS_AUD_ONLY_PREROLLING\n");

    /*
     * pmu/pmulibaudpre 0 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1
     */

	PM_PD_AUDPRE_CFG_T cfg;
	uint8_t speed,reset, retention;
	uint32_t pmu_pad_pa01_mask, pmu_rtc_mask, support_debugdump;
	uint32_t pmu_pad_pa23_mask, pmu_i2cw_mask, pmu_timer_mask, pmu_cmp_mask, pmu_ts_mask, pmu_senint_mask;

	uint32_t dcdcpin, freq, cm55mdiv, cm55sdiv, pmu_anti_mask;
	uint32_t cm55_m_rw, cm55_s_rw;

	speed = 0;
	reset = 0;
	retention = 1;
	pmu_pad_pa01_mask = 0;
	pmu_rtc_mask = 0;
	pmu_pad_pa23_mask = 0;
	pmu_i2cw_mask = 0;
	pmu_timer_mask = 0;
	pmu_cmp_mask = 0;
	pmu_ts_mask = 0;
	pmu_senint_mask = 1;
	support_debugdump = 0;

	dcdcpin = 0;
	freq = 0;
	cm55mdiv = 0;
	cm55sdiv = 0;
	pmu_anti_mask = 0;
	cm55_m_rw = 1;
	cm55_s_rw = 1;

	switch(speed)
	{
	case 0:
		cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_DISABLE;
		cfg.bootromspeed.pll_freq = 0;
		cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
		cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_1;
		break;
	default:
		cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_ENABLE;
		cfg.bootromspeed.pll_freq = freq;
		cfg.bootromspeed.cm55m_div = cm55mdiv;
		cfg.bootromspeed.cm55s_div = cm55sdiv;
		break;
	}

	cfg.cm55s_reset = reset;
	cfg.pmu_pad_pa01_mask = 0xFF;
	cfg.pmu_rtc_mask = 0xFF;
	cfg.pmu_pad_pa23_mask = 0xFF;			/**< PMU SB GPIO Interrupt Mask **/
	cfg.pmu_i2cw_mask = 0xFF;			/**< PMU I2C Wakeup Interrupt Mask **/
	cfg.pmu_timer_mask = 0xFFFF;			/**< PMU Timer0~5 Wakeup Interrupt Mask  **/
	cfg.pmu_cmp_mask = 0xFF;			/**< PMU CMP Wakeup Interrupt Mask  **/
	cfg.pmu_ts_mask = 0xFF;			/**< PMU TS Wakeup Interrupt Mask  **/
	cfg.pmu_senint_mask = 0xFF;		/**< PMU SENSOR Interrupt Mask **/
	cfg.support_debugdump = support_debugdump;

	cfg.tcm_retention = retention;			/**< CM55M TCM Retention**/
	cfg.hscsram_retention[0] = retention;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[1] = retention;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[2] = retention;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[3] = retention;	/**< HSC SRAM Retention**/
	cfg.lscsram_retention = retention;		/**< LSC SRAM Retention**/
	cfg.skip_bootflow.sec_mem_flag = retention;			/**< Skip Boot Flow**/
	cfg.skip_bootflow.first_bl_flag = retention; /*!< First BL Retention */
	cfg.skip_bootflow.cm55m_s_app_flag = retention; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_flag = retention; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_flag = retention; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_flag = retention; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.cm55m_model_flag = retention; /*!< cm55m model Retention */
	cfg.skip_bootflow.cm55s_model_flag = retention; /*!< cm55s model Retention */
	cfg.skip_bootflow.cm55m_appcfg_flag = retention; /*!< cm55m appcfg Retention */
	cfg.skip_bootflow.cm55s_appcfg_flag = retention; /*!< cm55s appcfg Retention */
	cfg.skip_bootflow.cm55m_s_app_rwdata_flag = cm55_m_rw; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = cm55_m_rw; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_rwdata_flag = cm55_s_rw; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = cm55_s_rw; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.secure_debug_flag = retention;
	cfg.pmu_anti_mask = pmu_anti_mask;
	cfg.pmu_dcdc_outpin = dcdcpin;
	hx_lib_pm_cfg_set(&cfg, NULL, PM_MODE_PS_AUD_ONLY_PREROLLING);

	SCU_LSC_CLK_CFG_T lsc_cfg;
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
	hsc_cfg.hscclk.hscclksrc = SCU_HSCCLKSRC_RC24M1M;
	hsc_cfg.hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
	hsc_cfg.hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
	hsc_cfg.i3chcdiv = SCU_HSCI3CHCLKDIV_1;
	hsc_cfg.sdiodiv = SCU_HSCSDIOCLKDIV_1;
	lsc_cfg.lscclksrc = SCU_LSCCLKSRC_RC24M1M;
	lsc_cfg.lscclkdiv = SCU_LSCCLKDIV_1;

	hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
}

uint8_t evt_cm55mmb_cb(void)
{
    dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_cb event: 0x%08X.....trigger........\n", g_hxmb_event);
    uint32_t dst_address = HXMB_COPY_DST_ADDR;
    uint32_t copy_size;

    if(g_hxmb_event == MBLIB_CM55BIG_EVT_VAD_BUF1_RDY)
    {

    	hx_lib_mb_copy_vad_data(MBLIB_MUTEX_SHAREDATA_IDX_VADBUF1, dst_address, &copy_size);
    	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_cb other MBLIB_CM55BIG_EVT_VAD_BUF1_RDY copy %d bytes\n", copy_size);
    }
    else if(g_hxmb_event == MBLIB_CM55BIG_EVT_VAD_BUF2_RDY)
    {
    	hx_lib_mb_copy_vad_data(MBLIB_MUTEX_SHAREDATA_IDX_VADBUF2, dst_address, &copy_size);
       	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_cb other MBLIB_CM55BIG_EVT_VAD_BUF2_RDY copy %d bytes\n", copy_size);
    }
    else if(g_hxmb_event == MBLIB_CM55BIG_EVT_CM55S_RDY)
    {
       	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_cb MBLIB_CM55BIG_EVT_CM55S_RDY\n");
       	cm55mPSSCLMode();
    }
    else if(g_hxmb_event == MBLIB_CM55BIG_EVT_CM55S_AUDIO_STOP)
    {
       	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_cb MBLIB_CM55BIG_EVT_CM55S_AUDIO_STOP\n");
       	cm55mPSSCLMode();
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "evt_cm55mmb_cb other event\n");
    }

    return HX_EVENT_RETURN_DONE;
}

