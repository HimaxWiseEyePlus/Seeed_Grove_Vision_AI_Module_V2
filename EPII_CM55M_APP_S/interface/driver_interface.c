/*
 * driver_interface.c
 *
 *  Created on: 2023�~5��9��
 *      Author: 902447
 */

#ifndef INTERFACE_DRIVER_INTERFACE_C_
#define INTERFACE_DRIVER_INTERFACE_C_

#include <stdio.h>
#include "WE2_device.h"
#include "WE2_core.h"
#ifdef LIB_COMMON
#include "xprintf.h"
#endif
#ifdef NSC
#include "veneer_clk_ctrl.h"
#include "veneer_sys_ctrl.h" //armclang 6.19
#else
#include "hx_drv_scu.h"
#endif
#include "driver_interface.h"

DRIVER_INTERFACE_E drv_interface_get_freq(SCU_CLK_FREQ_TYPE_E type, uint32_t *freq)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	*freq = veneer_clk_get_freq(type);
#else
    ret = hx_drv_scu_get_freq(type, freq);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

#ifdef IP_rtc
DRIVER_INTERFACE_E drv_interface_set_rtc_clk_en(RTC_ID_E id, uint8_t clk_en)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_set_rtc_clk_en(id, clk_en);
#else
	ret = hx_drv_scu_set_rtc_clk_en(id, clk_en);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set RTC cycle width
 *
 * \param[in]	cycle_width	 cycle width (if clock is 24Mhz, cycle_width 24Mhz is 1 HZ for RTC)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_cycle(RTC_ID_E id, uint32_t cycle_width)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_set_rtc_cycle(id, cycle_width);
#else
	ret = hx_drv_scu_set_rtc_cycle(id, cycle_width);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set rtc clock enable
 *
 * \param[in]	id	 RTC ID
 * \param[out]	clk_en	 Clock enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_rtc_clk_en(RTC_ID_E id, uint8_t *clk_en)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_get_rtc_clk_en(id, clk_en);
#else
	ret = hx_drv_scu_get_rtc_clk_en(id, clk_en);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	get rtc clock enable
 *
 * \param[in]	id	 RTC ID
 * \param[out]	cycle_width	 Cycle width
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_rtc_cycle(RTC_ID_E id, uint32_t *cycle_width)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_get_rtc_cycle(id, cycle_width);
#else
	ret = hx_drv_scu_get_rtc_cycle(id, cycle_width);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}
#endif

#ifdef IP_watchdog
DRIVER_INTERFACE_E drv_interface_set_wdg_clk_en(WATCHDOG_ID_E id, uint8_t clk_en)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_set_wdg_clk_en(id, clk_en);
#else
	ret = hx_drv_scu_set_wdg_clk_en(id, clk_en);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}


DRIVER_INTERFACE_E drv_interface_set_wdg_clkdiv(WATCHDOG_ID_E id, uint32_t clk_div)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_set_wdg_clkdiv(id, clk_div);
#else
	ret = hx_drv_scu_set_wdg_clkdiv(id, clk_div);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set WDT control owner
 *
 * \param[in]	id 0 or 1
 * \param[in]	ctrl enable by CPU or PMU
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdt_ctrl(WATCHDOG_ID_E id, SCU_WDTCTRL_E ctrl)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_sys_set_wdt_ctrl(id, ctrl);
#else
	ret = hx_drv_scu_set_wdt_ctrl(id, ctrl);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}
