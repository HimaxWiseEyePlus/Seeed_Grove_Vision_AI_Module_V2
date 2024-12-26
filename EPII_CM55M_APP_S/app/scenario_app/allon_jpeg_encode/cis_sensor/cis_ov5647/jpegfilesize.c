/*
 * jpegfilesize.c
 *
 *  Created on: 2024/12/24
 *      Author: 902447
 */

#include <stdio.h>
#include "WE2_device.h"
#include "board.h"
#include "WE2_core.h"
#include "cisdp_cfg.h"

#define JPEG_HEADER_BUFSIZE 36*4
__attribute__(( section(".bss.NoInit"))) uint8_t jpegfilesizebuf[JPEG_HEADER_BUFSIZE] __ALIGNED(32);





