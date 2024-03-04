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
#include "cvapp_yolov8_pose.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "img_proc_helium.h"


#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"
#include "yolo_postprocessing.h"
#include "send_result.h"
#define YOLOV8_POSE_INPUT_224 0
#define YOLOV8_POSE_INPUT_256 1

#if YOLOV8_POSE_INPUT_224
#define YOLOV8_POSE_INPUT_TENSOR_WIDTH   224
#define YOLOV8_POSE_INPUT_TENSOR_HEIGHT  224

#elif YOLOV8_POSE_INPUT_256
#define YOLOV8_POSE_INPUT_TENSOR_WIDTH   256
#define YOLOV8_POSE_INPUT_TENSOR_HEIGHT  256

#else
#define YOLOV8_POSE_INPUT_TENSOR_WIDTH   192
#define YOLOV8_POSE_INPUT_TENSOR_HEIGHT  192
#endif

#define INPUT_IMAGE_CHANNELS 3
#define YOLOV8_POSE_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS


#define  EACH_STEP_TICK 0
#define TOTAL_STEP_TICK 1

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

#if YOLOV8_POSE_INPUT_224
constexpr int tensor_arena_size = 398*1024;

#elif YOLOV8_POSE_INPUT_256
constexpr int tensor_arena_size = 518*1024;

#else
constexpr int tensor_arena_size = 294*1024;
#endif

static uint32_t tensor_arena=0;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *yolov8_pose_int_ptr=nullptr;
TfLiteTensor *yolov8_pose_input, *(yolov8_pose_output[7]);

};

int dim_total_size = 0;
static float* stride_756_1;
static float** anchor_756_2;

#define CPU_CLK	0xffffff+1
uint32_t systick_1, systick_2;
uint32_t loop_cnt_1, loop_cnt_2;
static uint32_t capture_image_tick = 0;

// #endif

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
    const void * ethosu_base_address = (void *)(U55_BASE);

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


void anchor_stride_matrix_construct()
{
			///// construct stride matrix 
	#if DBG_APP_LOG
		printf("construct stride matrix start\r\n");
	#endif

	int stride = 8;
	int start_stride_step = 0;
	int max_stride_step = pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride),2);
	for(int j=0;j<3;j++)
	{
		if(j==1)
		{
			stride = 16;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride),2);
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
		}
		else if(j==2)
		{
			stride = 32;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride),2);
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
		}
		else
		{
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
			////initial value
		}
		for(int i = start_stride_step;i < max_stride_step;i++)
		{
			stride_756_1[i] = stride;
			// printf("%d %f ",i,stride_756_1[i]);
		}
		
	}
	#if DBG_APP_LOG
		printf("construct stride matrix done\r\n");
	#endif
	///// construct stride matrix 
		///// construct anchor matrix 
	#if DBG_APP_LOG
		printf("construct anchor matrix start\r\n");
	#endif

	stride = 8;
	start_stride_step = 0;
	max_stride_step = pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride),2);
	for(int j=0;j<3;j++)
	{
		if(j==1)
		{
			stride = 16;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride),2);
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
		}
		else if(j==2)
		{
			stride = 32;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride),2);
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
		}
		else
		{
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
			////initial value
		}
		float strart_anchor_value = 0.5;
		int max_anchor_value = (YOLOV8_POSE_INPUT_TENSOR_WIDTH/stride);
		float anchor_step_value = 0.;
		float anchor_2_step_value = -1.;
		for(int i = start_stride_step;i < max_stride_step;i++)
		{
			if((i%max_anchor_value)==0)
			{
				strart_anchor_value = 0.5;
				anchor_step_value = 0.;
				anchor_2_step_value++;
				// printf("\r\n");
			}

			anchor_756_2[i][0] = strart_anchor_value + (anchor_step_value++);
			anchor_756_2[i][1] = strart_anchor_value + anchor_2_step_value;
			// printf("%f ",anchor_756_2[i][0]);
			// printf("%d %f ",i,anchor_756_2[i][1]);
		}
	}
	#if DBG_APP_LOG
		printf("construct anchor matrix done\r\n");
	#endif
}



