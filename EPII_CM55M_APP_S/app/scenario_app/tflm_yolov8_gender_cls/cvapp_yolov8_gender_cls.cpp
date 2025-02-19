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
#include "cvapp_yolov8_gender_cls.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"

#include "img_proc_helium.h"
#include "yolo_postprocessing.h"


#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"
#include <send_result.h>



#define INPUT_IMAGE_CHANNELS 3

#define YOLOV8N_GENDER_CLS_INPUT_TENSOR_WIDTH   160
#define YOLOV8N_GENDER_CLS_INPUT_TENSOR_HEIGHT  160
#define YOLOV8N_GENDER_CLS_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS

#define FD_INPUT_TENSOR_WIDTH   		160
#define FD_INPUT_TENSOR_HEIGHT  		160

#define CROP_IMAGE_WIDTH				640
#define CROP_IMAGE_HEIGHT				480

#define FD_COLOR_CHANNEL					1U

#define YOLOV8N_GENDER_CLS_DBG_APP_LOG 0


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

constexpr int tensor_arena_size = 507*1024;
constexpr int tensor_arena_model_tail_size = 1224;
static uint32_t tensor_arena=0;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *yolov8_gender_cls_int_ptr=nullptr;
TfLiteTensor *yolov8_gender_cls_input, *yolov8_gender_cls_output;

tflite::MicroInterpreter *fd_int_ptr=nullptr;
TfLiteTensor *fd_input, *fd_output, *fd_output2;
network fd_net;

static uint32_t g_fd_gender_init = 0, g_image_mapping_init = 0;
uint32_t num_face_counter = 0;
static tflite::MicroMutableOpResolver<1> op_resolver;

static uint32_t crop_img=0,pad_img=0,resized_img = 0;
};

#if YOLOV8N_GENDER_CLS_DBG_APP_LOG
std::string coco_classes[] = {"person","bicycle","car","motorcycle","airplane","bus","train","truck","boat","traffic light","fire hydrant","stop sign","parking meter","bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase","frisbee","skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup","fork","knife","spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair","couch","potted plant","bed","dining table","toilet","tv","laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink","refrigerator","book","clock","vase","scissors","teddy bear","hair drier","toothbrush"};
int coco_ids[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 28, 31,
                      32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
                      57, 58, 59, 60, 61, 62, 63, 64, 65, 67, 70, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 84, 85,
                      86, 87, 88, 89, 90};

#endif

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
	void * const ethosu_base_address = (void *)(U55_BASE);

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


static network yolo_post_processing_init(TfLiteTensor* out_ten, TfLiteTensor* out2_ten)
{
	int input_w = FD_INPUT_TENSOR_WIDTH;
	int input_h = FD_INPUT_TENSOR_HEIGHT;
	int num_classes = 1;
	int num_branch = 2;
	int topN = 0;
	branch* branchs = (branch*)calloc(num_branch, sizeof(branch));
	static float anchor1[] = {38,63, 66,100, 121,163};
	static float anchor2[] = {6,10 , 12,22, 23,39};
	branchs[0] = create_brach(input_w/32, 3, anchor1, out_ten->data.int8, out_ten->bytes , out_ten->params.scale, out_ten->params.zero_point);
	branchs[1] = create_brach(input_w/16, 3, anchor2, out2_ten->data.int8,  out2_ten->bytes, out2_ten->params.scale, out2_ten->params.zero_point);

	network net = creat_network(input_w, input_h, num_classes, num_branch, branchs,topN);
#ifdef FD_FL_DEBUG
	xprintf("is_post_processing_initialized = true\n");

	xprintf("input_w:%d,input_h:%d,num_classes:%d,num_branch:%d,branchs:%x\n",net.input_w,net.input_h,net.num_classes,net.num_branch,net.branchs);
	xprintf("[branch2]resolution:%d\n",net.branchs[1].resolution,net.branchs[1].num_box);
	xprintf("[branch]addr:%x:%x\n",net.branchs[0],net.branchs[1]);
#endif
	return net;
}