#endif
#ifdef IP_timer
/**
 * \brief	set Timer control owner
 *
 * \param[in]	id 0 ~ 8
 * \param[in]	ctrl enable by CPU or PMU
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_ctrl(TIMER_ID_E id, SCU_TIMERCTRL_E ctrl)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_sys_set_timer_ctrl(id, ctrl);
#else
	ret = hx_drv_scu_set_timer_ctrl(id, ctrl);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set Timer Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_clk_en(TIMER_ID_E id, uint8_t clk_en)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_set_timer_clk_en(id, clk_en);
#else
	ret = hx_drv_scu_set_timer_clk_en(id, clk_en);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set Timer Clock Div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_clkdiv(TIMER_ID_E id, uint32_t clk_div)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_ctrl_set_timer_clkdiv(id, clk_div);
#else
	ret = hx_drv_scu_set_timer_clkdiv(id, clk_div);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}
#endif


/**
 * \brief	set ADC LV Reset
 *
 * \param[in]	cfg	 reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_lsc_adclv_reset(SCU_SWRESET_E cfg)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_set_lsc_adclv_reset(cfg);
#else
	ret = hx_drv_scu_set_lsc_adclv_reset(cfg);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set ADC HV Reset
 *
 * \param[in]	cfg	 reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_lsc_adchv_reset(SCU_SWRESET_E cfg)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_set_lsc_adchv_reset(cfg);
#else
	ret = hx_drv_scu_set_lsc_adchv_reset(cfg);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set PD LSC adcckin CLK Configuration
 *
 * \param[in]	cfg	 PD LSC adcckin CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T cfg)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_set_pdlsc_adcckinclk_cfg(&cfg);
#else
	ret = hx_drv_scu_set_pdlsc_adcckinclk_cfg(cfg);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}
/**
 * \brief	set PDM Clk Div
 *
 * \param[in]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdm_clkdiv(uint8_t clk_div)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_set_pdm_clkdiv(clk_div);
#else
	ret = hx_drv_scu_set_pdm_clkdiv(clk_div);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[in]	channel	 capture channel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E channel)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_set_audio_capture_channel(audio_type, channel);
#else
	ret = hx_drv_scu_set_audio_capture_channel(audio_type, channel);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set PD LSC PDM CLK Source
 *
 * \param[in]	cfg	 PD LSC PDM CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E pdmsrc_clk)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#if(IC_VERSION >= 30)
#ifndef BOOT_USED
#ifdef NSC
    if(pdmsrc_clk == SCU_LSCPDMCLKSRC_EXT)
    {
        veneer_sys_set_PB2_pinmux(SCU_PB2_PINMUX_PDM_CLK_IN, 1);
        veneer_sys_set_PB3_pinmux(SCU_PB3_PINMUX_PDM_DATA0_OUT, 1);
    }
	veneer_set_pdlsc_pdmck_src(pdmsrc_clk);
#else
    if(pdmsrc_clk == SCU_LSCPDMCLKSRC_EXT)
    {
        hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_PDM_CLK_IN, 1);
        hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_PDM_DATA0_OUT, 1);
    }
	ret = hx_drv_scu_set_pdlsc_pdmck_src(pdmsrc_clk);
#endif
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set VAD D SRC Selection
 *
 * \param[in]	data	 VAD D SRC Sel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_vad_d_src_sel(SCU_LSCVADDCLKSRC_E data)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_set_vad_d_src_sel(data);
#else
	ret = hx_drv_scu_set_vad_d_src_sel(data);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set VAD D PCM Selection
 *
 * \param[in]	data	 VAD D PCM Sel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_vad_d_pcm_sel(SCU_VAD_PCM_DATA_SEL_E data)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_sys_set_vad_d_pcm_sel(data);
#else
	ret = hx_drv_scu_set_vad_d_pcm_sel(data);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set I2S Clk Div
 *
 * \param[in]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_i2s_clkdiv(uint8_t clk_div)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_set_i2s_clkdiv(clk_div);
#else
	ret = hx_drv_scu_set_i2s_clkdiv(clk_div);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set I2S mode
 *
 * \param[in]	mode	 I2S mode
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_i2s_mode(SCU_I2S_MODE_E mode)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_sys_set_I2S_mode(mode);
#else
	ret = hx_drv_scu_set_I2S_mode(mode);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}


/**
 * \brief	set DP SWReset
 *
 * \param[in]	cfg_swreset	 DP SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_dp_swreset(SCU_DP_SWRESET_T cfg_swreset)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_sys_set_DP_SWReset(&cfg_swreset);
#else
	ret = hx_drv_scu_set_DP_SWReset(cfg_swreset);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}


/**
 * \brief	get DP SWReset
 *
 * \param[out]	cfg_swreset	 DP SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_dp_swreset(SCU_DP_SWRESET_T *cfg_swreset)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOT_USED
#ifdef NSC
	veneer_sys_get_DP_SWReset(cfg_swreset);
#else
	ret = hx_drv_scu_get_DP_SWReset(cfg_swreset);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set PD HSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK Configuration
 * \param[in]	change_dprx	 change DP RX or not
 * \param[in]	change_dp	 change DP or not
 * \return	SCU_ERROR_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T cfg, uint8_t change_dprx, uint8_t change_dp)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_set_pdhsc_dpclk_cfg(&cfg, change_dprx, change_dp);
#else
	ret = hx_drv_scu_set_pdhsc_dpclk_cfg(cfg, change_dprx, change_dp);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	get PD HSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T *cfg)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_get_pdhsc_dpclk_cfg(cfg);
#else
	ret = hx_drv_scu_get_pdhsc_dpclk_cfg(cfg);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set PD HSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T cfg)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	veneer_clk_set_pdhsc_dpclken_cfg(&cfg);
#else
	ret = hx_drv_scu_set_pdhsc_dpclken_cfg(cfg);
#endif

    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	get PD HSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T *cfg)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifndef BOOTROM
#ifdef NSC
	veneer_clk_get_pdhsc_dpclken_cfg(cfg);
#else
	ret = hx_drv_scu_get_pdhsc_dpclken_cfg(cfg);
#endif
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;

}

/**
 * \brief	get CPU Clock
 *
 * \param[out]	cpu_clk	 Get CPU clock
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_cpu_clk(uint32_t *cpu_clk)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;

#ifdef NSC
#ifdef CM55_BIG
	*cpu_clk = veneer_clk_get_freq(SCU_CLK_FREQ_TYPE_HSC_CM55M);
#else
	*cpu_clk = veneer_clk_get_freq(SCU_CLK_FREQ_TYPE_LSC_CM55S);
#endif
#else
#ifdef CM55_BIG
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_CM55M, cpu_clk);
#else
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_LSC_CM55S, cpu_clk);
#endif
#endif

    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;

}

/**
 * \brief	get PD AON CLK Enable
 *
 * \param[out]	cfg	 PD AON CLK Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdaon_clken_cfg(SCU_PDAON_CLKEN_CFG_T *cfg)
{
#ifdef NSC
	veneer_clk_get_pdaon_clken_cfg(cfg);
#else
	hx_drv_scu_get_pdaon_clken_cfg(cfg);
#endif

    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	get PD SB CLK enable Configuration
 *
 * \param[out]	cfg	 PD SB CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdsb_clken_cfg(SCU_PDSB_CLKEN_CFG_T *cfg)
{
#ifdef NSC
	veneer_clk_get_pdsb_clken_cfg(cfg);
#else
	hx_drv_scu_get_pdsb_clken_cfg(cfg);
#endif

    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	get PD LSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD LSC CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdlsc_clken_cfg(SCU_PDLSC_CLKEN_CFG_T *cfg)
{
#ifdef NSC
	veneer_clk_get_pdlsc_clken_cfg(cfg);
#else
	hx_drv_scu_get_pdlsc_clken_cfg(cfg);
#endif

    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	get PD HSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_clken_cfg(SCU_PDHSC_CLKEN_CFG_T *cfg)
{
#ifdef NSC
	veneer_clk_get_pdhsc_clken_cfg(cfg);
#else
	hx_drv_scu_get_pdhsc_clken_cfg(cfg);
#endif

    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set SB Sensor i2cm Pin Mux
 *
 * \param[in]	i2cm_sda	 SB Sensor i2cm SDA Pin Mux
 * \param[in]	i2cm_scl	 SB Sensor i2cm SDL Pin Mux
 * \return	SCU_ERROR_E.
 */
