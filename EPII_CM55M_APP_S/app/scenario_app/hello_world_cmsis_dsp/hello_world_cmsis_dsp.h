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


int app_main(void);

#endif /* SCENARIO_APP_TFLM_PL_H_ */