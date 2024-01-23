/*
 * veneer_clk_ctrl.h
 *
 *  Created on: 2021¦~6¤ë11¤é
 *      Author: 902447
 */

#ifndef TRUSTZONE_NSC_FUNCTION_VENEER_CLK_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_VENEER_CLK_CTRL_H_

#include "hx_drv_timer.h"
#include "hx_drv_rtc.h"
#include "hx_drv_watchdog.h"
#include "hx_drv_scu_export.h"

/**
 * \brief	set Timer Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_set_timer_clk_en(TIMER_ID_E id, uint8_t clk_en);

/**
 * \brief	set Timer Clock Div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_set_timer_clkdiv(TIMER_ID_E id, uint32_t clk_div);



/**
 * \brief	set WDG Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_set_wdg_clk_en(WATCHDOG_ID_E id, uint8_t clk_en);


/**
 * \brief	set WDG clk div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_set_wdg_clkdiv(WATCHDOG_ID_E id, uint32_t clk_div);


/**
 * \brief	set RTC Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_set_rtc_clk_en(RTC_ID_E id, uint8_t clk_en);

/**
 * \brief	set RTC cycle width
 *
 * \param[in]	cycle_width	 cycle width (if clock is 24Mhz, cycle_width 24Mhz is 1 HZ for RTC)
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_set_rtc_cycle(RTC_ID_E id, uint32_t cycle_width);


/**
 * \brief	get RTC Clock Enable
 *
 * \param[out]	clk_en	 Clock Enable
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_get_rtc_clk_en(RTC_ID_E id, uint8_t *clk_en);

/**
 * \brief	set RTC cycle width
 *
 * \param[out]	cycle_width	 cycle width (if clock is 24Mhz, cycle_width 24Mhz is 1 HZ for RTC)
 * \return	uint32_t.
 */
uint32_t veneer_clk_ctrl_get_rtc_cycle(RTC_ID_E id, uint32_t *cycle_width);

/**
 * \brief	get Frequency
 *
 * \param[in]	type	 clock type
 * \return	uint32_t. Frequency
 */
uint32_t veneer_clk_get_freq(SCU_CLK_FREQ_TYPE_E type);


