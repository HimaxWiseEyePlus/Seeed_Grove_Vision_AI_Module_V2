/*
 * i2c_slave_commands.h
 *
 *  Created on: 21/07/2024
 *      Author: CGP
 *
 * Intermediate processing between client and i2c_slave_if.c
 */

#ifndef APP_SCENARIO_APP_I2C_SLAVE_TEST_I2C_SLAVE_COMMANDS_H_
#define APP_SCENARIO_APP_I2C_SLAVE_TEST_I2C_SLAVE_COMMANDS_H_

// Called by client to register callback and initialise i2c_slave_if.c
void i2c_slave_commands_init(void * clientCallback, uint8_t * clientBuffer, uint16_t clientBufferSize);


#endif /* APP_SCENARIO_APP_I2C_SLAVE_TEST_I2C_SLAVE_COMMANDS_H_ */
