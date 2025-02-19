/*
 * common_config.h
 *
 *  Created on: Nov 22, 2022
 *      Author: bigcat-himax
 */


#ifndef SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_
#define SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_

#define FRAME_CHECK_DEBUG	1
#define EN_ALGO				1
// #define SPI_SEN_PIC_CLK				(10000000)
#define SPI_SEN_PIC_CLK				(12000000)


#define DBG_APP_LOG 0




#define FACE_DECTECT_FLASH_ADDR         (BASE_ADDR_FLASH1_R_ALIAS+0x200000)

#define YOLOV8N_GENDER_CLS_FLASH_ADDR            (BASE_ADDR_FLASH1_R_ALIAS+0x280000)

#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_ */
