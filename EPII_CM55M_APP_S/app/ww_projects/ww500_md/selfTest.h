/*
 * selfTest.h
 *
 *  Maintains state of the hardware for reporting to the app
 *
 *  Created on: 18 Nov 2025
 *      Author: charl
 */

#ifndef SELFTEST_H_
#define SELFTEST_H_

/***************************************** Includes ***************************************/

/***************************************** Defines ****************************************/

/**
 * Bits in selfTestBit bit map
 *
 * IMPORTANT: The same list must be used by the AI processor and the app
 */
typedef enum {
	// Bits 0-7 are for the BLE processor
    SELF_TEST_LOW_BATTERY, 		// 0 = low battery,
    SELF_TEST_AI_PROC,			// 1 = AI processor not responding
    SELF_TEST_LORAWAN_ERROR,	// 2 = LoRaWAN error
    SELF_TEST_WATCHDOG_RESET,	// 3 = LoRaWAN error
    SELF_TEST_BROWNOUT_RESET,	// 4 = LoRaWAN error
	// Bits 8-15 are AI processor errors
    SELF_TEST_AI_NO_CAM	= 8,	// 8 = main camera error
    SELF_TEST_AI_NO_MD,			// 9 = HM0360 error
    SELF_TEST_AI_NO_FLASH,		// 10 = LED flash cct fail
    SELF_TEST_AI_NO_SD_CARD,	// 11 = SD card missing
    SELF_TEST_AI_PDM_ERROR,		// 12 = PDM microphone fails
    SELF_TEST_AI_NN_ERROR,		// 13 = Neural network error

} selfTest_type_t;

/******************************** Public Function Declarations ************************************************************/

void selfTest_init(void);
uint16_t selfTest_getErrorBits(void);
void selfTest_setErrorBits(uint16_t errBits);
void selfTest_clearErrorBits(uint16_t errBits);
void selfTest_test(void);

#endif /* SELFTEST_H_ */

