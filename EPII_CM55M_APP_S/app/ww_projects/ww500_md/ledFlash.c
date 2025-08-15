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
#include "xprintf.h"




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


/*************************************** Local variables ******************************/

// Need to maintain a copy of bits sent to the control/status chip, so we can change individual bits
uint8_t controlBits = 0;

static TimerHandle_t flashOffTimer;

/*************************************** Local Function Definitions *******************/

static void FlashOffTimerCallback(TimerHandle_t xTimer) {
	ledFlashDisable();
}

/********************************** Public Function Definitions ***********************/

// Enables the chip - returns true if OK
bool ledFlashInit(void) {
	HX_CIS_ERROR_E ret;

	ret = pca9574_init(PCA9574_I2C_ADDRESS_0);

	// pca9574_init() has set all output bits to 0
	controlBits = 0;

	// Note that at this stage both ethe IR and visible LEDs are enabled.
	// The application should call ledFlashSelectLED() to chnage this.

    flashOffTimer = xTimerCreate("FlashOffTimer",
            pdMS_TO_TICKS(10),    // initial dummy period
            pdFALSE,                // one-shot timer
            NULL,                   // timer ID (optional)
			FlashOffTimerCallback);

	return ret;
}

/**
 * Selects the LED brightness
 *
 * This is a 4-bit value
 *
 * @param led - a  value that determines brightness (0-15)
 */
void ledFlashBrightness(uint8_t brightness) {

	if (brightness > 15) {
		return;
	}

	// brightness is a value between 0-15 and these should map onto the 4 LS bits of the PCA9574
	// which control the feedback resistors for the PSUs

	controlBits &= ~0x0f;		// clear the 4 LS bits
	controlBits |= brightness;	// sets the 4 LS bits

	// Now send these bits to the PCA9574
	pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
}


/**
 * Selects the LED(s) to use
 *
 * In some hardware implementations there is only one LED - this is VISLED
 *
 * @param led - a bit mask, one for each LED
 */
void ledFlashSelectLED(FlashLeds_t led) {

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

	// Now send these bits to the PCA9574
	pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
}

/**
 * Turns on the Flash LED
 *
 * @param duration - a period after which the flash is turned off
 */
void ledFlashEnable(uint16_t duration) {

	// set a timer that will turn off the flash

	// Start a timer that delays for the defined interval.
	// When it expires, switch to CAPTURUNG state and request another image
    if (flashOffTimer != NULL)  {
        // Change the period and start the timer
    	// The callback issues a APP_MSG_IMAGETASK_RECAPTURE event
        xTimerChangePeriod(flashOffTimer, pdMS_TO_TICKS(duration), 0);
    }

	controlBits |= LF_FLENABLE;

	// Now send these bits to the PCA9574
	pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
}

/**
 * Turns off the Flash LED
 *
 */
void ledFlashDisable(void) {
	controlBits &= ~LF_FLENABLE;

	// Now send these bits to the PCA9574
	pca9574_write(PCA9574_I2C_ADDRESS_0, PCA9574_REG_OUT, controlBits);
}
