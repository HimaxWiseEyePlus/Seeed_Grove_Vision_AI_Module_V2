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
	APP_STATE_ALLON_EFFICIENTNET_LITE0_CLASS,
}APP_STATE_E;

int app_main(void);
void model_change();
void SetPSPDNoVid_24M();
void SetPSPDNoVid();
#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_ */
