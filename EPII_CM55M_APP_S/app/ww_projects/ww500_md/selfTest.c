/*
 * selfTest.c
 *
 *  Created on: 18 Nov 2025
 *      Author: charl
 */


/***************************************** Includes ***************************************/

#include <stdint.h>
#include <string.h>

#include "selfTest.h"
#include "xprintf.h"

/***************************************** Defines *******************************************************/


/******************************** Private Function Declarations ***************************************************/

/******************************** Local Variables ****************************************************/

static uint16_t errorBits = 0;

/******************************** Private Function Definitions ****************************************************/

/******************************** Public Function Definitions ************************************************************/

/**
 * initialises the bit mask
 *
 * Clears all error bits
 */
void selfTest_init(void) {
	errorBits = 0;
}

/**
 * Returns the error bits
 *
 * @return error bits
 */
uint16_t selfTest_getErrorBits(void) {
	return errorBits;
}

/**
 * Sets one or more error bits
 *
 * @param error bits from selfTest_type_t
 */
void selfTest_setErrorBits(uint16_t errBits) {
	errorBits |= errBits;
}

/**
 * Clears one or more error bits
 *
 * @param error bit from selfTest_type_t
 */
void selfTest_clearErrorBits(uint16_t errBits) {
	errorBits &= ~errBits;
}

/**
 * Run a few tests to give confidence
 */
void selfTest_test(void) {

	xprintf("Testing selfTest\n");

	selfTest_setErrorBits(1 << SELF_TEST_LOW_BATTERY);
	xprintf("Error bits = 0x%04x (expect 0x0001)\n", selfTest_getErrorBits());

	selfTest_setErrorBits((1 << SELF_TEST_AI_PDM_ERROR) | (1 << SELF_TEST_AI_NO_SD_CARD)) ;
	xprintf("Error bits = 0x%04x (expect 0x1801)\n", selfTest_getErrorBits());

	selfTest_clearErrorBits(1 << SELF_TEST_LOW_BATTERY);
	xprintf("Error bits = 0x%04x (expect 0x1800)\n", selfTest_getErrorBits());

	selfTest_clearErrorBits(1 << SELF_TEST_AI_PDM_ERROR);
	xprintf("Error bits = 0x%04x (expect 0x0800)\n", selfTest_getErrorBits());

	selfTest_clearErrorBits(1 << SELF_TEST_AI_NO_SD_CARD);
	xprintf("Error bits = 0x%04x (expect 0x0000)\n", selfTest_getErrorBits());
}