int cv_yolov8_pose_init(bool security_enable, bool privilege_enable, uint32_t model_addr) {
	int dim_stride = 8;
	int dim_stride_8_size = pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/dim_stride),2);
	for(int i = 0; i < 3;i++)
	{
		if(i==0)
		{
			dim_stride = 8;
		}
		else if(i==1)
		{
			dim_stride = 16;
		}
		else
		{
			dim_stride = 32;
		}
		dim_total_size  += pow((YOLOV8_POSE_INPUT_TENSOR_WIDTH/dim_stride),2);
	}
	stride_756_1 = (float*)calloc(dim_total_size, sizeof(float));
	anchor_756_2 = (float**)calloc(dim_total_size, sizeof(float *));
	for(int i=0;i<dim_total_size;i++)
	{
		anchor_756_2[i] = (float*)calloc(2, sizeof(float));
	}
    anchor_stride_matrix_construct();
	int ercode = 0;

	//set memory allocation to tensor_arena
	tensor_arena = mm_reserve_align(tensor_arena_size,0x20); //1mb
	xprintf("TA[%x]\r\n",tensor_arena);


	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	if(model_addr != 0) {
		static const tflite::Model*yolov8_pose_model = tflite::GetModel((const void *)model_addr);

		if (yolov8_pose_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] yolov8_pose_model's schema version %d is not equal "
				"to supported version %d\n",
				yolov8_pose_model->version(), TFLITE_SCHEMA_VERSION);
			return -1;
		}
		else {
			xprintf("yolov8_pose_model model's schema version %d\n", yolov8_pose_model->version());
		}

		static tflite::MicroErrorReporter yolov8_pose_micro_error_reporter;
		static tflite::MicroMutableOpResolver<2> yolov8_pose_op_resolver;

        yolov8_pose_op_resolver.AddTranspose();
		if (kTfLiteOk != yolov8_pose_op_resolver.AddEthosU()){
			xprintf("Failed to add Arm NPU support to op resolver.");
			return false;
		}

		static tflite::MicroInterpreter yolov8_pose_static_interpreter(yolov8_pose_model, yolov8_pose_op_resolver,
				(uint8_t*)tensor_arena, tensor_arena_size, &yolov8_pose_micro_error_reporter);

		if(yolov8_pose_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		yolov8_pose_int_ptr = &yolov8_pose_static_interpreter;
		yolov8_pose_input = yolov8_pose_static_interpreter.input(0);
        for(int i = 0;i < 7;i++)
        {
		    yolov8_pose_output[i] = yolov8_pose_static_interpreter.output(i);
        }

	}
	xprintf("initial done\n");
	return ercode;
}

typedef struct detection_cls_yolov8{
    box bbox;
    float confidence;
    float index;

} detection_cls_yolov8;

bool yolov8_det_comparator(detection_cls_yolov8 &pa, detection_cls_yolov8 &pb)
{
    return pa.confidence > pb.confidence;
}

static void  yolov8_NMSBoxes(std::vector<box> &boxes,std::vector<float> &confidences,float modelScoreThreshold,float modelNMSThreshold,std::vector<int>& nms_result)
{
    detection_cls_yolov8 yolov8_bbox;
    std::vector<detection_cls_yolov8> yolov8_bboxes{};
    for(int i = 0; i < boxes.size(); i++)
    {
        yolov8_bbox.bbox = boxes[i];
        yolov8_bbox.confidence = confidences[i];
        yolov8_bbox.index = i;
        yolov8_bboxes.push_back(yolov8_bbox);
    }
    sort(yolov8_bboxes.begin(), yolov8_bboxes.end(), yolov8_det_comparator);
    int updated_size = yolov8_bboxes.size();
    for(int k = 0; k < updated_size; k++)
    {
        if(yolov8_bboxes[k].confidence < modelScoreThreshold)
        {
            continue;
        }
        
        nms_result.push_back(yolov8_bboxes[k].index);
        for(int j = k + 1; j < updated_size; j++)
        {
            float iou = box_iou(yolov8_bboxes[k].bbox, yolov8_bboxes[j].bbox);
            // float iou = box_diou(yolov8_bboxes[k].bbox, yolov8_bboxes[j].bbox);
            if(iou > modelNMSThreshold)
            {
                yolov8_bboxes.erase(yolov8_bboxes.begin() + j);
                updated_size = yolov8_bboxes.size();
                j = j -1;
            }
        }

    }
}

