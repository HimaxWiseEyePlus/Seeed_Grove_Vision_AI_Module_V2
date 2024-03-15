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
#include "WE2_device.h"
#include "board.h"
#include "cvapp.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"
#include "WE2_device.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "xprintf.h"


#include "person_detect_model_data.h"
#include "memory_manage.h"

#define LOCAL_FRAQ_BITS (8)
#define SC(A, B) ((A<<8)/B)

#define INPUT_SIZE_X 96
#define INPUT_SIZE_Y 96

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

constexpr int tensor_arena_size = 125*1024;
//__attribute__((section(".bss.tensor_arena"))) static uint8_t tensor_arena[tensor_arena_size]={0};
uint32_t tensor_arena;
struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *int_ptr=nullptr;
TfLiteTensor* input, *output;
};

void img_rescale(
        const uint8_t*in_image,
        const int32_t width,
        const int32_t height,
        const int32_t nwidth,
        const int32_t nheight,
        int8_t*out_image,
        const int32_t nxfactor,
        const int32_t nyfactor) {
    int32_t x,y;
    int32_t ceil_x, ceil_y, floor_x, floor_y;

    int32_t fraction_x,fraction_y,one_min_x,one_min_y;
    int32_t pix[4];//4 pixels for the bilinear interpolation
    int32_t out_image_fix;

    for (y = 0; y < nheight; y++) {//compute new pixels
        for (x = 0; x < nwidth; x++) {
            floor_x = (x*nxfactor) >> LOCAL_FRAQ_BITS;//left pixels of the window
            floor_y = (y*nyfactor) >> LOCAL_FRAQ_BITS;//upper pixels of the window

            ceil_x = floor_x+1;//right pixels of the window
            if (ceil_x >= width) ceil_x=floor_x;//stay in image

            ceil_y = floor_y+1;//bottom pixels of the window
            if (ceil_y >= height) ceil_y=floor_y;

            fraction_x = x*nxfactor-(floor_x << LOCAL_FRAQ_BITS);//strength coefficients
            fraction_y = y*nyfactor-(floor_y << LOCAL_FRAQ_BITS);

            one_min_x = (1 << LOCAL_FRAQ_BITS)-fraction_x;
            one_min_y = (1 << LOCAL_FRAQ_BITS)-fraction_y;

            pix[0] = in_image[floor_y * width + floor_x];//store window
            pix[1] = in_image[floor_y * width + ceil_x];
            pix[2] = in_image[ceil_y * width + floor_x];
            pix[3] = in_image[ceil_y * width + ceil_x];

            //interpolate new pixel and truncate it's integer part
            out_image_fix = one_min_y*(one_min_x*pix[0]+fraction_x*pix[1])+fraction_y*(one_min_x*pix[2]+fraction_x*pix[3]);
            out_image_fix = out_image_fix >> (LOCAL_FRAQ_BITS * 2);
            out_image[nwidth*y+x] = out_image_fix-128;
        }
    }
}

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
    /* Get Ethos-U version */
	/*struct ethosu_driver_version driver_version;
	struct ethosu_hw_info hw_info;

	ethosu_get_driver_version(&driver_version);
	ethosu_get_hw_info(&ethosu_drv, &hw_info);

	xprintf("Ethos-U version info:\n");
	xprintf("\tArch:       v%u.%u.%u\n",
									hw_info.version.arch_major_rev,
									hw_info.version.arch_minor_rev,
									hw_info.version.arch_patch_rev);
	xprintf("\tDriver:     v%u.%u.%u\n",
									driver_version.major,
									driver_version.minor,
									driver_version.patch);
	xprintf("\tMACs/cc:    %u\n", (uint32_t)(1 << hw_info.cfg.macs_per_cc));
	xprintf("\tCmd stream: %u\n", hw_info.cfg.cmd_stream_version);
	xprintf("\tSHRAM size: %u\n", hw_info.cfg.shram_size);*/

    return 0;
}

int cvapp_init(bool security_enable, bool privilege_enable) {
	int ercode = 0;

	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	tensor_arena = mm_reserve(tensor_arena_size);

	static const tflite::Model*model = tflite::GetModel(g_person_detect_model_data);

	if (model->version() != TFLITE_SCHEMA_VERSION) {
		xprintf(
			"[ERROR] model's schema version %d is not equal "
			"to supported version %d\n",
			model->version(), TFLITE_SCHEMA_VERSION);
		return -1;
	}
	else {
		xprintf("model's schema version %d\n", model->version());
	}

	static tflite::MicroErrorReporter micro_error_reporter;

	static tflite::MicroMutableOpResolver<1> op_resolver;

	if (kTfLiteOk != op_resolver.AddEthosU()){
		xprintf("Failed to add Arm NPU support to op resolver.");
		return false;
	}

	static tflite::MicroInterpreter static_interpreter(model, op_resolver, (uint8_t*)tensor_arena, tensor_arena_size, &micro_error_reporter);

	if(static_interpreter.AllocateTensors()!= kTfLiteOk) {
		return false;
	}
	int_ptr = &static_interpreter;
	input = static_interpreter.input(0);
	output = static_interpreter.output(0);

	xprintf("initial done\n");

	return ercode;
}

int cvapp_run(int *score) {
	int ercode = 0;

	//give image to input tensor
	//due to sensor initial to color :BGR resolution :640x480 with a mono person detection model
	//we will use B color image as tensor input for model
	img_rescale((uint8_t*)app_get_raw_addr(), app_get_raw_width(), app_get_raw_height(), INPUT_SIZE_X, INPUT_SIZE_Y,
			input->data.int8, SC(app_get_raw_width(), INPUT_SIZE_X), SC(app_get_raw_height(), INPUT_SIZE_Y));

	TfLiteStatus invoke_status = int_ptr->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		xprintf("invoke fail\n");
		return -1;
	}
	else
		xprintf("invoke pass\n");

	//retrieve output data
	int8_t person_score = output->data.int8[1];
	int8_t no_person_score = output->data.int8[0];

	*score = person_score;
	xprintf("person_score:%d\n",person_score);
	//error_reporter->Report(
	//	   "person score: %d, no person score: %d\n", person_score,
	//	   no_person_score);

	return ercode;
}

