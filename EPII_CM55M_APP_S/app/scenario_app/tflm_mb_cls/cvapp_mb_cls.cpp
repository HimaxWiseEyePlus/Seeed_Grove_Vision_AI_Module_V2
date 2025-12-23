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
#include "cvapp_mb_cls.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#if TFLM2209_U55TAG2205
#include "tensorflow/lite/micro/micro_error_reporter.h"
#endif
#include "img_proc_helium.h"


#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"
#include <send_result.h>
#include <forward_list>

#define INPUT_IMAGE_CHANNELS 3


#define MB_CLS_INPUT_TENSOR_WIDTH   224
#define MB_CLS_INPUT_TENSOR_HEIGHT  224
#define MB_CLS_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS


#define MB_CLS_DBG_APP_LOG 0


// #define EACH_STEP_TICK
#define TOTAL_STEP_TICK

uint32_t systick_1, systick_2;
uint32_t loop_cnt_1, loop_cnt_2;
#define CPU_CLK	0xffffff+1
static uint32_t capture_image_tick = 0;
#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif


using namespace std;

namespace {

constexpr int tensor_arena_size = 450*1024;

static uint32_t tensor_arena=0;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *mb_cls_int_ptr=nullptr;
TfLiteTensor *mb_cls_input, *mb_cls_output;
};

static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void _arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

}

static int _arm_npu_init(bool security_enable, bool privilege_enable)
{
    int err = 0;

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
#if TFLM2209_U55TAG2205
	const void * ethosu_base_address = (void *)(U55_BASE);
#else 
	void * const ethosu_base_address = (void *)(U55_BASE);
#endif

    if (0 != (err = ethosu_init(
                            &ethosu_drv,             /* Ethos-U driver device pointer */
                            ethosu_base_address,     /* Ethos-U NPU's base address. */
                            NULL,       /* Pointer to fast mem area - NULL for U55. */
                            0, /* Fast mem region size. */
							security_enable,                       /* Security enable. */
							privilege_enable))) {                   /* Privilege enable. */
    	xprintf("failed to initalise Ethos-U device\n");
            return err;
        }

    xprintf("Ethos-U55 device initialised\n");

    return 0;
}


int cv_mb_cls_init(bool security_enable, bool privilege_enable, uint32_t model_addr) {
	int ercode = 0;

	//set memory allocation to tensor_arena
	tensor_arena = mm_reserve_align(tensor_arena_size,0x20); //1mb
	xprintf("TA[%x]\r\n",tensor_arena);


	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	if(model_addr != 0) {
		static const tflite::Model*mb_cls_model = tflite::GetModel((const void *)model_addr);

		if (mb_cls_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] mb_cls_model's schema version %d is not equal "
				"to supported version %d\n",
				mb_cls_model->version(), TFLITE_SCHEMA_VERSION);
			return -1;
		}
		else {
			xprintf("mb_cls model's schema version %d\n", mb_cls_model->version());
		}
		static tflite::MicroMutableOpResolver<1> mb_cls_op_resolver;
		if (kTfLiteOk != mb_cls_op_resolver.AddEthosU()){
			xprintf("Failed to add Arm NPU support to op resolver.");
			return false;
		}

		static tflite::MicroInterpreter mb_cls_static_interpreter(mb_cls_model, mb_cls_op_resolver,
				(uint8_t*)tensor_arena, tensor_arena_size);  
 


		if(mb_cls_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		mb_cls_int_ptr = &mb_cls_static_interpreter;
		mb_cls_input = mb_cls_static_interpreter.input(0);
		mb_cls_output = mb_cls_static_interpreter.output(0);
	}

	xprintf("initial done\n");
	return ercode;
}

