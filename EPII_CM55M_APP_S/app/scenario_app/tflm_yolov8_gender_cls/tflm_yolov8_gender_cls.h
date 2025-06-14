/*
 * hello_world.h
 *
 *  Created on: Dec 3, 2020
 *      Author: 902447
 */

#ifndef SCENARIO_TFLM_2IN1_FD_FL_PL_
#define SCENARIO_TFLM_2IN1_FD_FL_PL_

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef enum
{
	APP_STATE_ALLON_YOLOV8N_GENDER_CLS,
}CAMERA_CONFIG_E;

int tflm_yolov8_gender_cls_app(void);
void model_change();
void SetPSPDNoVid_24M();
void SetPSPDNoVid();
#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_ */
