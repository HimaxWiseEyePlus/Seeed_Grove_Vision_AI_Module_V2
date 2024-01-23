/*
 * common_config.h
 *
 *  Created on: Feb 15, 2023
 *      Author: bigcat-himax
 */


#ifndef SCENARIO_TFLM_FD_FM_COMMON_CONFIG_H_
#define SCENARIO_TFLM_FD_FM_COMMON_CONFIG_H_

#define FRAME_CHECK_DEBUG	1
#define EN_ALGO				1
#define SPI_SEN_PIC_CLK                 (12000000)
#define WATCHDOG_VERSION
#define DBG_APP_LOG 0

#define FACE_DECTECT_FLASH_ADDR         (BASE_ADDR_FLASH1_R_ALIAS+0x200000)

#define FACE_MESH_FLASH_ADDR            (BASE_ADDR_FLASH1_R_ALIAS+0x280000)

#define IRIS_LANDMARKS_FLASH_ADDR       (BASE_ADDR_FLASH1_R_ALIAS+0x32A000)

#endif /* SCENARIO_TFLM_FD_FM_COMMON_CONFIG_H_ */