static void softmax(float *input, size_t input_len) {
  assert(input);
  // assert(input_len >= 0);  Not needed

  float m = -INFINITY;
  for (size_t i = 0; i < input_len; i++) {
    if (input[i] > m) {
      m = input[i];
    }
  }

  float sum = 0.0;
  for (size_t i = 0; i < input_len; i++) {
    sum += expf(input[i] - m);
  }

  float offset = m + logf(sum);
  for (size_t i = 0; i < input_len; i++) {
    input[i] = expf(input[i] - offset);
  }
}


typedef struct struct_human_pose_17{
    struct_human_pose hpr[HUMAN_POSE_POINT_NUM];
} struct_human_pose_17;

/***
 * caculate yolov8 pose bbox dequant value
 * int dims_cnt_1: the index for dim 1 (max index = 756) 
 * int dims_cnt_2: the index for dim 2 (max index = 64 or 1) 
 * TfLiteTensor* output: pointer for output tensor
 * **/
float yolov8_pose_bbox_dequant_value(int dims_cnt_1, int dims_cnt_2,TfLiteTensor* output)
{
	int value =  output->data.int8[ dims_cnt_2 + dims_cnt_1 * output->dims->data[2]];
			
	float deq_value = ((float) value-(float)((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0]) * ((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0];
	return deq_value;
}

/***
 * caculate yolov8 pose key points dequant value
 * int dims_cnt_1: the index for dim 1 (max index = 756) 
 * int dims_cnt_2: the index for dim 2 (max index = 51) 
 * TfLiteTensor* output: pointer for output tensor
 * float anchor_val_0: anchor value for dim 1
 * float anchor_val_1: anchor value for dim 2
 * float anchor_val_1: stride value
 * **/
float yolov8_pose_key_pts_dequant_value(int dims_cnt_1, int dims_cnt_2,TfLiteTensor* output, float anchor_val_0, float anchor_val_1 , float stride_val)
{
	int value =  output->data.int8[ dims_cnt_2 + dims_cnt_1 * output->dims->data[2]];
			
	float deq_value = ((float) value-(float)((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0]) * ((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0];
	
	if(dims_cnt_2 % 3==0)
	{
		deq_value = ( deq_value * 2.0 +  (anchor_val_0 - 0.5) )* stride_val;
	}
	else if(dims_cnt_2 % 3==1)
	{
		deq_value = ( deq_value * 2.0 +  (anchor_val_1 - 0.5) )* stride_val;
	}
	else
	{
		deq_value = sigmoid(deq_value);
	}
	return deq_value;
}


/***
 * caculate bbox xywh for yolov8 pose
 * int j: the index for dim 1 (max index = 756) 
 * TfLiteTensor* output[7]: pointer for output tensor
 * box *bbox: output bbox result
 * float anchor_756_2[][2]: anchor matrix value
 * float *stride_756_1: stride matrix value
 * **/
// void yolov8_pose_cal_xywh(int j,TfLiteTensor* output[7], box *bbox, float anchor_756_2[][2],float *stride_756_1 )
// void yolov8_pose_cal_xywh(int j,TfLiteTensor* output[7], box *bbox, float** anchor_756_2,float *stride_756_1 )
void yolov8_pose_cal_xywh(int j,TfLiteTensor* output[7], box *bbox, float** anchor_756_2,float *stride_756_1, int *out_dim_size )
{
    float  xywh_result[4];
    //do DFL (softmax and than do conv2d)
    int output_data_idx;
    for(int k = 0 ; k < 4 ; k++)
    {
        float tmp_arr_softmax_conv2d[16];
        float tmp_arr_softmax_conv2d_result=0;
        for(int i = 0 ; i < 16 ; i++)
        {
			float tmp_result = 0;
            if(j < out_dim_size[0])//576
            {
                output_data_idx = 1;
                tmp_result = yolov8_pose_bbox_dequant_value(j, k*16+i,output[output_data_idx]);
            }
            else if(j < out_dim_size[1])//720
            {
                output_data_idx = 0;
                tmp_result = yolov8_pose_bbox_dequant_value(j - out_dim_size[0], k*16+i,output[output_data_idx]);
            }
            else 
            {
                output_data_idx = 5;
                tmp_result = yolov8_pose_bbox_dequant_value(j - out_dim_size[1], k*16+i,output[output_data_idx]);
            }
            tmp_arr_softmax_conv2d[i] = tmp_result;
            // tmp_arr_softmax_conv2d[i] = outputs_data_756_64[j][k*16+i];
            // if(j==0)
            // {
            //     printf("outputs_data_756_64[%d][%d]: %f\r\n",j,k*16+i,outputs_data_756_64[j][k*16+i]);
            // }
        }
        softmax(tmp_arr_softmax_conv2d,16);
        for(int i = 0 ; i < 16 ; i++)
        {

            tmp_arr_softmax_conv2d_result = tmp_arr_softmax_conv2d_result + tmp_arr_softmax_conv2d[i]*i;
            // if(j==0)printf("tmp_arr_softmax_conv2d[%d]: %f\r\n",i,tmp_arr_softmax_conv2d[i]);
        }
        xywh_result[k] = tmp_arr_softmax_conv2d_result;

    }

    /**dist2bbox * stride start***/
    float x1 = anchor_756_2[j][0] -  xywh_result[0];
    float y1 = anchor_756_2[j][1] -  xywh_result[1];
    float x2 = anchor_756_2[j][0] +  xywh_result[2];
    float y2 = anchor_756_2[j][1] +  xywh_result[3];
    
    float cx = (x1 + x2)/2.;
    float cy = (y1 + y2)/2.;
    float w = x2 - x1;
    float h = y2 - y1;

    xywh_result[0] = cx * stride_756_1[j];
    xywh_result[1] = cy * stride_756_1[j];
    xywh_result[2] = w * stride_756_1[j];
    xywh_result[3] = h * stride_756_1[j];

    bbox->x = xywh_result[0] - (0.5 * xywh_result[2]);
    bbox->y = xywh_result[1] - (0.5 * xywh_result[3]);
    bbox->w = xywh_result[2];
    bbox->h = xywh_result[3];
    return ;
}

// static void yolov8_pose_post_processing(tflite::MicroInterpreter* static_interpreter,float modelScoreThreshold, float modelNMSThreshold, struct_yolov8_pose_algoResult *alg)
static void yolov8_pose_post_processing(tflite::MicroInterpreter* static_interpreter,float modelScoreThreshold, float modelNMSThreshold, struct_yolov8_pose_algoResult *alg,	std::forward_list<el_keypoint_t> &el_keypoint_algo)
{
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
	TfLiteTensor* output[7];
	for(int i = 0; i < 7;i++)
	{
		output[i] = static_interpreter->output(i);
	}
	int output_data_idx = 0;

	int out_dim_total = 0;
	int numOutputs = static_interpreter->outputs_size();
	int out_dim_size_num = (numOutputs - 1) / 2;
	int out_dim_size[out_dim_size_num];
	for(int out_num = 0; out_num < out_dim_size_num; out_num++)
	{
		if(out_num==0)
		{
			output_data_idx = 4;
		}
		else if(out_num==1)
		{
			output_data_idx = 6;
		}
		else
		{
			output_data_idx = 2;
		}
		out_dim_total += output[output_data_idx]->dims->data[1];
		out_dim_size[out_num] = out_dim_total;
	}
	// ///////////////////////
	// // start postprocessing


	std::vector<int> class_ids;
	std::vector<float> confidences;
	std::vector<box> boxes;
	std::vector< struct_human_pose_17> kpts_vector;

	for(int dims_cnt_1=0;dims_cnt_1<dim_total_size;dims_cnt_1++)
	{
		//////conferen ok
		float maxScore = 0;

		float tmp_result = 0;
		if(dims_cnt_1 < out_dim_size[0])//576
		{
			output_data_idx = 4;
			maxScore = sigmoid(yolov8_pose_bbox_dequant_value(dims_cnt_1, 0,output[output_data_idx]));
		}
		else if(dims_cnt_1 < out_dim_size[1])//720
		{
			output_data_idx = 6;
			maxScore = sigmoid(yolov8_pose_bbox_dequant_value(dims_cnt_1 - out_dim_size[0], 0, output[output_data_idx]));
		}
		else 
		{
			output_data_idx = 2;
			maxScore = sigmoid(yolov8_pose_bbox_dequant_value(dims_cnt_1 - out_dim_size[1], 0,output[output_data_idx]));
		}
		// float maxScore = sigmoid(outputs_data_756_1[dims_cnt_2]);// the first four indexes are bbox information

		if (maxScore >= modelScoreThreshold)
		{
			box bbox;
	
			yolov8_pose_cal_xywh(dims_cnt_1, output, &bbox, anchor_756_2, stride_756_1,out_dim_size );
			boxes.push_back(bbox);
			confidences.push_back(maxScore);
			
			// printf("idx: %d,bbox.x: %f, bbox.y: %f, bbox.w: %f , bbox.h: %f\r\n",dims_cnt_2,bbox.x,bbox.y,bbox.w,bbox.h);
			struct_human_pose_17 kpts;
			for(int k = 0 ; k < 17 ; k++)
			{
				kpts.hpr[k].x = yolov8_pose_key_pts_dequant_value(dims_cnt_1,k*3 , output[3],anchor_756_2[dims_cnt_1][0],anchor_756_2[dims_cnt_1][1],stride_756_1[dims_cnt_1]);
				kpts.hpr[k].y = yolov8_pose_key_pts_dequant_value(dims_cnt_1,k*3+1 , output[3],anchor_756_2[dims_cnt_1][0],anchor_756_2[dims_cnt_1][1],stride_756_1[dims_cnt_1]);
				kpts.hpr[k].score = yolov8_pose_key_pts_dequant_value(dims_cnt_1,k*3+2 , output[3],anchor_756_2[dims_cnt_1][0],anchor_756_2[dims_cnt_1][1],stride_756_1[dims_cnt_1]);
				// printf("idx: %d,kpts[%d] x: %d, y: %d, score: %f\r\n",dims_cnt_2,k,kpts.hpr[k].x,kpts.hpr[k].y,kpts.hpr[k].score);
			}
			kpts_vector.push_back(kpts);
			// printf("\r\n");
		}
	}
	#if DBG_APP_LOG
		printf("boxes.size(): %d\r\n",boxes.size());
	#endif

	/**
	 * do nms
	 * **/

	std::vector<int> nms_result;
	yolov8_NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);
	std::vector<detection_yolov8_pose> results_yolov8_pose;
	for (int i = 0; i < nms_result.size(); i++)
	{
		if(!(MAX_TRACKED_YOLOV8_ALGO_RES-i))break;
		int idx = nms_result[i];

		alg->dypr[i].bbox.x = (uint32_t)boxes[idx].x;

		alg->dypr[i].bbox.y = (uint32_t)boxes[idx].y;
		alg->dypr[i].bbox.width = (uint32_t)boxes[idx].w;
		alg->dypr[i].bbox.height = (uint32_t)boxes[idx].h;

		if(alg->dypr[i].bbox.x >= YOLOV8_POSE_INPUT_TENSOR_WIDTH)alg->dypr[i].bbox.x = YOLOV8_POSE_INPUT_TENSOR_WIDTH;
		if(alg->dypr[i].bbox.y >= YOLOV8_POSE_INPUT_TENSOR_HEIGHT)alg->dypr[i].bbox.y = YOLOV8_POSE_INPUT_TENSOR_HEIGHT;

		if(alg->dypr[i].bbox.width  >= YOLOV8_POSE_INPUT_TENSOR_WIDTH)alg->dypr[i].bbox.width  = YOLOV8_POSE_INPUT_TENSOR_WIDTH;
		if(alg->dypr[i].bbox.height >= YOLOV8_POSE_INPUT_TENSOR_HEIGHT)alg->dypr[i].bbox.height = YOLOV8_POSE_INPUT_TENSOR_HEIGHT;

		alg->dypr[i].bbox.x = (float)alg->dypr[i].bbox.x / (float)YOLOV8_POSE_INPUT_TENSOR_WIDTH *(float)img_w;
		alg->dypr[i].bbox.y = (float)alg->dypr[i].bbox.y / (float)YOLOV8_POSE_INPUT_TENSOR_HEIGHT * (float)img_h;

		alg->dypr[i].bbox.width = (float)alg->dypr[i].bbox.width / (float)YOLOV8_POSE_INPUT_TENSOR_WIDTH *(float)img_w;
		alg->dypr[i].bbox.height = (float)alg->dypr[i].bbox.height / (float)YOLOV8_POSE_INPUT_TENSOR_HEIGHT * (float)img_h;


		alg->dypr[i].confidence = confidences[idx];

		el_keypoint_t temp_el_keypoint;
		for(int k = 0 ; k < KEYPOINT_NUM ; k++)
		{
			alg->dypr[i].hpr[k].x = kpts_vector[idx].hpr[k].x;
			alg->dypr[i].hpr[k].y = kpts_vector[idx].hpr[k].y;
			alg->dypr[i].hpr[k].score = kpts_vector[idx].hpr[k].score;
			#if DBG_APP_LOG
				printf("idx: %d,kpts[%d] x: %d, y: %d, score: %f\r\n",idx,k,kpts_vector[idx].hpr[k].x,kpts_vector[idx].hpr[k].y,kpts_vector[idx].hpr[k].score);
			#endif
			////resize to original image size
			if(alg->dypr[i].hpr[k].x >= YOLOV8_POSE_INPUT_TENSOR_WIDTH)alg->dypr[i].hpr[k].x = YOLOV8_POSE_INPUT_TENSOR_WIDTH;
			if(alg->dypr[i].hpr[k].y >= YOLOV8_POSE_INPUT_TENSOR_HEIGHT)alg->dypr[i].hpr[k].y = YOLOV8_POSE_INPUT_TENSOR_HEIGHT;

			alg->dypr[i].hpr[k].x = (float)alg->dypr[i].hpr[k].x / (float)YOLOV8_POSE_INPUT_TENSOR_WIDTH *(float)img_w;
			alg->dypr[i].hpr[k].y = (float)alg->dypr[i].hpr[k].y / (float)YOLOV8_POSE_INPUT_TENSOR_HEIGHT * (float)img_h;
			///resize to original image size

			/***set uart ouput format***/
			temp_el_keypoint.el_keypoint[k].score =  alg->dypr[i].hpr[k].score*100;
			temp_el_keypoint.el_keypoint[k].target =  k;
			temp_el_keypoint.el_keypoint[k].x = alg->dypr[i].hpr[k].x;
			temp_el_keypoint.el_keypoint[k].y =  alg->dypr[i].hpr[k].y;
		}
		#if DBG_APP_LOG
			printf("id: %d,confidence: %f x: %d, y: %d, w: %d, h: %d\r\n",i,alg->dypr[i].confidence, alg->dypr[i].bbox.x,alg->dypr[i].bbox.y,alg->dypr[i].bbox.width,alg->dypr[i].bbox.height);
		#endif


		temp_el_keypoint.el_box.score =  alg->dypr[i].confidence*100;
		temp_el_keypoint.el_box.target =  i;
		temp_el_keypoint.el_box.x = alg->dypr[i].bbox.x;
		temp_el_keypoint.el_box.y =  alg->dypr[i].bbox.y;
		temp_el_keypoint.el_box.w = alg->dypr[i].bbox.width;
		temp_el_keypoint.el_box.h = alg->dypr[i].bbox.height;


		// printf("temp_el_box.x %d,temp_el_box.y: %d\r\n",temp_el_box.x,temp_el_box.y);
		el_keypoint_algo.emplace_front(temp_el_keypoint);

	}
	
}


int cv_yolov8_pose_run(struct_yolov8_pose_algoResult *algoresult_yolov8_pose) {
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t ch = app_get_raw_channels();
    uint32_t raw_addr = app_get_raw_addr();
    uint32_t expand = 0;
	std::forward_list<el_keypoint_t> el_keypoint_algo;
	

	#if DBG_APP_LOG
    xprintf("raw info: w[%d] h[%d] ch[%d] addr[%x]\n",img_w, img_h, ch, raw_addr);
	#endif

	#if TOTAL_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
	#endif

    if(yolov8_pose_int_ptr!= nullptr) {
    	//get image from sensor and resize
		w_scale = (float)(img_w - 1) / (YOLOV8_POSE_INPUT_TENSOR_WIDTH - 1);
		h_scale = (float)(img_h - 1) / (YOLOV8_POSE_INPUT_TENSOR_HEIGHT - 1);

        #if EACH_STEP_TICK
            SystemGetTick(&systick_1, &loop_cnt_1);
        #endif
		hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr, (uint8_t*)yolov8_pose_input->data.data,  
		                    img_w, img_h, ch, 
                        	YOLOV8_POSE_INPUT_TENSOR_WIDTH, YOLOV8_POSE_INPUT_TENSOR_HEIGHT, w_scale,h_scale);
		#if EACH_STEP_TICK						
            SystemGetTick(&systick_2, &loop_cnt_2);
            xprintf("Tick for resize image BGR8U3C_to_RGB24_helium for yolov8 POSE:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));							
		#endif
        #if EACH_STEP_TICK
            SystemGetTick(&systick_1, &loop_cnt_1);
        #endif
        
        // //uint8 to int8
		for (int i = 0; i < yolov8_pose_input->bytes; ++i) {
			*((int8_t *)yolov8_pose_input->data.data+i) = *((int8_t *)yolov8_pose_input->data.data+i) - 128;
    	}

        #if EACH_STEP_TICK
            SystemGetTick(&systick_2, &loop_cnt_2);
            xprintf("Tick for Invoke for uint8toint8 for YOLOV8_POSE:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
        #endif	


        #if EACH_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
        #endif

		TfLiteStatus invoke_status = yolov8_pose_int_ptr->Invoke();

        #if EACH_STEP_TICK
			SystemGetTick(&systick_2, &loop_cnt_2);
			xprintf("Tick for invoke of yolov8n pose:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));	
        #endif

		if(invoke_status != kTfLiteOk)
		{
			xprintf("yolov8n pose invoke fail\n");
			return -1;
		}
		else
		{
			#if DBG_APP_LOG
			xprintf("yolov8n pose invoke pass\n");
			#endif
		}

        #if EACH_STEP_TICK
            SystemGetTick(&systick_1, &loop_cnt_1);
        #endif
		//retrieve output data


		yolov8_pose_post_processing(yolov8_pose_int_ptr,0.25, 0.45, algoresult_yolov8_pose,el_keypoint_algo );

		#if EACH_STEP_TICK
			SystemGetTick(&systick_2, &loop_cnt_2);
			xprintf("Tick for Invoke for YOLOV8_POSE_post_processing:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
        #endif

		#if DBG_APP_LOG
			xprintf("yolov8 pose done\r\n");
		#endif
    }


#ifdef UART_SEND_ALOGO_RESEULT
	#if TOTAL_STEP_TICK						
		SystemGetTick(&systick_2, &loop_cnt_2);
		algoresult_yolov8_pose->algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2) + capture_image_tick;				
	#endif
uint32_t judge_case_data;
uint32_t g_trans_type;
hx_drv_swreg_aon_get_appused1(&judge_case_data);
g_trans_type = (judge_case_data>>16);
if( g_trans_type == 0 || g_trans_type == 2)// transfer type is (UART) or (UART & SPI) 
{	
	el_img_t temp_el_jpg_img = el_img_t{};
	temp_el_jpg_img.data = (uint8_t *)app_get_jpeg_addr();
	temp_el_jpg_img.size = app_get_jpeg_sz();
	temp_el_jpg_img.width = app_get_raw_width();
	temp_el_jpg_img.height = app_get_raw_height();
	temp_el_jpg_img.format = EL_PIXEL_FORMAT_JPEG;
	temp_el_jpg_img.rotate = EL_PIXEL_ROTATE_0;

	 send_device_id();
	// event_reply(concat_strings(", ", keypoint_results_2_json_str(el_keypoint_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
	event_reply(concat_strings(", ", algo_tick_2_json_str(algoresult_yolov8_pose->algo_tick),", ", keypoint_results_2_json_str(el_keypoint_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
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

int cv_yolov8_pose_deinit()
{
	free(stride_756_1);
	for(int i = 0; i < dim_total_size; i++)
	{
		free(anchor_756_2[i]);
	}
	free(anchor_756_2);
	return 0;
}

