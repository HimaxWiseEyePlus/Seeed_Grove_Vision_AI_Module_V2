/*
 * system_ctrl.h
 *
 *  Created on: 2021/10/24
 *      Author: 902447
 */

#ifndef TRUSTZONE_NSC_FUNCTION_VENEER_SYS_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_VENEER_SYS_CTRL_H_

#include "hx_drv_scu_export.h"
#include "hx_drv_swreg_aon_export.h"

/**
 * \brief	get U55 State
 *
 * \param[out]	state	 U55S State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_u55_state(SCU_U55_STATE_E *state);


/**
 * \brief	get  U55_PORPL The Power-On-Reset Privilege Level (PORPL)
 *
 * \param[out]	data	U55_PORPL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_U55_PORPL(SCU_U55_PORPL_E *data);


/**
 * \brief	get CM55S State
 *
 * \param[out]	state	 CM55S State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_cm55s_state(SCU_CM55S_STATE_E *state);

/**
 * \brief	set I2S mode
 *
 * \param[in]	mode	 I2S mode
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_I2S_mode(SCU_I2S_MODE_E mode);

/**
 * \brief	get I2S mode
 *
 * \param[out]	mode	 I2S mode
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_I2S_mode(SCU_I2S_MODE_E *mode);

/**
 * \brief	set PDM IP Selection
 *
 * \param[in]	mode	 PDM Selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PDM_IP(SCU_PDM_IP_E mode);

/**
 * \brief	get PDM IP Selection
 *
 * \param[out]	mode	 PDM Selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_PDM_IP(SCU_PDM_IP_E *mode);

/**
 * \brief	set XIP Enable
 *
 * \param[in]	data	XIP Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_xip_en(uint8_t data);

/**
 * \brief	set OSPI XIP enable
 *
 * \param[in]	enable	 OSPI XIP enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_ospi_xip_en(uint8_t enable);

/**
 * \brief	set OSPI slave
 *
 * \param[in]	data	 OSPI slave
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_ospi_slave(uint8_t data);

/**
 * \brief	set OSPI ResetN
 *
 * \param[in]	normal	 1: normal, 0: reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_ospi_resetn(uint8_t normal);

/**
 * \brief	set VAD D PCM Selection
 *
 * \param[in]	data	 VAD D PCM Sel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_vad_d_pcm_sel(SCU_VAD_PCM_DATA_SEL_E data);

/**
 * \brief	set SDIO_ITCFVAL
 *
 * \param[in]	data	 SDIO_ITCFVAL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_sdio_itcfval(uint32_t data);

/**
 * \brief	set SDIO_ITCFMUL
 *
 * \param[in]	data	 SDIO_ITCFMUL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_sdio_itcfmul(uint32_t data);

/**
 * \brief	set SDIO_ITCFSELL
 *
 * \param[in]	data	 SDIO_ITCFSEL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_sdio_itcfsel(uint32_t data);


/**
 * \brief	get  Version
 *
 * \param[out]	version1	version
 * \param[out]	version2	version
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_version(uint32_t *version1, uint32_t *version2);

/**
 * \brief	set PA1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver Pull Disable  
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PA1_pinmux(SCU_PA1_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PA0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver Pull Disable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PA0_pinmux(SCU_PA0_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PA2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver I2CM,UART Pull enable/High. 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PA2_pinmux(SCU_PA2_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PA3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull Disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PA3_pinmux(SCU_PA3_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SB Sensor i2cm Pin Mux
 *
 * \param[in]	i2cm_sda	 SB Sensor i2cm SDA Pin Mux
 * \param[in]	i2cm_scl	 SB Sensor i2cm SDL Pin Mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_sensor_i2cm_pinmux(SCU_SEN_I2CM_SDA_PINMUX_E i2cm_sda, SCU_SEN_I2CM_SCL_PINMUX_E i2cm_scl);

/**
 * \brief	set SEN_FAE pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_FAE_pinmux(SCU_SEN_FAE_PINMUX_E pinmux);

/**
 * \brief	set SEN_TRIG pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_TRIG_pinmux(SCU_SEN_TRIG_PINMUX_E pinmux);

/**
 * \brief	set SEN_INT pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_INT_pinmux(SCU_SEN_INT_PINMUX_E pinmux);

/**
 * \brief	set SEN_XSLEEP pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_XSLEEP_pinmux(SCU_SEN_XSLEEP_PINMUX_E pinmux);

/**
 * \brief	set SEN_MCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_MCLK_pinmux(SCU_SEN_MCLK_PINMUX_E pinmux);

/**
 * \brief	set SEN_GPIO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_sensorgpio_pinmux(SCU_SEN_GPIO_PINMUX_E pinmux);

/**
 * \brief	set SEN_CSW0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_CSW0_pinmux(SCU_SEN_CSW0_PINMUX_E pinmux);

/**
 * \brief	set SEN_CSW1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_CSW1_pinmux(SCU_SEN_CSW1_PINMUX_E pinmux);

/**
 * \brief	set SEN_PCLKO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_PCLKO_pinmux(SCU_SEN_PCLK_PINMUX_E pinmux);

/**
 * \brief	set SEN_D0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D0_pinmux(SCU_SEN_D0_PINMUX_E pinmux);

/**
 * \brief	set SEN_D1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D1_pinmux(SCU_SEN_D1_PINMUX_E pinmux);

/**
 * \brief	set SEN_D2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_E pinmux);

/**
 * \brief	set SEN_D3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D3_pinmux(SCU_SEN_D3_PINMUX_E pinmux);

/**
 * \brief	set SEN_D4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D4_pinmux(SCU_SEN_D4_PINMUX_E pinmux);

/**
 * \brief	set SEN_D5 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D5_pinmux(SCU_SEN_D5_PINMUX_E pinmux);

/**
 * \brief	set SEN_D6 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D6_pinmux(SCU_SEN_D6_PINMUX_E pinmux);

/**
 * \brief	set SEN_D7 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_D7_pinmux(SCU_SEN_D7_PINMUX_E pinmux);

/**
 * \brief	set SEN_FVALID pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_FVALID_pinmux(SCU_SEN_FVALID_PINMUX_E pinmux);

/**
 * \brief	set SEN_LVALID pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SEN_LVALID_pinmux(SCU_SEN_LVALID_PINMUX_E pinmux);

/**
 * \brief	set SDIO_SDCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SDIO_SDCLK_pinmux(SCU_SDIO_SDCLK_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_CMD pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SDIO_CMD_pinmux(SCU_SDIO_CMD_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SDIO_DAT0_pinmux(SCU_SDIO_DATA0_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SDIO_DAT1_pinmux(SCU_SDIO_DATA1_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SDIO_DAT2_pinmux(SCU_SDIO_DATA2_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SDIO_DAT3_pinmux(SCU_SDIO_DATA3_PINMUX_E pinmux, uint8_t autocfg_pullcfg);


/**
 * \brief	set SW_DATA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SW_DATA_pinmux(SCU_SW_DATA_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SW_CLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SW_CLK_pinmux(SCU_SW_CLK_PINMUX_E pinmux, uint8_t autocfg_pullcfg);


/**
 * \brief	set PB2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB2_pinmux(SCU_PB2_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB3_pinmux(SCU_PB3_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB4_pinmux(SCU_PB4_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB5 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB5_pinmux(SCU_PB5_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB6 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB6_pinmux(SCU_PB6_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB7 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB7_pinmux(SCU_PB7_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB8 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB8_pinmux(SCU_PB8_PINMUX_E pinmux, uint8_t autocfg_pullcfg);


/**
 * \brief	set PB9 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB9_pinmux(SCU_PB9_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB10 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB10_pinmux(SCU_PB10_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB11 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB11_pinmux(SCU_PB11_PINMUX_E pinmux, uint8_t autocfg_pullcfg);
/**
 * \brief	set PC3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PC3_pinmux(SCU_PC3_PINMUX_E pinmux);

/**
 * \brief	set PC4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PC4_pinmux(SCU_PC4_PINMUX_E pinmux);

#if 0//remove SCU 0524
/**
 * \brief	set OSPI pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_OSPI_pinmux(SCU_OSPI_PINMUX_E pinmux);
#endif
/**
 * \brief	set I2CM_SCLSDA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_I2CM_SCLSDA_pinmux(SCU_I2CM_SCLSDA_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SDIO, UART, I2CM pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB0_pinmux(SCU_PB0_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_PB1_pinmux(SCU_PB1_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set UART pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (UART RX/TX pull enable/pull high. others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_UART_RX_TX_CTS_RTS_pinmux(SCU_UART_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SPIS_DO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SPIS_DO_pinmux(SCU_SPIS_DO_PINMUX_E pinmux);

/**
 * \brief	set SPIS_DI pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SPIS_DI_pinmux(SCU_SPIS_DI_PINMUX_E pinmux);

/**
 * \brief	set SPIS_SCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SPIS_SCLK_pinmux(SCU_SPIS_SCLK_PINMUX_E pinmux);

/**
 * \brief	set SPIS_CS pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_SPIS_CS_pinmux(SCU_SPIS_CS_PINMUX_E pinmux);

/**
 * \brief	set I2C_SLV Share Enable
 *
 * \param[in]	enable	disable or enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_I2C_SLV_share_enable(SCU_I2CSLV_SHARE_E enable);

/**
 * \brief	set I3C_SLV Share Enable
 *
 * \param[in]	enable	disable or enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_I3C_SLV_share_enable(SCU_I3CSLV_SHARE_E enable);

/**
 * \brief	set DP related SW reset
 *
 * \param[in]	cfg	DP Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_DP_SWReset(SCU_DP_SWRESET_T *cfg);

/**
 * \brief	get DP related SW reset
 *
 * \param[out]	cfg	DP Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_get_DP_SWReset(SCU_DP_SWRESET_T *cfg);

/**
 * \brief	set WDT control owner
 *
 * \param[in]	id 0 or 1
 * \param[in]	ctrl enable by CPU or PMU
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_wdt_ctrl(uint32_t id, SCU_WDTCTRL_E ctrl);

/**
 * \brief	set Timer control owner
 *
 * \param[in]	id 0 ~ 8
 * \param[in]	ctrl enable by CPU or PMU
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_sys_set_timer_ctrl(uint32_t id, SCU_TIMERCTRL_E ctrl);

#ifndef BOOT_USED
/**
 * \brief	set Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[in]	channel	 capture channel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E channel);

/**
 * \brief	get Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[out]	channel	 capture channel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_get_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E *channel);
#endif

#ifndef BOOT_USED
/**
 * \brief	set ADC LV Reset
 *
 * \param[in]	cfg	 reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_lsc_adclv_reset(SCU_SWRESET_E cfg);
/**
 * \brief	set ADC HV Reset
 *
 * \param[in]	cfg	 reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_lsc_adchv_reset(SCU_SWRESET_E cfg);
#endif

#ifndef BOOT_USED
/**
 * \brief	set SRSTN pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_SRSTN_pinmux(SCU_PAD_SRSTN_PINMUX_E pinmux);

/**
 * \brief	set SWCLK TCK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_SWCLKTCK_pinmux(SCU_PAD_SWCLK_TCK_PINMUX_E pinmux);

/**
 * \brief	set SWDIO TMS pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_SWDIO_TMS_pinmux(SCU_PAD_SWDIO_TMS_PINMUX_E pinmux);
#endif

/**
 * \brief	get All Pin Mux Configuration
 *
 * \param[out]	pinmux_cfg	 all pin mux config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_get_all_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);
/**
 * \brief	set All Pin Mux Configuration
 *
 * \param[in]	pinmux_cfg	 all pinmux config
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull Disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_all_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg, uint8_t autocfg_pullcfg);

#if(IC_VERSION >= 30)
/**
 * \brief	set PD LSC PDM CLK Source
 *
 * \param[in]	pdmsrc_clk	PDM SRC CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E pdmsrc_clk);
#endif

/**
 * \brief	Get PLL Frequency
 *
 * \param[out]	freq	 PLL Frequency
 * \return	void.
 */
