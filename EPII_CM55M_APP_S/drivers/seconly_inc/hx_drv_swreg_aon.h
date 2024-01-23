/*
 * hx_drv_swreg_aon.h
 *
 *  Created on: 2021/9/5
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SWREG_AON_H_
#define DRIVERS_INC_HX_DRV_SWREG_AON_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon_export.h"
/**
 * \defgroup	SWREG_AON_DRV	SWREG AON Driver
 * \ingroup		SWREG_AON_DRV
 * \brief	SWREG AON Driver Declaration
 * \details Application can use SW Reg AON Structure to control Hardware\n
 */
/**
 * \defgroup	SWREG_AON_DRV_FUNCDLR	SWREG AON Driver Function Declaration
 * \ingroup	SWREG_AON_DRV
 * \brief	Contains declarations of SWREG AON  Driver functions.
 * @{
 */
/**
 * \brief	Set Retention flag
 *
 * \param[in]	cfg	 retention flag configuration
 * \return	void.
 */
void hx_drv_swreg_aon_set_retention_flag(SWREG_AON_RETENTION_CFG_T cfg);

/**
 * \brief	Get Retention flag
 *
 * \param[out]	cfg	 retention flag configuration
 * \return	void.
 */
void hx_drv_swreg_aon_get_retention_flag(SWREG_AON_RETENTION_CFG_T *cfg);

/**
 * \brief	Set Notify flag
 *
 * \param[in]	cfg	 notify flag configuration
 * \return	void.
 */
void hx_drv_swreg_aon_set_notify_flag(SWREG_AON_PMUWake_Notify_CFG_T cfg);

/**
 * \brief	Get Notify flag
 *
 * \param[out]	cfg	 notify flag configuration
 * \return	void.
 */
void hx_drv_swreg_aon_get_notify_flag(SWREG_AON_PMUWake_Notify_CFG_T *cfg);

/**
 * \brief	Set CM55S Re-Reset flag after PMU wakeup
 *
 * \param[in]	cfg	 re-reset flag configuration
 * \return	void.
 */
void hx_drv_swreg_aon_set_rerest_cm55s_flag(SWREG_AON_PMUWAKE_CM55S_RERESET_E cfg);

/**
 * \brief	Get CM55S Re-Reset flag after PMU wakeup
 *
 * \param[out]	cfg	 re-reset flag configuration
 * \return	void.
 */
void hx_drv_swreg_aon_get_rerest_cm55s_flag(SWREG_AON_PMUWAKE_CM55S_RERESET_E *cfg);

/**
 * \brief	Set OTA flag
 *
 * \param[in]	cfg	 OTA flag
 * \return	void.
 */
void hx_drv_swreg_aon_set_ota_flag(SWREG_AON_OTA_E cfg);

/**
 * \brief	Get OTA flag
 *
 * \param[out]	cfg	 OTA flag
 * \return	void.
 */
void hx_drv_swreg_aon_get_ota_flag(SWREG_AON_OTA_E *cfg);


/**
 * \brief	Set PMU Wake up bootrom cpu clk
 *
 * \param[in]	pll_freq	 bootrom cpu PMU wake up PLL frequency
 * \param[in]	cm55m_div	 bootrom cpu PMU wake up CM55M PLL Div
 * \param[in]	cm55s_div	 bootrom cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void hx_drv_swreg_aon_set_pmuwakeup_freq(SCU_PLL_FREQ_E pll_freq, SCU_HSCCLKDIV_E cm55m_div, SCU_LSCCLKDIV_E cm55s_div);

/**
 * \brief	Get PMU Wake up bootrom cpu clk
 *
 * \param[out]	pll_freq	 bootrom cpu PMU wake up PLL frequency
 * \param[out]	cm55m_div	 bootrom cpu PMU wake up CM55M PLL Div
 * \param[out]	cm55s_div	 bootrom cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void hx_drv_swreg_aon_get_pmuwakeup_freq(SCU_PLL_FREQ_E *pll_freq, SCU_HSCCLKDIV_E *cm55m_div, SCU_LSCCLKDIV_E *cm55s_div);


/**
 * \brief	Set PLL Freq
 *
 * \param[in]	cfg	 PLL Cfg
 * \return	void.
 */
