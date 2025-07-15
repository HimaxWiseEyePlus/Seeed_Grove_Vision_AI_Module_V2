/*
 * cisdp_sensor.h
 *
 *  Created on: 2022/11/18
 *      Author: 901912
 */

#ifndef APP_SCENARIO_CISDP_SENSOR_H_
#define APP_SCENARIO_CISDP_SENSOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "cisdp_cfg.h"
#include "hm0360_regs.h"
#include "WE2_device.h"
#include "sensor_dp_lib.h"
#include "hx_drv_scu.h"
#include "hx_drv_CIS_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// CGP some defines here

// I am unsure of the Himax code is making the right choice between these two (from hx_drcv_dp.h)
//DP_MCLK_SRC_INT_SEL_RC36M	  = 0,	/**< MCLK Internal Source from 0:rc_36m */
//DP_MCLK_SRC_INT_SEL_XTAL	  = 1,	/**< MCLK Internal Source from 1:xtal_24m */
// if defined the change to DP_MCLK_SRC_INT_SEL_RC36M
//#define MCLKUSESRC36M

int cisdp_sensor_init(bool sensor_init);
int cisdp_dp_init(bool inp_init, SENSORDPLIB_PATH_E dp_type, sensordplib_CBEvent_t cb_event, uint32_t jpg_ratio, APP_DP_INP_SUBSAMPLE_E subs);
void cisdp_sensor_start();
void cisdp_sensor_stop();
void cisdp_get_jpginfo(uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr);

uint32_t app_get_jpeg_addr();
uint32_t app_get_jpeg_sz();
uint32_t app_get_raw_addr();
uint32_t app_get_raw_sz();
uint32_t app_get_raw_width();
uint32_t app_get_raw_height();
uint32_t app_get_raw_channels();

// CGP added these:
int cisdp_sensor_md_init();

HX_CIS_ERROR_E cisdp_sensor_set_mode(uint8_t context, mode_select_t mode, uint8_t numFrames, uint16_t sleepTime);
HX_CIS_ERROR_E cisdp_sensor_get_int_status(uint8_t * val);
HX_CIS_ERROR_E cisdp_sensor_clear_interrupt(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* APP_SCENARIO_CISDP_SENSOR_H_ */