void veneer_get_pllfreq(uint32_t *freq);

/**
 * \brief	Get PMU Wake up bootrom cpu clk
 *
 * \param[out]	pll_freq	 bootrom cpu PMU wake up PLL frequency
 * \param[out]	cm55m_div	 bootrom cpu PMU wake up CM55M PLL Div
 * \param[out]	cm55s_div	 bootrom cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void veneer_get_pmuwakeup_freq(SCU_PLL_FREQ_E *pll_freq, SCU_HSCCLKDIV_E *cm55m_div, SCU_LSCCLKDIV_E *cm55s_div);


/**
 * \brief	Set Sensor Init status
 *
 * \param[in]	init	 Sensor Init status
 * \return	void.
 */
void veneer_set_sensorinit(SWREG_AON_SENSORINIT_E init);

/**
 * \brief	Get Sensor Init status
 *
 * \param[out]	init	 Sensor Init status
 * \return	void.
 */
void veneer_get_sensorinit(SWREG_AON_SENSORINIT_E *init);


#ifndef BOOT_USED
/**
 * \brief	Set VMUTE releated control
 *
 * \param[in]	ctrl	 VMUTE releated control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_vmute(SCU_VMUTE_CFG_T *ctrl);
#endif

/**
 * \brief	Set BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 *
 * \param[in]	warmbootclk	 set BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 * \return	void.
 */
