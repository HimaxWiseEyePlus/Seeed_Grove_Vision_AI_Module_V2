/*
 * pca9574.c
 *
 *  Created on: 2 Jul 2023
 *      Author: charl
 *
 *  Supports PCA9584 I/O expander chip.
 *  This chip is used on WAT200 for irrigation.
 *  But this file contains only generic PCA9574 code plus STM32WLxx I2C initialisation.
 *
 *  It uses interrupt-driven I2C but just waits polling for teh interrupt
 *  This because of teh example I worked from:
 *  	    D:\Development\STM32Cube_FW_WL_V1.2.0\Projects\NUCLEO-WL55JC\Examples\I2C\I2C_TwoBoards_ComIT
 *
 * TODO simpler and safer to use a blocking version...
 */


/*********************************************** Includes ****************************************************/

#include "stdio.h"
#include "stdbool.h"

#include "hx_drv_CIS_common.h"
#include "pca9574.h"
#include "hm0360_md.h"
#include "xprintf.h"

/*********************************************** Local Defines **********************************************/


/*********************************************** Local Variables ********************************************/

// Interrupt counters
uint16_t rxInterruptCount;
uint16_t txInterruptCount;

// Somewhere to sore the I2C address of the main image sensor
static uint8_t mainCameraID;

/*********************************************** Local Function Declarations *********************************/

static void saveMainCameraConfig(uint8_t deviceAddress);
static void restoreMainCameraConfig(void);

/*********************************************** Local Function Definitions *********************************/

/**
 * Read and save settings of the main camera before accessing the PCA9574.
 *
 * Configure the image sensor I2C address to be the HM0360
 * 	// TODO do we need some critical section or semaphore code here, in case we change task mid-stream?
 *
 *  @param - I2C address of the PCA9574 to be selected.
 */
static void saveMainCameraConfig(uint8_t deviceAddress) {

	hx_drv_cis_get_slaveID(&mainCameraID);
    hx_drv_cis_set_slaveID(deviceAddress);
}

/**
 * Restore settings of the main camera after accessing the HM0360
 */
static void restoreMainCameraConfig(void) {
	hx_drv_cis_set_slaveID(mainCameraID);
}


/*********************************************** Global Function Definitions *********************************/

/**
 *
 */
HX_CIS_ERROR_E pca9574_init(uint8_t deviceAddr) {
	HX_CIS_ERROR_E ret;

	// Test for the I2C extender
	if (hm0360_md_isSensorPresent(deviceAddr)) {
		xprintf("PCA9574 present at 0x%02x\n", deviceAddr);
	}
	else {
		xprintf("PCA9574 not present at 0x%02x\n", deviceAddr);
		// expect a driver error message as well...
		restoreMainCameraConfig();
		return HX_CIS_ERROR_I2C;
	}

	// Initialise registers of PCA9574 I/O expander and set outputs inactive

	// PCA9574_I2C_ADDRESS_0 contains all 8 ENABLE output pins
	ret = pca9574_write(deviceAddr, PCA9574_REG_PUPD, 0);	// pull-down on all pins
	ret |= pca9574_write(deviceAddr, PCA9574_REG_BKEN, 2);	// Disable bus hold, enable pu/pd
	ret |= pca9574_write(deviceAddr, PCA9574_REG_OUT, 0);	// output: all bits set to 0 (ENABLEn = 0)
	ret |= pca9574_write(deviceAddr, PCA9574_REG_CFG, 0);	// configuration: all bits set to output
	ret |= pca9574_write(deviceAddr, PCA9574_REG_MSK, 0xff);	// Interrupt mask (interrupt not used, disable all)

//	// NOTE: The following is applicable only for the WAT200
//
//	// PCA9574_I2C_ADDRESS_1 contains control and status pins
//	pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_INVRT, WAT200_INVRT);	// Invert one input pin
//	//pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_BKEN, 1);				// Enable bus hold, disable pu/pd
//	pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_BKEN, 0);				// Disable bus hold, disable pu/pd
//	pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_PUPD, 0);				// p.u/p.d ignored
//	pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_OUT, controlBits);		// output: all output bits set to 0
//	pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_CFG, WAT200_INPUTS);	// configuration: Some pins are inputs
//	pca9574_write(PCA9574_I2C_ADDRESS_1, PCA9574_REG_MSK, 0xff);	// Interrupt mask (interrupt not used, disable all)
//

	return ret;
}

/**
 * Writes to a PCA9574 register
 *
 * @param deviceAddress - one of two I2C addresses
 * @param reg = register
 * @param val = value to write
 * @return error code
 */
HX_CIS_ERROR_E pca9574_write(uint8_t deviceAddr, uint8_t reg, uint8_t val) {
	HX_CIS_ERROR_E ret;

	saveMainCameraConfig(deviceAddr);

	ret = hx_drv_cis_set_reg_1byte(reg, val, 0);
	if (ret != HX_CIS_NO_ERROR) {
		xprintf("DEBUG: error %d writing %d to 0x%02x\n", val, reg);
	}

    restoreMainCameraConfig();

	return ret;
}


/**
 * Read from a PCA9574 register
 * Register was previously defned by pca9574_setRegister()
 *
 * @param deviceAddress - one of two I2C addresses
 * @param rxByte - place to write the read byte
 * @return error code
 */
HX_CIS_ERROR_E pca9574_read(uint8_t deviceAddr, uint8_t reg, uint8_t *rxByte) {
	HX_CIS_ERROR_E ret;

	saveMainCameraConfig(deviceAddr);

	ret = hx_drv_cis_get_reg_1byte(reg, rxByte);
	if (ret != HX_CIS_NO_ERROR) {
		xprintf("DEBUG: error %d reading from 0x%02x\n", reg);
	}

    restoreMainCameraConfig();
	return ret;
}


// Write values to read/write registers and check you can get them back.

void pca9574_readWriteTests(uint8_t deviceAddr) {
	uint8_t returned;
	uint8_t sent = 0xaa;

	xprintf( "For the PCA9574 at 0x%02x:\n\n", deviceAddr);

	// PCA9574_REG_INVRT is read/write so I should be able to write this and read it back
	pca9574_write(deviceAddr, PCA9574_REG_INVRT, sent);
	pca9574_read(deviceAddr, PCA9574_REG_INVRT, &returned);

	xprintf( "Wrote 0x%02x Read 0x%02x Should be 0x%02x\n", sent, returned, (sent ^ 0xff));

	for (uint8_t i=0; i<8; i++) {
		pca9574_write(deviceAddr, PCA9574_REG_INVRT, (1 << i));
		pca9574_read(deviceAddr, PCA9574_REG_INVRT, &returned);
		xprintf( "Read 0x%02x\n", returned);
	}
}

