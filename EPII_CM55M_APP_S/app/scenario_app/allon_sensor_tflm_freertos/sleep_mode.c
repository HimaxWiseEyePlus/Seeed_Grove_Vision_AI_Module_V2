/*
 * sleep_mode.h
 *
 *  Created on: 2023/10/24
 *      Author: Himax
 */

#include "xprintf.h"
#include "hx_drv_gpio.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_timer.h"
#include "hx_drv_pmu_export.h"
#include "powermode.h"

/*******************************************************************************
 * PMU Sample Code
 ******************************************************************************/
#define MAX_STRING  100

static char wakeup_event[][MAX_STRING]={
	{"[0]: PS_DPD : wakeup by (ext_force_pmu_dc_sync)"},
	{"[1]: PS_DPD wakeup by (RTC_timer_int)"},
	{"[2]: PS_DPD wakeup by (anti_tamp_int)"},
	{"[3]: PS_PD wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)"},
	{"[4]: PS_PD wakeup by (ext_int_nxt_dc)"},
	{"[5]: PS_PD wakeup by (RTC_timer_int)"},
	{"[6]: PS_PD wakeup by (SB_timer_int)"},
	{"[7]: PS_PD wakeup by (CMP_int)"},
	{"[8]: PS_PD wakeup by (TS_int)"},
	{"[9]: PS_PD wakeup by (I2C_W_int)"},
	{"[10]: PS_PD wakeup by (REG_CPU_SCENARIO == 4'b0010 && SB_timer_int[0])"},
};

static char wakeup_event1[][MAX_STRING]={
	{"[0]: PS_DPD wakeup by (~pmu_sc_active &&PAD_AON_GPIO_0)"},
	{"[1]: PS_DPD wakeup by (~pmu_sc_active &&PAD_VMUTE)"},
	{"[2]: PS_DPD wakeup by (~pmu_sc_active && ext_int_nxt_dc)"},
	{"[8]: PS_PD wakeup by (anti_tamp_int)"},
};

static void print_wakeup_event(uint32_t event, uint32_t event1)
{

	if((event & 0x1)  != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[0]);
	}
	if((event & 0x2) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[1]);
	}
	if((event & 0x4) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[2]);
	}
	if((event & 0x8) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[3]);
	}
	if((event & 0x10) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[4]);
	}
	if((event & 0x20) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[5]);
	}
	if((event & 0x40) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[6]);
	}
	if((event & 0x80) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[7]);
	}
	if((event & 0x100) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[8]);
	}
	if((event & 0x200) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[9]);
	}
	if((event & 0x400) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[10]);
	}

	if((event1 & 0x1)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[0]);
	}
	if((event1 & 0x2)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[1]);
	}
	if((event1 & 0x4)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[2]);
	}
	if((event1 & 0x100)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[3]);
	}

}


void setCM55MTimerAlarmPMU(uint32_t timer_ms)
{
	TIMER_ERROR_E ret;
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = timer_ms;
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_PMU;
	timer_cfg.state = TIMER_STATE_PMU;

	ret = hx_drv_timer_cm55m_start(&timer_cfg, NULL);
}


