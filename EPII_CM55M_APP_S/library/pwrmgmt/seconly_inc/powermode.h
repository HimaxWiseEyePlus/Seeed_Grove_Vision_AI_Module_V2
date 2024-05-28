/**
 ********************************************************************************************
 *  @file      powermode.h
 *  @details   This file contains all power mode related function
 *  @author    himax/902447
 *  @version   V1.0.0
 *  @date      8-Aug-2021
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
/**
 * \defgroup	PWRMGMT_LIB	Power Manager Library
 * \ingroup	PWRMGMT_LIB
 * \brief	Power Manager library Declaration
 */

#ifndef LIBRARY_PWRMGMT_POWERMODE_H_
#define LIBRARY_PWRMGMT_POWERMODE_H_
#include <stdio.h>
#include <string.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"
#include "hx_drv_scu.h"
#include "hx_drv_pmu.h"
#include "BITOPS.h"
#include "powermode_export.h"
#include "hx_drv_swreg_lsc.h"
/**
 * \defgroup	PWRMGMT_LIB	Sensor Control Driver
 * \ingroup		PWRMGMT_LIB
 * \brief	Power Management Library Declaration
 * \details Application can use Power Management API and data structure(PM_CFG_T) to control WE-2 Power Management Unit\n
 */

/****************************************************
 * Function Declaration                             *
 ***************************************************/
/**
 * \defgroup	PWRMGMT_LIB_FUNCDLR	Power Manager library Function Declaration
 * \ingroup	PWRMGMT_LIB
 * \brief	Contains declarations of Power Manager library functions.
 * @{
 */
#ifdef CM55_BIG
/**
 * \brief	Initialization of power manager library
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_init(void);

/**
 * \brief	De-Initialization of power manager library
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_deinit(void);


/**
 * \brief	check PMU done capture frame done when PMU to ALLON.
 *
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_pmu_dpdone_check(sensordplib_waitpmudmadone_t cb_fun);

/**
 * \brief	hand over control from PMU to CPU. Then CPU can control everything
 *
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_ctrl_fromPMUtoCPU(sensordplib_pmutocpuctrl_t cb_fun);


/**
 * \brief	Configure the power manager library for specific power mode
 *
 * \param[in] PM_CFG_T		configuration of power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_get_defcfg_bymode(void *aCfg, PM_CFG_PWR_MODE_E mode);

/**
 * \brief	Configure the power manager library for specific power mode
 *
 * \param[in] PM_CFG_T		configuration of power mode
 * \param[in] cb_fun		DP initial before enter PMU only use when DP in NS side
 * \param[in] mode		power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_cfg_set(void *aCfg, sensordplib_pmudpinit_t cb_fun, PM_CFG_PWR_MODE_E mode);

/**
 * \brief	Get Current power mode in the power manager library
 *
 * \param[out] PM_CFG_T		configuration of power mode
 * \param[in] mode		power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_cfg_get(void *aCfg, PM_CFG_PWR_MODE_E mode);

/**
 * \brief	Get Current power mode in the power manager library
 *
 * \param[out] mode		configuration of power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_mode_get(PM_CFG_PWR_MODE_E *mode);

/**
 * \brief	The function is used to trigger to enter PMU mode. It is handled by PMU library hx_lib_pm_mode_set so application doesn't need take care it.
 *
 * \param[in] hsc_cfg		hsc clock cfg in PMU mode
 * \param[in] lsc_cfg		lsc clock cfg in PMU mode
 * \param[in] clkparactrl	clock parameter reference app or PMU lib
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_trigger(SCU_PDHSC_HSCCLK_CFG_T hsc_cfg, SCU_LSC_CLK_CFG_T lsc_cfg, PM_CLK_PARA_CTRL_E clkparactrl);
#endif

/**
 * \brief	The function is used to get the PMU wake up event
 *
 * \param[out] aWakeupEvt	store the PMU wakeup event
 * \param[out] aWakeupEvt1	store the PMU wakeup event1
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_get_event(PMU_WAKEUPEVENT_E *aWakeupEvt, PMU_WAKEUPEVENT1_E *aWakeupEvt1);

/**
 * \brief	The function is used to clear the PMU wake up event flag
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_clear_event(void);
/**
 * \brief	The function is used to Force PMU to go to power state
 *
 * \param[in] state	 force power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_force_state_set(PMU_PS_STATE_E state);

/**
 * \brief	The function is used to get force power state
 *
 * \param[in] state	 get force power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_force_state_get(PMU_PS_STATE_E *state);


/**
 * \brief	The function is used to get power state
 *
 * \param[in] state	 get power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_state_get(PMU_PS_STATE_E *state);

/**
 * \brief	The function is used to deinitial some BSP driver for retention reboot. It is handled by PMU library hx_lib_pm_mode_set so application doesn't need take care it.
 *
 */
void hx_lib_pm_deinit_bsp_driver();

/**
 * \brief	The function is used to get pmu ext interrupt status
 *
 * \param[in] type	 get pmu ext interrupt status type
 * \param[out] status	 pmu ext interrupt status
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_ext_int_get(PM_EXT_INT_STATUS_TYPE_E type, uint8_t *status);


/**
 * \brief	The function is used to Set DC decrease level base on Trim
 *
 * \param[in] decrease_level	 decrease level base on DC Trim
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_dc_volt_decrease(uint8_t decrease_level);

/**
 * \brief	The function is used to Set PD decrease level base on Trim
 *
 * \param[in] decrease_level	 decrease level base on PD Trim
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_pd_volt_decrease(uint8_t decrease_level);

/**
 * \brief	The function is used to Set MIPI TX off
 *
 * \retval	void
 */
void hx_lib_pm_mipi_txoff_lvofhv();

/**
 * \brief	The function is used to Set Sensor Control to PMU
 *
 * \retval	void
 */
void hx_lib_pm_sensor_control();

/**
 * \brief	The function is used to CM55S Control DP case PMU to DC
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_cm55s_ctrldp_to_DC();

/**
 * \brief	The function is used to Set SW Wakeup from PMU Event
 * \param[in] sw_wakeup_event	 set sw trigger pmu wakeup event
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_set_sw_wakeup_event(SW_FORCE_WAKEUPEVENT_E sw_wakeup_event);

/**
 * \brief	The function is used to Get SW Wakeup from PMU Event
 * \param[out] sw_wakeup_event	 get sw trigger pmu wakeup event
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_get_sw_wakeup_event(SW_FORCE_WAKEUPEVENT_E *sw_wakeup_event);


/**
 * \brief	The function is used to CM55S Control DP change to RC96M
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_cm55s_ctrl_dp_changeRC96M();

/**
 * \brief	The function is used to CM55S Control DP change to Original Clock
 * \param[in] change_dpclk	 change DP clock to original
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E hx_lib_pm_cm55s_ctrl_dp_changeNoRC96M(uint32_t change_dpclk);

void hx_lib_pm_sync_register_delay(uint32_t tick_cnt);
/** @} */


#endif /* LIBRARY_PWRMGMT_POWERMODE_H_ */
