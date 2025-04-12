/*
 * hm0360_x.c
 *
 * Functions to extend HM0360 support
 *
 *  Created on: 16 Mar 2025
 *      Author: charl
 */

/******************************** Includes ***********************************************/

#include "hm0360_x.h"

#include "hm0360_regs.h"
#include "xprintf.h"
#include "timer_interface.h"


#include "FreeRTOS.h"
#include "timers.h"

#include "hx_drv_CIS_common.h"

/******************************** Defines ****************************/

#define I2CDELAY	1

/******************************** Local variables ****************************/


static HX_CIS_SensorSetting_t  HM0360_roi[] = {
			{HX_CIS_I2C_Action_W, ROI_START_END_V_A, 0x00},	// start end end V
			{HX_CIS_I2C_Action_W, ROI_START_END_H_A, 0x00},	// start end end H
	};

static HX_CIS_SensorSetting_t  HM0360_roi_all[] = {
		{HX_CIS_I2C_Action_W, ROI_START_END_V, 0x00},	// main start & end V
		{HX_CIS_I2C_Action_W, ROI_START_END_H, 0x00},	// main start & end H
		{HX_CIS_I2C_Action_W, ROI_START_END_V_A, 0x00},	// context A start & end V
		{HX_CIS_I2C_Action_W, ROI_START_END_H_A, 0x00},	// context A start & end H
		{HX_CIS_I2C_Action_W, ROI_START_END_V_B, 0x00},	// context B start & end V
		{HX_CIS_I2C_Action_W, ROI_START_END_H_B, 0x00},	// context B start & end H
	};

static HX_CIS_SensorSetting_t  HM0360_threshold[] = {
			{HX_CIS_I2C_Action_W, MD_BLOCK_NUM_TH_A, 0x00},	//
	};

static HX_CIS_SensorSetting_t  HM0360_threshold_all[] = {
		{HX_CIS_I2C_Action_W, MD_BLOCK_NUM_TH, 0x00},	//	Main
		{HX_CIS_I2C_Action_W, MD_BLOCK_NUM_TH_A, 0x00},	//	Context A
		{HX_CIS_I2C_Action_W, MD_BLOCK_NUM_TH_B, 0x00},	//	Context B
	};

static HX_CIS_SensorSetting_t  HM0360_sensitivity[] = {
		{HX_CIS_I2C_Action_W, MD_TH_STR_L_A, 0x00},	// Context A Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_TH_STR_H_A, 0x00},	// Context A Threshold strength MS
		{HX_CIS_I2C_Action_W, MD_LIGHT_COEF, 0x00},	// Light coefficient
		{HX_CIS_I2C_Action_W, MD_IIR_PARAM, 0x00},	// IIR parameter
	};

static HX_CIS_SensorSetting_t  HM0360_sensitivity_all[] = {
		{HX_CIS_I2C_Action_W, MD_TH_STR_L, 0x00},	// Main Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_TH_STR_H, 0x00},	// Main Threshold strength MS
		{HX_CIS_I2C_Action_W, MD_TH_STR_L_A, 0x00},	// Context A Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_TH_STR_H_A, 0x00},	// Context A Threshold strength MS
		{HX_CIS_I2C_Action_W, MD_TH_STR_L_B, 0x00},	// Context B Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_TH_STR_H_B, 0x00},	// Context B Threshold strength MS
		{HX_CIS_I2C_Action_W, MD_LIGHT_COEF, 0x00},	// Light coefficient
		{HX_CIS_I2C_Action_W, MD_IIR_PARAM, 0x00},	// IIR parameter
	};

static HX_CIS_SensorSetting_t  HM0360_latency[] = {
		{HX_CIS_I2C_Action_W, MD_CTRL_A, 0x00},		// Context A Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_LATENCY, 0x00},	//
		{HX_CIS_I2C_Action_W, MD_LATENCY_TH, 0x00},	//
		{HX_CIS_I2C_Action_W, MD_CTRL1, 0x00},		//
	};

static HX_CIS_SensorSetting_t  HM0360_latency_all[] = {
		{HX_CIS_I2C_Action_W, MD_CTRL, 0x00},		// Main Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_CTRL_A, 0x00},		// Context A Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_CTRL_B, 0x00},		// Context B Threshold strength LS
		{HX_CIS_I2C_Action_W, MD_LATENCY, 0x00},	//
		{HX_CIS_I2C_Action_W, MD_LATENCY_TH, 0x00},	//
		{HX_CIS_I2C_Action_W, MD_CTRL1, 0x00},		//
	};



/******************************** Private Function Declarations ****************************/

static void print_registers(char * heading, HX_CIS_SensorSetting_t * sensor_settings, uint16_t num_entries);


/******************************** Private Function Definitions ****************************/

/**
 * Prints values of registers
 */
static void print_registers(char * heading, HX_CIS_SensorSetting_t * sensor_settings, uint16_t num_entries) {
	uint16_t address;
	uint8_t value;

	xprintf("%s", heading);

	for (uint16_t i = 0; i < num_entries; i++) {
		address = sensor_settings[i].RegAddree;
		value = sensor_settings[i].Value;
		xprintf("0x%04X = 0x%02X\n", address, value);
	}
}

