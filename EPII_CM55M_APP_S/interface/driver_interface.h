/*
 * driver_interface.h
 *
 *  Created on: 2023¦~5¤ë9¤é
 *      Author: 902447
 */

#ifndef INTERFACE_DRIVER_INTERFACE_H_
#define INTERFACE_DRIVER_INTERFACE_H_

#include "hx_drv_scu_export.h"
#ifdef IP_rtc
#include "hx_drv_rtc.h"
#endif
#ifdef IP_watchdog
#include "hx_drv_watchdog.h"
#endif
#ifdef IP_timer
#include "hx_drv_timer.h"
#endif
#ifdef IP_timer
#include "timer_interface.h"
#endif
/**
 * \enum DRIVER_INTERFACE_E
 * \brief Driver Interface error
 */
typedef enum
{
	DRIVER_INTERFACE_NO_ERROR                     	= 0,	/**< No ERROR */
	DRIVER_INTERFACE_UNKNOWN_ERROR      			= 1,	/**< ERROR MSG: UNKNOWN ERROR*/
} DRIVER_INTERFACE_E;

/**
 * \brief	get current Frequency
 *
 * \param[in]	type	 Frequency Type
 * \param[out]	freq	 Freq
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_freq(SCU_CLK_FREQ_TYPE_E type, uint32_t *freq);
#ifdef IP_rtc
/**
 * \brief	set rtc clock enable
 *
 * \param[in]	id	 RTC ID
 * \param[in]	clk_en	 Clock enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_clk_en(RTC_ID_E id, uint8_t clk_en);

/**
 * \brief	get rtc clock enable
 *
 * \param[in]	id	 RTC ID
 * \param[in]	cycle_width	 Cycle width
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_cycle(RTC_ID_E id, uint32_t cycle_width);

/**
 * \brief	set rtc clock enable
 *
 * \param[in]	id	 RTC ID
 * \param[out]	clk_en	 Clock enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_rtc_clk_en(RTC_ID_E id, uint8_t *clk_en);

/**
 * \brief	get rtc clock enable
 *
 * \param[in]	id	 RTC ID
 * \param[out]	cycle_width	 Cycle width
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_rtc_cycle(RTC_ID_E id, uint32_t *cycle_width);
#endif
#ifdef IP_watchdog
/**
 * \brief	set WDG Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdg_clk_en(WATCHDOG_ID_E id, uint8_t clk_en);

/**
 * \brief	set WDG clk div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdg_clkdiv(WATCHDOG_ID_E id, uint32_t clk_div);

/**
 * \brief	set WDT control owner
 *
 * \param[in]	id 0 or 1
 * \param[in]	ctrl enable by CPU or PMU
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdt_ctrl(WATCHDOG_ID_E id, SCU_WDTCTRL_E ctrl);
#endif
#ifdef IP_timer
/**
 * \brief	set Timer control owner
 *
 * \param[in]	id 0 ~ 8
 * \param[in]	ctrl enable by CPU or PMU
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_ctrl(TIMER_ID_E id, SCU_TIMERCTRL_E ctrl);

/**
 * \brief	set Timer Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_clk_en(TIMER_ID_E id, uint8_t clk_en);

/**
 * \brief	set Timer Clock Div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_clkdiv(TIMER_ID_E id, uint32_t clk_div);
#endif

/**
 * \brief	set ADC LV Reset
 *
 * \param[in]	cfg	 reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_lsc_adclv_reset(SCU_SWRESET_E cfg);

/**
 * \brief	set ADC HV Reset
 *
 * \param[in]	cfg	 reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_lsc_adchv_reset(SCU_SWRESET_E cfg);

/**
 * \brief	set PD LSC adcckin CLK Configuration
 *
 * \param[in]	cfg	 PD LSC adcckin CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T cfg);

/**
 * \brief	set PDM Clk Div
 *
 * \param[in]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdm_clkdiv(uint8_t clk_div);

/**
 * \brief	set Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[in]	channel	 capture channel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E channel);

/**
 * \brief	set PD LSC PDM CLK Source
 *
 * \param[in]	cfg	 PD LSC PDM CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E pdmsrc_clk);

/**
 * \brief	set VAD D SRC Selection
 *
 * \param[in]	data	 VAD D SRC Sel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_vad_d_src_sel(SCU_LSCVADDCLKSRC_E data);

/**
 * \brief	set VAD D PCM Selection
 *
 * \param[in]	data	 VAD D PCM Sel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_vad_d_pcm_sel(SCU_VAD_PCM_DATA_SEL_E data);


/**
 * \brief	set I2S Clk Div
 *
 * \param[in]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_i2s_clkdiv(uint8_t clk_div);

/**
 * \brief	set I2S mode
 *
 * \param[in]	mode	 I2S mode
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_i2s_mode(SCU_I2S_MODE_E mode);

/**
 * \brief	set DP SWReset
 *
 * \param[in]	cfg_swreset	 DP SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_dp_swreset(SCU_DP_SWRESET_T cfg_swreset);


/**
 * \brief	get DP SWReset
 *
 * \param[out]	cfg_swreset	 DP SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_dp_swreset(SCU_DP_SWRESET_T *cfg_swreset);

/**
 * \brief	set PD HSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK Configuration
 * \param[in]	change_dprx	 change DP RX or not
 * \param[in]	change_dp	 change DP or not
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T cfg, uint8_t change_dprx, uint8_t change_dp);


/**
 * \brief	get PD HSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T *cfg);

/**
 * \brief	set PD HSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T cfg);


/**
 * \brief	get PD HSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T *cfg);

/**
 * \brief	get CPU Clock
 *
 * \param[out]	cpu_clk	 Get CPU clock
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_cpu_clk(uint32_t *cpu_clk);

/**
 * \brief	get PD AON CLK Enable
 *
 * \param[out]	cfg	 PD AON CLK Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdaon_clken_cfg(SCU_PDAON_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD SB CLK enable Configuration
 *
 * \param[out]	cfg	 PD SB CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdsb_clken_cfg(SCU_PDSB_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD LSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD LSC CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdlsc_clken_cfg(SCU_PDLSC_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD HSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_clken_cfg(SCU_PDHSC_CLKEN_CFG_T *cfg);


/**
 * \brief	set SB Sensor i2cm Pin Mux
 *
 * \param[in]	i2cm_sda	 SB Sensor i2cm SDA Pin Mux
 * \param[in]	i2cm_scl	 SB Sensor i2cm SDL Pin Mux
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_sensor_i2cm_pinmux(SCU_SEN_I2CM_SDA_PINMUX_E i2cm_sda, SCU_SEN_I2CM_SCL_PINMUX_E i2cm_scl);


/**
 * \brief	set SEN_PCLKO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_SEN_PCLKO_pinmux(SCU_SEN_PCLK_PINMUX_E pinmux);

#ifdef IP_rtc
/**
 * \brief	set RTC State
 *
 * \param[in]	state	 RTC state
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_state(RTC_ID_E id, SCU_RTC_STATE_E state);
#endif

#ifndef BOOT_USED
/**
 * \brief	Set MIPI Control
 *
 * \param[in]	ctrl	 MIPI control
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_E ctrl);
/**
 * \brief	Get MIPI Control
 *
 * \param[out]	ctrl	 MIPI control
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_mipi_ctrl(SCU_MIPI_CTRL_E *ctrl);

#endif

/**
 * \brief	set LSC SWReset
 *
 * \param[in]	cfg_swreset	 LSC SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_LSC_swreset(SCU_LSC_SWRESET_T cfg_swreset);

/**
 * \brief	set MIPI TX Phy RX clock source
 *
 * \param[in]	txphyrxclksrc	 MIPI TX Phy RX Clock source
 * \return	SCU_ERROR_E.
 */
DRIVER_INTERFACE_E drv_interface_set_MIPITXPHY_RXClkSRC(SCU_TXPHY_RCLK_SRC_SEL_E txphyrxclksrc);

#endif /* INTERFACE_DRIVER_INTERFACE_H_ */
