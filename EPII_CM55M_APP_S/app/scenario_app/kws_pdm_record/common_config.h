/*
 * common_config.h
 *
 *  Created on: Jul 18, 2024
 *      Author: karan_kapur-himax
 */


#ifndef SCENARIO_KWS_PDM_RECORD_COMMON_CONFIG_H_
#define SCENARIO_KWS_PDM_RECORD_COMMON_CONFIG_H_

// #define FRAME_CHECK_DEBUG	1
#define EN_ALGO				1
// #define SPI_SEN_PIC_CLK				(10000000)
#define SPI_SEN_PIC_CLK				(12000000)


#define DBG_APP_LOG 0

#define KWS_MODEL_VELA 1

//current FW image is 409600 bytes => 0x64000. set  0~0x171000 as FW area
// #define FW_IMG_SZ							0x3A171000

//0x3AB7B000 //(2220032 bytes => 0x21E000, set to 0x21E000)
#define KWS_FLASH_ADDR 0x3AB7B000


#endif /* SCENARIO_KWS_PDM_RECORD_COMMON_CONFIG_H_ */
