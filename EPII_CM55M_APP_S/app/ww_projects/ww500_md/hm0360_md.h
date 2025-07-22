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

// Structure to contain some valves
typedef struct {
	uint16_t integration;	// Value of INTEGRATION_H, INTEGRATION_L
	uint8_t analogGain;	// Value of ANALOG_GAIN
	uint16_t digitalGain;	// Value of DIGITAL_GAIN_H, DIGITAL_GAIN_L
} HM0360_GAIN_T;

/*************************************** Public Function Declarations **************************/

bool hm0360_md_present(void);

void hm0360_md_init(bool isMain, bool sensor_init);

HX_CIS_ERROR_E hm0360_md_get_int_status(uint8_t * val);

HX_CIS_ERROR_E hm0360_md_clear_interrupt(uint8_t val);

// Prepare the MD
HX_CIS_ERROR_E hm0360_md_prepare(void);

void hm0360_md_setFrameInterval(uint16_t interval);
uint16_t hm0360_md_getFrameInterval(void);

HX_CIS_ERROR_E hm0360_md_getGainRegs(HM0360_GAIN_T * val);

void hm0360_md_getMDOutput(uint8_t * regTable, uint8_t length);

#endif /* HM0360_MD_H_ */