void hx_drv_swreg_aon_set_pll_freq_flag(SCU_PLL_FREQ_E cfg);

/**
 * \brief	Get PLL Freq
 *
 * \param[out]	cfg	 Disable PLL CFG
 * \return	void.
 */
void hx_drv_swreg_aon_get_pll_freq_flag(SCU_PLL_FREQ_E *cfg);


/**
 * \brief	Set System clk case
 *
 * \param[in]	cfg	 System clk case
 * \return	void.
 */
void hx_drv_swreg_aon_set_systemclkcase_flag(SCU_SYSCLKSRC_HW_E cfg);

/**
 * \brief	Get System clk case
 *
 * \param[out]	cfg	 System clk case
 * \return	void.
 */
void hx_drv_swreg_aon_get_systemclkcase_flag(SCU_SYSCLKSRC_HW_E *cfg);

/**
 * \brief	Set LSC CLK change policy when PMU wakeup
 *
 * \param[in]	cfg	 LSC CLK change policy when PMU wakeup
 * \return	void.
 */
void hx_drv_swreg_aon_set_lscclkchg_ref_flag(SWREG_AON_LSCCLKCHANGE_REF_E cfg);

/**
 * \brief	Get LSC CLK change policy when PMU wakeup
 *
 * \param[out]	cfg	 LSC CLK change policy when PMU wakeup
 * \return	void.
 */
void hx_drv_swreg_aon_get_lscclkchg_ref_flag(SWREG_AON_LSCCLKCHANGE_REF_E *cfg);

/**
 * \brief	Set LSC CLK change policy when PMU wakeup in SW policy
 *
 * \param[in]	cfg	 LSC CLK change policy when PMU wakeup in SW policy
 * \return	void.
 */
void hx_drv_swreg_aon_set_lscclkchg_sw_flag(SWREG_AON_LSCCLKCHANGE_E cfg);

/**
 * \brief	Get LSC CLK change or not when PMU wakeup in SW policy
 *
 * \param[out]	cfg	 LSC CLK change policy when PMU wakeup in SW policy
 * \return	void.
 */
void hx_drv_swreg_aon_get_lscclkchg_sw_flag(SWREG_AON_LSCCLKCHANGE_E *cfg);


/**
 * \brief	Set SPI RX sample delay
 *
 * \param[in]	spirxsampledelay	 SPI RX sample delay
 * \return	void.
 */
void hx_drv_swreg_aon_set_spi_rxsample_delay(uint8_t spirxsampledelay);

/**
 * \brief	Get SPI RX sample delay
 *
 * \param[out]	spirxsampledelay	 SPI RX sample delay
 * \return	void.
 */
void hx_drv_swreg_aon_get_spi_rxsample_delay(uint8_t *spirxsampledelay);

/**
 * \brief	Set Sensor Init status
 *
 * \param[in]	init	 Sensor Init status
 * \return	void.
 */
void hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSORINIT_E init);

/**
 * \brief	Get Sensor Init status
 *
 * \param[out]	init	 Sensor Init status
 * \return	void.
 */
void hx_drv_swreg_aon_get_sensorinit(SWREG_AON_SENSORINIT_E *init);

/**
 * \brief	Set PLL Frequency
 *
 * \param[in]	freq	 PLL Frequency
 * \return	void.
 */
void hx_drv_swreg_aon_set_pllfreq(uint32_t freq);

/**
 * \brief	Get PLL Frequency
 *
 * \param[out]	freq	 PLL Frequency
 * \return	void.
 */
void hx_drv_swreg_aon_get_pllfreq(uint32_t *freq);

#ifndef BOOTROM
/**
 * \brief	Set FW image size
 *
 * \param[in]	size	 FW image sizes
 * \return	void.
 */
