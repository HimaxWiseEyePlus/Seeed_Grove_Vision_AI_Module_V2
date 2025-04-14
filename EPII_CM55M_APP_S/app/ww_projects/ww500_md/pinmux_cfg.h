/*
 * pinmux_cfg.h
 *
 *  Created on: 2023/10/24
 *      Author: Himax
 */

#ifndef APP_SCENARIO_APP_FREERTOS_PINMUX_CFG_H_
#define APP_SCENARIO_APP_FREERTOS_PINMUX_CFG_H_

#include "hx_drv_scu_export.h"

// Init UART0 pin mux to PB0 and PB1 - for USB cable comms
void uart0_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init UART2 pin mux to PB9 and PB10 (RX2, TX2)
// But on the Seeed Vision AI V2 these pins are I2C pins to the digital microphone
void uart2_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init PDM pin mux to PB9 and PB10 (PDM_CLK, PDM_DATA0)
void pdm_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin
void aon_gpio1_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init PB10 for OV5647 enable pin
void sensor_enable_gpio1_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init SPI master pin mux (share with SDIO)
// PB2, PB3, PB4, PB11 to inter-board connectors
// PB2, PB3, PB4, PB5 to SD card socket
// PB5 is configured in spi_ffs.c SSPI_CS_GPIO_Pinmux()
void spi_m_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

/* Init SDIO pin mux for SD card */
// This is not implemented!
void sdio_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)
// These go to inter-baord connectors
void swd_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

// Init I2C slave 0 pin mux to PA2, PA3 (SCL, SDA)
// I2C slave to inter-board connectors
void i2cs0_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);

#endif  /* APP_SCENARIO_APP_FREERTOS_PINMUX_CFG_H_ */
