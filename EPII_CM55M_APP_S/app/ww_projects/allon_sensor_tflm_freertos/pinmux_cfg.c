/*
 * pinmux_cfg.c
 *
 *  Created on: 2023/10/24
 *      Author: Himax
 */

#include "hx_drv_scu.h"
#include "pinmux_cfg.h"

/*******************************************************************************
 * WE2 Grove Vision AI module Pin Mux Configuration
 ******************************************************************************/

/* Init UART0 pin mux to PB0 and PB1 */
void uart0_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb0 = SCU_PB0_PINMUX_UART0_RX_1;		/*!< pin PB0*/
	pinmux_cfg->pin_pb1 = SCU_PB1_PINMUX_UART0_TX_1;		/*!< pin PB1*/
}


/* Init UART2 pin mux to PB9 and PB10 (RX2, TX2) */
void uart2_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb9 = SCU_PB9_PINMUX_UART2_RX;          /*!< pin PB9*/
	pinmux_cfg->pin_pb10 = SCU_PB10_PINMUX_UART2_TX;		/*!< pin PB10*/
}


/* Init PDM pin mux to PB9 and PB10 (PDM_CLK, PDM_DATA0) */
void pdm_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb9 = SCU_PB9_PINMUX_PDM_CLK_1;         /*!< pin PB9*/
	pinmux_cfg->pin_pb10 = SCU_PB10_PINMUX_PDM_DATA0_1;     /*!< pin PB10*/
}


/* Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin */
void aon_gpio1_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pa1 = SCU_PA1_PINMUX_AON_GPIO1;         /*!< pin PA1*/
}


/* Init SPI master pin mux */
void spi_m_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb2 = SCU_PB2_PINMUX_SPI_M_DO_1;        /*!< pin PB2*/
	pinmux_cfg->pin_pb3 = SCU_PB3_PINMUX_SPI_M_DI_1;        /*!< pin PB3*/
	pinmux_cfg->pin_pb4 = SCU_PB4_PINMUX_SPI_M_SCLK_1;      /*!< pin PB4*/
	pinmux_cfg->pin_pb11 = SCU_PB11_PINMUX_SPI_M_CS;        /*!< pin PB11*/
}


/* Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)*/
void swd_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb6 = SCU_PB6_PINMUX_SRSTN;             /*!< pin PB6*/
	pinmux_cfg->pin_pb7 = SCU_PB7_PINMUX_SWCLK_0;           /*!< pin PB7*/
    pinmux_cfg->pin_pb8 = SCU_PB8_PINMUX_SWDIO_0;           /*!< pin PB8*/
}


/* Init I2C slave 0 pin mux to PA2, PA3 (SCL, SDA)*/
void i2cs0_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pa2 = SCU_PA2_PINMUX_SB_I2C_S_SCL_0;    /*!< pin PA2*/
	pinmux_cfg->pin_pa3 = SCU_PA3_PINMUX_SB_I2C_S_SDA_0;    /*!< pin PA3*/
}
