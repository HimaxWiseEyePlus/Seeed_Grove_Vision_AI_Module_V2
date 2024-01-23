/*
 * tflm_pose_landmark.h
 *
 *  Created on: Han 10, 2022
 *      Author: 904207
 */

#ifndef SCENARIO_APP_TFLM_PL_H_
#define SCENARIO_APP_TFLM_PL_H_


#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef enum
{
	APP_STATE_YOLOV8_POSE,
}APP_STATE_E;

int tflm_yolov8_pose_app(void);
void model_change();
void SetPSPDNoVid_24M();
void SetPSPDNoVid();

#endif /* SCENARIO_APP_TFLM_PL_H_ */