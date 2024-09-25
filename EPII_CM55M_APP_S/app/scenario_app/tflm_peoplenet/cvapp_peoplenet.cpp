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
#include "cvapp_peoplenet.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "img_proc_helium.h"
#include <forward_list>

#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"
#include <send_result.h>
#define PEOPLENET_INPUT_TENSOR_WIDTH   320
#define PEOPLENET_INPUT_TENSOR_HEIGHT  240


#define INPUT_IMAGE_CHANNELS 3
#define PEOPLENET_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS


#define SRAM2_START_ADDR 0x36000000

uint32_t str_assign_buf = (uint32_t)SRAM2_START_ADDR;

#define EACH_STEP_TICK 0
#define PEOPLENET_DBG_APP_LOG 0
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

constexpr int tensor_arena_size = 1155*1024;

static uint32_t tensor_arena=0;
// __attribute__(( section(".bss.NoInit"))) uint8_t tensor_arena[1155*1024] __ALIGNED(32);

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *peoplenet_int_ptr=nullptr;
TfLiteTensor *peoplenet_input;
TfLiteTensor *score_output;
TfLiteTensor *box_output;

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

int cv_peoplenet_init(bool security_enable, bool privilege_enable, uint32_t model_addr) {
	int ercode = 0;

	//set memory allocation to tensor_arena
	tensor_arena = mm_reserve_align(tensor_arena_size,0x20); //1mb
	xprintf("TA[%x]\r\n",tensor_arena);


	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	if(model_addr != 0) {
		static const tflite::Model*peoplenet_model = tflite::GetModel((const void *)model_addr);

		if (peoplenet_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] peoplenet_model's schema version %d is not equal "
				"to supported version %d\n",
				peoplenet_model->version(), TFLITE_SCHEMA_VERSION);
			return -1;
		}
		else {
			xprintf("peoplenet_model model's schema version %d\n", peoplenet_model->version());
		}

		static tflite::MicroErrorReporter peoplenet_micro_error_reporter;
		static tflite::MicroMutableOpResolver<1> peoplenet_resolver;


		if (kTfLiteOk != peoplenet_resolver.AddEthosU()){
			xprintf("Failed to add Arm NPU support to op resolver.");
			return false;
		}
		#if TFLM2209_U55TAG2205
			static tflite::MicroInterpreter peoplenet_static_interpreter(peoplenet_model, peoplenet_resolver,
					(uint8_t*)tensor_arena, tensor_arena_size, &peoplenet_micro_error_reporter);
		#else
			static tflite::MicroInterpreter peoplenet_static_interpreter(peoplenet_model, peoplenet_resolver,
					(uint8_t*)tensor_arena, tensor_arena_size);  
		#endif  


		if(peoplenet_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		peoplenet_int_ptr = &peoplenet_static_interpreter;
		peoplenet_input = peoplenet_static_interpreter.input(0);
        score_output = peoplenet_static_interpreter.output(0);
        box_output = peoplenet_static_interpreter.output(1);

	}
	xprintf("initial done\n");
	return ercode;
}



