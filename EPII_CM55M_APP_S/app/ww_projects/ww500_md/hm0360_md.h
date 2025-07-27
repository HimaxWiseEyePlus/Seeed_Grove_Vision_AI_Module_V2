/*
 * hm0360_md.h
 *
 * Supports HM0360 for motion detection, when it is used with other cameras.
 *
 *  Created on: 8 Jul 2025
 *      Author: charl
 */

#ifndef HM0360_MD_H_
#define HM0360_MD_H_


/********************************** Includes ******************************************/

#include <stdbool.h>
#include "hx_drv_CIS_common.h"

/*************************************** Definitions *******************************************/

#define HM0360_SENSOR_I2CID				(0x24)

// CGP some defines here

#define HM0360NUMGAINREGS 5

// Structure to contain some values relating to automatic exposure
typedef struct {
	uint16_t integration;	// Value of INTEGRATION_H, INTEGRATION_L
	uint16_t digitalGain;	// Value of DIGITAL_GAIN_H, DIGITAL_GAIN_L
	uint8_t analogGain;		// Value of ANALOG_GAIN
	uint8_t aeMean;			// Value of AE_MEAN
	uint8_t aeConverged;	// Value of AE_CONVERGED
} HM0360_GAIN_T;

// Select streaming mode by writing to 0x0100.
// See data sheet 6.1 & 10.2
typedef enum {
    MODE_SLEEP,				// 0 Hardware sleep
	MODE_SW_CONTINUOUS,		// 1 SW triggers continuous streaming
	MODE_SW_NFRAMES_SLEEP,	// 2 SW trigger, output N frames then sleep for a set interval
	MODE_SW_NFRAMES_STANDBY,// 3 SW trigger, output N frames then s/w standby (needs a reset to restart???)
	MODE_HW_TRIGGER,		// 4 TRIGGER pin starts streaming
	MODE_RFU,				// 5 Not defined
	MODE_HW_NFRAMES_STANDBY,// 6 HW trigger, output N frames then s/w standby (needs a reset to restart???)
	MODE_HW_NFRAMES_SLEEP,	// 7 HW trigger, output N frames then sleep
} mode_select_t;

/*************************************** Public Function Declarations **************************/

bool hm0360_md_isSensorPresent(uint8_t sensorAddress);

void hm0360_md_init(bool isMain, bool sensor_init);

HX_CIS_ERROR_E hm0360_md_setMode(uint8_t context, mode_select_t newMode, uint8_t numFrames, uint16_t sleepTime);

HX_CIS_ERROR_E hm0360_md_get_int_status(uint8_t * val);

HX_CIS_ERROR_E hm0360_md_clear_interrupt(uint8_t val);

// Prepare the MD
HX_CIS_ERROR_E hm0360_md_prepare(void);

void hm0360_md_setFrameInterval(uint16_t interval);
uint16_t hm0360_md_getFrameInterval(void);

HX_CIS_ERROR_E hm0360_md_getGainRegs(HM0360_GAIN_T * val);

void hm0360_md_getMDOutput(uint8_t * regTable, uint8_t length);

// Configure the HM0360 STROBE pin which can drive the flash cct
HX_CIS_ERROR_E hm0360_md_configureStrobe(uint8_t val);

HX_CIS_ERROR_E hm0360_md_enableMD(void);

#endif /* HM0360_MD_H_ */
