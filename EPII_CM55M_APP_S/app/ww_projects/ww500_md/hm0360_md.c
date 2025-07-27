/*
 * hm0360_md.c
 *
 * Supports HM0360 for motion detection, when it is used with other cameras.
 *
 *  Created on: 8 Jul 2025
 *      Author: charl
 */

/********************************** Includes ******************************************/

#include <stdbool.h>
#include "xprintf.h"
#include "printf_x.h"	// Print colours
#include "hm0360_md.h"
#include "WE2_debug.h"
#include "hx_drv_CIS_common.h"
#include "hm0360_regs.h"


/*************************************** Defines **************************************/

// Default interval in ms between frame grabs in motion detect mode
#define DPDINTERVAL 1000

/*************************************** Local Function Declarations ******************/

static void saveMainCameraConfig(void);
static void restoreMainCameraConfig(void);

static uint16_t calculateSleepTime(uint32_t interval);

/*************************************** Local variables ******************************/

static uint8_t mainCameraID;
static bool hm0360MainCamera = false;

static HX_CIS_SensorSetting_t HM0360_md_init_setting[] = {
#include "../../../ww500_md/cis_sensor/cis_hm0360/HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i"
};

// This is the interval in ms between frame grabs in motion detect mode
uint16_t sleepInterval;

/*************************************** Local Function Definitions *******************/

/**
 * Read and save settings of the main camera before accessing the HM0360.
 *
 * Configure the image sensor I2C address to be the HM0360.
 */
static void saveMainCameraConfig(void) {
	if (hm0360MainCamera) {
		// Don't waste time saving and restoring this
		return;
	}
	hx_drv_cis_get_slaveID(&mainCameraID);
    hx_drv_cis_set_slaveID(HM0360_SENSOR_I2CID);
}

/**
 * Restore settings of the main camera after accessing the HM0360
 */
static void restoreMainCameraConfig(void) {
	if (hm0360MainCamera) {
		// Don't waste time saving and restoring this
		return;
	}
	hx_drv_cis_set_slaveID(mainCameraID);
}


/**
 * Calculate values for the HM0360 sleep time registers.
 * This is the value used in Streaming 2 mode.
 *
 * Do this once per boot.
 * 0x0830 gives about 1s
 *
 * @param interval - in ms
 * @param value for HM0360 registers
 */
static uint16_t calculateSleepTime(uint32_t interval) {
	uint32_t sleepCount;

	sleepCount = interval * 0x8030 / 1000;

	// Make sure this does not exceed 16 bits
	if (sleepCount > 0xffff) {
		sleepCount = 0xffff;
	}

	xprintf("Interval of %dms gives sleep count = 0x%04x\n", interval, sleepCount);

	return (uint16_t) sleepCount;
}

/********************************** Public Function Definitions ***********************/

/**
 * Tests whether the HM0360 is present, by doing a read from the I2C address
 *
 * If the HM0360 is present the I2C read will work. Otherwise the driver code will
 * print an error and the call will fail.
 *
 *@param sensorAddress = I2C address of target image sensor
 *@param @return = true if present
 */
bool hm0360_md_isSensorPresent(uint8_t sensorAddress) {
	IIC_ERR_CODE_E ret;
	uint8_t rBuffer;

	/*    Usage-4: reads data from a specified I2C slave device using the I2C Master 0
	*      uint8_t rBuffer[2] = {0};
	*      uint8_t dataLen = 2;
	*/
	ret = hx_drv_i2cm_read_data(USE_DW_IIC_1, sensorAddress, &rBuffer, 1);

	return (ret == IIC_ERR_OK);
}