DRIVER_INTERFACE_E drv_interface_set_sensor_i2cm_pinmux(SCU_SEN_I2CM_SDA_PINMUX_E i2cm_sda, SCU_SEN_I2CM_SCL_PINMUX_E i2cm_scl)
{
#ifndef BOOT_USED
#ifdef NSC
	veneer_sys_set_sensor_i2cm_pinmux(i2cm_sda, i2cm_scl);
#else
	hx_drv_scu_set_SEN_I2C_MST_SDA_pinmux(i2cm_sda);
	hx_drv_scu_set_SEN_I2C_MST_SCL_pinmux(i2cm_scl);
#endif
#endif
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set SEN_PCLKO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_SEN_PCLKO_pinmux(SCU_SEN_PCLK_PINMUX_E pinmux)
{
#ifndef BOOT_USED
#if(IC_VERSION >= 30)
#ifdef NSC
	veneer_sys_set_SEN_PCLKO_pinmux(pinmux);
#else
	//set to default pinmux
	hx_drv_scu_set_SEN_PCLKO_pinmux(pinmux);
#endif
#endif//#if(IC_VERSION >= 30)
#endif//BOOT_USED
    return DRIVER_INTERFACE_NO_ERROR;
}

#ifdef IP_rtc
/**
 * \brief	set RTC State
 *
 * \param[in]	state	 RTC state
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_state(RTC_ID_E id, SCU_RTC_STATE_E state)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	ret = veneer_set_rtc_state(id, state);
#else
	ret = hx_drv_scu_set_rtc_state(id,  state);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}
#endif

#ifndef BOOT_USED
/**
 * \brief	Set MIPI Control
 *
 * \param[in]	ctrl	 MIPI control
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_E ctrl)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	ret = veneer_set_mipi_ctrl(ctrl);
#else
	ret = hx_drv_scu_set_mipi_ctrl(ctrl);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}
/**
 * \brief	Get MIPI Control
 *
 * \param[out]	ctrl	 MIPI control
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_mipi_ctrl(SCU_MIPI_CTRL_E *ctrl)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;
#ifdef NSC
	ret = veneer_get_mipi_ctrl(ctrl);
#else
	ret = hx_drv_scu_get_mipi_ctrl(ctrl);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

#endif
/**
 * \brief	set LSC SWReset
 *
 * \param[in]	cfg_swreset	 LSC SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_LSC_swreset(SCU_LSC_SWRESET_T cfg_swreset)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;

#ifdef NSC
	veneer_set_LSC_SWReset(&cfg_swreset);
#else
	ret = hx_drv_scu_set_LSC_SWReset(cfg_swreset);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

/**
 * \brief	set MIPI TX Phy RX clock source
 *
 * \param[in]	txphyrxclksrc	 MIPI TX Phy RX Clock source
 * \return	SCU_ERROR_E.
 */
DRIVER_INTERFACE_E drv_interface_set_MIPITXPHY_RXClkSRC(SCU_TXPHY_RCLK_SRC_SEL_E txphyrxclksrc)
{
	SCU_ERROR_E ret = SCU_NO_ERROR;

#ifdef NSC
	veneer_set_MIPITXPHY_RXClkSRC(&txphyrxclksrc);
#else
	ret = hx_drv_scu_set_MIPITXPHY_RXClkSRC(txphyrxclksrc);
#endif
    if(ret != SCU_NO_ERROR)
    {
    	return DRIVER_INTERFACE_UNKNOWN_ERROR;
    }
    return DRIVER_INTERFACE_NO_ERROR;
}

#endif /* INTERFACE_DRIVER_INTERFACE_C_ */
