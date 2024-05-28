/*
 * board.h
 *
 *  Created on: 2021�~2��3��
 *      Author: 902447
 */

#ifndef BOARD_EPII_FPGA_BOARD_H_
#define BOARD_EPII_FPGA_BOARD_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "WE2_device.h"
#include "WE2_hardware_config.h"

#ifdef __cplusplus
extern "C" {
#endif

int board_init();

int board_deinit();

int board_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_EPII_FPGA_BOARD_H_ */
