
/*
 * system_ctrl.c
 *
 *  Created on: 2021/10/24
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
#include "veneer_sys_ctrl.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
/**
 * \brief	get U55 State
 *
 * \param[out]	state	 U55S State
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_get_u55_state(SCU_U55_STATE_E *state)
{
	return hx_drv_scu_get_u55_state(state);
}

/**
 * \brief	get  U55_PORPL The Power-On-Reset Privilege Level (PORPL)
 *
 * \param[out]	data	U55_PORPL
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_get_U55_PORPL(SCU_U55_PORPL_E *data)
{
	return hx_drv_scu_get_U55_PORPL(data);
}

/**
 * \brief	get CM55S State
 *
 * \param[out]	state	 CM55S State
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_get_cm55s_state(SCU_CM55S_STATE_E *state)
{
	return hx_drv_scu_get_cm55s_state(state);
}

/**
 * \brief	set I2S mode
 *
 * \param[in]	mode	 I2S mode
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_I2S_mode(SCU_I2S_MODE_E mode)
{
	return hx_drv_scu_set_I2S_mode(mode);
}

/**
 * \brief	get I2S mode
 *
 * \param[out]	mode	 I2S mode
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_get_I2S_mode(SCU_I2S_MODE_E *mode)
{
	return hx_drv_scu_get_I2S_mode(mode);
}

/**
 * \brief	set PDM IP Selection
 *
 * \param[in]	mode	 PDM Selection
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_PDM_IP(SCU_PDM_IP_E mode)
{
	return hx_drv_scu_set_PDM_IP(mode);
}

/**
 * \brief	get PDM IP Selection
 *
 * \param[out]	mode	 PDM Selection
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_get_PDM_IP(SCU_PDM_IP_E *mode)
{
	return hx_drv_scu_get_PDM_IP(mode);
}

/**
 * \brief	set XIP Enable
 *
 * \param[in]	data	XIP Enable
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_xip_en(uint8_t data)
{
	return hx_drv_scu_set_xip_en(data);
}

/**
 * \brief	set OSPI XIP enable
 *
 * \param[in]	enable	 OSPI XIP enable
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_ospi_xip_en(uint8_t enable)
{
	return hx_drv_scu_set_ospi_xip_en(enable);
}

/**
 * \brief	set OSPI slave
 *
 * \param[in]	data	 OSPI slave
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_ospi_slave(uint8_t data)
{
	return hx_drv_scu_set_ospi_slave(data);
}

/**
 * \brief	set OSPI ResetN
 *
 * \param[in]	normal	 1: normal, 0: reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_ospi_resetn(uint8_t normal)
{
	return hx_drv_scu_set_ospi_resetn(normal);
}

/**
 * \brief	set VAD D PCM Selection
 *
 * \param[in]	data	 VAD D PCM Sel
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_vad_d_pcm_sel(SCU_VAD_PCM_DATA_SEL_E data)
{
	return hx_drv_scu_set_vad_d_pcm_sel(data);
}

/**
 * \brief	set SDIO_ITCFVAL
 *
 * \param[in]	data	 SDIO_ITCFVAL
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_sdio_itcfval(uint32_t data)
{
	return hx_drv_scu_set_sdio_itcfval(data);
}

/**
 * \brief	set SDIO_ITCFMUL
 *
 * \param[in]	data	 SDIO_ITCFMUL
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_sdio_itcfmul(uint32_t data)
{
	return hx_drv_scu_set_sdio_itcfmul(data);
}

/**
 * \brief	set SDIO_ITCFSELL
 *
 * \param[in]	data	 SDIO_ITCFSEL
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))  SCU_ERROR_E veneer_sys_set_sdio_itcfsel(uint32_t data)
{
	return hx_drv_scu_set_sdio_itcfsel(data);
}


/**
 * \brief	get  Version
 *
 * \param[out]	version1	version
 * \param[out]	version2	version
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_get_version(uint32_t *version1, uint32_t *version2)
{
	return hx_drv_scu_get_version(version1, version2);
}

/**
 * \brief	set PA1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver Pull Disable 
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PA1_pinmux(SCU_PA1_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PA1_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PA0 pin mux
 *
 * \param[in]	pinmux	pin mux,
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver Pull Disable
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PA0_pinmux(SCU_PA0_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PA0_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set PA2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver I2CM,UART Pull enable/High. 
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PA2_pinmux(SCU_PA2_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PA2_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set PA3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PA3_pinmux(SCU_PA3_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PA3_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set SB Sensor i2cm Pin Mux
 *
 * \param[in]	i2cm_sda	 SB Sensor i2cm SDA Pin Mux
 * \param[in]	i2cm_scl	 SB Sensor i2cm SDL Pin Mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_sensor_i2cm_pinmux(SCU_SEN_I2CM_SDA_PINMUX_E i2cm_sda, SCU_SEN_I2CM_SCL_PINMUX_E i2cm_scl)
{
	hx_drv_scu_set_SEN_I2C_MST_SDA_pinmux(i2cm_sda);
	hx_drv_scu_set_SEN_I2C_MST_SCL_pinmux(i2cm_scl);
	return SCU_NO_ERROR;
}


/**
 * \brief	set SEN_FAE pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_FAE_pinmux(SCU_SEN_FAE_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_FAE_pinmux(pinmux);
}


/**
 * \brief	set SEN_TRIG pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_TRIG_pinmux(SCU_SEN_TRIG_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_TRIG_pinmux(pinmux);
}

/**
 * \brief	set SEN_INT pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_INT_pinmux(SCU_SEN_INT_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_INT_pinmux(pinmux);
}

/**
 * \brief	set SEN_XSLEEP pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_XSLEEP_pinmux(SCU_SEN_XSLEEP_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_XSLEEP_pinmux(pinmux);
}


/**
 * \brief	set SEN_MCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_MCLK_pinmux(SCU_SEN_MCLK_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_MCLK_pinmux(pinmux);
}

/**
 * \brief	set SEN_GPIO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_sensorgpio_pinmux(SCU_SEN_GPIO_PINMUX_E pinmux)
{
	return hx_drv_scu_set_SEN_GPIO_pinmux(pinmux);
}

/**
 * \brief	set SEN_CSW0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_CSW0_pinmux(SCU_SEN_CSW0_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_CSW0_pinmux(pinmux);
}

/**
 * \brief	set SEN_CSW1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_CSW1_pinmux(SCU_SEN_CSW1_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_CSW1_pinmux(pinmux);
}

/**
 * \brief	set SEN_PCLKO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_PCLKO_pinmux(SCU_SEN_PCLK_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_PCLKO_pinmux(pinmux);
}


/**
 * \brief	set SEN_D0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D0_pinmux(SCU_SEN_D0_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D0_pinmux(pinmux);
}

/**
 * \brief	set SEN_D1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D1_pinmux(SCU_SEN_D1_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D1_pinmux(pinmux);
}

/**
 * \brief	set SEN_D2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D2_pinmux(pinmux);
}

/**
 * \brief	set SEN_D3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D3_pinmux(SCU_SEN_D3_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D3_pinmux(pinmux);
}

/**
 * \brief	set SEN_D4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D4_pinmux(SCU_SEN_D4_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D4_pinmux(pinmux);
}

/**
 * \brief	set SEN_D5 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D5_pinmux(SCU_SEN_D5_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D5_pinmux(pinmux);
}

/**
 * \brief	set SEN_D6 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D6_pinmux(SCU_SEN_D6_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D6_pinmux(pinmux);
}

/**
 * \brief	set SEN_D7 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_D7_pinmux(SCU_SEN_D7_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_D7_pinmux(pinmux);
}

/**
 * \brief	set SEN_FVALID pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_FVALID_pinmux(SCU_SEN_FVALID_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_FVALID_pinmux(pinmux);
}

/**
 * \brief	set SEN_LVALID pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SEN_LVALID_pinmux(SCU_SEN_LVALID_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SEN_LVALID_pinmux(pinmux);
}

/**
 * \brief	set SDIO_SDCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SDIO_SDCLK_pinmux(SCU_SDIO_SDCLK_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SDIO_SDCLK_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set SDIO_CMD pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SDIO_CMD_pinmux(SCU_SDIO_CMD_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SDIO_CMD_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set SDIO_DAT0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SDIO_DAT0_pinmux(SCU_SDIO_DATA0_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SDIO_DAT0_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set SDIO_DAT1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SDIO_DAT1_pinmux(SCU_SDIO_DATA1_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SDIO_DAT1_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set SDIO_DAT2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SDIO_DAT2_pinmux(SCU_SDIO_DATA2_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SDIO_DAT2_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set SDIO_DAT3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SDIO_DAT3_pinmux(SCU_SDIO_DATA3_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SDIO_DAT3_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set SW_DATA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SW_DATA_pinmux(SCU_SW_DATA_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SW_DATA_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set SW_CLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SW_CLK_pinmux(SCU_SW_CLK_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_SW_CLK_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set PB2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB2_pinmux(SCU_PB2_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB2_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set PB3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB3_pinmux(SCU_PB3_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB3_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB4_pinmux(SCU_PB4_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB4_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB5 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB5_pinmux(SCU_PB5_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB5_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB6 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB6_pinmux(SCU_PB6_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB6_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB7 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB7_pinmux(SCU_PB7_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB7_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB8 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB8_pinmux(SCU_PB8_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB8_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB9 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB9_pinmux(SCU_PB9_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB9_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB10 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB10_pinmux(SCU_PB10_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB10_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB11 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB11_pinmux(SCU_PB11_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB11_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PC3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PC3_pinmux(SCU_PC3_PINMUX_E pinmux)
{
    return hx_drv_scu_set_PC3_pinmux(pinmux);
}

/**
 * \brief	set PC4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PC4_pinmux(SCU_PC4_PINMUX_E pinmux)
{
    return hx_drv_scu_set_PC4_pinmux(pinmux);
}
#if 0//remove SCU 0524
/**
 * \brief	set OSPI pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_OSPI_pinmux(SCU_OSPI_PINMUX_E pinmux)
{
    return hx_drv_scu_set_OSPI_pinmux(pinmux);
}
#endif
/**
 * \brief	set I2CM_SCLSDA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_I2CM_SCLSDA_pinmux(SCU_I2CM_SCLSDA_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_I2CM_SCLSDA_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SDIO, UART, I2CM pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB0_pinmux(SCU_PB0_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB0_pinmux(pinmux, autocfg_pullcfg);
}

/**
 * \brief	set PB1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_PB1_pinmux(SCU_PB1_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_PB1_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set UART pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_UART_RX_TX_CTS_RTS_pinmux(SCU_UART_PINMUX_E pinmux, uint8_t autocfg_pullcfg)
{
    return hx_drv_scu_set_UART_RX_TX_CTS_RTS_pinmux(pinmux, autocfg_pullcfg);
}


/**
 * \brief	set SPIS_DO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SPIS_DO_pinmux(SCU_SPIS_DO_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SPIS_DO_pinmux(pinmux);
}

/**
 * \brief	set SPIS_DI pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SPIS_DI_pinmux(SCU_SPIS_DI_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SPIS_DI_pinmux(pinmux);
}

/**
 * \brief	set SPIS_SCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SPIS_SCLK_pinmux(SCU_SPIS_SCLK_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SPIS_SCLK_pinmux(pinmux);
}

/**
 * \brief	set SPIS_CS pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_SPIS_CS_pinmux(SCU_SPIS_CS_PINMUX_E pinmux)
{
    return hx_drv_scu_set_SPIS_CS_pinmux(pinmux);
}


__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_I2C_SLV_share_enable(SCU_I2CSLV_SHARE_E enable)
{
    return hx_drv_scu_set_I2C_SLV_share_enable(enable);
}


__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_I3C_SLV_share_enable(SCU_I3CSLV_SHARE_E enable)
{
    return hx_drv_scu_set_I3C_SLV_share_enable(enable);
}

/**
 * \brief	set DP related SW reset
 *
 * \param[in]	cfg	DP Related SW Reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_DP_SWReset(SCU_DP_SWRESET_T *cfg)
{
	SCU_DP_SWRESET_T cfg_S;
	memcpy(&cfg_S, cfg, sizeof(SCU_DP_SWRESET_T));
    return hx_drv_scu_set_DP_SWReset(cfg_S);
}

__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_get_DP_SWReset(SCU_DP_SWRESET_T *cfg)
{
    return hx_drv_scu_get_DP_SWReset(cfg);
}
/**
 * \brief	set WDT control owner
 *
 * \param[in]	id 0 or 1
 * \param[in]	ctrl enable by CPU or PMU
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_wdt_ctrl(uint32_t id, SCU_WDTCTRL_E ctrl)
{
	return hx_drv_scu_set_wdt_ctrl(id, ctrl);
}

/**
 * \brief	set Timer control owner
 *
 * \param[in]	id 0 ~ 8
 * \param[in]	ctrl enable by CPU or PMU
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_sys_set_timer_ctrl(uint32_t id, SCU_TIMERCTRL_E ctrl)
{
	return hx_drv_scu_set_timer_ctrl(id, ctrl);
}

#ifndef BOOT_USED
/**
 * \brief	set Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[in]	channel	 capture channel
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E channel)
{
	return hx_drv_scu_set_audio_capture_channel(audio_type, channel);
}

/**
 * \brief	get Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[out]	channel	 capture channel
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_get_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E *channel)
{
	return hx_drv_scu_get_audio_capture_channel(audio_type, channel);
}
#endif

#ifndef BOOT_USED
/**
 * \brief	set ADC LV Reset
 *
 * \param[in]	cfg	 reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_lsc_adclv_reset(SCU_SWRESET_E cfg)
{
	return hx_drv_scu_set_lsc_adclv_reset(cfg);
}
/**
 * \brief	set ADC HV Reset
 *
 * \param[in]	cfg	 reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_lsc_adchv_reset(SCU_SWRESET_E cfg)
{
	return hx_drv_scu_set_lsc_adchv_reset(cfg);
}
#endif

#ifndef BOOT_USED
/**
 * \brief	set SRSTN pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_SRSTN_pinmux(SCU_PAD_SRSTN_PINMUX_E pinmux)
{
	return hx_drv_scu_set_SRSTN_pinmux(pinmux);
}

/**
 * \brief	set SWCLK TCK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_SWCLKTCK_pinmux(SCU_PAD_SWCLK_TCK_PINMUX_E pinmux)
{
	return hx_drv_scu_set_SWCLKTCK_pinmux(pinmux);
}
/**
 * \brief	set SWDIO TMS pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_SWDIO_TMS_pinmux(SCU_PAD_SWDIO_TMS_PINMUX_E pinmux)
{
	return hx_drv_scu_set_SWDIO_TMS_pinmux(pinmux);
}
#endif

/**
 * \brief	get All Pin Mux Configuration
 *
 * \param[out]	pinmux_cfg	 all pin mux config
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_get_all_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	return hx_drv_scu_get_all_pinmux_cfg(pinmux_cfg);
}
/**
 * \brief	set All Pin Mux Configuration
 *
 * \param[in]	pinmux_cfg	 reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_all_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg, uint8_t autocfg_pullcfg)
{
	return hx_drv_scu_set_all_pinmux_cfg(pinmux_cfg, autocfg_pullcfg);
}

#if(IC_VERSION >= 30)
/**
 * \brief	set PD LSC PDM CLK Source
 *
 * \param[in]	pdmsrc_clk	 PD LSC PDM CLK Configuration
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E pdmsrc_clk)
{
	return hx_drv_scu_set_pdlsc_pdmck_src(pdmsrc_clk);
}
#endif

/**
 * \brief	Get PLL Frequency
 *
 * \param[out]	freq	 PLL Frequency
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_pllfreq(uint32_t *freq)
{
	hx_drv_swreg_aon_get_pllfreq(freq);
}

/**
 * \brief	Get PMU Wake up bootrom cpu clk
 *
 * \param[out]	pll_freq	 bootrom cpu PMU wake up PLL frequency
 * \param[out]	cm55m_div	 bootrom cpu PMU wake up CM55M PLL Div
 * \param[out]	cm55s_div	 bootrom cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void  veneer_get_pmuwakeup_freq(SCU_PLL_FREQ_E *pll_freq, SCU_HSCCLKDIV_E *cm55m_div, SCU_LSCCLKDIV_E *cm55s_div)
{
	hx_drv_swreg_aon_get_pmuwakeup_freq(pll_freq, cm55m_div, cm55s_div);
}

/**
 * \brief	Set Sensor Init status
 *
 * \param[in]	init	 Sensor Init status
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_sensorinit(SWREG_AON_SENSORINIT_E init)
{
	hx_drv_swreg_aon_set_sensorinit(init);
}


/**
 * \brief	Get Sensor Init status
 *
 * \param[out]	init	 Sensor Init status
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_sensorinit(SWREG_AON_SENSORINIT_E *init)
{
	hx_drv_swreg_aon_get_sensorinit(init);
}


#ifndef BOOT_USED
/**
 * \brief	Set VMUTE releated control
 *
 * \param[in]	ctrl	 VMUTE releated control
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_vmute(SCU_VMUTE_CFG_T *ctrl)
{
	return hx_drv_scu_set_vmute(ctrl);
}
#endif


/**
 * \brief	Set BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 *
 * \param[in]	warmbootclk	 set BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_bl_warmbootclk(SWREG_AON_WARMBOOTDISPLL_BLCHG_E warmbootclk)
{
	hx_drv_swreg_aon_set_bl_warmbootclk(warmbootclk);
}
/**
 * \brief	Get BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 *
 * \param[out]	warmbootclk	 Get BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_bl_warmbootclk(SWREG_AON_WARMBOOTDISPLL_BLCHG_E *warmbootclk)
{
	hx_drv_swreg_aon_get_bl_warmbootclk(warmbootclk);
}


/**
 * \brief	Set CoreVoltage for Clock tree
 *
 * \param[in]	corev	 set Core Voltage for clock tree
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_corevolt_clktree(SWREG_AON_COREVOLT_E corev)
{
	hx_drv_swreg_aon_set_corevolt_clktree(corev);
}
/**
 * \brief	Get CoreVoltage for Clock tree
 *
 * \param[out]	corev	 get Core Voltage for clock tree
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_corevolt_clktree(SWREG_AON_COREVOLT_E *corev)
{
	hx_drv_swreg_aon_get_corevolt_clktree(corev);
}

/**
 * \brief	Set PMU Wake up BL cpu clk when Warmboot BOOTROM dis PLL
 *
 * \param[in]	pll_freq	 BL cpu PMU wake up PLL frequency
 * \param[in]	cm55m_div	 BL cpu PMU wake up CM55M PLL Div
 * \param[in]	cm55s_div	 BL cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_bl_pmuwakeup_freq(SCU_PLL_FREQ_E pll_freq, SCU_HSCCLKDIV_E cm55m_div, SCU_LSCCLKDIV_E cm55s_div)
{
	hx_drv_swreg_aon_set_bl_pmuwakeup_freq(pll_freq, cm55m_div, cm55s_div);
}

/**
 * \brief	Get PMU Wake up BL cpu clk
 *
 * \param[out]	pll_freq	 BL cpu PMU wake up PLL frequency
 * \param[out]	cm55m_div	 BL cpu PMU wake up CM55M PLL Div
 * \param[out]	cm55s_div	 BL cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_bl_pmuwakeup_freq(SCU_PLL_FREQ_E *pll_freq, SCU_HSCCLKDIV_E *cm55m_div, SCU_LSCCLKDIV_E *cm55s_div)
{
	hx_drv_swreg_aon_get_bl_pmuwakeup_freq(pll_freq, cm55m_div, cm55s_div);
}

/**
 * \brief	Set BL PLL Frequency
 *
 * \param[in]	freq	 PLL Frequency
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_bl_pllfreq(uint32_t freq)
{
	hx_drv_swreg_aon_set_bl_pllfreq(freq);
}

/**
 * \brief	Get BL PLL Frequency
 *
 * \param[out]	freq	 PLL Frequency
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_bl_pllfreq(uint32_t *freq)
{
	hx_drv_swreg_aon_get_bl_pllfreq(freq);
}


/**
 * \brief	Jump to 2ndloader. Receive intoupg command
 *
 * \return	void.
 */
