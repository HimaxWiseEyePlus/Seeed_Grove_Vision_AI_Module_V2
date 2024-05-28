/**************************************************************************************************
 (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
 ------------------------------------------------------------------------
 File        : main.c
 Project     : EPII
 DATE        : 2021/04/01
 AUTHOR      : 902447
 BRIFE       : main function
 HISTORY     : Initial version - 2021/04/01 created by Jacky
 **************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"
#ifdef LIB_COMMON
#include "xprintf.h"
#include "console_io.h"
#endif
#include "pinmux_init.h"
#include "platform_driver_init.h"

#define CIS_XSHUT_SGPIO0

#ifdef HM_COMMON
#include "hx_drv_CIS_common.h"
#endif
#if defined(CIS_HM0360_MONO_REVB) || defined(CIS_HM0360_MONO_OSC_REVB) \
	|| defined(CIS_HM0360_BAYER_REVB) || defined(CIS_HM0360_BAYER_OSC_REVB) \
	||  defined(CIS_HM0360_MONO) || defined(CIS_HM0360_BAYER)
#include "hx_drv_hm0360.h"
#endif
#ifdef CIS_HM11B1
#include "hx_drv_hm11b1.h"
#endif
#if defined(CIS_HM01B0_MONO) || defined(CIS_HM01B0_BAYER)
#include "hx_drv_hm01b0.h"
#endif
#ifdef CIS_HM2140
#include "hx_drv_hm2140.h"
#endif
#ifdef CIS_XSHUT_SGPIO0
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2 
#else
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2 
#endif


#ifdef ALLON_SENSOR_TFLM
#include "allon_sensor_tflm.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef ALLON_SENSOR_TFLM_FATFS
#include "allon_sensor_tflm_fatfs.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef ALLON_SENSOR_TFLM_FREERTOS
#include "allon_sensor_tflm.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef HELLO_WORLD_FREERTOS_TZ_S_ONLY
#include "hello_world_freertos_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef TFLM_FD_FM
#include "tflm_fd_fm.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef FATFS_TEST
#include "fatfs_test.h"

/** main entry */
int main(void)
{
	board_init();
	fatfs_test();
	return 0;
}
#endif


#ifdef TFLM_YOLOV8_OD
#include "tflm_yolov8_od.h"

/** main entry */
int main(void)
{
	board_init();
	tflm_yolov8_od_app();
	return 0;
}
#endif


#ifdef TFLM_YOLOV8_POSE
#include "tflm_yolov8_pose.h"

/** main entry */
int main(void)
{
	board_init();
	tflm_yolov8_pose_app();
	return 0;
}
#endif


#ifdef TFLM_2IN1_FD_FL_FR_ENROLL_YOLOV8
#include "tflm_2in1_fd_fl_fr_enroll_yolov8.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef PDM_RECORD
#include "pdm_record.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef IMU_READ_APP
#include "imu_read_app.h"

/* main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef HELLO_WORLD_CMSIS_DSP
#include "hello_world_cmsis_dsp.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif

