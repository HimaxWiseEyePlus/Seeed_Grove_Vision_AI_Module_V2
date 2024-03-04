/*
 * tflm_fd_fm.h
 *
 *  Created on: Feb 15, 2023
 *      Author: bigcat-himax
 */

#ifndef SCENARIO_TFLM_FD_FM_
#define SCENARIO_TFLM_FD_FM_

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef enum
{
	APP_STATE_ALLON_FD_FL,
	APP_STATE_ALLON_PL,
	APP_STATE_ALLON_FD_FM,
	APP_STATE_ALLON_FD_FL_EL_9_POINT,
}APP_STATE_E;

int app_main(void);
void SetPSPDNoVid();
#endif /* SCENARIO_TFLM_FD_FM_ */
