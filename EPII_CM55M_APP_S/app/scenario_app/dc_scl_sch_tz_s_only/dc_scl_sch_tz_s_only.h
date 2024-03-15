/*
 * hello_world.h
 *
 *  Created on: Dec 3, 2020
 *      Author: 902447
 */

#ifndef APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_DC_SCL_SCH_TZ_S_ONLY_H_
#define APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_DC_SCL_SCH_TZ_S_ONLY_H_

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef enum
{
	APP_STATE_ENTER_PMU,
	APP_STATE_ALGO_JPEG_STORE,
}APP_STATE_E;

int app_main(void);

#endif /* APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_DC_SCL_SCH_TZ_S_ONLY_H_ */
