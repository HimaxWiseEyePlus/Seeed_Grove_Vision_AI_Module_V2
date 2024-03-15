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

//current FW image is 409600 bytes => 0x64000. set  0~0x171000 as FW area
#define FW_IMG_SZ							0x3A171000

#define EFFICIENTNET_LITE0_CLASSIFICATION_FLASH_ADDR (FW_IMG_SZ)




#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_ */
