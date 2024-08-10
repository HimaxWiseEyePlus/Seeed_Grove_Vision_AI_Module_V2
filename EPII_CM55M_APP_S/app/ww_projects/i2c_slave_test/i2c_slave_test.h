/*
 * i2c_slave_test.h
 *
 *  Created on: 11/07/2024
 *      Author: charl
 */

#ifndef APP_SCENARIO_APP_I2C_SLAVE_TEST_I2C_SLAVE_TEST_H_
#define APP_SCENARIO_APP_I2C_SLAVE_TEST_I2C_SLAVE_TEST_H_


#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

int app_main(void);


#endif /* APP_SCENARIO_APP_I2C_SLAVE_TEST_I2C_SLAVE_TEST_H_ */
