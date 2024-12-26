#include <stdio.h>
#include "WE2_device.h"
#include "board.h"
#include "WE2_core.h"
#include "cisdp_cfg.h"

__attribute__(( section(".bss.NoInit"))) uint8_t raw_buff[(int)1280*960] __ALIGNED(32);
__attribute__(( section(".bss.NoIni2"))) uint8_t quater_raw_buff[(int)640*480] __ALIGNED(32);
//YUV420 x4 Compress = ((623+ (W/16)*(H/16)* 96 + 35) >>2 ) <<2  byte, W=640 H =480
__attribute__(( section(".bss.NoInit"))) uint8_t jpg_buff[(int)115856] __ALIGNED(32);
//YUV420 data
__attribute__(( section(".bss.NoInit"))) uint8_t yuv_buff[460800] __ALIGNED(32);
