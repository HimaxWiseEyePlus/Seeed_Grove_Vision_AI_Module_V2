/*
 * veneer_power_ctrl.c
 *
 *  Created on: 2021��蕭12嚙踝蕭嚙�9嚙踐��
 *      Author: 902447
 */
#if !defined(HX_TFM)
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include <stdio.h>
#include "stdint.h"
#include "arm_cmse.h"
#include "veneer_power_ctrl.h"
#include "hx_drv_scu.h"
#include "hx_drv_pmu.h"
#include "powermode.h"

#ifdef CM55_BIG

/**
 * \brief Sensor DP library PMU to CPU control
 *
 * \retval void.
 */
#if defined(__IAR_SYSTEMS_ICC__)
typedef __cmse_nonsecure_call void (*sensordplib_pmutocpuctrl_NS_t)(void);
#else
typedef void (*sensordplib_pmutocpuctrl_NS_t)(void) __attribute__((cmse_nonsecure_call));
#endif
/**
 * \brief Sensor DP library Wait DMA Done
 *
 * \retval void.
 */
#if defined(__IAR_SYSTEMS_ICC__)
typedef __cmse_nonsecure_call void (*sensordplib_waitpmudmadone_NS_t)(void);
#else
typedef void (*sensordplib_waitpmudmadone_NS_t)(void) __attribute__((cmse_nonsecure_call));
#endif
/**
 * \brief Sensor DP library Init
 *
 * \retval void.
 */
#if defined(__IAR_SYSTEMS_ICC__)
typedef __cmse_nonsecure_call void (*sensordplib_pmudpinit_NS_t)(void);
#else
typedef void (*sensordplib_pmudpinit_NS_t)(void) __attribute__((cmse_nonsecure_call));
#endif

sensordplib_waitpmudmadone_NS_t g_sensordplib_waitpmudmadoneCB = NULL;
sensordplib_pmutocpuctrl_NS_t g_sensordplib_pmutocpuctrlCB = NULL;
sensordplib_pmudpinit_NS_t g_sensordplib_pmudpinitCB = NULL;

/**
 * \brief Sensor DP library PMU to CPU control
 *
 * \retval void.
 */
void sensordplib_pmutocpuctrl_s()
{
	if(g_sensordplib_pmutocpuctrlCB !=NULL )
		g_sensordplib_pmutocpuctrlCB();
}

/**
 * \brief Sensor DP library Wait DMA Done
 *
 * \retval void.
 */
void sensordplib_waitpmudmadone_s()
{
	if(g_sensordplib_waitpmudmadoneCB !=NULL )
		g_sensordplib_waitpmudmadoneCB();
}

/**
 * \brief Sensor DP library Init
 *
 * \retval void.
 */
