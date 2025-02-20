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
#include "cvapp_yolo11n_ob.h"
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

#define YOLO11_NO_POST_SEPARATE_OUTPUT 1

#define INPUT_IMAGE_CHANNELS 3

#if YOLO11_NO_POST_SEPARATE_OUTPUT
#define YOLO11_OB_INPUT_TENSOR_WIDTH   224
#define YOLO11_OB_INPUT_TENSOR_HEIGHT  224
#else
#define YOLO11_OB_INPUT_TENSOR_WIDTH   192
#define YOLO11_OB_INPUT_TENSOR_HEIGHT  192
#endif
#define YOLO11_OB_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS

#define YOLO11N_OB_DBG_APP_LOG 0


// #define EACH_STEP_TICK
#define TOTAL_STEP_TICK
#define YOLO11_POST_EACH_STEP_TICK 0
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

#if YOLO11_NO_POST_SEPARATE_OUTPUT
constexpr int tensor_arena_size = 442*1024;
#else
constexpr int tensor_arena_size = 1061*1024;
#endif
static uint32_t tensor_arena=0;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *yolo11n_ob_int_ptr=nullptr;
#if YOLO11_NO_POST_SEPARATE_OUTPUT
TfLiteTensor *yolo11n_ob_input, *(yolo11n_ob_output[6]);
#else
TfLiteTensor *yolo11n_ob_input, *yolo11n_ob_output, *yolo11n_ob_output2;
#endif
};
#if YOLO11_NO_POST_SEPARATE_OUTPUT
int dim_total_size = 0;
static float* stride_756_1;
static float** anchor_756_2;
#endif
#if YOLO11N_OB_DBG_APP_LOG
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

#if YOLO11_NO_POST_SEPARATE_OUTPUT
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
/***
 * caculate yolo11detection without post-proccessing bbox dequant value
 * int dims_cnt_1: the index for dim 1 (max index = 24 or 12 or 6) 
 * int dims_cnt_2: the index for dim 2 (max index = 24 or 12 or 6) 
 * int dims_cnt_3: the index for dim 3 (max index = 144) 
 * TfLiteTensor* output: pointer for output tensor
 * **/
