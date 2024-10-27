/*
 * cvapp.cpp
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#include <cstdio>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "WE2_device.h"
#include "board.h"
#include "cvapp_hello_world_cmsis_cv.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "img_proc_helium.h"


#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"
#include "send_result.h"

#include "arm_cv.h"
// #include "dsp/support_functions.h"
#include "spi_master_protocol.h"
#define ImageOut_WIDTH_SIZE 320
#define ImageOut_HEIGHT_SIZE 240
#define ImageOut_BUFSIZE  (ImageOut_WIDTH_SIZE*ImageOut_HEIGHT_SIZE)
#define SRAM2_START_ADDR 0x36000000
/**
 * RUN_CANNY_SOBEL_ALGO
 * 1: run the canny sobel algorithm and send the canny sobel result jpeg image out
 * 0: send the guassion filter result jpeg image out
 * **/
#define RUN_CANNY_SOBEL_ALGO 1

#include "JPEGENC.h"




__attribute__(( section(".bss.NoInit"))) uint8_t o_uint8_buf[ImageOut_BUFSIZE] __ALIGNED(32);
__attribute__(( section(".bss.NoInit"))) uint8_t o_jpeg_uint8_buf[ImageOut_BUFSIZE] __ALIGNED(32);


uint32_t o_canny_sobel_buff = (uint32_t)SRAM2_START_ADDR; 
uint32_t str_assign_buf = (uint32_t)(o_canny_sobel_buff + ImageOut_BUFSIZE*sizeof(uint8_t));


#define  EACH_STEP_TICK 0
#define TOTAL_STEP_TICK 1



using namespace std;

namespace {


};

#define CPU_CLK	0xffffff+1
uint32_t systick_1, systick_2;
uint32_t loop_cnt_1, loop_cnt_2;
static uint32_t capture_image_tick = 0;

static q15_t *gaussian_tmp_buffer;
static q15_t *canny_tmp_buffer;

void cv_hello_world_cmsis_cv_init() {
	// //set memory allocation
    gaussian_tmp_buffer = (q15_t*)malloc(arm_get_linear_scratch_size_buffer_15(ImageOut_WIDTH_SIZE));

#if RUN_CANNY_SOBEL_ALGO
    canny_tmp_buffer = (q15_t*)malloc(arm_cv_get_scratch_size_canny_sobel(ImageOut_WIDTH_SIZE));
#endif
}

int cv_hello_world_cmsis_cv_run() {
	int ercode = 0;


	arm_cv_image_gray8_t guass_ImageIn;
	guass_ImageIn.width = ImageOut_WIDTH_SIZE;
	guass_ImageIn.height = ImageOut_HEIGHT_SIZE;
	guass_ImageIn.pData = (uint8_t *)app_get_raw_addr();

	arm_cv_image_gray8_t guass_ImageOut;
	guass_ImageOut.width= ImageOut_WIDTH_SIZE;
	guass_ImageOut.height= ImageOut_HEIGHT_SIZE;



	guass_ImageOut.pData = (uint8_t *)o_uint8_buf;

	arm_gaussian_filter_5x5_fixp(&guass_ImageIn,
		                         &guass_ImageOut,
		                         gaussian_tmp_buffer,
		                         ARM_CV_BORDER_NEAREST);
	hx_CleanDCache_by_Addr((volatile void*)guass_ImageOut.pData, sizeof(uint8_t) *ImageOut_BUFSIZE );


#if RUN_CANNY_SOBEL_ALGO
	arm_cv_image_gray8_t canny_sobel_ImageOut;
	uint8_t low_threshold = 50;
	uint8_t high_threshold = 33;

	canny_sobel_ImageOut.width = ImageOut_WIDTH_SIZE;
	canny_sobel_ImageOut.height = ImageOut_HEIGHT_SIZE;
	canny_sobel_ImageOut.pData = (uint8_t *)o_canny_sobel_buff;

	arm_cv_canny_edge_sobel(&guass_ImageOut, &canny_sobel_ImageOut, canny_tmp_buffer,low_threshold, high_threshold);

	hx_CleanDCache_by_Addr((volatile void*)canny_sobel_ImageOut.pData, sizeof(uint8_t) *ImageOut_BUFSIZE );


	el_img_t temp_el_raw_img = el_img_t{};
	temp_el_raw_img.data = (uint8_t *)canny_sobel_ImageOut.pData;
	temp_el_raw_img.size = ImageOut_BUFSIZE;
	temp_el_raw_img.width = ImageOut_WIDTH_SIZE;
	temp_el_raw_img.height = ImageOut_HEIGHT_SIZE;
	temp_el_raw_img.format = EL_PIXEL_FORMAT_GRAYSCALE;
	temp_el_raw_img.rotate = EL_PIXEL_ROTATE_0;

	///////////////////////send raw image
	// img_2_json_str_assign_buf(&temp_el_raw_img,(char *)str_assign_buf);

	hx_drv_spi_mst_protocol_write_sp((uint32_t )temp_el_raw_img.data, ImageOut_BUFSIZE, DATA_TYPE_RAW_IMG);

#else

	////////////////////////////////////////////////////
	el_img_t temp_el_raw_img = el_img_t{};
	temp_el_raw_img.data = (uint8_t *)guass_ImageOut.pData;
	temp_el_raw_img.size = ImageOut_BUFSIZE;
	temp_el_raw_img.width = ImageOut_WIDTH_SIZE;
	temp_el_raw_img.height = ImageOut_HEIGHT_SIZE;
	temp_el_raw_img.format = EL_PIXEL_FORMAT_GRAYSCALE;
	temp_el_raw_img.rotate = EL_PIXEL_ROTATE_0;
	///////////////
	///////////////////////send raw image
	// img_2_json_str_assign_buf(&temp_el_raw_img,(char *)str_assign_buf);
	
	hx_drv_spi_mst_protocol_write_sp((uint32_t )temp_el_raw_img.data, ImageOut_BUFSIZE, DATA_TYPE_RAW_IMG);

#endif

	////////////////////////////////////////
	///////////////////////send jpeg image
	el_img_t temp_el_jpg_img_dst = el_img_t{};
	temp_el_jpg_img_dst.data = (uint8_t *)o_jpeg_uint8_buf;
	temp_el_jpg_img_dst.size = ImageOut_BUFSIZE;
	temp_el_jpg_img_dst.width = ImageOut_WIDTH_SIZE;
	temp_el_jpg_img_dst.height = ImageOut_HEIGHT_SIZE;
	temp_el_jpg_img_dst.format = EL_PIXEL_FORMAT_GRAYSCALE;
	temp_el_jpg_img_dst.rotate = EL_PIXEL_ROTATE_0;
	rgb_to_jpeg(&temp_el_raw_img,&temp_el_jpg_img_dst);
	hx_CleanDCache_by_Addr((volatile void*)temp_el_jpg_img_dst.data, sizeof(uint8_t) *ImageOut_BUFSIZE );
	send_device_id();
	event_reply(concat_strings(", ", img_2_json_str(&temp_el_jpg_img_dst)));
	//////////////////////////////////////


	SystemGetTick(&systick_1, &loop_cnt_1);
	//recapture image
	sensordplib_retrigger_capture();

	SystemGetTick(&systick_2, &loop_cnt_2);
	capture_image_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2);	
	
	return ercode;
}



