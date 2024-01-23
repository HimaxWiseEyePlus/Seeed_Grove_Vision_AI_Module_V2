/*
 * platform_driver_init.h
 *
 *  Created on: 2023¦~9¤ë8¤é
 *      Author: 902447
 */

#ifndef BOARD_EPII_EVB_PLATFORM_DRIVER_INIT_H_
#define BOARD_EPII_EVB_PLATFORM_DRIVER_INIT_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "WE2_device.h"
#ifdef IP_2x2
#include "hx_drv_hw2x2.h"
#endif
#ifdef IP_5x5
#include "hx_drv_hw5x5.h"
#endif
#ifdef IP_cdm
#include "hx_drv_cdm.h"
#endif
#ifdef IP_csirx
#include "hx_drv_csirx.h"
#endif
#ifdef IP_csitx
#include "hx_drv_csitx.h"
#endif
#ifdef IP_dp
#include "hx_drv_dp.h"
#endif
#ifdef IP_edm
#include "hx_drv_edm.h"
#endif
#ifdef IP_hxautoi2c_mst
#include "hx_drv_hxautoi2c_mst.h"
#endif
#ifdef IP_inp
#include "hx_drv_inp.h"
#endif
#ifdef IP_inp1bitparser
#include "hx_drv_inp1bitparser.h"
#endif
#ifdef IP_inpovparser
#include "hx_drv_inpovparser.h"
#endif
#ifdef IP_jpeg
#include "hx_drv_jpeg.h"
#endif
#ifdef IP_sensorctrl
#include "hx_drv_sensorctrl.h"
#endif
#ifdef IP_tpg
#include "hx_drv_tpg.h"
#endif
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#endif
#ifdef IP_uart
#include"hx_drv_uart.h"
#endif
#ifdef IP_mb
#include "hx_drv_mb.h"
#endif
#ifdef IP_scu
#include "hx_drv_scu.h"
#endif
#ifdef IP_timer
#include "hx_drv_timer.h"
#endif
#ifdef IP_rtc
#include "hx_drv_rtc.h"
#endif
#ifdef IP_watchdog
#include "hx_drv_watchdog.h"
#endif
#ifdef IP_iic
#include"hx_drv_iic.h"
#endif
#ifdef IP_gpio
#include"hx_drv_gpio.h"
#endif
#ifdef IP_spi
#include"hx_drv_spi.h"
#endif
#ifdef IP_pwm
#include"hx_drv_pwm.h"
#endif
#ifdef IP_i3c_slv
#include"hx_drv_i3c_slv.h"
#endif
#ifdef IP_dma
#include "hx_drv_dmac.h"
#endif
#ifdef IP_timer
#include "timer_interface.h"
#endif

void platform_driver_init();

#endif /* BOARD_EPII_EVB_PLATFORM_DRIVER_INIT_H_ */