static void yolo_post_processing(network* net, struct_yolov8_gender_cls_algoResult * alg)
{
	int nboxes=0;
	float thresh = .50;
	float nms = .45;
	uint32_t sensor_width = app_get_raw_width();
	uint32_t sensor_height = app_get_raw_height();

	std::forward_list<detection> dets = get_network_boxes(net, sensor_width, sensor_height, thresh, &nboxes);
#ifdef FD_FL_DEBUG
	dbg_printf(DBG_LESS_INFO,"box:%d\n",nboxes);
#endif

	//clear_alg_rsult(&alg_result);
	// do nms
	diounms_sort(dets, net->num_classes, nms);

	for (std::forward_list<detection>::iterator it=dets.begin(); it != dets.end(); ++it){
		/**************
		 *
		 * To let FD bbox do not too tight to do FL
			python code to calculate boxes
			ymin = max(d['y'] - d['h']/2, 0)
			d['w'] = d['w'] * 1.2
			d['h'] = d['h'] * 1.2
			xmin = max(d['x'] - d['w']/2, 0)
			xmax = min(d['x'] + d['w']/2, image_shape[1])
			ymax = min(d['y'] + d['h']/2, image_shape[0])
			boxes.append([xmin, ymin, xmax, ymax])
		**************/
		/*** do not change final it->bbox.w and it->bbox.h value
		 * so combine this part at below code
		//		it->bbox.w = it->bbox.w * 1.2;
		//		it->bbox.h = it->bbox.h * 1.2;
		*****/

		float box_scale_factor = 1.3;

		float ymin = it->bbox.y - ((it->bbox.h ) / box_scale_factor);
		float xmin = it->bbox.x - ((it->bbox.w * box_scale_factor) / 2.);
		float xmax = it->bbox.x + ((it->bbox.w * box_scale_factor) / 2.);
		float ymax = it->bbox.y + ((it->bbox.h * box_scale_factor) / 2.);

		if (xmin < 0) xmin = 0;
		if (ymin < 0) ymin = 0;
		if (xmax > sensor_width) xmax = sensor_width;
		if (ymax > sensor_height) ymax = sensor_height;

		float bx = xmin;
		float by = ymin;
		float bw = xmax - xmin;
		float bh = ymax - ymin;
		for (int j = 0; j < net->num_classes; ++j) {
			if (it->prob[j] > 0 && num_face_counter < MAX_TRACKED_YOLOV8_ALGO_RES) {
				alg->obr[num_face_counter].confidence = it->prob[j];
				alg->obr[num_face_counter].bbox.x = (uint32_t)(bx);      //xmin
				alg->obr[num_face_counter].bbox.y = (uint32_t)(by);      //ymin
				alg->obr[num_face_counter].bbox.width = (uint32_t)(bw);  //xmax
				alg->obr[num_face_counter].bbox.height = (uint32_t)(bh); //ymax
				alg->obr[num_face_counter].class_idx = 0;//face

				num_face_counter++;
			}
		}
		#ifdef FD_DEBUG
		xprintf("alg_result->num_tracked_human_targets: %d\r\n",alg_result->num_tracked_human_targets);
		#endif
	}
	free_dets(dets);
	//free((net->branchs));
}




int cv_yolov8_gender_cls_init(bool security_enable, bool privilege_enable, uint32_t fd_model_addr, uint32_t gender_cls_model_addr ) {
	int ercode = 0;

	//set memory allocation to tensor_arena
	tensor_arena = mm_reserve_align(tensor_arena_size,0x20); //1mb
	xprintf("TA[%x]\r\n",tensor_arena);


	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	if(fd_model_addr != 0) {
		static const tflite::Model*fd_model 			   = tflite::GetModel((const void *)fd_model_addr);
		static const tflite::Model*yolov8_gender_cls_model = tflite::GetModel((const void *)gender_cls_model_addr);
		if (fd_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] FD model's schema version %d is not equal "
				"to supported version %d\n",
				fd_model->version(), TFLITE_SCHEMA_VERSION);
			return false;
		}
		else {
			xprintf("FD model's schema version %d\n", fd_model->version());
		}
		if (yolov8_gender_cls_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] yolov8_gender_cls_model's schema version %d is not equal "
				"to supported version %d\n",
				yolov8_gender_cls_model->version(), TFLITE_SCHEMA_VERSION);
			return -1;
		}
		else {
			xprintf("yolov8_gender_cls_model model's schema version %d\n", yolov8_gender_cls_model->version());
		}

		if(g_fd_gender_init == 0)
		{
			
			if (kTfLiteOk != op_resolver.AddEthosU()){
				xprintf("Failed to add Arm NPU support to op resolver.");
				return false;
			}
		}


		static tflite::MicroInterpreter yolov8_gender_cls_static_interpreter(yolov8_gender_cls_model, op_resolver,(uint8_t*)tensor_arena, tensor_arena_size);  
		static tflite::MicroInterpreter fd_static_interpreter(fd_model, op_resolver, (uint8_t*)tensor_arena, tensor_arena_size-tensor_arena_model_tail_size);


		if(yolov8_gender_cls_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		if(fd_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		yolov8_gender_cls_int_ptr = &yolov8_gender_cls_static_interpreter;
		yolov8_gender_cls_input = yolov8_gender_cls_static_interpreter.input(0);
		yolov8_gender_cls_output = yolov8_gender_cls_static_interpreter.output(0);

		fd_int_ptr = &fd_static_interpreter;
		fd_input = fd_static_interpreter.input(0);
		fd_output = fd_static_interpreter.output(0);
		fd_output2 = fd_static_interpreter.output(1);

		fd_net = yolo_post_processing_init(fd_output, fd_output2);

	}

	xprintf("initial done\n");
	g_fd_gender_init = 1;
	return ercode;
}

