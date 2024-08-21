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

// Number of parameter bytes that precede payload
#define I2C_SLAVE_IF_CTRL_BYTES	3
// The payload
#define I2C_SLAVE_IF_PAYLOAD_SIZE	48
// The total
#define I2C_SLAVE_IF_BUF_SIZE	(I2C_SLAVE_IF_CTRL_BYTES + I2C_SLAVE_IF_PAYLOAD_SIZE)
/**
 * List of command types send by the WW130 to the AI board.
 * Both boards must use the same list, in the same sequence!
 *
 * Also, must align with aiProcessor_msgTypeStr[] entries
 */
typedef enum {
    AI_PROCESSOR_MSG_NONE,
    AI_PROCESSOR_MSG_TX_STRING,
    AI_PROCESSOR_MSG_TX_BASE64,
    AI_PROCESSOR_MSG_TX_BINARY,
    AI_PROCESSOR_MSG_RX_STRING,
    AI_PROCESSOR_MSG_RX_BASE64,
    AI_PROCESSOR_MSG_RX_BINARY,
	// Internally generated callbacks for the client
    AI_PROCESSOR_MSG_TX_BINARY_COMPLETE,
    AI_PROCESSOR_MSG_RX_BINARY_COMPLETE,
    AI_PROCESSOR_MSG_PA0,

	// This is used just to check on the end of the list
    AI_PROCESSOR_MSG_END
} aiProcessor_msg_type_t;


/********************************* Structure define ************************************************/

// Define the structure
struct ExpectedMsgStruct {
    char incomingString[I2C_SLAVE_IF_PAYLOAD_SIZE];  // You can adjust the size according to your needs
    void (*processingFunction)(char * parameter);  // Pointer to a function with no arguments and void return type
};

/****************************** Exported Function Declarations ************************/

// Initialise I2C slave and inter-processor interrupt pin
void i2c_slave_if_init(void * clientCb, void * commandsCb);

// De-initialise I2C slave
void i2c_slave_if_deinit(void);

// Transmit string to I2C master
void i2cs_slave_if_send_string(char * message);

// Toggle inter-processor interrupt pin to wake WW130
void i2cs_slave_wake_master(uint32_t pulseWidth);

void i2cs_slave_if_binaryBufferInit(uint8_t * binaryBuffer, uint16_t bufSize);

bool i2cs_slave_if_processCommandString(char *message, struct ExpectedMsgStruct possibleMessages[], uint16_t numMessages);

// A utility to test for ISR context
bool i2cs_slave_if_inISR(void);

// A utility for printing the contents of a buffer
void i2cs_slave_if_printBuffer(uint8_t * buff, uint16_t length);

#endif // I2C_SLAVE_IF_
