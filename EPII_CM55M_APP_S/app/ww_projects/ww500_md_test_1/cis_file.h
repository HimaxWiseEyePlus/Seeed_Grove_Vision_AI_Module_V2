/*
 * cis_file.h
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

#ifndef CIS_FILE_H_
#define CIS_FILE_H_

#include <stdint.h>
#include <stdbool.h>
#include "hx_drv_CIS_common.h"
#include "ff.h"  // FatFs library

/**
 * @brief Processes a binary file and applies the sensor register settings.
 *
 * @param filename The path to the binary file.
 * @return HX_CIS_ERROR_E Returns HX_CIS_NO_ERROR on success, otherwise an error code.
 */
HX_CIS_ERROR_E cis_file_process(const char *filename);

/**
 * @brief Test function to either apply settings or print parsed data.
 *
 * @param filename The path to the binary file.
 * @param apply_settings If true, calls hx_drv_cis_setRegTable(); otherwise, prints data.
 */
void cis_file_test(const char *filename, bool apply_settings);

#endif /* CIS_FILE_H_ */
