/*
 * ledFlash.c
 *
 *  Created on: 14 Aug 2025
 *      Author: charl
 */



/********************************** Includes ******************************************/

#include <stdbool.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "timers.h"

#include "xprintf.h"
#include "printf_x.h"	// Print colours

#include "ledFlash.h"
#include "pca9574.h"


/*************************************** Defines **************************************/

#define LF_NUMCHANNELS 	8

// Defines for bits on the  PCA9574
#define LF_BRSEL0		(1 << 0)
#define LF_BRSEL1		(1 << 1)
#define LF_BRSEL2		(1 << 2)
#define LF_BRSEL3		(1 << 3)
#define LF_VISENABLE 	(1 << 4)
#define LF_IRENABLE		(1 << 5)
#define LF_RFU			(1 << 6)
#define LF_FLENABLE		(1 << 7)


/*************************************** Local Function Declarations ******************/

static void FlashOffTimerCallback(TimerHandle_t xTimer);

/*************************************** Local variables ******************************/

static bool ledFlashInitialised = false;

// Need to maintain a copy of bits sent to the control/status chip, so we can change individual bits
static uint8_t controlBits = 0;

static TimerHandle_t flashOffTimer;

static bool inhibitFlash;

/*************************************** Local Function Definitions *******************/

static void FlashOffTimerCallback(TimerHandle_t xTimer) {
	ledFlashDisable();
}

/********************************** Public Function Definitions ***********************/


/**
 * Enables the LED Flash system.
 *
 * Leaves with both LEDs deselected, brightness at minimum and inhibitFlash = true
 *
 * @return -true on success
 */
bool ledFlashInit(void) {
	HX_CIS_ERROR_E ret;

    XP_LT_RED;
    xprintf("DEBUG: ledFlashInit()\n");
    XP_WHITE;

	ret = pca9574_init(PCA9574_I2C_ADDRESS_0);

	if (ret != HX_CIS_NO_ERROR) {
		// Failure. Maybe no PCA9574
		ledFlashInitialised = false;
	    XP_LT_RED;
	    xprintf("Failed to initialise Flash LED\n");
	    XP_WHITE;
		return false;
	}

    XP_LT_RED;
    xprintf("DEBUG: ledFlashInit()\n");
    XP_WHITE;

	ledFlashInitialised = true;

	// pca9574_init() has set all output bits to output, 0
	controlBits = 0;

	inhibitFlash = true;

	ledFlashSelectLED(0);	// de-select both LEDs

	// Note that at this stage both the IR and visible LEDs are enabled.
	// The application should call ledFlashSelectLED() to change this.

    flashOffTimer = xTimerCreate("FlashOffTimer",
            pdMS_TO_TICKS(10),    // initial dummy period
            pdFALSE,                // one-shot timer
            NULL,                   // timer ID (optional)
			FlashOffTimerCallback);

	return true;
}

/**
 * Selects the LED brightness
 *
 * Takes the brightness percentage and approximates a value to write to the MOSFETs.
 * This is rough! Might need to revise this e.g. with a switch(brightness)
 *
 * @param brightness - a  value that determines brightness - percentage. 0 inhibits flash
 */
void ledFlashBrightness(uint8_t brightness) {
	uint8_t brBits;

	if (!ledFlashInitialised) {
		return;
	}

	inhibitFlash = (brightness == 0);

	// Convert to a value between 0-15 - approximate!
	brBits = (brightness * 15 / 100);

	if (brBits > 15) {
		brBits = 15;
	}

	// brightness is a value between 0-15 and these should map onto the 4 LS bits of the PCA9574
	// which control the feedback resistors for the PSUs

	controlBits &= ~0x0f;	// clear the 4 LS bits
	controlBits |= brBits;	// sets the 4 LS bits

	// Don't send except in ledFlashEnable() andledFlashDisable()
	// Now send these bits to the PCA9574
	//pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);

	XP_LT_RED;
    xprintf("DEBUG: ledFlashBrightness(%d)\n", brightness);
    XP_WHITE;

}

/**
 * Selects the LED(s) to use
 *
 * In some hardware implementations there is only one LED - this is VISLED
 *
 * Make sure ledFlashBrightness() is called first as this determines inhibitFlash
 *
 * @param led - a bit mask, one for each LED
 */
void ledFlashSelectLED(FlashLeds_t led) {

	if (!ledFlashInitialised) {
		return;
	}

	if (inhibitFlash) {
		// Disable both LEDs by setting these bits high
		controlBits |= LF_VISENABLE;
		controlBits |= LF_IRENABLE;
	}
	else {
		if (led & VIS_LED) {
			// Active low, so clear this bit to select the LED
			controlBits &= ~LF_VISENABLE;
		}
		else {
			controlBits |= LF_VISENABLE;
		}

		if (led & IR_LED) {
			// Active low, so clear this bit to select the LED
			controlBits &= ~LF_IRENABLE;
		}
		else {
			controlBits |= LF_IRENABLE;
		}
	}

	// Don't send except in ledFlashEnable() andledFlashDisable()
	// Now send these bits to the PCA9574
	//pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
	XP_LT_RED;
    xprintf("DEBUG: ledFlashSelectLED(%d)\n", led);
    XP_WHITE;

}

/**
 * Turns on the Flash LED
 *
 * @param duration - a period after which the flash is turned off
 */
void ledFlashEnable(uint16_t duration) {

	if (!ledFlashInitialised) {
		return;
	}

	XP_LT_RED;
    xprintf("DEBUG: ledFlashEnable(%dms)\n", duration);
    XP_WHITE;


	if (inhibitFlash) {
		ledFlashDisable();
		return;
	}

	if ((controlBits & LF_FLENABLE) == 0) {
		// Change is needed
		controlBits |= LF_FLENABLE;

		// Start a timer that delays for the defined interval.
	    if (flashOffTimer != NULL) {
	        // Change the period and start the timer
	    	// The callback is FlashOffTimerCallback() which calls ledFlashDisable
	        xTimerChangePeriod(flashOffTimer, pdMS_TO_TICKS(duration), 0);
	    }
	    else {
	    	return;
	    }

		// Now send these bits to the PCA9574
		pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
	}
	else {
		xprintf("DEBUG: flash was already on\n");
	}
}

/**
 * Turns off the Flash LED
 *
 *
 */
void ledFlashDisable(void) {

	if (!ledFlashInitialised) {
		return;
	}

	XP_LT_RED;
	xprintf("DEBUG: ledFlashDisable()\n");

	if ((controlBits & LF_FLENABLE) == LF_FLENABLE) {
		// Change is needed
		controlBits &= ~LF_FLENABLE;
		// Now send these bits to the PCA9574
		pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
	}
	else {
		xprintf("DEBUG: flash was already off\n");
	}

    XP_WHITE;
}