void hm0360_md_init(bool isMain, bool sensor_init) {
	HX_CIS_ERROR_E ret;

	hm0360MainCamera = isMain;

	if (hm0360MainCamera) {
		// All HM0360 initialisation will have been done elsewhere.
		return;
	}

    dbg_printf(DBG_LESS_INFO, "Initialising HM0360 at 0x%02x for MD only.\r\n", HM0360_SENSOR_I2CID);

    sleepInterval = DPDINTERVAL;

    saveMainCameraConfig();

    // Set HM0360 mode to SLEEP before initialisation
    ret = hm0360_md_setMode(CONTEXT_A, MODE_SLEEP, 0, 0);

    if (ret != HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 initialisation failed %d\r\n", ret);
    	restoreMainCameraConfig();
        return;
    }

	if (sensor_init == true) {
		// This is the long list of registers
		if(hx_drv_cis_setRegTable(HM0360_md_init_setting, HX_CIS_SIZE_N(HM0360_md_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR) {
			dbg_printf(DBG_LESS_INFO, "HM0360 Init fail \r\n");
			restoreMainCameraConfig();
			return;
		}
		else {
			dbg_printf(DBG_LESS_INFO, "HM0360 registers initialised for MD.\n");
		}
	}
	// This writes to register 0x2065 - we could put this into the big config file?
	hm0360_md_clear_interrupt(0xff);		// clear all bits

	restoreMainCameraConfig();
}


/*
 * Change HM0360 operating mode
 *
 * @param context - bits to write to the context control register (PMU_CFG_3, 0x3024)
 * @param mode - one of 8 modes of MODE_SELECT register
 * @param numFrames - the number of frames to capture before sleeping
 * @param sleepTime - the time (in ms) to sleep before waking again
 * @return error code
 */
HX_CIS_ERROR_E hm0360_md_setMode(uint8_t context, mode_select_t newMode, uint8_t numFrames, uint16_t sleepTime) {
	mode_select_t currentMode;
	HX_CIS_ERROR_E ret;
	uint16_t sleepCount;


    saveMainCameraConfig();

	ret = hx_drv_cis_get_reg(MODE_SELECT , &currentMode);
	if (ret != HX_CIS_NO_ERROR) {
	    restoreMainCameraConfig();
		return ret;
	}

	xprintf("  Changing mode from %d to %d with nFrames=%d and sleepTime=%d\r\n",
			currentMode, newMode, numFrames, sleepTime);

	// Disable before making changes
	ret = hx_drv_cis_set_reg(MODE_SELECT, MODE_SLEEP, 0);
	if (ret != HX_CIS_NO_ERROR) {
	    restoreMainCameraConfig();
		return ret;
	}

	// Context control
	ret = hx_drv_cis_set_reg(PMU_CFG_3, context, 0);
	if (ret != HX_CIS_NO_ERROR) {
	    restoreMainCameraConfig();
		return ret;
	}

	if (numFrames != 0) {
		// Applies to MODE_SW_NFRAMES_SLEEP, MODE_SW_NFRAMES_STANDBY and MODE_HW_NFRAMES_SLEEP
		// This is the number of frames to take continguously, after the sleep finishes
		// It is NOT the total number of frames
		ret = hx_drv_cis_set_reg(PMU_CFG_7, numFrames, 0);
		if (ret != HX_CIS_NO_ERROR) {
		    restoreMainCameraConfig();
			return ret;
		}
	}

	if (sleepTime != 0) {
		// Applies to MODE_SW_NFRAMES_SLEEP and MODE_HW_NFRAMES_SLEEP
		// This is the period of time between groups of frames.
		// Convert this to regsiter values for PMU_CFG_8 and PMU_CFG_9
		sleepCount = calculateSleepTime(sleepTime);
		ret = hx_drv_cis_set_reg(PMU_CFG_8, (uint8_t) (sleepCount >> 8), 0);	// msb
		if (ret != HX_CIS_NO_ERROR) {
		    restoreMainCameraConfig();
			return ret;
		}
		ret = hx_drv_cis_set_reg(PMU_CFG_9, (uint8_t) (sleepCount & 0xff), 0);	// lsb
		if (ret != HX_CIS_NO_ERROR) {
		    restoreMainCameraConfig();
			return ret;
		}
	}

	if (currentMode == MODE_SW_CONTINUOUS) {
		// consider delaying to finish current image before changing mode
	}

	ret = hx_drv_cis_set_reg(MODE_SELECT, newMode, 0);

    restoreMainCameraConfig();

	return ret;
}

/**
 * Read the HM0360 interrupt status register
 *
 * @param - pointer to byte to receive the status
 * @return error code
 */
HX_CIS_ERROR_E hm0360_md_get_int_status(uint8_t * val) {
	uint8_t currentStatus;
	HX_CIS_ERROR_E ret;

    saveMainCameraConfig();

	ret = hx_drv_cis_get_reg(INT_INDIC , &currentStatus);
	if (ret == HX_CIS_NO_ERROR) {
		*val = currentStatus;
	}

	restoreMainCameraConfig();

	return ret;
}


/**
 * Clear the HM0360 interrupt bits
 *
 * @param - mask for the bits to clear
 * @return error code
 */
HX_CIS_ERROR_E hm0360_md_clear_interrupt(uint8_t val) {
	HX_CIS_ERROR_E ret;

    saveMainCameraConfig();

	ret = hx_drv_cis_set_reg(INT_CLEAR, val, 0);

	restoreMainCameraConfig();

	return ret;
}

/**
 * Called when the AI processor is about to enter DPD.
 *
 * Get the HM0360 ready to detect motion
 *
 *  Select CONTEXT_B registers
 */
HX_CIS_ERROR_E hm0360_md_prepare(void) {
	HX_CIS_ERROR_E ret;

    saveMainCameraConfig();

	ret = hm0360_md_setMode(CONTEXT_B, MODE_SW_NFRAMES_SLEEP, 1, sleepInterval);

	restoreMainCameraConfig();

	return ret;
}

/**
 * Set the interval between frames in MD mode
 */
void hm0360_md_setFrameInterval(uint16_t interval) {
	sleepInterval = interval;
}

uint16_t hm0360_md_getFrameInterval(void) {
	return sleepInterval;
}

/**
 * Reads status regsiters related to Automatic Exposure

 * @param val - pointer to a structure that accepts the results
 * @return error code
 */
HX_CIS_ERROR_E hm0360_md_getGainRegs(HM0360_GAIN_T * val) {
	HX_CIS_ERROR_E ret;
	uint8_t valueH;
	uint8_t valueL;

    saveMainCameraConfig();

    // Integration registers 0x0202 & 0x0203
    // See data sheet section 9.3. Relates to "exposure time"
	ret = hx_drv_cis_get_reg(INTEGRATION_H, &valueH);
	ret |= hx_drv_cis_get_reg(INTEGRATION_L, &valueL);
	val->integration = (valueH << 8) + valueL;

	// Analog gain register 0x0205
	ret |= hx_drv_cis_get_reg(ANALOG_GAIN, &valueL);
	val->analogGain = valueL;

	// Digital gain registers 0x020E & 0x020F
	ret |= hx_drv_cis_get_reg(DIGITAL_GAIN_H, &valueH);
	ret |= hx_drv_cis_get_reg(DIGITAL_GAIN_L, &valueL);
	val->digitalGain = (valueH << 8) + valueL;

	// AE Mean register 0x205d
	ret |= hx_drv_cis_get_reg(AE_MEAN, &valueL);
	val->aeMean = valueL;

	// AE converged register 0x2060
	ret |= hx_drv_cis_get_reg(AE_CONVERGE, &valueL);
	val->aeConverged = valueL;

	restoreMainCameraConfig();
	return ret;
}

/**
 * Reads the Motion Detection ROI registers
 *
 * There are 32 ROI_OUT registers 20A1-20C0
 */
void hm0360_md_getMDOutput(uint8_t * regTable, uint8_t length) {
	uint8_t val;

	if (length != ROIOUTENTRIES) {
		return;
	}

	saveMainCameraConfig();

	for (uint8_t i=0; i < ROIOUTENTRIES; i++) {
		hx_drv_cis_get_reg(MD_ROI_OUT_0 + i, &val);
		regTable[i] = val;
	}

	restoreMainCameraConfig();
}

/**
 * Sets the HM0360 STROBE_CFG register
 *
 * This controls the STROBE pin which activates the flash.
 * See data sheet 10.23
 *
 * Bit 0 - 1 enables, 0 disables
 *
 * @param - value written to the register
 * @return error code
 */
HX_CIS_ERROR_E hm0360_md_configureStrobe(uint8_t val) {
	HX_CIS_ERROR_E ret;

    saveMainCameraConfig();

    ret = hx_drv_cis_set_reg(STROBE_CFG, val, 0);

    restoreMainCameraConfig();

	return ret;

}

/**
 * Sets HM0360 for motion detection, prior to entering deep sleep.
 *
 * This is a heavily redacted version of the original Himax code.
 * See ww500_md_test_1 to see what I cut out.
 */
HX_CIS_ERROR_E hm0360_md_enableMD(void) {
	HX_CIS_ERROR_E ret;

    saveMainCameraConfig();

	// Clear interrupts
    hx_drv_cis_set_reg(INT_CLEAR, 0xff, 0x01);

    // Set HM0360 mode to SLEEP before initialisation
    ret = hm0360_md_setMode(CONTEXT_B, MODE_SW_NFRAMES_SLEEP, 1, DPDINTERVAL);

    if (ret != HX_CIS_NO_ERROR) {
    	dbg_printf(DBG_LESS_INFO, "HM0360 md on fail\r\n");
        restoreMainCameraConfig();
        return -1;
    }

    // This version has no delay
    dbg_printf(DBG_LESS_INFO, "HM0360 Motion Detection on!\r\n");

    restoreMainCameraConfig();

	return 0;
}
