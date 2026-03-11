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

/*
TEST_SAMLL_MODEL = 1 
use add_quant_himax_ini_opt_size_vela_4_5_0_inout_in32.pte
*/
#define TEST_SAMLL_MODEL 0

/*
UINT_TEST =1
for Mobilenet v2 floating input or Int8 input
use fix image

UINT_TEST = 0
use camera
*/
#define UINT_TEST 0
#define TEST_IMG_NO 0
/**
 * TEST_RESIZE_FROM_320_240_3 = 1
 * image is original size 320*240*3
 * [bbbb...bbb][ggg...ggg][rrr...rrrr]
 * to test resize to put tensor
 * 
 * TEST_RESIZE_FROM_320_240_3 = 0
 * image is original size 240*240*3
 * [rrr...rrr][ggg...ggg][bbbb...bbb]
 * 
 * **/
#define TEST_RESIZE_FROM_320_240_3 0
#if TEST_SAMLL_MODEL
/*
add_quant_himax_ini_opt_size_vela_4_5_0_inout_in32.pte
*/
#define MOBILENET_CLASSIFICATION_FLASH_ADDR 0x3A200000
#define MOBILENET_CLASSIFICATION_MODEL_SIZE (2252)
#else
//0x3AB7B000 //(2220032 bytes => 0x21E000, set to 0x21E000)
/*
1. mv2_quant_himax_ini_opt_size_vela_4_5_0_inout_in8.pte:   intput is int8
2. mv2_quant_himax_ini_opt_size_vela_4_5_0.pte :            input is float
*/
#define MOBILENET_CLASSIFICATION_FLASH_ADDR 0x3AB7B000
#define MOBILENET_CLASSIFICATION_MODEL_SIZE (3478912)
#endif
#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_ */