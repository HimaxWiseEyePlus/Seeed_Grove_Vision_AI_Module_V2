/*
 * i2c_slave_IF.h
 *
 *  Created on: 11 July 2024
 *      Author: CGP
 *
 * Provides the interface betwenn WW130 and Seeed Vision AI V2
 * This uses the I2C interface (the Seeed board is the slave)
 * and the PA0 pin configured as a bidirectional interrupt
 */

#ifndef I2C_SLAVE_IF_
#define I2C_SLAVE_IF_

/********************************* Global Defines ******************************/


#define I2C_SLAVE_IF_BUFF_SIZE	48
/**
 * List of command types send by the WW130 to the AI board.
 * Bothe boards must use the same list, in the same sequence!
 */
typedef enum {
    AI_PROCESSOR_MSG_TX_STRING,
    AI_PROCESSOR_MSG_TX_BINARY,
    AI_PROCESSOR_MSG_RX_STRING,
    AI_PROCESSOR_MSG_RX_BINARY,

	// This is used just to check on the end of the list
    AI_PROCESSOR_MSG_END
} aiProcessor_msg_type_t;


/****************************** Exported Function Declarations ************************/

// Initialise I2C slave and inter-processor interrupt pin
void i2c_slave_if_init( void * callback);

// De-initialise I2C slave
void i2c_slave_if_deinit(void);

// Transmit string to I2C master
void i2cs_slave_if_send_string(char * message);

// Toggle inter-processor interrupt pin to wake WW130
void i2cs_slave_wake_master(void);


#endif // I2C_SLAVE_IF_
