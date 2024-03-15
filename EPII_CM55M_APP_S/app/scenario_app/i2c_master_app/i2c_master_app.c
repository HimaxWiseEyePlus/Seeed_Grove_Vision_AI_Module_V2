#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
//#include "veneer_table.h"
//
#endif
#endif

#include "xprintf.h"
#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "board.h"
#include "hx_drv_scu.h"
#include "hx_drv_gpio.h"
#include "hx_drv_timer.h"
#include "hx_drv_pmu_export.h"
#include "powermode.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_iic.h"

void setCM55MTimerAlarmPMU(uint32_t timer_ms);

/**
 * This function sets TIMER ID 2 or AON_GPIO0(PA0)/AON_GPIO1(PA1) as Power Down mode wake up sources.
 * This function supports memory retention or memory no retention.
 *
 * @param timer_ms The wake up timer(millisecond).
 * @param aon_gpio The index of the AON_GPIO pin to set as wake up source.
 * 				   0: AON_GPIO0(PA0), 1: AON_GPIO1(PA1), 0xFF: disable aon_gpio wake up.
 * @param retention Memory retention or not.
 *
 */
void app_pmu_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention);


/*!
 * @brief Main function
 */
int app_main(void) {

	xprintf("Start I2C Master App\n");

	i2cm_test();

	return 0;
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

	xprintf("speed=%d,reset=%d,nframeend_ctrl=%d,retention=%d,precap=%d\n", cfg.bootromspeed.bootromclkfreq, cfg.cm55s_reset, cfg.nframeend_ctrl,cfg.tcm_retention ,cfg.support_bootwithcap);
	xprintf("pa01_mask=0x%x,rtc_mask=0x%x,pa23_mask=0x%x,i2cw_mask=0x%x,timer_mask=0x%x,cmp_mask=0x%x,ts_mask=0x%x\n", cfg.pmu_pad_pa01_mask,cfg.pmu_rtc_mask,cfg.pmu_pad_pa23_mask,cfg.pmu_i2cw_mask,cfg.pmu_timer_mask,cfg.pmu_cmp_mask,cfg.pmu_ts_mask);
	xprintf("dcdcpin=%d, pmu_anti_mask=0x%x\n", cfg.pmu_dcdc_outpin, cfg.pmu_anti_mask);
	xprintf("freq=%d, cm55mdiv=%d,cm55sdiv=%d\n", cfg.bootromspeed.pll_freq, cfg.bootromspeed.cm55m_div, cfg.bootromspeed.cm55s_div);

	hx_lib_pm_cfg_set(&cfg, NULL, mode);

	/* Setup CM55M Timer(Timer2) Wakeup */
	setCM55MTimerAlarmPMU(timer_ms);

	/* Use PMU lib to control HSC_CLK and LSC_CLK so set those parameter to 0 */
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));

	/* Trigger to PMU mode */
	hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
}


volatile uint32_t g_tx_done = 0;
volatile uint32_t g_err_cb = 0;
volatile uint32_t g_wait_result = 0;

void i2cm_0_tx_cb(void *status)
{
    HX_DRV_DEV_IIC *iic_obj = status;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	int32_t len, ofs;
	len = iic_info_ptr->tx_buf.len;
	ofs = iic_info_ptr->tx_buf.ofs;

	g_tx_done = 1;
	xprintf("[%s] g_tx_done = %d, ofs:%d\n", __FUNCTION__, g_tx_done, ofs);
}

void i2cm_0_err_cb(void *status)
{
    HX_DRV_DEV_IIC *iic_obj = status;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

	g_err_cb = 1;
	//g_wait_result = 0;
	xprintf("[%s] err:%d \n", __FUNCTION__, iic_info_ptr->err_state);
}

static void i2cm_0_rx_cb(void *status)
{
    HX_DRV_DEV_IIC *iic_obj = status;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	int32_t len, ofs;
	uint8_t *rbuffer;

	len = iic_info_ptr->rx_buf.len;
	ofs = iic_info_ptr->rx_buf.ofs;
	rbuffer = iic_info_ptr->rx_buf.buf;

    xprintf("\n");
    xprintf("[%s] ofs:%d \n", __FUNCTION__, ofs);
	for ( int i = 0; i < 12; i++ )
	{
		xprintf("0x%x ", *(rbuffer+i));
	}
    xprintf("\n");
	g_wait_result = 0;
}

int32_t sleep_time = 0;

void TIMER0_ISR_CB(uint32_t event) {
	xprintf("\n\n #### TIMER0_ISR_CB event=%d\n", event); //RTC0_Cycle_IRQ_Handler

	app_pmu_enter_sleep(500, 0xFF, 1);	// 5 second or AON_GPIO0 wake up, memory no retention
}

void setTimer0AlarmPMU() {
	TIMER_ID_E id;
	TIMER_ERROR_E ret;
	TIMER_CFG_T timer_cfg;
	uint32_t boot_cnt;

	boot_cnt = hx_get_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C) % 500;
	timer_cfg.period = 500+boot_cnt;

	xprintf("timer_cfg.period = %d\n", timer_cfg.period);
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_CPU;
	timer_cfg.state = TIMER_STATE_DC;
	id = TIMER_ID_0;

	ret = hx_drv_timer_hw_start(id, &timer_cfg, TIMER0_ISR_CB);
}