void hx_drv_swreg_aon_set_fwimagesize(uint32_t size);

/**
 * \brief	Get FW image size
 *
 * \param[out]	size	 FW image size
 * \return	void.
 */
void hx_drv_swreg_aon_get_fwimagesize(uint32_t *size);

/**
 * \brief	Set image memory descriptor addr
 *
 * \param[in]	memdescaddr	 Memory descriptor
 * \return	void.
 */
void hx_drv_swreg_aon_set_memdescaddr(uint32_t memdescaddr);

/**
 * \brief	Get image memory descriptor addr
 *
 * \param[out]	memdescaddr	 Memory descriptor
 * \return	void.
 */
void hx_drv_swreg_aon_get_memdescaddr(uint32_t *memdescaddr);

/**
 * \brief	Set register APP Used1(No particular use)
 *
 * \param[in]	data	 data
 * \return	void.
 */
void hx_drv_swreg_aon_set_appused1(uint32_t data);

/**
 * \brief	Get register APP Used1(No particular purpose)
 *
 * \param[out]	data	 data
 * \return	void.
 */
void hx_drv_swreg_aon_get_appused1(uint32_t *data);
#endif

/**
 * \brief	Set PLL Frequency
 *
 * \param[in]	swbootopt	 SW BootOPT
 * \return	void.
 */
void hx_drv_swreg_aon_set_swbootopt(uint32_t swbootopt);
/**
 * \brief	Get SW BOOT OPT
 *
 * \param[out]	swbootopt	 SW BootOPT
 * \return	void.
 */
void hx_drv_swreg_aon_get_swbootopt(uint32_t *swbootopt);

/**
 * \brief	Set BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 *
 * \param[in]	warmbootclk	 set BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 * \return	void.
 */
void hx_drv_swreg_aon_set_bl_warmbootclk(SWREG_AON_WARMBOOTDISPLL_BLCHG_E warmbootclk);
/**
 * \brief	Get BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 *
 * \param[out]	warmbootclk	 Get BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 * \return	void.
 */
void hx_drv_swreg_aon_get_bl_warmbootclk(SWREG_AON_WARMBOOTDISPLL_BLCHG_E *warmbootclk);


/**
 * \brief	Set CoreVoltage for Clock tree
 *
 * \param[in]	corev	 set Core Voltage for clock tree
 * \return	void.
 */
void hx_drv_swreg_aon_set_corevolt_clktree(SWREG_AON_COREVOLT_E corev);
/**
 * \brief	Get CoreVoltage for Clock tree
 *
 * \param[out]	corev	 get Core Voltage for clock tree
 * \return	void.
 */
void hx_drv_swreg_aon_get_corevolt_clktree(SWREG_AON_COREVOLT_E *corev);

/**
 * \brief	Set PMU Wake up BL cpu clk when Warmboot BOOTROM dis PLL
 *
 * \param[in]	pll_freq	 BL cpu PMU wake up PLL frequency
 * \param[in]	cm55m_div	 BL cpu PMU wake up CM55M PLL Div
 * \param[in]	cm55s_div	 BL cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void hx_drv_swreg_aon_set_bl_pmuwakeup_freq(SCU_PLL_FREQ_E pll_freq, SCU_HSCCLKDIV_E cm55m_div, SCU_LSCCLKDIV_E cm55s_div);

/**
 * \brief	Get PMU Wake up BL cpu clk
 *
 * \param[out]	pll_freq	 BL cpu PMU wake up PLL frequency
 * \param[out]	cm55m_div	 BL cpu PMU wake up CM55M PLL Div
 * \param[out]	cm55s_div	 BL cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void hx_drv_swreg_aon_get_bl_pmuwakeup_freq(SCU_PLL_FREQ_E *pll_freq, SCU_HSCCLKDIV_E *cm55m_div, SCU_LSCCLKDIV_E *cm55s_div);

/**
 * \brief	Set BL PLL Frequency
 *
 * \param[in]	freq	 PLL Frequency
 * \return	void.
 */
