/*
 * cisdp_sensor.h
 *
 *  Created on: 2022�~11��18��
 *      Author: 901912
 */

#ifndef APP_SCENARIO_APP_AIOT_NB_TZ_S_ONLY_CISDP_SENSOR_H_
#define APP_SCENARIO_APP_AIOT_NB_TZ_S_ONLY_CISDP_SENSOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "WE2_device.h"
#include "hxevent_debug.h"
#include "sensor_dp_lib.h"
#include "evt_datapath.h"
#include "cisdp_cfg.h"
#include "hx_drv_scu.h"

#ifdef __cplusplus
extern "C" {
#endif
int cisdp_sensor_init();
int cisdp_dp_init(bool inp_init, SENSORDPLIB_PATH_E dp_type,
		evthandlerdp_CBEvent_t cb_event, uint32_t jpg_ratio, APP_DP_INP_SUBSAMPLE_E subs);
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

#ifdef __cplusplus
}
#endif

#endif /* APP_SCENARIO_APP_AIOT_NB_TZ_S_ONLY_CISDP_SENSOR_H_ */