/******************************** Public Function Definitions ****************************/

void hm0360_x_set_roi(uint8_t startV, uint8_t endV, uint8_t startH, uint8_t endH) {
	uint16_t length;

	length = HX_CIS_SIZE_N(HM0360_roi, HX_CIS_SensorSetting_t);

	// We must have HM0360_roi[0] = ROI_START_END_V
	HM0360_roi[0].Value = (endV << 4) + startV;

	// We must have HM0360_roi[1] = ROI_START_END_H
	HM0360_roi[1].Value = (endH << 4) + startH;

//	setting.I2C_ActionType = HX_CIS_I2C_Action_W;
//
//	setting.RegAddree = ROI_START_END_V; 	// 0x2081;
//	setting.Value = (endV << 4) + startV;
//	HM0360_roi[0] = setting;
//
//	setting.I2C_ActionType = HX_CIS_I2C_Action_W;
//	setting.RegAddree = ROI_START_END_H; // 0x2082;
//	setting.Value = (endH << 4) + startH;
//	HM0360_roi[1] = setting;

	hx_drv_cis_setRegTable(HM0360_roi, length);
}

/**
 * Prints the region of interest registers
 */
void hm0360_x_print_roi(char * heading) {
	uint16_t length;
	uint8_t value;

	length = HX_CIS_SIZE_N(HM0360_roi_all, HX_CIS_SensorSetting_t);

	for (uint8_t i=0; i < length ; i++) {
		hx_drv_cis_get_reg(HM0360_roi_all[i].RegAddree, &value);
		HM0360_roi_all[i].Value = value;
	}

    print_registers(heading, HM0360_roi_all, length);
}

/**
 * Some tests
 */
void hm0360_x_test_roi(void) {

	// print initial values
	// from "HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i"
	hm0360_x_print_roi("\nROI Initial values:");				// expect a4, b5

	hm0360_x_set_roi(1, 13, 2, 14);
	hm0360_x_print_roi("\nROI test set 1:");				// expect D1, E2

	hm0360_x_set_roi(2, 12, 3, 13);
	hm0360_x_print_roi("\nROI test set 2:");				// expect c2, d3
}


// Threshold settings
// TODO check min and max values?
void hm0360_x_set_threshold(uint8_t numBlocks) {
	uint16_t length;

	length = HX_CIS_SIZE_N(HM0360_threshold, HX_CIS_SensorSetting_t);

	// We must have HM0360_threshold[0] = MD_BLOCK_NUM_TH_A
	HM0360_threshold[0].Value = numBlocks;

	hx_drv_cis_setRegTable(HM0360_threshold, length);
}

// Prints current threshold settings
void hm0360_x_print_threshold(char * heading) {
	uint16_t length;
	uint8_t value;

	length = HX_CIS_SIZE_N(HM0360_threshold_all, HX_CIS_SensorSetting_t);

	for (uint8_t i=0; i < length ; i++) {
		hx_drv_cis_get_reg(HM0360_threshold_all[i].RegAddree, &value);
		HM0360_threshold_all[i].Value = value;
	}

    print_registers(heading, HM0360_threshold_all, length);
}

// Some tests for Threshold settings
void hm0360_x_test_threshold(void) {
	// print initial values
	// from "HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i"
	hm0360_x_print_threshold("\nThreshold Initial values:");				// expect ?

	hm0360_x_set_threshold(8);
	hm0360_x_print_threshold("\nThreshold test set 1:");				// expect 8

	hm0360_x_set_threshold(32);
	hm0360_x_print_threshold("\nThreshold test set 2:");				// expect 32
}

// Sensitivity settings
void hm0360_x_set_sensitivity(uint8_t thresholdStrLsb, uint8_t thresholdStrMsb,
		uint8_t lightCoeff, uint8_t iirParameter) {
	uint16_t length;

	length = HX_CIS_SIZE_N(HM0360_sensitivity, HX_CIS_SensorSetting_t);

	// We must have HM0360_sensitivity[0] = MD_TH_STR_L_A
	HM0360_sensitivity[0].Value = thresholdStrLsb;

	// We must have HM0360_sensitivity[1] = MD_TH_STR_H_A
	HM0360_sensitivity[1].Value = thresholdStrMsb;

	// We must have HM0360_sensitivity[2] = MMD_LIGHT_COEF
	HM0360_sensitivity[2].Value = lightCoeff;

	// We must have HM0360_sensitivity[3] = MD_TIIR_PARAM
	HM0360_sensitivity[3].Value = iirParameter;

	hx_drv_cis_setRegTable(HM0360_sensitivity, length);
}