void sensordplib_pmudpinit_s()
{
	if(g_sensordplib_pmudpinitCB !=NULL )
		g_sensordplib_pmudpinitCB();
}
/**
 * \brief	Configure the power manager library for specific power mode
 *
 * \param[in] PM_CFG_T		configuration of power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_get_defcfg_bymode(void *aCfg, PM_CFG_PWR_MODE_E mode)
{
	return hx_lib_pm_get_defcfg_bymode(aCfg,  mode);
}

/**
 * \brief	check PMU done capture frame done when PMU to ALLON.
 *
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_pmu_dpdone_check(sensordplib_waitpmudmadone_t cb_fun)
{

	g_sensordplib_waitpmudmadoneCB = (sensordplib_waitpmudmadone_NS_t) cmse_nsfptr_create(cb_fun);

	return hx_lib_pm_pmu_dpdone_check(sensordplib_waitpmudmadone_s);
}

/**
 * \brief	hand over control from PMU to CPU. Then CPU can control everything
 *
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_ctrl_fromPMUtoCPU(sensordplib_pmutocpuctrl_t cb_fun)
{

	g_sensordplib_pmutocpuctrlCB = (sensordplib_pmutocpuctrl_NS_t) cmse_nsfptr_create(cb_fun);

	return hx_lib_pm_ctrl_fromPMUtoCPU(sensordplib_pmutocpuctrl_s);
}

/**
 * \brief	Configure the power manager library for specific power mode
 *
 * \param[in] PM_CFG_T		configuration of power mode
 * \param[in] cb_fun		DP initial before enter PMU only use when DP in NS side
 * \param[in] mode		power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_cfg_set(void *aCfg, sensordplib_pmudpinit_t cb_fun, PM_CFG_PWR_MODE_E mode)
{
	g_sensordplib_pmudpinitCB = (sensordplib_pmudpinit_NS_t) cmse_nsfptr_create(cb_fun);

	return hx_lib_pm_cfg_set(aCfg, sensordplib_pmudpinit_s, mode);
}

/**
 * \brief	Get Current power mode in the power manager library
 *
 * \param[out] PM_CFG_T		configuration of power mode
 * \param[in] mode		power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_cfg_get(void *aCfg, PM_CFG_PWR_MODE_E mode)
{
	return hx_lib_pm_cfg_get(aCfg, mode);
}

/**
 * \brief	Get Current power mode in the power manager library
 *
 * \param[out] mode		configuration of power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_mode_get(PM_CFG_PWR_MODE_E *mode)
{
	return hx_lib_pm_mode_get(mode);
}
/**
 * \brief	The function is used to trigger to enter PMU mode. It is handled by PMU library hx_lib_pm_mode_set so application doesn't need take care it.
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_trigger(SCU_PDHSC_HSCCLK_CFG_T *hsc_cfg, SCU_LSC_CLK_CFG_T *lsc_cfg, PM_CLK_PARA_CTRL_E clkparactrl)
{
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg_S;
	SCU_LSC_CLK_CFG_T lsc_cfg_S;
	memcpy(&hsc_cfg_S, hsc_cfg, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memcpy(&lsc_cfg_S, lsc_cfg, sizeof(SCU_LSC_CLK_CFG_T));
	return hx_lib_pm_trigger(hsc_cfg_S, lsc_cfg_S, clkparactrl);
}
#endif

__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_get_event(PMU_WAKEUPEVENT_E *aWakeupEvt, PMU_WAKEUPEVENT1_E *aWakeupEvt1)
{
	return hx_lib_pm_get_event(aWakeupEvt, aWakeupEvt1);
}


/**
 * \brief	The function is used to clear the PMU wake up event flag
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_clear_event(void)
{
	return hx_lib_pm_clear_event();
}
/**
 * \brief	The function is used to Force PMU to go to power state
 *
 * \param[in] state	 force power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_force_state_set(PMU_PS_STATE_E state)
{
	return hx_lib_pm_force_state_set(state);
}

/**
 * \brief	The function is used to get power state
 *
 * \param[in] state	 get power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_force_state_get(PMU_PS_STATE_E *state)
{
	return hx_lib_pm_force_state_get(state);
}

/**
 * \brief	The function is used to set dma usage cfg
 *
 * \param[in] dma_int	 dma_int cfg
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_set_dma_cfg(uint8_t dma_int)
{
	hx_drv_pmu_set_ctrl(PMU_REG_PMU_DMA_CFG, dma_int);//WDMA1
	return PM_NO_ERROR;
}

/**
 * \brief	The function is used to get pmu ext interrupt status
 *
 * \param[in] type	 get pmu ext interrupt status type
 * \param[out] status	 pmu ext interrupt status
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_ext_int_get(PM_EXT_INT_STATUS_TYPE_E type, uint8_t *status)
{
	return hx_lib_pm_ext_int_get(type, status);
}


/**
 * \brief	The function is used to Set DC decrease level base on Trim
 *
 * \param[in] decrease_level	 decrease level base on DC Trim
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry)) PM_ERROR_E veneer_pm_dc_volt_decrease(uint8_t decrease_level)
{
	return hx_lib_pm_dc_volt_decrease(decrease_level);
}

/**
 * \brief	The function is used to Set PD decrease level base on Trim
 *
 * \param[in] decrease_level	 decrease level base on PD Trim
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry)) PM_ERROR_E veneer_pm_pd_volt_decrease(uint8_t decrease_level)
{
	return hx_lib_pm_pd_volt_decrease(decrease_level);
}


/**
 * \brief	The function is used to Set DC Trim
 *
 * \param[in] trim	  DC Trim value
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry)) PM_ERROR_E veneer_pm_set_DC_volttrim(uint8_t trim)
{
	hx_drv_pmu_set_ctrl(PMU_REG_SIMO_VOUT1_SEL, trim);
	hx_drv_pmu_set_ctrl(PMU_REG_SIMO_VOUT1_SEL_N, trim);
	hx_drv_pmu_set_ctrl(PMU_REG_SIMO_VOUT1_SEL_SW, 1);
	return PM_NO_ERROR;
}

/**
 * \brief	The function is used to Get DC Trim
 *
 * \param[out] trim	  DC Trim value
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry)) PM_ERROR_E veneer_pm_get_DC_volttrim(uint8_t *trim)
{
    hx_drv_pmu_get_ctrl(PMU_REG_SIMO_VOUT1_SEL, trim);
	return PM_NO_ERROR;
}


/**
 * \brief	The function is used to Set PD Trim
 *
 * \param[in] trim	  PD Trim value
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry)) PM_ERROR_E veneer_pm_set_PD_volttrim(uint8_t trim)
{
	hx_drv_pmu_set_ctrl(PMU_REG_SIMO_VOUT1_SEL_R, trim);
	return PM_NO_ERROR;
}

/**
 * \brief	The function is used to Get PD Trim
 *
 * \param[out] trim	  PD Trim value
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry)) PM_ERROR_E veneer_pm_get_PD_volttrim(uint8_t *trim)
{
    hx_drv_pmu_get_ctrl(PMU_REG_SIMO_VOUT1_SEL_R, trim);
	return PM_NO_ERROR;
}


/**
 * \brief	The function is used to Force PMU to DC
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
__attribute__((cmse_nonsecure_entry))  PM_ERROR_E veneer_pm_force_dc()
{
	hx_lib_pm_force_state_set(PMU_PS_STATE_DC);
	return PM_NO_ERROR;
}
#endif //#if !defined(HX_TFM)