void app_pmu_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention)
{
	uint8_t  gpio_value;
	uint32_t boot_cnt;
	PM_PD_NOVIDPRE_CFG_T cfg;
	uint32_t freq;
	SCU_LSC_CLK_CFG_T lsc_cfg;
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
	PM_CFG_PWR_MODE_E mode;
	SCU_PLL_FREQ_E pmuwakeup_pll_freq;
	SCU_HSCCLKDIV_E pmuwakeup_cm55m_div;
	SCU_LSCCLKDIV_E pmuwakeup_cm55s_div;

	boot_cnt = hx_get_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C);
	boot_cnt++;
	hx_set_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C, boot_cnt);
	xprintf("boot cnt= %d\r\n", boot_cnt);

	/*Clear PMU Wakeup Event*/
	hx_lib_pm_clear_event();

	/*Clear Wakeup related IP Interrupt*/
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_clr_int_status(AON_GPIO1);
	hx_drv_timer_ClearIRQ(TIMER_ID_2);

	if ( aon_gpio == 0 )
	{
		hx_drv_gpio_set_input(AON_GPIO0);
		hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH);
		hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
		hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
		hx_drv_gpio_get_in_value(AON_GPIO0, &gpio_value);
		xprintf("AON_GPIO0 = %d\n", gpio_value);
	}
	else if ( aon_gpio == 1 )
	{
		hx_drv_gpio_set_input(AON_GPIO1);
		hx_drv_gpio_set_int_type(AON_GPIO1, GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH);
		hx_drv_scu_set_PA1_pinmux(SCU_PA1_PINMUX_AON_GPIO1, 1);
		hx_drv_gpio_set_int_enable(AON_GPIO1, 1);
		hx_drv_gpio_get_in_value(AON_GPIO1, &gpio_value);
		xprintf("AON_GPIO1 = %d\n", gpio_value);
	}

	/*Get System Current Clock*/
	hx_drv_swreg_aon_get_pmuwakeup_freq(&pmuwakeup_pll_freq, &pmuwakeup_cm55m_div, &pmuwakeup_cm55s_div);
	hx_drv_swreg_aon_get_pllfreq(&freq);
	xprintf("pmuwakeup_freq_type=%d, pmuwakeup_cm55m_div=%d, pmuwakeup_cm55s_div=%d\n", pmuwakeup_pll_freq, pmuwakeup_cm55m_div, pmuwakeup_cm55s_div);
	xprintf("pmuwakeup_run_freq=%d\n", freq);

	mode = PM_MODE_PS_NOVID_PREROLLING;
	hx_lib_pm_get_defcfg_bymode(&cfg, mode);

	/*Setup bootrom clock speed when PMU Warm boot wakeup*/
	cfg.bootromspeed.bootromclkfreq = pmuwakeup_pll_freq;
	cfg.bootromspeed.pll_freq = freq;
	cfg.bootromspeed.cm55m_div = pmuwakeup_cm55m_div;
	cfg.bootromspeed.cm55s_div = pmuwakeup_cm55s_div;

	/*Setup CM55 Small can be reset*/
	cfg.cm55s_reset = SWREG_AON_PMUWAKE_CM55S_RERESET_YES;
	/*Mask RTC Interrupt for PMU*/
	cfg.pmu_rtc_mask = PM_RTC_INT_MASK_ALLMASK;
	/*Mask PA23 Interrupt for PMU*/
	cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK;
	/*Mask I2CWakeup Interrupt for PMU*/
	cfg.pmu_i2cw_mask = PM_IP_INT_MASK;
	/*Mask CMP Interrupt for PMU*/
	cfg.pmu_cmp_mask = PM_IP_INT_MASK;
	/*Mask TS Interrupt for PMU*/
	cfg.pmu_ts_mask = PM_IP_INT_MASK;
	/*Mask ANTI TAMPER Interrupt for PMU*/
	cfg.pmu_anti_mask = PM_IP_INT_MASK;
	/*No Debug Dump message*/
	cfg.support_debugdump = 0;

	/*UnMask PA01 Interrupt for PMU*/
	cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK_ALL_UNMASK;

	/*UnMask Timer2 Interrupt others timer interrupt are mask for PMU*/
	cfg.pmu_timer_mask = 0x1FB;

	if ( retention == 1 )
	{
		/*Setup Memory retention*/
		xprintf("Setup Memory retention\n");
		cfg.tcm_retention = PM_MEM_RET_YES;			/**< CM55M TCM Retention**/
		cfg.hscsram_retention[0] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.hscsram_retention[1] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.hscsram_retention[2] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.hscsram_retention[3] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.lscsram_retention = PM_MEM_RET_NO;		/**< LSC SRAM Retention**/
		cfg.skip_bootflow.sec_mem_flag = SWREG_AON_RETENTION;			/**< Skip Boot Flow**/
		cfg.skip_bootflow.first_bl_flag = SWREG_AON_RETENTION; /*!< First BL Retention */
		cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_RETENTION; /*!< cm55m_s_app Retention */
		cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_RETENTION; /*!< cm55m_ns_app Retention */
		cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_s_app Retention */
		cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_ns_app Retention */
		cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_RETENTION; /*!< cm55m model Retention */
		cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_NO_RETENTION; /*!< cm55s model Retention */
		cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_RETENTION; /*!< cm55m appcfg Retention */
		cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_NO_RETENTION; /*!< cm55s appcfg Retention */
		cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_s_app RW Data Retention */
		cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_ns_app RW Data Retention */
		cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_s_app RW Data Retention */
		cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_ns_app RW Data Retention */
		cfg.skip_bootflow.secure_debug_flag = SWREG_AON_RETENTION;
	}
	else
	{
		/*Setup Memory no retention*/
		xprintf("Setup Memory no retention\n");
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
	}

	/**No Pre-capture when boot up**/
	cfg.support_bootwithcap = PM_BOOTWITHCAP_NO;

	/*Not DCDC pin output*/
	cfg.pmu_dcdc_outpin = PM_CFG_DCDC_MODE_OFF;
	/** No Pre-capture when boot up**/
	cfg.ioret = PM_CFG_PD_IORET_ON;

	cfg.sensor_type = PM_SENSOR_TIMING_FVLDLVLD_CON;
	/*SIMO on in PD*/
	cfg.simo_pd_onoff = PM_SIMO_PD_ONOFF_ON;

#if 0	//debug message
	xprintf("speed=%d,reset=%d,nframeend_ctrl=%d,retention=%d,precap=%d\n", cfg.bootromspeed.bootromclkfreq, cfg.cm55s_reset, cfg.nframeend_ctrl,cfg.tcm_retention ,cfg.support_bootwithcap);
	xprintf("pa01_mask=0x%x,rtc_mask=0x%x,pa23_mask=0x%x,i2cw_mask=0x%x,timer_mask=0x%x,cmp_mask=0x%x,ts_mask=0x%x\n", cfg.pmu_pad_pa01_mask,cfg.pmu_rtc_mask,cfg.pmu_pad_pa23_mask,cfg.pmu_i2cw_mask,cfg.pmu_timer_mask,cfg.pmu_cmp_mask,cfg.pmu_ts_mask);
	xprintf("dcdcpin=%d, pmu_anti_mask=0x%x\n", cfg.pmu_dcdc_outpin, cfg.pmu_anti_mask);
	xprintf("freq=%d, cm55mdiv=%d,cm55sdiv=%d\n", cfg.bootromspeed.pll_freq, cfg.bootromspeed.cm55m_div, cfg.bootromspeed.cm55s_div);
#endif

	hx_lib_pm_cfg_set(&cfg, NULL, mode);

	/* Setup CM55M Timer(Timer2) Wakeup */
	setCM55MTimerAlarmPMU(timer_ms);

	/* Use PMU lib to control HSC_CLK and LSC_CLK so set those parameter to 0 */
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));

	/* Trigger to PMU mode */
	hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
}