// Prints current sensitivity settings
void hm0360_x_print_sensitivity(char * heading) {
	uint16_t length;
	uint8_t value;

	length = HX_CIS_SIZE_N(HM0360_sensitivity_all, HX_CIS_SensorSetting_t);

	for (uint8_t i=0; i < length ; i++) {
		hx_drv_cis_get_reg(HM0360_sensitivity_all[i].RegAddree, &value);
		HM0360_sensitivity_all[i].Value = value;
	}

    print_registers(heading, HM0360_sensitivity_all, length);
}

// Some tests for sensitivity settings
void hm0360_x_test_sensitivity(void) {
	// print initial values
	// from "HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i"
	hm0360_x_print_sensitivity("\nSensitivity Initial values:");				// expect ?

	hm0360_x_set_sensitivity(0x30, 0x30, 0x41, 0x00);	// Low sensitivity values
	hm0360_x_print_sensitivity("\nSensitivity (low):");				// expect

	hm0360_x_set_sensitivity(0x20, 0x20, 0x31, 0x80);	// Medium sensitivity values
	hm0360_x_print_sensitivity("\nSensitivity (medium):");				// expect

	hm0360_x_set_sensitivity(0x10, 0x10, 0x21, 0xF0);	// High sensitivity values
	hm0360_x_print_sensitivity("\nSensitivity (high):");				// expect
}


// Sensitivity settings
void hm0360_x_set_latency(uint8_t ctrl, uint8_t latency,
		uint8_t latencyTh, uint8_t ctrl1) {
	uint16_t length;

	length = HX_CIS_SIZE_N(HM0360_latency, HX_CIS_SensorSetting_t);

	// We must have HM0360_latency[0] = MD_CTRL_A
	HM0360_latency[0].Value = ctrl;

	// We must have HM0360_latency[1] = MD_LATENCY
	HM0360_latency[1].Value = latency;

	// We must have HM0360_latency[2] = MD_LATENCY_TH
	HM0360_latency[2].Value = latencyTh;

	// We must have HM0360_latency[3] = MD_CTRL1
	HM0360_latency[3].Value = ctrl1;

	hx_drv_cis_setRegTable(HM0360_latency, length);

}

// Prints current sensitivity settings
void hm0360_x_print_latency(char * heading) {
	uint16_t length;
	uint8_t value;

	length = HX_CIS_SIZE_N(HM0360_latency_all, HX_CIS_SensorSetting_t);

	for (uint8_t i=0; i < length ; i++) {
		hx_drv_cis_get_reg(HM0360_latency_all[i].RegAddree, &value);
		HM0360_latency_all[i].Value = value;
	}

    print_registers(heading, HM0360_latency_all, length);
}

// Some tests for sensitivity settings
void hm0360_x_test_latency(void) {
	// print initial values
	// from "HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i"

	hm0360_x_print_latency("\nLatency Initial values:");				// expect ?

//	hm0360_x_set_latency(8);
//	hm0360_x_print_latency("\nLatency test set 1:");				// expect

//	hm0360_x_set_latency(32);
//	hm0360_x_print_latency("\nLatency test set 2:");				// expect
}


// Read ROI_OUT registers
void hm0360_x_get_roi_out(uint8_t * regTable, uint8_t length) {
	uint8_t val;

	if (length != ROIOUTENTRIES) {
		return;
	}

	for (uint8_t i=0; i < ROIOUTENTRIES; i++) {
		hx_drv_cis_get_reg(MD_ROI_OUT_0 + i, &val);
		regTable[i] = val;

		// TODO is a delay needed?
        //vTaskDelay(pdMS_TO_TICKS(1));
	}
}

// Read one register
void hm0360_x_get_reg(uint16_t regAddr, uint8_t * val) {
	HX_CIS_ERROR_E res;

	static HX_CIS_SensorSetting_t HM0360_singleRegister[] = {
	        {HX_CIS_I2C_Action_R, 0x0000, 0x00},
	};

	HM0360_singleRegister[0].RegAddree = regAddr;

#if 1
	// This seems to work
	res = hx_drv_cis_setRegTable(HM0360_singleRegister, HX_CIS_SIZE_N(HM0360_singleRegister, HX_CIS_SensorSetting_t));

    if(res == HX_CIS_NO_ERROR) {
    	//printf("Read 0x%04x as 0x%02x\r\n", regAddr, HM0360_singleRegister[0].Value);
    	*val = HM0360_singleRegister[0].Value;
    }
    else {
    	printf("hx_drv_cis_setRegTable() of 0x%04x fail. err %d\r\n", regAddr, res);
    }
#else
	// This also seems to work OK.
	res = hx_drv_cis_getRegTable(HM0360_singleRegister, HX_CIS_SIZE_N(HM0360_singleRegister, HX_CIS_SensorSetting_t));

    if(res == HX_CIS_NO_ERROR) {
    	//printf("Read 0x%04x as 0x%02x\r\n", regAddr, HM0360_singleRegister[0].Value);
    	*val = HM0360_singleRegister[0].Value;
    }
    else {
    	printf("hx_drv_cis_getRegTable() of 0x%04x fail. err %d\r\n", regAddr, res);
    }

#endif
}
