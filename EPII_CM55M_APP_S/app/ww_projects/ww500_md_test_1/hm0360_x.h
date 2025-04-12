/*
 * hm0360_x.h
 *
 * Functions to extend HM0360 support
 *
 *  Created on: 16 Mar 2025
 *      Author: charl
 */

#ifndef HM0360_X_H_
#define HM0360_X_H_


#include "hx_drv_CIS_common.h"

// Sets region of interest settings
void hm0360_x_set_roi(uint8_t startV, uint8_t endV, uint8_t startH, uint8_t endH);

// Prints current region of interest settings
void hm0360_x_print_roi(char * heading);

// Some tests for ROI settings
void hm0360_x_test_roi(void);

// Threshold settings
void hm0360_x_set_threshold(uint8_t numBlocks);

// Prints current threshold settings
void hm0360_x_print_threshold(char * heading);

// Some tests for Threshold settings
void hm0360_x_test_threshold(void);


// Sensitivity settings
void hm0360_x_set_sensitivity(uint8_t thresholdStrLsb, uint8_t thresholdStrMsb,
		uint8_t lightCoeff, uint8_t iirParameter);

// Prints current sensitivity settings
void hm0360_x_print_sensitivity(char * heading);

// Some tests for sensitivity settings
void hm0360_x_test_sensitivity(void);


// Sensitivity settings
void hm0360_x_set_latency(uint8_t ctrl, uint8_t latency,
		uint8_t latencyTh, uint8_t ctrl1);

// Prints current sensitivity settings
void hm0360_x_print_latency(char * heading);

// Some tests for sensitivity settings
void hm0360_x_test_latency(void);

// Read ROI_OUT registers
void hm0360_x_get_roi_out(uint8_t * regTable, uint8_t length);

// Read one register
void hm0360_x_get_reg(uint16_t register, uint8_t * val);

#endif /* HM0360_X_H_ */
