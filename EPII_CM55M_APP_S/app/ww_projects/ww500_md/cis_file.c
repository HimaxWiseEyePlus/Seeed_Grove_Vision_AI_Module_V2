/*
 * cis_file.c
 *
 * Functions to program cis sensors with data from files containing binary data.
 *
 * These files are written by the python script 'scan_cis_settings.py'
 * and contain binary data.
 *
 * Thanks to ChatGPT!
 *
 *  Created on: 16 Mar 2025
 *      Author: charl
 */
#include "cis_file.h"

#include <stdio.h>
#include <stdlib.h>
#include "xprintf.h"

/**
 * Read CIS register settings from a file and process them
 *
 * @param filename - name of file containing binary data
 * @return error code
 */
HX_CIS_ERROR_E cis_file_process(const char *filename) {
    FIL file;
    FRESULT res;
    UINT bytes_read;
    HX_CIS_ERROR_E result;
    DWORD file_size;
    uint16_t num_entries ;

    // Open file
    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        xprintf("Error opening '%s': %d\n", filename, res);
        return HX_CIS_ERROR_INVALID_PARAMETERS;
    }

    // Get file size
    file_size = f_size(&file);

    if (file_size % sizeof(HX_CIS_SensorSetting_t) != 0) {
        xprintf("Error: Invalid file size\n");
        f_close(&file);
        return HX_CIS_ERROR_INVALID_PARAMETERS;
    }

    num_entries = file_size / sizeof(HX_CIS_SensorSetting_t);

    // Allocate memory
    HX_CIS_SensorSetting_t *sensor_settings = malloc(file_size);

    if (!sensor_settings) {
        xprintf("Memory allocation failed\n");
        f_close(&file);
        return HX_CIS_UNKNOWN_ERROR;
    }

    // Read the binary data
    res = f_read(&file, sensor_settings, file_size, &bytes_read);
    f_close(&file);

    if (res != FR_OK || bytes_read != file_size) {
        xprintf("Error reading file: %d\n", res);
        free(sensor_settings);
        return HX_CIS_UNKNOWN_ERROR;
    }

    // Apply the settings
    result = hx_drv_cis_setRegTable(sensor_settings, num_entries);
    if (result == HX_CIS_NO_ERROR) {
        xprintf("Processed %d settings from '%s'\n", num_entries, filename);
    }
    else {
        xprintf("Error: hx_drv_cis_setRegTable failed with code %d\n", result);
    }

    free(sensor_settings);

    return result;
}

/**
 * Allows testing
 *
 * @param filename -
 * @param apply_settings - if true then write actual values. If false then print info
 */
void cis_file_test(const char *filename, bool apply_settings) {
	FIL file;
	FRESULT res;
	UINT bytes_read;
	HX_CIS_ERROR_E result;

	// Open file
	res = f_open(&file, filename, FA_READ);
	if (res != FR_OK) {
		xprintf("Error opening file: %d\n", res);
		return;
	}

	// Get file size
	DWORD file_size = f_size(&file);

	if (file_size % sizeof(HX_CIS_SensorSetting_t) != 0) {
		xprintf("Error: Invalid file size\n");
		f_close(&file);
		return;
	}

	uint16_t num_entries = file_size / sizeof(HX_CIS_SensorSetting_t);

	// Allocate memory
	HX_CIS_SensorSetting_t *sensor_settings = malloc(file_size);

	if (!sensor_settings) {
		xprintf("Memory allocation failed\n");
		f_close(&file);
		return;
	}

	// Read the binary data
	res = f_read(&file, sensor_settings, file_size, &bytes_read);
	f_close(&file);

	if (res != FR_OK || bytes_read != file_size) {
		xprintf("Error reading file: %d\n", res);
		free(sensor_settings);
		return;
	}

	if (apply_settings) {
		// Apply the settings using hx_drv_cis_setRegTable
		result = hx_drv_cis_setRegTable(sensor_settings, num_entries);
	    if (result == HX_CIS_NO_ERROR) {
	        xprintf("Processed %d settings from '%s'\n", num_entries, filename);
	    }
	    else {
	        xprintf("Error: hx_drv_cis_setRegTable failed with code %d\n", result);
	    }
	}
	else {
		// Print the parsed data
	    xprintf("Parsing %d settings from '%s':\n", num_entries, filename);

		for (uint16_t i = 0; i < num_entries; i++) {
			xprintf("Action: 0x%02X, Register: 0x%04X, Value: 0x%02X\n",
					sensor_settings[i].I2C_ActionType, sensor_settings[i].RegAddree, sensor_settings[i].Value);
		}
	}

	free(sensor_settings);
}

