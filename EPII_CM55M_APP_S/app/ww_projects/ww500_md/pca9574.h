/*
 * pca9574.h
 *
 *  Created on: 2 Jul 2023
 *      Author: charl
 *
 * Definitions for PCA9584 I/O expander chip
 */


#ifndef PCA9574_H_
#define PCA9574_H_

/*********************************************** Includes ****************************************************/

#include "stdbool.h"
#include "hx_drv_CIS_common.h"

/*********************************************** Global Defines **********************************************/


// Chip I2C addresses (assumes 8-bit address - LS bit is read/write)
//#define PCA9574_I2C_ADDRESS_0	0x40
//#define PCA9574_I2C_ADDRESS_1	0x42
// 7-bit addresses expected by the driver
#define PCA9574_I2C_ADDRESS_0	0x20
#define PCA9574_I2C_ADDRESS_1	0x21

// Register addresses
#define PCA9574_REG_IN		0
#define PCA9574_REG_INVRT	1
#define PCA9574_REG_BKEN	2
#define PCA9574_REG_PUPD	3
#define PCA9574_REG_CFG		4
#define PCA9574_REG_OUT		5
#define PCA9574_REG_MSK		6
#define PCA9574_REG_INTS	7


/*********************************************** Global Function Declarations *********************************/

HX_CIS_ERROR_E pca9574_init(uint8_t deviceAddr);

// Define a register and write a byte to it
HX_CIS_ERROR_E pca9574_write(uint8_t deviceAddr, uint8_t reg, uint8_t val);

// Read data from a register defined previously
HX_CIS_ERROR_E pca9574_read(uint8_t deviceAddr, uint8_t reg, uint8_t *rxByte);

// For debugging
void pca9574_readWriteTests(uint8_t deviceAddr);

#endif /* PCA9574_H_ */
