/*
 * pinmux_init.c
 *
 *  Created on: 2023�~9��8��
 *      Author: 902447
 */


#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "WE2_device.h"

#ifdef IP_scu
#include "hx_drv_scu.h"
#endif
#include "pinmux_init.h"

void __attribute__((weak)) pinmux_init()
{
	SCU_PINMUX_CFG_T pinmux_cfg;

	hx_drv_scu_get_all_pinmux_cfg(&pinmux_cfg);
	/*Change UART0 pin mux to PB0 and PB1*/
	pinmux_cfg.pin_pb0 = SCU_PB0_PINMUX_UART0_RX_1;   /*!< pin PB0*/
	pinmux_cfg.pin_pb1 = SCU_PB1_PINMUX_UART0_TX_1;   /*!< pin PB1*/
	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}