void veneer_set_bl_warmbootclk(SWREG_AON_WARMBOOTDISPLL_BLCHG_E warmbootclk);
/**
 * \brief	Get BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 *
 * \param[out]	warmbootclk	 Get BL Warm Boot use Cold boot Freq or not when Warm boot disPLL
 * \return	void.
 */
void veneer_get_bl_warmbootclk(SWREG_AON_WARMBOOTDISPLL_BLCHG_E *warmbootclk);


/**
 * \brief	Set CoreVoltage for Clock tree
 *
 * \param[in]	corev	 set Core Voltage for clock tree
 * \return	void.
 */
void veneer_set_corevolt_clktree(SWREG_AON_COREVOLT_E corev);
/**
 * \brief	Get CoreVoltage for Clock tree
 *
 * \param[out]	corev	 get Core Voltage for clock tree
 * \return	void.
 */
void veneer_get_corevolt_clktree(SWREG_AON_COREVOLT_E *corev);

/**
 * \brief	Set PMU Wake up BL cpu clk when Warmboot BOOTROM dis PLL
 *
 * \param[in]	pll_freq	 BL cpu PMU wake up PLL frequency
 * \param[in]	cm55m_div	 BL cpu PMU wake up CM55M PLL Div
 * \param[in]	cm55s_div	 BL cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void veneer_set_bl_pmuwakeup_freq(SCU_PLL_FREQ_E pll_freq, SCU_HSCCLKDIV_E cm55m_div, SCU_LSCCLKDIV_E cm55s_div);

/**
 * \brief	Get PMU Wake up BL cpu clk
 *
 * \param[out]	pll_freq	 BL cpu PMU wake up PLL frequency
 * \param[out]	cm55m_div	 BL cpu PMU wake up CM55M PLL Div
 * \param[out]	cm55s_div	 BL cpu PMU wake up CM55S PLL Div
 * \return	void.
 */