/**
 * \brief	set VAD D SRC Selection
 *
 * \param[in]	data	 VAD D SRC Sel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_vad_d_src_sel(SCU_LSCVADDCLKSRC_E data);

/**
 * \brief	get VAD D SRC Selection
 *
 * \param[out]	data	 VAD D SRC Sel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_vad_d_src_sel(SCU_LSCVADDCLKSRC_E *data);

/**
 * \brief	set I2S Clk Div
 *
 * \param[in]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_i2s_clkdiv(uint8_t clk_div);

/**
 * \brief	get I2S Clk Div
 *
 * \param[out]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_i2s_clkdiv(uint8_t *clk_div);

/**
 * \brief	set PDM Clk Div
 *
 * \param[in]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdm_clkdiv(uint8_t clk_div);

/**
 * \brief	get PDM Clk Div
 *
 * \param[out]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdm_clkdiv(uint8_t *clk_div);

/**
 * \brief	set XTAL32K enable
 *
 * \param[in]	enable	 xtal32K enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_xtal32k_en(uint8_t enable);

/**
 * \brief	get xtal32k enable
 *
 * \param[out]	enable	 xtal32K enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_xtal32k_en(uint8_t *enable);

/**
 * \brief	set XTAL24M enable
 *
 * \param[in]	enable	 xtal24m enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_xtal24m_en(uint8_t enable);

/**
 * \brief	get xtal24M enable
 *
 * \param[out]	enable	 xtal24m enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_xtal24m_en(uint8_t *enable);

#ifdef CM55_BIG
/**
 * \brief	set PLL Configuration and enable
 *
 * \param[in]	cfg	 PLL related config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pll_cfg(SCU_PLL_CLK_CFG_T *cfg);
#endif

/**
 * \brief	get PLL Configuration
 *
 * \param[out]	pll_cfg	 PLL configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pll_cfg(SCU_PLL_CLK_CFG_T *pll_cfg);

/**
 * \brief	get PLL Src
 *
 * \param[out]	pllclksrc	 PLL CLK Src
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pll_src(SCU_PLLCLKSRC_E *pllclksrc);

#ifdef CM55_BIG
/**
 * \brief	set PLL enable and power on/off
 *
 * \param[in]	enable	pll enable(1)/disable(0)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pll_enable(uint8_t enable);
#endif
/**
 * \brief	get PLL lock status
 *
 * \param[out]	lock	 PLL Lock
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pll_lock_status(uint8_t *lock);


/**
 * \brief	set PD HSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK Configuration
 * \param[in]	change_dprx	 change DP RX or not
 * \param[in]	change_dp	 change DP or not
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T *cfg, uint8_t change_dprx, uint8_t change_dp);


/**
 * \brief	get PD HSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T *cfg);

/**
 * \brief	set PD HSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T *cfg);

/**
 * \brief	get PD HSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T *cfg);

/**
 * \brief	set PD HSC HSC_CLK related Configuration
 *
 * \param[in]	cfg	 PD HSC HSC_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdhsc_hscclk_cfg(SCU_PDHSC_HSCCLK_CFG_T *cfg);

/**
 * \brief	get PD HSC HSC_CLK related Configuration
 *
 * \param[out]	cfg	 PD HSC HSC_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdhsc_hscclk_cfg(SCU_PDHSC_HSCCLK_CFG_T *cfg);

/**
 * \brief	set PD LSC adcckin CLK Configuration
 *
 * \param[in]	cfg	 PD LSC adcckin CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T *cfg);

/**
 * \brief	set PD LSC lsc CLK Configuration
 *
 * \param[in]	cfg	 PD LSC lsc CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_lscclk_cfg(SCU_LSC_CLK_CFG_T *cfg);

/**
 * \brief	set PD AON CLK enable Configuration
 *
 * \param[in]	cfg	 PD AON CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdaon_clken_cfg(SCU_PDAON_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD AON CLK enable Configuration
 *
 * \param[out]	cfg	 PD AON CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdaon_clken_cfg(SCU_PDAON_CLKEN_CFG_T *cfg);

/**
 * \brief	set PD SB CLK enable Configuration
 *
 * \param[in]	cfg	 PD SB CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdsb_clken_cfg(SCU_PDSB_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD SB CLK enable Configuration
 *
 * \param[out]	cfg	 PD SB CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdsb_clken_cfg(SCU_PDSB_CLKEN_CFG_T *cfg);

/**
 * \brief	set PD LSC CLK enable Configuration
 *
 * \param[in]	cfg	 PD LSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_clken_cfg(SCU_PDLSC_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD LSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD LSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_clken_cfg(SCU_PDLSC_CLKEN_CFG_T *cfg);

/**
 * \brief	set PD HSC CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdhsc_clken_cfg(SCU_PDHSC_CLKEN_CFG_T *cfg);

/**
 * \brief	get PD HSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdhsc_clken_cfg(SCU_PDHSC_CLKEN_CFG_T *cfg);
#ifdef TODO
/**
 * \brief	set PD HSC HSC32K_CLK related Configuration
 *
 * \param[in]	cfg	 PD HSC HSC32K_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdhsc_hsc32kclk_cfg(SCU_HSC32KCLKSRC_E cfg);

/**
 * \brief	get PD HSC HSC32K_CLK related Configuration
 *
 * \param[out]	cfg	 PD HSC HSC32K_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdhsc_hsc32kclk_cfg(SCU_HSC32KCLKSRC_E *cfg);

/**
 * \brief	set PD HSC CLK Configuration
 *
 * \param[in]	cfg	 PD HSC CLK Configuration

 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdhsc_clk_cfg(SCU_PDHSC_CLK_CFG_T *cfg);

/**
 * \brief	get PD HSC CLK Configuration
 *
 * \param[out]	cfg	 PD HSC CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdhsc_clk_cfg(SCU_PDHSC_CLK_CFG_T *cfg);

/**
 * \brief	set PD LSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD LSC DP CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_dpclk_cfg(SCU_PDLSC_DPCLK_CFG_T *cfg);

/**
 * \brief	get PD LSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD LSC DP CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_dpclk_cfg(SCU_PDLSC_DPCLK_CFG_T *cfg);

/**
 * \brief	set PD LSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD LSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_dpclken_cfg(SCU_PDLSC_DPCLKEN_CFG_T *cfg);

/**
 * \brief	get PD LSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD LSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_dpclken_cfg(SCU_PDLSC_DPCLKEN_CFG_T *cfg);



/**
 * \brief	get PD LSC lsc CLK Configuration
 *
 * \param[out]	cfg	 PD LSC lsc CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_lscclk_cfg(SCU_LSC_CLK_CFG_T *cfg);

/**
 * \brief	set PD LSC Reference CLK Configuration
 *
 * \param[in]	cfg	 PD LSC Reference CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_lscrefclk_cfg(SCU_PDLSC_LSCREF_CFG_T *cfg);

/**
 * \brief	get PD LSC Reference CLK Configuration
 *
 * \param[out]	cfg	 PD LSC Reference CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_lscrefclk_cfg(SCU_PDLSC_LSCREF_CFG_T *cfg);


/**
 * \brief	get PD LSC adcckin CLK Configuration
 *
 * \param[out]	cfg	 PD LSC adcckin CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T *cfg);

/**
 * \brief	set PD LSC CLK Configuration
 *
 * \param[in]	cfg	 PD LSC CLK Configuration
 * \param[in]	change_dp	 PD LSC Change DP related
 * \param[in]	vad_src_change	 PD LSC VAD Source Change
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdlsc_clk_cfg(SCU_PDLSC_CLK_CFG_T *cfg, uint8_t change_dp, uint8_t vad_src_change);

/**
 * \brief	get PD LSC CLK Configuration
 *
 * \param[out]	cfg	 PD LSC CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdlsc_clk_cfg(SCU_PDLSC_CLK_CFG_T *cfg);





/**
 * \brief	set PD SB CLK Configuration
 *
 * \param[in]	cfg	 PD SB CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdsb_clk_cfg(SCU_PDSB_CLK_CFG_T *cfg);

/**
 * \brief	get PD SB CLK Configuration
 *
 * \param[out]	cfg	 PD SB CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdsb_clk_cfg(SCU_PDSB_CLK_CFG_T *cfg);



/**
 * \brief	set PD AON CLK Configuration
 *
 * \param[in]	cfg	 PD AON CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_pdaon_clk_cfg(SCU_PDAON_CLK_CFG_T *cfg);

/**
 * \brief	get PD AON CLK Configuration
 *
 * \param[out]	cfg	 PD AON CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_pdaon_clk_cfg(SCU_PDAON_CLK_CFG_T *cfg);
#endif
#ifdef CM55_BIG
/**
 * \brief	set Simple CLK Configuration without PLL
 *
 * \param[in]	cfg	 Simple CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_set_simple_clk_cfg(SCU_SIMPLE_CLK_CFG_T *cfg);

/**
 * \brief	get Simple CLK Configuration
 *
 * \param[out]	cfg	 Simple CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_clk_get_simple_clk_cfg(SCU_SIMPLE_CLK_CFG_T *cfg);
#endif

/**
 * \brief	Get System clk case
 *
 * \param[out]	cfg	 System clk case
 * \return	void.
 */
void veneer_clk_get_systemclkcase_flag(SCU_SYSCLKSRC_HW_E *cfg);

/**
 * \brief	set RTC State
 *
 * \param[in]	state	 RTC state
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_rtc_state(RTC_ID_E id, SCU_RTC_STATE_E state);


#endif /* TRUSTZONE_NSC_FUNCTION_VENEER_CLK_CTRL_H_ */