//Expand Y channel to 3 Y channel u_int8
static void Y_to_YYY_uint8(uint8_t*in_image, uint8_t*out_image,int32_t in_image_width, int32_t in_image_height)
{
	int32_t x,y;
	uint8_t *Y_image=in_image;
	
	//channel adjust (in_image Y->YYY)
	for (y = 0; y < in_image_height; y++) {//compute new pixels
			for (x = 0; x < in_image_width; x++) {

				out_image[(in_image_width*y+x)*3] = Y_image[y * in_image_width + x];
				out_image[(in_image_width*y+x)*3+1] = Y_image[y * in_image_width + x];
				out_image[(in_image_width*y+x)*3+2] = Y_image[y * in_image_width + x];
			}
	}


}

int cv_yolov8_gender_cls_run(struct_yolov8_gender_cls_algoResult *algoresult_yolov8_gender_cls) {
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t raw_addr = app_get_raw_addr();
    uint32_t expand = 0;
	std::forward_list<el_box_t> el_algo;
	std::forward_list<el_box_t> el_gender_cls_algo;
	TfLiteStatus invoke_status=kTfLiteOk;

	#if YOLOV8N_GENDER_CLS_DBG_APP_LOG
    xprintf("raw info: w[%d] h[%d] ch[%d] addr[%x]\n",img_w, img_h, ch, raw_addr);
	#endif

    if(yolov8_gender_cls_int_ptr!= nullptr) {
		#ifdef TOTAL_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif

		/////start Face detection
		if(g_image_mapping_init==0) {
			g_image_mapping_init = 1;
			resized_img = mm_reserve_align(YOLOV8N_GENDER_CLS_INPUT_TENSOR_WIDTH * YOLOV8N_GENDER_CLS_INPUT_TENSOR_HEIGHT,0x20); //160*160*1
			crop_img = mm_reserve_align(img_w*img_h*FD_COLOR_CHANNEL,0x20); //640*480*1
			pad_img = mm_reserve_align(img_w*img_w*FD_COLOR_CHANNEL,0x20); //640*480*1
			#if DBG_APP_LOG
			xprintf("addr: crop[%x] \n",crop_img);
			#endif
		}

		w_scale = (float)(img_w - 1) / (FD_INPUT_TENSOR_WIDTH - 1);
		h_scale = (float)(img_h - 1) / (FD_INPUT_TENSOR_HEIGHT - 1);
		hx_lib_image_resize_helium((uint8_t*)raw_addr, (uint8_t*)fd_input->data.data,
			img_w, img_h, (int)FD_COLOR_CHANNEL,
			FD_INPUT_TENSOR_WIDTH, FD_INPUT_TENSOR_HEIGHT, w_scale,h_scale);

		for (int i = 0; i < fd_input->bytes; ++i) {
			*((int8_t *)fd_input->data.data+i) = *((int8_t *)fd_input->data.data+i) - 128;
		}
		invoke_status = fd_int_ptr->Invoke();
		
		if(invoke_status != kTfLiteOk)
		{
			xprintf("face detection invoke fail\n");
			return -1;
		}
		else
		{
			#if DBG_APP_LOG
			xprintf("face detection invoke pass\n");
			#endif
		}

		num_face_counter = 0;

		//retrieve output data
		yolo_post_processing(&fd_net, algoresult_yolov8_gender_cls);

		if(num_face_counter == 0)
		{

		}
		else
		{
			/* sort biggest face */
			for(int how_many_face_i=0;how_many_face_i < num_face_counter;how_many_face_i++)
			{
				if((algoresult_yolov8_gender_cls->obr[0].bbox.width * algoresult_yolov8_gender_cls->obr[0].bbox.height)<(algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.width *algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.height))
				{
					uint32_t temp_width = algoresult_yolov8_gender_cls->obr[0].bbox.width ;
					uint32_t temp_height = algoresult_yolov8_gender_cls->obr[0].bbox.height;
					uint32_t temp_x = algoresult_yolov8_gender_cls->obr[0].bbox.x;
					uint32_t temp_y = algoresult_yolov8_gender_cls->obr[0].bbox.y;
					float temp_score = algoresult_yolov8_gender_cls->obr[0].confidence;
					algoresult_yolov8_gender_cls->obr[0].bbox.width  = algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.width ;
					algoresult_yolov8_gender_cls->obr[0].bbox.height = algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.height;
					algoresult_yolov8_gender_cls->obr[0].bbox.x = algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.x;
					algoresult_yolov8_gender_cls->obr[0].bbox.y = algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.y;
					algoresult_yolov8_gender_cls->obr[0].confidence = (float)algoresult_yolov8_gender_cls->obr[how_many_face_i].confidence;

					algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.width = temp_width;
					algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.height = temp_height;
					algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.x = temp_x;
					algoresult_yolov8_gender_cls->obr[how_many_face_i].bbox.y = temp_y;
					algoresult_yolov8_gender_cls->obr[how_many_face_i].confidence = (float)temp_score;
				}
			}

			int cur_face_index = 0;// do the biggest face
			//crop face area to crop_img
			hx_lib_image_copy_helium((uint8_t*)raw_addr, (uint8_t*)crop_img, img_w, img_h, FD_COLOR_CHANNEL, \
					algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.x, algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.y, \
					algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width, algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height);

			//do padding
			int pad_sz = 0;
			if(algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height > algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width)
			{
				int diff = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height  - algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width;
				pad_sz = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height ;

				hx_lib_pad_image((uint8_t*)crop_img, (uint8_t*)pad_img,
						algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width, algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height, FD_COLOR_CHANNEL,
					0, 0, (diff / 2),  (diff / 2 + diff % 2), pad_sz, pad_sz);/**pad_top, pad_bottom, pad_left,  pad_right**/

			}
			else if(algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width > algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height)
			{
				int diff = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width - algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height;
				pad_sz = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width;

				hx_lib_pad_image((uint8_t*)crop_img, (uint8_t*)pad_img,
						algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width, algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height, FD_COLOR_CHANNEL,
					(diff / 2),  (diff / 2 + diff % 2) , 0,  0, pad_sz, pad_sz);/**pad_top, pad_bottom, pad_left,  pad_right**/

			}
			else
			{
				pad_sz = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width;

				memcpy((void *)crop_img, (void *)pad_img, sizeof(uint8_t)*pad_sz*pad_sz*FD_COLOR_CHANNEL);
			}

			w_scale = (float)(pad_sz- 1) / (YOLOV8N_GENDER_CLS_INPUT_TENSOR_WIDTH - 1);
			h_scale = (float)(pad_sz - 1) / (YOLOV8N_GENDER_CLS_INPUT_TENSOR_HEIGHT - 1);
			hx_lib_image_resize_helium((uint8_t*)pad_img, (uint8_t*)resized_img,
						pad_sz, pad_sz, FD_COLOR_CHANNEL, \
						YOLOV8N_GENDER_CLS_INPUT_TENSOR_WIDTH, YOLOV8N_GENDER_CLS_INPUT_TENSOR_HEIGHT,w_scale,h_scale);

			
			Y_to_YYY_uint8((uint8_t*)resized_img, (uint8_t*)yolov8_gender_cls_input->data.uint8, YOLOV8N_GENDER_CLS_INPUT_TENSOR_WIDTH, YOLOV8N_GENDER_CLS_INPUT_TENSOR_HEIGHT);
			#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
			#endif
			invoke_status = yolov8_gender_cls_int_ptr->Invoke();

			#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_2, &loop_cnt_2);
			#endif
			if(invoke_status != kTfLiteOk)
			{
				xprintf("yolov8n gender classification invoke fail\n");
				return -1;
			}
			else
			{
				#if YOLOV8N_GENDER_CLS_DBG_APP_LOG
				xprintf("yolov8n gender classification invoke pass\n");
				#endif
			}
			#ifdef EACH_STEP_TICK
				dbg_printf(DBG_LESS_INFO,"Tick for Invoke for yolov8n gender classification:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
			#endif

			#ifdef EACH_STEP_TICK
				SystemGetTick(&systick_1, &loop_cnt_1);
			#endif

			//gender post-proccessing
			//retrieve output data
			//male or female
			//   "0": "Female",
  			//   "1": "Male"
			uint8_t final_cls = 0;
			float final_score = 0;
			if(yolov8_gender_cls_output->data.uint8[0] < yolov8_gender_cls_output->data.uint8[1] ) 
			{
				final_cls = 1;
				final_score = (float)yolov8_gender_cls_output->data.uint8[1] / 255. ;
			}
			else
			{
				final_cls = 0;
				final_score = (float)yolov8_gender_cls_output->data.uint8[0] / 255.;

			}
			algoresult_yolov8_gender_cls->gen_cls[cur_face_index].confidence = (float)final_score;
			algoresult_yolov8_gender_cls->gen_cls[cur_face_index].class_idx = final_cls;
			algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.x = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.x; 
			algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.y = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.y;
			algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.width = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.width ;
			algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.height = algoresult_yolov8_gender_cls->obr[cur_face_index].bbox.height;

			el_box_t temp_el_box;
			temp_el_box.score =  algoresult_yolov8_gender_cls->gen_cls[cur_face_index].confidence* 100.;
			temp_el_box.target =  algoresult_yolov8_gender_cls->gen_cls[cur_face_index].class_idx ;
			temp_el_box.x = algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.x; //xmin
			temp_el_box.y = algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.y;
			temp_el_box.w = algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.width ; //xmax
			temp_el_box.h = algoresult_yolov8_gender_cls->gen_cls[cur_face_index].bbox.height; //ymax

			el_gender_cls_algo.emplace_front(temp_el_box);
			cur_face_index++;
			#ifdef EACH_STEP_TICK
				SystemGetTick(&systick_2, &loop_cnt_2);
				dbg_printf(DBG_LESS_INFO,"Tick for Invoke for yolov8n gender classification post_processing:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
			#endif
		}

		for (int kkk = 0; kkk < num_face_counter; kkk++) {
			if (kkk < MAX_TRACKED_YOLOV8_ALGO_RES) {

				el_box_t temp_el_box;
				temp_el_box.score =  algoresult_yolov8_gender_cls->obr[kkk].confidence * 100.;
				temp_el_box.target =  algoresult_yolov8_gender_cls->obr[kkk].class_idx ;
				temp_el_box.x = algoresult_yolov8_gender_cls->obr[kkk].bbox.x ; //xmin
				temp_el_box.y = algoresult_yolov8_gender_cls->obr[kkk].bbox.y ;
				temp_el_box.w = algoresult_yolov8_gender_cls->obr[kkk].bbox.width; //xmax
				temp_el_box.h = algoresult_yolov8_gender_cls->obr[kkk].bbox.height; //ymax


				// printf("temp_el_box.x %d,temp_el_box.y: %d\r\n",temp_el_box.x,temp_el_box.y);
				el_algo.emplace_front(temp_el_box);
			}
		}
		
		

    }
	
	#ifdef TOTAL_STEP_TICK						
		SystemGetTick(&systick_2, &loop_cnt_2);
		// dbg_printf(DBG_LESS_INFO,"Tick for TOTAL  yolov8n gender classification:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));		
	#endif
#ifdef UART_SEND_ALOGO_RESEULT
	algoresult_yolov8_gender_cls->algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2) + capture_image_tick;
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
	event_reply(concat_strings(", ", algo_tick_2_json_str(algoresult_yolov8_gender_cls->algo_tick),", ", 	fm_face_bbox_results_2_json_str(el_algo), ", ",  gender_cls_box_results_2_json_str(el_gender_cls_algo), ", ",img_2_json_str(&temp_el_jpg_img)));
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

int cv_yolov8_gender_cls_deinit()
{
	
	return 0;
}