void veneer_get_bl_pmuwakeup_freq(SCU_PLL_FREQ_E *pll_freq, SCU_HSCCLKDIV_E *cm55m_div, SCU_LSCCLKDIV_E *cm55s_div);

/**
 * \brief	Set BL PLL Frequency
 *
 * \param[in]	freq	 PLL Frequency
 * \return	void.
 */
void veneer_set_bl_pllfreq(uint32_t freq);

/**
 * \brief	Get BL PLL Frequency
 *
 * \param[out]	freq	 PLL Frequency
 * \return	void.
 */
void veneer_get_bl_pllfreq(uint32_t *freq);

/**
 * \brief	Jump to 2ndloader. Receive intoupg command
 *
 * \return	void.
 */
void veneer_jump_2ndloader();


/**
 * \brief	Set SRAM Voltage Control
 *
 * \param[in]	ctrl	 SRAM Voltage control
 * \return	SCU_ERROR_E.
 */
void veneer_set_sram_volt(SCU_COREVOLT_SRAM_E ctrl);
/**
 * \brief	Get SRAM Voltage Control
 *
 * \param[out]	ctrl	 SRAM Voltage control
 * \return	SCU_ERROR_E.
 */
void veneer_get_sram_volt(SCU_COREVOLT_SRAM_E *ctrl);