float yolo11_nopost_dequant_value(int dims_cnt_1, int dims_cnt_2,int dims_cnt_3,TfLiteTensor* output)
{
    // int height  = output->dims->data[1];
    // int width = output->dims->data[2];
    // int channel  = output->dims->data[3];
    //h * width * channel + w * channel + c
	int value =  output->data.int8[ dims_cnt_1 * output->dims->data[2] * output->dims->data[3] + dims_cnt_2 * output->dims->data[3] + dims_cnt_3];   
	// float deq_value = ((float) value-(float)((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0]) * ((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0];
	float deq_value = (float)( value-((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0]) * ((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0];
	// printf("output->data.uint8[%d][%d][%d]: %d\r\n",dims_cnt_1,dims_cnt_2,dims_cnt_3,value);
    // printf("zero_point: %f , zero_point: %f\r\n",(float)((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0] , (((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0]));
    // printf("deq_value[%d][%d][%d]: %f\r\n",dims_cnt_1,dims_cnt_2,dims_cnt_3,deq_value);
    return deq_value;
}


/***
 * caculate bbox xywh for yolo11 detection without post-proccessing
 * int j: the index for dim 1 (max index = 756) 
 * int dims_cnt_1 : dim 1 counter
 * int dims_cnt_2 : dim 2 counter
 * TfLiteTensor* output: pointer for output tensor
 * box *bbox: output bbox result
 * float anchor_756_2[][2]: anchor matrix value
 * float *stride_756_1: stride matrix value
 * **/
void yolo11_nopost_cal_xywh(int j, int dims_cnt_1, int dims_cnt_2, TfLiteTensor* output, box *bbox, float** anchor_756_2,float *stride_756_1 )
{
     float  xywh_result[4];
    //do DFL (softmax and than do conv2d)
    for(int k = 0 ; k < 4 ; k++)
    {
        float tmp_arr_softmax_conv2d[16];
        float tmp_arr_softmax_conv2d_result = 0;
        for(int i = 0 ; i < 16 ; i++)
        {
            float tmp_result = 0;
            tmp_result = yolo11_nopost_dequant_value(dims_cnt_1, dims_cnt_2,k*16+i,output);
            tmp_arr_softmax_conv2d[i] = tmp_result;
        }
        softmax(tmp_arr_softmax_conv2d,16);
        for(int i = 0 ; i < 16 ; i++)
        {
            tmp_arr_softmax_conv2d_result = tmp_arr_softmax_conv2d_result + tmp_arr_softmax_conv2d[i]*i;
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
void anchor_stride_matrix_construct()
{
			///// construct stride matrix 
	#if DBG_APP_LOG
		printf("construct stride matrix start\r\n");
	#endif

	int stride = 8;
	int start_stride_step = 0;
	int max_stride_step = pow((YOLO11_OB_INPUT_TENSOR_WIDTH/stride),2);
	for(int j=0;j<3;j++)
	{
		if(j==1)
		{
			stride = 16;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLO11_OB_INPUT_TENSOR_WIDTH/stride),2);
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
		}
		else if(j==2)
		{
			stride = 32;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLO11_OB_INPUT_TENSOR_WIDTH/stride),2);
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
	max_stride_step = pow((YOLO11_OB_INPUT_TENSOR_WIDTH/stride),2);
	for(int j=0;j<3;j++)
	{
		if(j==1)
		{
			stride = 16;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLO11_OB_INPUT_TENSOR_WIDTH/stride),2);
			#if DBG_APP_LOG
				printf("start_stride_step: %d, max_stride_step: %d\r\n",start_stride_step,max_stride_step);
			#endif
		}
		else if(j==2)
		{
			stride = 32;
			start_stride_step = max_stride_step;
			max_stride_step = start_stride_step + pow((YOLO11_OB_INPUT_TENSOR_WIDTH/stride),2);
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
		int max_anchor_value = (YOLO11_OB_INPUT_TENSOR_WIDTH/stride);
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
#endif
void print_float(float f_z)
{
	float o_f_z = f_z;
	short D_z, D_f_z;
	D_z = (short)f_z;
	f_z -=  D_z;

	if(f_z <=0)
		f_z = 0-f_z;

	D_f_z = f_z*1000;
	if(D_z==0 && o_f_z < 0)printf("-%d.%03d\r\n",D_z, D_f_z);
	else printf("%d.%03d\r\n",D_z, D_f_z);
}
void print_model_info(tflite::MicroInterpreter * static_interpreter_ptr)
{
	if(static_interpreter_ptr!= nullptr)
	{
		xprintf("input branch size = %d\r\n",static_interpreter_ptr->inputs_size());
		xprintf("output branch size = %d\r\n",static_interpreter_ptr->outputs_size());
		for(int k = 0 ; k < static_interpreter_ptr->inputs_size(); k++)
		{
			TfLiteTensor *tensor = static_interpreter_ptr->input(k);
			xprintf("static_interpreter_ptr->input(%d) tensor information\r\n",k);
			xprintf("\ttensor type is %s\r\n", TfLiteTypeGetName(tensor->type));
			xprintf("\ttensor occupies %d bytes with dimensions\r\n",
				tensor->bytes);
			for (int i = 0 ; i < tensor->dims->size; ++i) 
			{
				xprintf ("\t\tinput %d dim[%d]: %3d\r\n",k, i, tensor->dims->data[i]);
			}

			TfLiteQuantization quant = tensor->quantization;
			if (kTfLiteAffineQuantization == quant.type) 
			{
				auto* quantParams = (TfLiteAffineQuantization*)quant.params;
				// xprintf("Quant dimension: %d\r\n", quantParams->quantized_dimension);
				for (int i = 0; i < quantParams->scale->size; ++i) {
					xprintf("input %d Scale[%d] = ",k, i);
					print_float(quantParams->scale->data[i]);
				}
				for (int i = 0; i < quantParams->zero_point->size; ++i) {
					xprintf("input %d ZeroPoint[%d] = %d\r\n\n",k, i, quantParams->zero_point->data[i]);
				}
			}
			else
			{
				xprintf("input %d No Quantization number\r\n\n",k);
			}

		}
		

		for(int k = 0 ; k < static_interpreter_ptr->outputs_size(); k++)
		{
			TfLiteTensor *tensor = static_interpreter_ptr->output(k);
			xprintf("static_interpreter_ptr->output(%d) tensor information\r\n",k);
			xprintf("\ttensor type is %s\r\n", TfLiteTypeGetName(tensor->type));
			xprintf("\ttensor occupies %d bytes with dimensions\r\n",
				tensor->bytes);
			for (int i = 0 ; i < tensor->dims->size; ++i) 
			{
				xprintf ("\t\toutput %d dim[%d]: %3d\r\n",k, i, tensor->dims->data[i]);
			}

			TfLiteQuantization quant = tensor->quantization;
			if (kTfLiteAffineQuantization == quant.type) 
			{
				auto* quantParams = (TfLiteAffineQuantization*)quant.params;
				// xprintf("Quant dimension: %d\r\n", quantParams->quantized_dimension);
				for (int i = 0; i < quantParams->scale->size; ++i) {
					xprintf("output %d Scale[%d] = ",k, i);
					print_float(quantParams->scale->data[i]);
				}
				for (int i = 0; i < quantParams->zero_point->size; ++i) {
					xprintf("output %d ZeroPoint[%d] = %d\r\n\n",k, i, quantParams->zero_point->data[i]);
				}
			}
			else
			{
				xprintf("output %d No Quantization number\r\n\n",k);
			}

		}
	}
	return ;
}
int cv_yolo11n_ob_init(bool security_enable, bool privilege_enable, uint32_t model_addr) {
	#if YOLO11_NO_POST_SEPARATE_OUTPUT
	int dim_stride = 8;
	int dim_stride_8_size = pow((YOLO11_OB_INPUT_TENSOR_WIDTH/dim_stride),2);
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
		dim_total_size  += pow((YOLO11_OB_INPUT_TENSOR_WIDTH/dim_stride),2);
	}
	stride_756_1 = (float*)calloc(dim_total_size, sizeof(float));
	anchor_756_2 = (float**)calloc(dim_total_size, sizeof(float *));
	for(int i=0;i<dim_total_size;i++)
	{
		anchor_756_2[i] = (float*)calloc(2, sizeof(float));
	}
    anchor_stride_matrix_construct();
	#endif
	int ercode = 0;

	//set memory allocation to tensor_arena
	tensor_arena = mm_reserve_align(tensor_arena_size,0x20); //1mb
	xprintf("TA[%x]\r\n",tensor_arena);


	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	if(model_addr != 0) {
		static const tflite::Model*yolo11n_ob_model = tflite::GetModel((const void *)model_addr);

		if (yolo11n_ob_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] yolo11n_ob_model's schema version %d is not equal "
				"to supported version %d\n",
				yolo11n_ob_model->version(), TFLITE_SCHEMA_VERSION);
			return -1;
		}
		else {
			xprintf("yolo11n_ob model's schema version %d\n", yolo11n_ob_model->version());
		}

		static tflite::MicroMutableOpResolver<3> yolo11n_ob_op_resolver;

		yolo11n_ob_op_resolver.AddTranspose();
		yolo11n_ob_op_resolver.AddBatchMatMul();
		if (kTfLiteOk != yolo11n_ob_op_resolver.AddEthosU()){
			xprintf("Failed to add Arm NPU support to op resolver.");
			return false;
		}

		static tflite::MicroInterpreter yolo11n_ob_static_interpreter(yolo11n_ob_model, yolo11n_ob_op_resolver,
				(uint8_t*)tensor_arena, tensor_arena_size);  



		if(yolo11n_ob_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		print_model_info(&yolo11n_ob_static_interpreter);
		yolo11n_ob_int_ptr = &yolo11n_ob_static_interpreter;
		yolo11n_ob_input = yolo11n_ob_static_interpreter.input(0);
		#if YOLO11_NO_POST_SEPARATE_OUTPUT
		int numOutputs = yolo11n_ob_static_interpreter.outputs_size();
		for(int i = 0;i < numOutputs;i++)
        {
		    yolo11n_ob_output[i] = yolo11n_ob_static_interpreter.output(i);
        }
		#else
		yolo11n_ob_output = yolo11n_ob_static_interpreter.output(0);
		#endif
	}

	xprintf("initial done\n");
	return ercode;
}



typedef struct detection_cls_yolo11{
    box bbox;
    float confidence;
    float index;

} detection_cls_yolo11;

static bool yolo11_det_comparator(detection_cls_yolo11 &pa, detection_cls_yolo11 &pb)
{
    return pa.confidence > pb.confidence;
}

static void  yolo11_NMSBoxes(std::vector<box> &boxes,std::vector<float> &confidences,float modelScoreThreshold,float modelNMSThreshold,std::vector<int>& nms_result)
{
    detection_cls_yolo11 yolo11_bbox;
    std::vector<detection_cls_yolo11> yolo11_bboxes{};
    for(int i = 0; i < boxes.size(); i++)
    {
        yolo11_bbox.bbox = boxes[i];
        yolo11_bbox.confidence = confidences[i];
        yolo11_bbox.index = i;
        yolo11_bboxes.push_back(yolo11_bbox);
    }
    sort(yolo11_bboxes.begin(), yolo11_bboxes.end(), yolo11_det_comparator);
    int updated_size = yolo11_bboxes.size();
    for(int k = 0; k < updated_size; k++)
    {
        if(yolo11_bboxes[k].confidence < modelScoreThreshold)
        {
            continue;
        }
        
        nms_result.push_back(yolo11_bboxes[k].index);
        for(int j = k + 1; j < updated_size; j++)
        {
            float iou = box_iou(yolo11_bboxes[k].bbox, yolo11_bboxes[j].bbox);
            // float iou = box_diou(yolo11_bboxes[k].bbox, yolo11_bboxes[j].bbox);
            if(iou > modelNMSThreshold)
            {
                yolo11_bboxes.erase(yolo11_bboxes.begin() + j);
                updated_size = yolo11_bboxes.size();
                j = j -1;
            }
        }

    }
}

#if YOLO11_NO_POST_SEPARATE_OUTPUT
static void yolo11_ob_post_processing(tflite::MicroInterpreter* static_interpreter,float modelScoreThreshold, float modelNMSThreshold, struct_yolov8_ob_algoResult *alg,	std::forward_list<el_box_t> &el_algo)
{
	uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
	int numOutputs = static_interpreter->outputs_size();

	TfLiteTensor* output[numOutputs];
	// for(int i = 0; i < numOutputs;i++)
	// {
	// 	output[i] = static_interpreter->output(i);
	// }
	/***
	 * Change the model output order
	 * |   origin     ->> change     |
	 * 0: 1*28*28*144
	 * 1: 1*7*7*144   ->> 1*14*14*144
	 * 2: 1*14*14*144 ->> 1*7*7*144
	 * ***/
	output[0] = static_interpreter->output(0);//1*28*28*144
	output[1] = static_interpreter->output(2);//1*14*14*144
	output[2] = static_interpreter->output(1);//1*7*7*144
	// init postprocessing 	
	int out_dim_total = 0;
	int out_dim_size[numOutputs];
	for(int out_num = 0; out_num < numOutputs; out_num++)
	{
		out_dim_total += (output[out_num]->dims->data[1] * output[out_num]->dims->data[2] );
		out_dim_size[out_num] = out_dim_total;
	}


	// end init
	///////////////////////
	// start postprocessing
	int nboxes=0;
	int input_w = YOLO11_OB_INPUT_TENSOR_WIDTH;
	int input_h = YOLO11_OB_INPUT_TENSOR_HEIGHT;

	std::vector<uint16_t> class_idxs;
	std::vector<float> confidences;
	std::vector<box> boxes;

	#if YOLO11_POST_EACH_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
	#endif
	/***
	 * dequantize the output result for box
	 * 
	 * 
	 ******/
	 for(int j = 0;j < out_dim_total; j++)
	{
		int class_id_start = 64;
		int output_data_idx;
		int dims_cnt_1 ;
		int dims_cnt_2 ;
		int idx;
		if(j < out_dim_size[0] )//576
		{
			output_data_idx = 0;
			dims_cnt_1 = j / output[output_data_idx]->dims->data[1];//28
			dims_cnt_2 = j % output[output_data_idx]->dims->data[2];//28
		}
		else if(j < out_dim_size[1] )//720
		{
			output_data_idx = 1;
			idx = j - out_dim_size[0] ;//576
			dims_cnt_1 = idx / output[output_data_idx]->dims->data[1];//14
			dims_cnt_2 = idx % output[output_data_idx]->dims->data[2];//14
		}
		else
		{
			output_data_idx = 2;
			idx = j - out_dim_size[1] ;//720
			dims_cnt_1 = idx / output[output_data_idx]->dims->data[1];//7
			dims_cnt_2 = idx % output[output_data_idx]->dims->data[2];//7
		}
		float maxScore_no_sigmoid = yolo11_nopost_dequant_value(dims_cnt_1, dims_cnt_2,class_id_start,output[output_data_idx]);
		uint16_t maxClassIndex = 0;
		for(int class_id_cnt = class_id_start;class_id_cnt < output[output_data_idx]->dims->data[3];class_id_cnt++)
		{
			float tmp_score_no_sigmoid = yolo11_nopost_dequant_value(dims_cnt_1, dims_cnt_2,class_id_cnt,output[output_data_idx]);
			/***
			 * find maximum Score and correspond Class id
			 * **/
			if(maxScore_no_sigmoid<tmp_score_no_sigmoid)
			{
				maxScore_no_sigmoid=tmp_score_no_sigmoid;
				maxClassIndex = class_id_cnt - class_id_start;
			}
		}
		float maxScore = sigmoid(maxScore_no_sigmoid);
		if (maxScore >= modelScoreThreshold)
		{
			box bbox;
			yolo11_nopost_cal_xywh(j,dims_cnt_1, dims_cnt_2,output[output_data_idx],&bbox, anchor_756_2,stride_756_1 );
			boxes.push_back(bbox);
			class_idxs.push_back(maxClassIndex);
			confidences.push_back(maxScore);
			
		}
	}

	#if YOLO11_POST_EACH_STEP_TICK						
		SystemGetTick(&systick_2, &loop_cnt_2);
		dbg_printf(DBG_LESS_INFO,"Tick for dequantize the output result for box for yolo11 OB:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));							
	#endif
	#if DBG_APP_LOG
		xprintf("boxes.size(): %d\r\n",boxes.size());
	#endif
	/**
	 * do nms
	 * 
	 * **/

	std::vector<int> nms_result;
	yolo11_NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);
	#if DBG_APP_LOG
		xprintf("nms_result.size(): %d\r\n",nms_result.size());
	#endif
	for (int i = 0; i < nms_result.size(); i++)
	{
		if(!(MAX_TRACKED_YOLOV8_ALGO_RES-i))break;
		int idx = nms_result[i];

		float scale_factor_w = (float)img_w / (float)YOLO11_OB_INPUT_TENSOR_WIDTH; 
		float scale_factor_h = (float)img_h / (float)YOLO11_OB_INPUT_TENSOR_HEIGHT; 
		alg->obr[i].confidence = confidences[idx];
		alg->obr[i].bbox.x = (uint32_t)(boxes[idx].x * scale_factor_w);
		alg->obr[i].bbox.y = (uint32_t)(boxes[idx].y * scale_factor_h);
		alg->obr[i].bbox.width = (uint32_t)(boxes[idx].w * scale_factor_w);
		alg->obr[i].bbox.height = (uint32_t)(boxes[idx].h * scale_factor_h);
		alg->obr[i].class_idx = class_idxs[idx];
		el_box_t temp_el_box;
		temp_el_box.score =  confidences[idx]*100;
		temp_el_box.target =  class_idxs[idx];
		temp_el_box.x = (uint32_t)(boxes[idx].x * scale_factor_w);
		temp_el_box.y =  (uint32_t)(boxes[idx].y * scale_factor_h);
		temp_el_box.w = (uint32_t)(boxes[idx].w * scale_factor_w);
		temp_el_box.h = (uint32_t)(boxes[idx].h * scale_factor_h);


		// printf("temp_el_box.x %d,temp_el_box.y: %d\r\n",temp_el_box.x,temp_el_box.y);
		el_algo.emplace_front(temp_el_box);
		// for (auto box : el_algo) {
		// 	printf("el_algo.box.x %d,el_algo.box.y%d\r\n",box.x,box.y);
		// }
		#if YOLO11N_OB_DBG_APP_LOG
			printf("detect object[%d]: %s confidences: %f\r\n",i, coco_classes[class_idxs[idx]].c_str(),confidences[idx]);

		#endif
	}
}
#else
static void yolo11_ob_post_processing(tflite::MicroInterpreter* static_interpreter,float modelScoreThreshold, float modelNMSThreshold, struct_yolov8_ob_algoResult *alg,	std::forward_list<el_box_t> &el_algo)
{
	uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
	TfLiteTensor* output = static_interpreter->output(0);
	// init postprocessing 	
	int num_classes = output->dims->data[1] - 4;

	
	// end init
	///////////////////////
	// start postprocessing
	int nboxes=0;
	int input_w = YOLO11_OB_INPUT_TENSOR_WIDTH;
	int input_h = YOLO11_OB_INPUT_TENSOR_HEIGHT;

	std::vector<uint16_t> class_idxs;
	std::vector<float> confidences;
	std::vector<box> boxes;


	float output_scale = ((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0];
	int output_zeropoint = ((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0];
	int output_size = output->bytes;

	#if YOLO11N_OB_DBG_APP_LOG
		// xprintf("output->dims->size: %d\r\n",output->dims->size);
		// printf("output_scale: %f\r\n",output_scale);
		// xprintf("output_zeropoint: %d\r\n",output_zeropoint);
		// xprintf("output_size: %d\r\n",output_size);
		// xprintf("output->dims->data[0]: %d\r\n",output->dims->data[0]);//1
		// xprintf("output->dims->data[1]: %d\r\n",output->dims->data[1]);//84
		// xprintf("output->dims->data[2]: %d\r\n",output->dims->data[2]);//756
	#endif
	/***
	 * dequantize the output result
	 * 
	 * 
	 ******/
	for(int dims_cnt_2 = 0; dims_cnt_2 < output->dims->data[2]; dims_cnt_2++)
	{
		float outputs_bbox_data[4];
		float maxScore = (-1);// the first four indexes are bbox information
		uint16_t maxClassIndex = 0;
		for(int dims_cnt_1 = 0; dims_cnt_1 < output->dims->data[1]; dims_cnt_1++)
		{
			int value =  output->data.int8[ dims_cnt_2 + dims_cnt_1 * output->dims->data[2]];
			
			float deq_value = ((float) value-(float)output_zeropoint) * output_scale ;
			if(dims_cnt_1<4)
			{
				/***
				 * fix big score
				 * ****/
				if(dims_cnt_1%2)//==1
				{
					deq_value *= (float)input_h;
				}
				else
				{
					deq_value *= (float)input_w;
				}
				outputs_bbox_data[dims_cnt_1] = deq_value;
			}
			else
			{
				/***
				 * find maximum Score and correspond Class idx
				 * **/
				if(maxScore < deq_value)
				{
					maxScore = deq_value;
					maxClassIndex = dims_cnt_1-4;
				}
			}

		}
		if (maxScore >= modelScoreThreshold)
		{
			box bbox;
			
			bbox.x = (outputs_bbox_data[0] - (0.5 * outputs_bbox_data[2]));
			bbox.y = (outputs_bbox_data[1] - (0.5 * outputs_bbox_data[3]));
			bbox.w =(outputs_bbox_data[2]);
			bbox.h = (outputs_bbox_data[3]);
			boxes.push_back(bbox);
			class_idxs.push_back(maxClassIndex);
			confidences.push_back(maxScore);
			
		}
	}
	#if YOLO11N_OB_DBG_APP_LOG
		xprintf("boxes.size(): %d\r\n",boxes.size());
	#endif
	/**
	 * do nms
	 * 
	 * **/

	std::vector<int> nms_result;
	yolo11_NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);
	for (int i = 0; i < nms_result.size(); i++)
	{
		if(!(MAX_TRACKED_YOLOV8_ALGO_RES-i))break;
		int idx = nms_result[i];

		float scale_factor_w = (float)img_w / (float)YOLO11_OB_INPUT_TENSOR_WIDTH; 
		float scale_factor_h = (float)img_h / (float)YOLO11_OB_INPUT_TENSOR_HEIGHT; 
		alg->obr[i].confidence = confidences[idx];
		alg->obr[i].bbox.x = (uint32_t)(boxes[idx].x * scale_factor_w);
		alg->obr[i].bbox.y = (uint32_t)(boxes[idx].y * scale_factor_h);
		alg->obr[i].bbox.width = (uint32_t)(boxes[idx].w * scale_factor_w);
		alg->obr[i].bbox.height = (uint32_t)(boxes[idx].h * scale_factor_h);
		alg->obr[i].class_idx = class_idxs[idx];
		el_box_t temp_el_box;
		temp_el_box.score =  confidences[idx]*100;
		temp_el_box.target =  class_idxs[idx];
		temp_el_box.x = (uint32_t)(boxes[idx].x * scale_factor_w);
		temp_el_box.y =  (uint32_t)(boxes[idx].y * scale_factor_h);
		temp_el_box.w = (uint32_t)(boxes[idx].w * scale_factor_w);
		temp_el_box.h = (uint32_t)(boxes[idx].h * scale_factor_h);


		// printf("temp_el_box.x %d,temp_el_box.y: %d\r\n",temp_el_box.x,temp_el_box.y);
		el_algo.emplace_front(temp_el_box);
		// for (auto box : el_algo) {
		// 	printf("el_algo.box.x %d,el_algo.box.y%d\r\n",box.x,box.y);
		// }
		#if YOLO11N_OB_DBG_APP_LOG
			printf("detect object[%d]: %s confidences: %f\r\n",i, coco_classes[class_idxs[idx]].c_str(),confidences[idx]);

		#endif
	}

}
#endif

int cv_yolo11n_ob_run(struct_yolov8_ob_algoResult *algoresult_yolo11n_ob) {
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t ch = app_get_raw_channels();
    uint32_t raw_addr = app_get_raw_addr();
    uint32_t expand = 0;
	std::forward_list<el_box_t> el_algo;

	#if YOLO11N_OB_DBG_APP_LOG
    xprintf("raw info: w[%d] h[%d] ch[%d] addr[%x]\n",img_w, img_h, ch, raw_addr);
	#endif

    if(yolo11n_ob_int_ptr!= nullptr) {
		#ifdef TOTAL_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
    	//get image from sensor and resize
		w_scale = (float)(img_w - 1) / (YOLO11_OB_INPUT_TENSOR_WIDTH - 1);
		h_scale = (float)(img_h - 1) / (YOLO11_OB_INPUT_TENSOR_HEIGHT - 1);

		
		hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr, (uint8_t*)yolo11n_ob_input->data.data,  
		                    img_w, img_h, ch, 
                        	YOLO11_OB_INPUT_TENSOR_WIDTH, YOLO11_OB_INPUT_TENSOR_HEIGHT, w_scale,h_scale);
		#ifdef EACH_STEP_TICK						
			SystemGetTick(&systick_2, &loop_cnt_2);
			dbg_printf(DBG_LESS_INFO,"Tick for resize image BGR8U3C_to_RGB24_helium for yolo11 OB:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));							
		#endif

		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif

		// //uint8 to int8
		for (int i = 0; i < yolo11n_ob_input->bytes; ++i) {
			*((int8_t *)yolo11n_ob_input->data.data+i) = *((int8_t *)yolo11n_ob_input->data.data+i) - 128;
    	}

		#ifdef EACH_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		dbg_printf(DBG_LESS_INFO,"Tick for Invoke for uint8toint8 for YOLO11_OB:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif	

		#ifdef EACH_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		TfLiteStatus invoke_status = yolo11n_ob_int_ptr->Invoke();

		#ifdef EACH_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		#endif
		if(invoke_status != kTfLiteOk)
		{
			xprintf("yolo11 object detect invoke fail\n");
			return -1;
		}
		else
		{
			#if YOLO11N_OB_DBG_APP_LOG
			xprintf("yolo11 object detect  invoke pass\n");
			#endif
		}
		#ifdef EACH_STEP_TICK
    		dbg_printf(DBG_LESS_INFO,"Tick for Invoke for YOLO11_OB:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif

		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_1, &loop_cnt_1);
		#endif
		//retrieve output data
		yolo11_ob_post_processing(yolo11n_ob_int_ptr,0.25, 0.45, algoresult_yolo11n_ob,el_algo);
		#ifdef EACH_STEP_TICK
			SystemGetTick(&systick_2, &loop_cnt_2);
			dbg_printf(DBG_LESS_INFO,"Tick for Invoke for YOLO11_OB_post_processing:[%d]\r\n\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));    
		#endif
		#if YOLO11N_OB_DBG_APP_LOG
			xprintf("yolo11_ob_post_processing done\r\n");
		#endif
		#ifdef TOTAL_STEP_TICK						
			SystemGetTick(&systick_2, &loop_cnt_2);
			// dbg_printf(DBG_LESS_INFO,"Tick for TOTAL YOLO11 OB:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));		
		#endif

    }
	

#ifdef UART_SEND_ALOGO_RESEULT
	algoresult_yolo11n_ob->algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2) + capture_image_tick;
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
	// event_reply(concat_strings(", ", box_results_2_json_str(el_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
	event_reply(concat_strings(", ", algo_tick_2_json_str(algoresult_yolo11n_ob->algo_tick),", ", box_results_2_json_str(el_algo), ", ", img_2_json_str(&temp_el_jpg_img)));
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

int cv_yolo11n_ob_deinit()
{
	#if YOLO11_NO_POST_SEPARATE_OUTPUT
	free(stride_756_1);
	for(int i = 0; i < dim_total_size; i++)
	{
		free(anchor_756_2[i]);
	}
	free(anchor_756_2);
	#endif
	return 0;
}