static void peoplenet_post_processing(tflite::MicroInterpreter* static_interpreter,float* scoreThresholds, float* iouThresholds, struct_peoplenet_algoResult *alg,	std::forward_list<el_box_t> &el_algo)
{
	uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
	int _numDetections[3];
    float _scores[15 * 20][3];
    float _boxes[15 * 20][3][4];
	
	//////post-proccess threshold
	int8_t* qScores = static_interpreter->output(0)->data.int8;
	float scoreScale = static_interpreter->output(0)->params.scale;
	int32_t scoreZeroPoint = static_interpreter->output(0)->params.zero_point;

	int8_t* qBox = static_interpreter->output(1)->data.int8;
	float boxScale = static_interpreter->output(1)->params.scale;
	int32_t boxZeroPoint = static_interpreter->output(1)->params.zero_point;
	//setupt how many class at here
	int num_class = static_interpreter->output(0)->dims->data[3];

	_numDetections[0] = 0;
	_numDetections[1] = 0;
	_numDetections[2] = 0;

	for (int j = 0; j < 15; j++) {
		for (int i = 0; i < 20; i++) {
			for (int c = 0; c < num_class; c++) {
				float score =  (*qScores++ - scoreZeroPoint) * scoreScale;
				float x1 = (*qBox++ - boxZeroPoint) * boxScale;
				float y1 = (*qBox++ - boxZeroPoint) * boxScale;
				float x2 = (*qBox++ - boxZeroPoint) * boxScale;
				float y2 = (*qBox++ - boxZeroPoint) * boxScale;

				float xCenter = (16.0 * i + 0.5);
				float yCenter = (16.0 * j + 0.5);

				x1 = xCenter - x1 * 35.0;
				y1 = yCenter - y1 * 35.0;
				x2 = xCenter + x2 * 35.0;
				y2 = yCenter + y2 * 35.0;

				if (score > scoreThresholds[c]) {
					_scores[_numDetections[c]][c] = score;
					_boxes[_numDetections[c]][c][0] = x1;
					_boxes[_numDetections[c]][c][1] = y1;
					_boxes[_numDetections[c]][c][2] = x2;
					_boxes[_numDetections[c]][c][3] = y2;

					_numDetections[c]++;
				}
			}
		}
	}


	//////post-proccess NMS threshold

	for (int c = 0; c < num_class; c++) {
		int numNonSupressed = 0;

		int numDetections = _numDetections[c];

		for (int i = 0; i < numDetections; i++) {
			float aScore = _scores[i][c];

			if (aScore == 0.0f) {
			continue;
			}

			float aX1 = _boxes[i][c][0];
			float aY1 = _boxes[i][c][1];
			float aX2 = _boxes[i][c][2];
			float aY2 = _boxes[i][c][3];
			float aW = aX2 - aX1;
			float aH = aY2 - aY1;
			float aArea = aW * aH;

			bool maximum = true;

			for (int j = (i + 1); j < numDetections && maximum; j++) {
				float bScore = _scores[j][c];

				if (bScore == 0.0f) {
					continue;
				}

				float bX1 = _boxes[j][c][0];
				float bY1 = _boxes[j][c][1];
				float bX2 = _boxes[j][c][2];
				float bY2 = _boxes[j][c][3];
				float bW = bX2 - bX1;
				float bH = bY2 - bY1;

				float iX1 = max(aX1, bX1);
				float iY1 = max(aY1, bY1);
				float iX2 = min(aX2, bX2);
				float iY2 = min(aY2, bY2);

				if (iX1 >= iX2 || iY1 >= iY2) {
					continue;
				}

				float bArea = bW * bH;
				float iArea = (iX2 - iX1) * (iY2 - iY1);

				float iou = iArea / (aArea + bArea - iArea);

				if (iou < iouThresholds[c]) {
					continue;
				}

				if (bScore < aScore) {
					_scores[j][c] = 0.0f;
				} else {
					maximum = false;
				}
			}

			if (maximum) {
				_scores[numNonSupressed][c] = aScore;
				_boxes[numNonSupressed][c][0] = aX1;
				_boxes[numNonSupressed][c][1] = aY1;
				_boxes[numNonSupressed][c][2] = aX2;
				_boxes[numNonSupressed][c][3] = aY2;


				alg->result[numNonSupressed][c].bbox.x = aX1;
				alg->result[numNonSupressed][c].bbox.y = aY1;
				alg->result[numNonSupressed][c].bbox.width = aX2 - aX1;
				alg->result[numNonSupressed][c].bbox.height = aY2 - aY1; 
				alg->result[numNonSupressed][c].confidence = aScore;

				numNonSupressed++;
			}
		}

		_numDetections[c] = numNonSupressed;
	}

	//the result sent by uart
	int max_detect_num = 0;
	for (int c = 0; c < num_class; c++)
	{
		for(int i = 0; i < _numDetections[c]; i++)
		{
			max_detect_num++;
			if(!(MAX_TRACKED_YOLOV8_ALGO_RES-max_detect_num))break;
			el_box_t temp_el_box;
			temp_el_box.score =  (uint8_t)((alg->result[i][c].confidence)*100.0);
			temp_el_box.target =  c;
			temp_el_box.x = alg->result[i][c].bbox.x;
			temp_el_box.y =  alg->result[i][c].bbox.y;
			temp_el_box.w = alg->result[i][c].bbox.width;
			temp_el_box.h = alg->result[i][c].bbox.height;

			el_algo.emplace_front(temp_el_box);
		}
	}
	
}