void hx_drv_swreg_aon_set_bl_pllfreq(uint32_t freq);

/**
 * \brief	Get BL PLL Frequency
 *
 * \param[out]	freq	 PLL Frequency
 * \return	void.
 */
void hx_drv_swreg_aon_get_bl_pllfreq(uint32_t *freq);

/**
 * \brief	Record Colboot DC Volt
 *
 * \param[in]	volt	 trim voltage val
 * \return	void.
 */
void hx_drv_swreg_aon_set_coldboot_dcvolt(uint8_t volt);

/**
 * \brief	Get Colboot DC Volt
 *
 * \param[out]	volt	 trim voltage val
 * \return	void.
 */
void hx_drv_swreg_aon_get_coldboot_dcvolt(uint8_t *volt);

/**
 * \brief	Record Colboot PD Volt
 *
 * \param[in]	volt	 trim voltage val
 * \return	void.
 */
void hx_drv_swreg_aon_set_coldboot_pdvolt(uint8_t volt);

/**
 * \brief	Get Colboot PD Volt
 *
 * \param[out]	volt	 trim voltage val
 * \return	void.
 */
void hx_drv_swreg_aon_get_coldboot_pdvolt(uint8_t *volt);


/**
 * \brief	Set LSC No PLL
 *
 * \param[in]	nopll	 PLL or not in LSC
 * \return	void.
 */
void hx_drv_swreg_aon_set_lscnopll(SWREG_AON_LSCPLL_E nopll);

/**
 * \brief	Get LSC No PLL
 *
 * \param[out]	nopll	  PLL or not in LSC
 * \return	void.
 */
void hx_drv_swreg_aon_get_lscnopll(SWREG_AON_LSCPLL_E *nopll);

/**
 * \brief	Set RC Trim Type
 *
 * \param[in]	type	 TRIM Type
 * \param[in]	trim_val	 TRIM value
 * \return	void.
 */
void hx_drv_swreg_aon_set_rctrim(SWREG_AON_RCTRIM_TYPE_E type, uint8_t trim_val);

/**
 * \brief	Get RC Trim
 *
 * \param[in]	type	 TRIM Type
 * \param[out]	trim_val	 TRIM value
 * \return	void.
 */
void hx_drv_swreg_aon_get_rctrim(SWREG_AON_RCTRIM_TYPE_E type, uint8_t *trim_val);

/**
 * \brief	Set DC Trim Volt Reload or not when warm boot
 *
 * \param[in]	policy	 Reload Trim DC or not
 * \return	void.
 */
void hx_drv_swreg_aon_set_reload_trimdcvolt(SWREG_AON_WARMBOOT_RELOAD_TRIM_DCVOLT_E policy);

/**
 * \brief	Get DC Trim Volt Reload or not when warm boot
 *
 * \param[out]	policy	Reload Trim DC or not
 * \return	void.
 */
void hx_drv_swreg_aon_get_reload_trimdcvolt(SWREG_AON_WARMBOOT_RELOAD_TRIM_DCVOLT_E *policy);

/**
 * \brief	Set Modem OTA flag
 *
 * \param[in]	cfg	 Modem OTA flag
 * \return	void.
 */
void hx_drv_swreg_aon_set_modemota_flag(SWREG_AON_MODEMOTA_E cfg);

/**
 * \brief	Get Modem OTA flag
 *
 * \param[out]	cfg	 Modem OTA flag
 * \return	void.
 */
void hx_drv_swreg_aon_get_modemota_flag(SWREG_AON_MODEMOTA_E *cfg);

/**
 * \brief	Get Firmware Version
 *
 * \param[out]	version	 Firmware Version
 * \return	void.
 */
void hx_drv_swreg_aon_get_fw_version(uint32_t *version);
/** @} */

#endif /* DRIVERS_INC_HX_DRV_SWREG_AON_H_ */