typedef void (*funcptr_void)(void);

__attribute__((cmse_nonsecure_entry)) void veneer_jump_2ndloader()
{
	funcptr_void ResetHandler;

	hx_drv_swreg_aon_set_ota_flag(SWREG_AON_OTA_YES_FLAG);
	ResetHandler = (funcptr_void)(*((uint32_t *)((BOOTLOADER_BASE) + 4U)));
	ResetHandler();
}

/**
 * \brief	Set SRAM Voltage Control
 *
 * \param[in]	ctrl	 SRAM Voltage control
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_sram_volt(SCU_COREVOLT_SRAM_E ctrl)
{
	hx_drv_scu_set_sram_volt(ctrl);
}
/**
 * \brief	Get SRAM Voltage Control
 *
 * \param[out]	ctrl	 SRAM Voltage control
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_sram_volt(SCU_COREVOLT_SRAM_E *ctrl)
{
	hx_drv_scu_get_sram_volt(ctrl);
}


/**
 * \brief	Record Colboot DC Volt
 *
 * \param[in]	volt	 trim voltage val
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_coldboot_dcvolt(uint8_t volt)
{
	hx_drv_swreg_aon_set_coldboot_dcvolt(volt);
}

/**
 * \brief	Get Colboot DC Volt
 *
 * \param[out]	volt	 trim voltage val
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_coldboot_dcvolt(uint8_t *volt)
{
	hx_drv_swreg_aon_get_coldboot_dcvolt(volt);
}

/**
 * \brief	Record Colboot PD Volt
 *
 * \param[in]	volt	 trim voltage val
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_coldboot_pdvolt(uint8_t volt)
{
	hx_drv_swreg_aon_set_coldboot_pdvolt(volt);
}

/**
 * \brief	Get Colboot PD Volt
 *
 * \param[out]	volt	 trim voltage val
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_coldboot_pdvolt(uint8_t *volt)
{
	hx_drv_swreg_aon_get_coldboot_pdvolt(volt);
}

/**
 * \brief	set QSPI related SW reset
 *
 * \param[in]	cfg	QSPI Related SW Reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_set_QSPI_SWReset(SCU_SWRESET_E cfg)
{
	hx_drv_scu_set_QSPI_SWReset(cfg);
}

#ifndef BOOT_USED
/**
 * \brief	Check Memory Space attribute
 *
 * \param[in]	start_addr    start address for check memory attribute
 * \param[in]	size    memory size for check memory attribute
 * \param[in]	attr    memory attribute that app expected
 * \return	SCU_ERROR_E. memory space is as app expected or not
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_check_mem_attr(uint32_t start_addr, uint32_t size, SCU_MEM_ATTR_E attr)
{
	return hx_drv_scu_check_mem_attr(start_addr,  size,  attr);
}
#endif

/**
 * \brief	set CM55S State
 *
 * \param[in]	state	 CM55S State
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_cm55s_state(SCU_CM55S_STATE_E state)
{
	return hx_drv_scu_set_cm55s_state(state);
}

/**
 * \brief	set CM55S_CPUWAIT
 *
 * \param[in]	data	CM55S_CPUWAIT
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_CM55S_CPUWAIT(SCU_CM55_CPUWAIT_E data)
{
	return hx_drv_scu_set_CM55S_CPUWAIT(data);
}

/**
 * \brief	Get CM55S Re-Reset flag after PMU wakeup
 *
 * \param[out]	cfg	 re-reset flag configuration
 * \return	void.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_rerest_cm55s_flag(SWREG_AON_PMUWAKE_CM55S_RERESET_E *cfg)
{
	hx_drv_swreg_aon_get_rerest_cm55s_flag(cfg);
}

#ifndef BOOT_USED
/**
 * \brief	Set MIPI Control
 *
 * \param[in]	ctrl	 MIPI control
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))SCU_ERROR_E veneer_set_mipi_ctrl(SCU_MIPI_CTRL_E ctrl)
{
	return hx_drv_scu_set_mipi_ctrl(ctrl);
}
/**
 * \brief	Get MIPI Control
 *
 * \param[out]	ctrl	 MIPI control
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry))SCU_ERROR_E veneer_get_mipi_ctrl(SCU_MIPI_CTRL_E *ctrl)
{
	return hx_drv_scu_get_mipi_ctrl(ctrl);
}

/**
 * \brief	Get Version
 *
 * \param[out]	version	 FW version
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) void veneer_get_fw_version(uint32_t *version)
{
	hx_drv_swreg_aon_get_fw_version(version);
}

/**
 * \brief	set LSC related SW reset
 *
 * \param[in]	cfg	LSC Related SW Reset
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_LSC_SWReset(SCU_LSC_SWRESET_T *cfg)
{
	SCU_LSC_SWRESET_T cfg_S;
	memcpy(&cfg_S, cfg, sizeof(SCU_LSC_SWRESET_T));

	return hx_drv_scu_set_LSC_SWReset(cfg_S);
}

/**
 * \brief	set MIPI TX Phy RX clock source
 *
 * \param[in]	txphyrxclksrc	 MIPI TX Phy RX Clock source
 * \return	SCU_ERROR_E.
 */
__attribute__((cmse_nonsecure_entry)) SCU_ERROR_E veneer_set_MIPITXPHY_RXClkSRC(SCU_TXPHY_RCLK_SRC_SEL_E *txphyrxclksrc)
{
	SCU_TXPHY_RCLK_SRC_SEL_E txphyrxclksrc_s;
	memcpy(&txphyrxclksrc_s, txphyrxclksrc, sizeof(SCU_TXPHY_RCLK_SRC_SEL_E));

	return hx_drv_scu_set_MIPITXPHY_RXClkSRC(txphyrxclksrc_s);
}
#endif

#endif //#if !defined(HX_TFM)