/**
 * \brief	Record Colboot DC Volt
 *
 * \param[in]	volt	 trim voltage val
 * \return	void.
 */
void veneer_set_coldboot_dcvolt(uint8_t volt);

/**
 * \brief	Get Colboot DC Volt
 *
 * \param[out]	volt	 trim voltage val
 * \return	void.
 */
void veneer_get_coldboot_dcvolt(uint8_t *volt);

/**
 * \brief	Record Colboot PD Volt
 *
 * \param[in]	volt	 trim voltage val
 * \return	void.
 */
void veneer_set_coldboot_pdvolt(uint8_t volt);

/**
 * \brief	Get Colboot PD Volt
 *
 * \param[out]	volt	 trim voltage val
 * \return	void.
 */
void veneer_get_coldboot_pdvolt(uint8_t *volt);

/**
 * \brief	set QSPI related SW reset
 *
 * \param[in]	cfg	QSPI Related SW Reset
 * \return	SCU_ERROR_E.
 */
void veneer_set_QSPI_SWReset(SCU_SWRESET_E cfg);

#ifndef BOOT_USED
/**
 * \brief	Check Memory Space attribute
 *
 * \param[in]	start_addr    start address for check memory attribute
 * \param[in]	size    memory size for check memory attribute
 * \param[in]	attr    memory attribute that app expected
 * \return	SCU_ERROR_E. memory space is as app expected or not
 */
SCU_ERROR_E veneer_check_mem_attr(uint32_t start_addr, uint32_t size, SCU_MEM_ATTR_E attr);
#endif

/**
 * \brief	set CM55S State
 *
 * \param[in]	state	 CM55S State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_cm55s_state(SCU_CM55S_STATE_E state);

/**
 * \brief	set CM55S_CPUWAIT
 *
 * \param[in]	data	CM55S_CPUWAIT
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_CM55S_CPUWAIT(SCU_CM55_CPUWAIT_E data);

/**
 * \brief	Get CM55S Re-Reset flag after PMU wakeup
 *
 * \param[out]	cfg	 re-reset flag configuration
 * \return	void.
 */
void veneer_get_rerest_cm55s_flag(SWREG_AON_PMUWAKE_CM55S_RERESET_E *cfg);

#ifndef BOOT_USED
/**
 * \brief	Set MIPI Control
 *
 * \param[in]	ctrl	 MIPI control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_mipi_ctrl(SCU_MIPI_CTRL_E ctrl);
/**
 * \brief	Get MIPI Control
 *
 * \param[out]	ctrl	 MIPI control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_get_mipi_ctrl(SCU_MIPI_CTRL_E *ctrl);

/**
 * \brief	Get Version
 *
 * \param[out]	version	 FW version
 * \return	void.
 */
void veneer_get_fw_version(uint32_t *version);

/**
 * \brief	set LSC related SW reset
 *
 * \param[in]	cfg	LSC Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_LSC_SWReset(SCU_LSC_SWRESET_T *cfg);

/**
 * \brief	set MIPI TX Phy RX clock source
 *
 * \param[in]	txphyrxclksrc	 MIPI TX Phy RX Clock source
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E veneer_set_MIPITXPHY_RXClkSRC(SCU_TXPHY_RCLK_SRC_SEL_E *txphyrxclksrc);

#endif

#endif /* TRUSTZONE_NSC_FUNCTION_VENEER_SYS_CTRL_H_ */
