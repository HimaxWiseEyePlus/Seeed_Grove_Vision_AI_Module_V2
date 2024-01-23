/*
 * cisdp_sensor.h
 *
 *  Created on: 2022�~11��18��
 *      Author: 901912
 */

#ifndef APP_SCENARIO_CISDP_SENSOR_H_
#define APP_SCENARIO_CISDP_SENSOR_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum CISDP_INIT_TYPE_E
 * \brief
 */
typedef enum CISDP_INIT_TYPE_S
{
	CISDP_INIT_TYPE_NONE   	= 0x00,
	CISDP_INIT_TYPE_VIDEO_STREAM 	= 0x01,		   /*For CPU usage*/
	CISDP_INIT_TYPE_AOS	   	= 0x02,          /*For PMU USAGE*/
} CISDP_INIT_TYPE_E;

int cisdp_sensor_init();
int cisdp_dp_init(bool inp_init, SENSORDPLIB_PATH_E dp_type, evthandlerdp_CBEvent_t cb_event, uint32_t jpg_ratio, APP_DP_INP_SUBSAMPLE_E subs);
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

#endif /* APP_SCENARIO_CISDP_SENSOR_H_ */
