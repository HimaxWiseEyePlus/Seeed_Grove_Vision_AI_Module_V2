/*
 * crc16_ccitt.h
 *
 *  Created on: 1 Aug 2024
 *      Author: charl
 *
 * Implements a CRC16-CCITT algorithm from here:
 * 		https://github.com/zqad/crc16-ccitt
 * which references this:
 * 		https://srecord.sourceforge.net/crc16-ccitt.html
 *
 * These .c & .h files simplify the file structure and place everything necessary in two files.
 *
 * There are more notes in the readme.txt file of project ble_app_crc
 *
 */

#ifndef CRC16_CCITT_H_
#define CRC16_CCITT_H_

// Returns the CRC for a buffer of a given length
void crc16_ccitt_generate(uint8_t * data, uint16_t length, uint16_t * CRC);

// Checks the CRC for a buffer of given length, with the CRC being the final 2 bytes (BE)
bool crc16_ccitt_validate(uint8_t * data, uint16_t length);


#endif /* CRC16_CCITT_H_ */
