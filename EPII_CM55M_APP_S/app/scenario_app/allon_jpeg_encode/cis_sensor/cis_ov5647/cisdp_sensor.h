/*
 * cisdp_sensor.h
 *
 *  Created on: 20240122
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
	CISDP_INIT_TYPE_INP_CROP_496x368_JPG   	= 0x00,
	CISDP_INIT_TYPE_INP_CROP_1280x960_RAW,
	CISDP_INIT_TYPE_INP_CROP_1280x960_CH_BINNING_2X_JPG,
	CISDP_INIT_TYPE_INP_CROP_992x736_CH_BINNING_2X_JPG,
	CISDP_INIT_TYPE_INP_CROP_1008x752_Q_BINNING_2X_RAW,
	CISDP_INIT_TYPE_INP_CROP_1008x752_Q_BINNING_4X_RAW,
} CISDP_INIT_TYPE_E;

int cisdp_sensor_init();
int cisdp_dp_init(bool inp_init, CISDP_INIT_TYPE_E type, SENSORDPLIB_PATH_E dp_type, sensordplib_CBEvent_t dplib_cb, uint32_t jpg_ratio);
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
uint32_t app_mem_to_hw5x5_jpg(uint32_t raw_addr, uint32_t jpg_addr, uint32_t raw_w, uint32_t raw_h);
uint32_t app_mem_to_jpg(uint32_t yuv_addr, uint32_t jpg_addr, uint32_t w, uint32_t h);
uint32_t copy_mem_to_mem(uint32_t src_addr, uint32_t dst_addr, uint32_t src_w, uint32_t src_h, uint32_t start_x, uint32_t start_y, uint32_t len_x, uint32_t len_y);

uint32_t cisdp_get_wdma1_sram_addr();
uint32_t cisdp_get_wdma2_sram_addr();
uint32_t cisdp_get_wdma3_sram_addr();
uint32_t cisdp_get_jpegautofill_sram_addr();
uint32_t cisdp_get_raw_addr();
uint32_t cisdp_get_yuv_addr();
uint32_t cisdp_get_quater_raw_addr();
uint32_t cisdp_get_raw_width();
uint32_t cisdp_get_raw_height();
uint8_t  cisdp_get_hw5x5_jpeg_flag();
uint8_t  cisdp_get_hw5x5_flag();
#ifdef __cplusplus
}
#endif

#endif /* APP_SCENARIO_CISDP_SENSOR_H_ */