#define FEATURE_TRANSPORT               0x10
#define FEATURE_TRANSPORT_CMD_STATUS    0x00
#define FEATURE_TRANSPORT_CMD_READ      0x01
#define FEATURE_TRANSPORT_CMD_WRITE     0x02
#define FEATURE_TRANSPORT_CMD_AVAILABLE 0x03
#define FEATURE_TRANSPORT_CMD_START     0x04
#define FEATURE_TRANSPORT_CMD_STOP      0x05
#define FEATURE_TRANSPORT_CMD_RESET     0x06

int i2cm_test()
{
	//uint8_t wbuffer[60] = {0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15, 0x30, 0x01, 0x00, 0x00, 0x0f, 0x15};
	uint8_t wbuffer[6] = {0x30, 0x01, 0x00, 0x00, 0x0f, 0x15};
	uint8_t rbuffer[128];
	uint32_t i, data_size = 6, r_size = 32;
	IIC_ERR_CODE_E i2c_err;

    xprintf("I2C Master 0 Init\n");

	// The output pin of I2C Master 0 is defined by the user application.
	hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_I2C_M_SCL, 1);
	hx_drv_scu_set_PB8_pinmux(SCU_PB8_PINMUX_I2C_M_SDA, 1);

	// initializes the I2C Master 0 with SCL speed of 400 KHz
	hx_drv_i2cm_init(USE_DW_IIC_0, HX_I2C_HOST_MST_0_BASE, DW_IIC_SPEED_FAST);
	hx_drv_i2cm_set_err_cb(USE_DW_IIC_0, i2cm_0_err_cb);

	//Transmit data using interrupt mode with I2C master 0

	wbuffer[0] = FEATURE_TRANSPORT;
	wbuffer[1] = FEATURE_TRANSPORT_CMD_WRITE;

	xprintf("\nStart I2C Master write test ...\n");
	for (int i = 0; i < 6; i++ )
	{
		wbuffer[2] = i;
		xprintf("\nwrite index = %d\n", wbuffer[2]);
		hx_drv_i2cm_interrupt_write(USE_DW_IIC_0, 0x62, wbuffer, data_size, i2cm_0_tx_cb);
		hx_drv_timer_cm55x_delay_ms(100, TIMER_STATE_DC);
	}

	xprintf("\nStart I2C Master read test ...\n");
	for (int i = 0; i < 6; i++ )
	{
		xprintf("\nread(%d)\n", i);
		memset(rbuffer, 0x00, 128);
		hx_drv_i2cm_interrupt_read(USE_DW_IIC_0, 0x62, rbuffer, r_size, i2cm_0_rx_cb);
		hx_drv_timer_cm55x_delay_ms(100, TIMER_STATE_DC);
	}
	//while(1);

	uint8_t index = 0;
	uint32_t boot_cnt;

	xprintf("\nStart I2C Master write/read test ...\n");
	wbuffer[0] = FEATURE_TRANSPORT;
	wbuffer[1] = FEATURE_TRANSPORT_CMD_READ;

	while ( 1 )
	{
		if ( g_wait_result == 0 )
		{
			wbuffer[2] = index%10;
			index++;
			i2c_err = hx_drv_i2cm_interrupt_write(USE_DW_IIC_0, 0x62, wbuffer, data_size, i2cm_0_tx_cb);
			xprintf("\nwrite(CMD_READ) : %d\n", wbuffer[2]);
			g_wait_result = 1;
			#if 1
			int wait_cnt = 0;
			while ( g_tx_done == 0 )
			{
				if ( (wait_cnt % 100) == 0 )
					xprintf(".");
				hx_drv_timer_cm55x_delay_ms(1, TIMER_STATE_DC);
			}
			xprintf("\n");
			#else
			while ( 1 )
			{
				if ( g_tx_done == 0 )
				{
					xprintf("g_tx_done = %d\n", g_tx_done);
					hx_drv_timer_cm55x_delay_ms(1, TIMER_STATE_DC);
				}
				else
				{
					xprintf("g_tx_done = %d\n", g_tx_done);
					break;
				}
			}
			#endif

			g_tx_done = 0;
			memset(rbuffer, 0, 128);
			sleep_time++;
			hx_drv_timer_cm55x_delay_ms(sleep_time%50, TIMER_STATE_DC);
			if ( g_err_cb == 0 )
			{
				hx_drv_i2cm_interrupt_read(USE_DW_IIC_0, 0x62, rbuffer, r_size, i2cm_0_rx_cb);
				xprintf("hx_drv_i2cm_interrupt_read(), sleep_time = %d\n", sleep_time%50);
			}
			else
			{
				g_err_cb = 0;
				g_wait_result = 0;
				xprintf("stop read!\n");
			}
			#if 1
			if ( index == 6 )
			{
				//boot_cnt = 100 - hx_get_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C);
				boot_cnt = hx_get_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C);
				xprintf("\n### boot_cnt = %d, delay (%d)ms to reboot ###\n\n", boot_cnt, (boot_cnt*10)%500);
				hx_drv_timer_cm55x_delay_ms((boot_cnt*10)%500, TIMER_STATE_DC);
				app_pmu_enter_sleep((boot_cnt%10)*111+1, 0xFF, 1);	// 5 second or AON_GPIO0 wake up, memory no retention
			}
			#endif
		}
		else
		{
			hx_drv_timer_cm55x_delay_ms(2, TIMER_STATE_DC);
			xprintf(".");
		}
	}

	return 0;
}