int cv_peoplenet_run(struct_peoplenet_algoResult *algoresult_peoplenet){
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t ch = app_get_raw_channels();
    uint32_t raw_addr = app_get_raw_addr();
    uint32_t expand = 0;
	std::forward_list<el_box_t> el_algo;

	#if PEOPLENET_DBG_APP_LOG
    xprintf("raw info: w[%d] h[%d] ch[%d] addr[%x]\n",img_w, img_h, ch, raw_addr);
	#endif

    if(peoplenet_int_ptr!= nullptr) {
		#ifdef TOTAL_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		#if EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
    	//get image from sensor and resize
		w_scale = (float)(img_w - 1) / (PEOPLENET_INPUT_TENSOR_WIDTH - 1);
		h_scale = (float)(img_h - 1) / (PEOPLENET_INPUT_TENSOR_HEIGHT - 1);

		
		hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr, (uint8_t*)peoplenet_input->data.data,  
		                    img_w, img_h, ch, 
                        	PEOPLENET_INPUT_TENSOR_WIDTH, PEOPLENET_INPUT_TENSOR_HEIGHT, w_scale,h_scale);
		#if EACH_STEP_TICK						
			SystemGetTick(&systick_2, &loop_cnt_2);
			dbg_printf(DBG_LESS_INFO,"Tick for resize image BGR8U3C_to_RGB24_helium for peoplenet:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));							
		#endif

		#if EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif

		// //uint8 to int8
		for (int i = 0; i < peoplenet_input->bytes; ++i) {
			*((int8_t *)peoplenet_input->data.data+i) = *((int8_t *)peoplenet_input->data.data+i) - 128;
    	}

		#if EACH_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		dbg_printf(DBG_LESS_INFO,"Tick for Invoke for uint8toint8 for peoplenet:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif	

		#if EACH_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		TfLiteStatus invoke_status = peoplenet_int_ptr->Invoke();

		#if EACH_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		#endif
		if(invoke_status != kTfLiteOk)
		{
			xprintf("peoplenet invoke fail\n");
			return -1;
		}
		else
		{
			#if PEOPLENET_DBG_APP_LOG
			xprintf("peoplenet  invoke pass\n");
			#endif
		}
		#if EACH_STEP_TICK
    		dbg_printf(DBG_LESS_INFO,"Tick for Invoke for peoplenet:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif

		#if EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		//retrieve output data
		float scoreThresholds[3] = {0.1, 0.2, 0.2};
		float iouThresholds[3] = {0.5, 0.5, 0.5};
        peoplenet_post_processing(peoplenet_int_ptr, scoreThresholds, iouThresholds, algoresult_peoplenet,el_algo);
		#if EACH_STEP_TICK
			SystemGetTick(&systick_2, &loop_cnt_2);
			dbg_printf(DBG_LESS_INFO,"Tick for Invoke for peoplenet_post_processing:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif
		#if PEOPLENET_DBG_APP_LOG
			xprintf("peoplenet_post_processing done\r\n");
		#endif
		#ifdef TOTAL_STEP_TICK						
			SystemGetTick(&systick_2, &loop_cnt_2);
			// dbg_printf(DBG_LESS_INFO,"Tick for TOTAL YOLOV8 OB:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));		
		#endif

    }
	

#ifdef UART_SEND_ALOGO_RESEULT
	algoresult_peoplenet->algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2) + capture_image_tick;
uint32_t judge_case_data;
uint32_t g_trans_type;
hx_drv_swreg_aon_get_appused1(&judge_case_data);
g_trans_type = (judge_case_data>>16);
if( g_trans_type == 0 || g_trans_type == 2)// transfer type is (UART) or (UART & SPI) 
{
	//invalid dcache to let uart can send the right jpeg img out
	hx_InvalidateDCache_by_Addr((volatile void *)app_get_jpeg_addr(), sizeof(uint8_t) *app_get_jpeg_sz());

	el_img_t temp_el_jpg_img = el_img_t{};
	temp_el_jpg_img.data = (uint8_t *)app_get_jpeg_addr();
	temp_el_jpg_img.size = app_get_jpeg_sz();
	temp_el_jpg_img.width = app_get_raw_width();
	temp_el_jpg_img.height = app_get_raw_height();
	temp_el_jpg_img.format = EL_PIXEL_FORMAT_JPEG;
	temp_el_jpg_img.rotate = EL_PIXEL_ROTATE_0;

	send_device_id();
	// event_reply(concat_strings(", ", peoplenet_box_results_2_json_str(el_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
	event_reply(concat_strings(", ", algo_tick_2_json_str(algoresult_peoplenet->algo_tick),", ", peoplenet_box_results_2_json_str(el_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
	// event_reply(concat_strings(", ", algo_tick_2_json_str(algoresult_peoplenet->algo_tick),", ", peoplenet_box_results_2_json_str(el_algo), ", ", img_2_json_str_assign_buf(&temp_el_jpg_img,(char *)str_assign_buf)));

}
	set_model_change_by_uart();
#endif	

	SystemGetTick(&systick_1, &loop_cnt_1);
	//recapture image
	sensordplib_retrigger_capture();

	
	SystemGetTick(&systick_2, &loop_cnt_2);
	capture_image_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2);	
	return ercode;
}

int cv_peoplenet_deinit()
{
	return 0;
}

