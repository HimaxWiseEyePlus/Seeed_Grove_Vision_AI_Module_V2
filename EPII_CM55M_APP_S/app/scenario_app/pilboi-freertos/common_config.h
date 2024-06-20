/*
 * common_config.h
 *
 *  Created on: Nov 22, 2022
 *      Author: bigcat-himax
 */

#ifndef APP_SCENARIO_ALLON_SENSOR_TFLM_COMMON_CONFIG_H_
#define APP_SCENARIO_ALLON_SENSOR_TFLM_COMMON_CONFIG_H_

/** MODEL location:
 *	0: model file is a c file which will locate to memory.
 *		in this example, model data is "person_detect_model_data_vela.cc" file.
 *
 *	1: model file will off-line burn to dedicated location in flash,
 *		use flash memory mapped address to load model.
 *		in this example, model data is pre-burn to flash address: 0x180000
 * **/
#define FLASH_XIP_MODEL 0
#define MEM_FREE_POS		(BOOT2NDLOADER_BASE) ////0x3401F000

#endif /* APP_SCENARIO_ALLON_SENSOR_TFLM_COMMON_CONFIG_H_ */