int cv_mb_cls_run(struct_yolov8_ob_algoResult *algoresult_yolov8n_ob) {
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t ch = app_get_raw_channels();
    uint32_t raw_addr = app_get_raw_addr();
    uint32_t expand = 0;
	std::forward_list<el_box_t> el_algo;

    if(mb_cls_int_ptr!= nullptr) {
		#ifdef TOTAL_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
    	//get image from sensor and resize
		w_scale = (float)(img_w - 1) / (MB_CLS_INPUT_TENSOR_WIDTH - 1);
		h_scale = (float)(img_h - 1) / (MB_CLS_INPUT_TENSOR_HEIGHT - 1);

		
		hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr, (uint8_t*)mb_cls_input->data.data,  
		                    img_w, img_h, ch, 
                        	MB_CLS_INPUT_TENSOR_WIDTH, MB_CLS_INPUT_TENSOR_HEIGHT, w_scale,h_scale);
		#ifdef EACH_STEP_TICK						
			SystemGetTick(&systick_2, &loop_cnt_2);
			dbg_printf(DBG_LESS_INFO,"Tick for resize image BGR8U3C_to_RGB24_helium for MB CLS:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));							
		#endif

		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif

		// //uint8 to int8
		for (int i = 0; i < mb_cls_input->bytes; ++i) {
			*((int8_t *)mb_cls_input->data.data+i) = *((int8_t *)mb_cls_input->data.data+i) - 128;
    	}

		#ifdef EACH_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		dbg_printf(DBG_LESS_INFO,"Tick for Invoke for uint8toint8 for mb cls:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif	

		#ifdef EACH_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		TfLiteStatus invoke_status = mb_cls_int_ptr->Invoke();

		#ifdef EACH_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		#endif
		if(invoke_status != kTfLiteOk)
		{
			xprintf("mb cls invoke fail\n");
			return -1;
		}
		else
		{
			#if MB_CLS_DBG_APP_LOG
			xprintf("mb cls invoke pass\n");
			#endif
		}
		#ifdef EACH_STEP_TICK
    		dbg_printf(DBG_LESS_INFO,"Tick for Invoke for MB_CLS:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif

		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif

		#ifdef TOTAL_STEP_TICK						
			SystemGetTick(&systick_2, &loop_cnt_2);
			// dbg_printf(DBG_LESS_INFO,"Tick for TOTAL MB CLS:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));		
		#endif



		std::string cifar10_labels[] = {"airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck"};

		float output_scale = ((TfLiteAffineQuantization*)(mb_cls_output->quantization.params))->scale->data[0];
		int output_zeropoint = ((TfLiteAffineQuantization*)(mb_cls_output->quantization.params))->zero_point->data[0];

		algoresult_yolov8n_ob->obr[0].confidence = ((float)mb_cls_output->data.int8[0] - (float)output_zeropoint) * output_scale;
		algoresult_yolov8n_ob->obr[0].class_idx = 0;
		algoresult_yolov8n_ob->obr[0].bbox.x = 0;
		algoresult_yolov8n_ob->obr[0].bbox.y = 0;
		algoresult_yolov8n_ob->obr[0].bbox.width = img_w;
		algoresult_yolov8n_ob->obr[0].bbox.height = img_h;

		for(int i=1 ; i<10 ; i++)
		{
 			float cond1 = ((float)mb_cls_output->data.int8[i] - (float)output_zeropoint) * output_scale;
			float cond2 = algoresult_yolov8n_ob->obr[0].confidence;
			if( cond1 >  cond2)
			{
				algoresult_yolov8n_ob->obr[0].class_idx = i;
				algoresult_yolov8n_ob->obr[0].confidence = cond1;
			}
		}
		xprintf("Result is %s\n",cifar10_labels[algoresult_yolov8n_ob->obr[0].class_idx].c_str());
    }
	

#ifdef UART_SEND_ALOGO_RESEULT
	algoresult_yolov8n_ob->algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2) + capture_image_tick;
// uint32_t judge_case_data;
// uint32_t g_trans_type;
// hx_drv_swreg_aon_get_appused1(&judge_case_data);
// g_trans_type = (judge_case_data>>16);
// if( g_trans_type == 0 || g_trans_type == 2)// transfer type is (UART) or (UART & SPI) 
// {
// 	//invalid dcache to let uart can send the right jpeg img out
// 	hx_InvalidateDCache_by_Addr((volatile void *)app_get_jpeg_addr(), sizeof(uint8_t) *app_get_jpeg_sz());

// 	el_img_t temp_el_jpg_img = el_img_t{};
// 	temp_el_jpg_img.data = (uint8_t *)app_get_jpeg_addr();
// 	temp_el_jpg_img.size = app_get_jpeg_sz();
// 	temp_el_jpg_img.width = app_get_raw_width();
// 	temp_el_jpg_img.height = app_get_raw_height();
// 	temp_el_jpg_img.format = EL_PIXEL_FORMAT_JPEG;
// 	temp_el_jpg_img.rotate = EL_PIXEL_ROTATE_0;

// 	send_device_id();
// 	// event_reply(concat_strings(", ", box_results_2_json_str(el_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
// 	event_reply(concat_strings(", ", algo_tick_2_json_str(algoresult_yolov8n_ob->algo_tick),", ", box_results_2_json_str(el_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
// }
// 	set_model_change_by_uart();
#endif	

	SystemGetTick(&systick_1, &loop_cnt_1);
	//recapture image
	sensordplib_retrigger_capture();

	
	SystemGetTick(&systick_2, &loop_cnt_2);
	capture_image_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2);	
	return ercode;
}

int cv_mb_cls_deinit()
{
	
	return 0;
}

