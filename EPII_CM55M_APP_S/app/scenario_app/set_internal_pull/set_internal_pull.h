/*
 * set_internal_pull.h
 *
 *  Created on: Aug 17, 2023
 *      Author: 902449
 */

#ifndef APP_SCENARIO_SET_INTERNAL_PULL_
#define APP_SCENARIO_SET_INTERNAL_PULL_

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

int app_main(void);

#endif /* APP_SCENARIO_SET_INTERNAL_PULL_ */
