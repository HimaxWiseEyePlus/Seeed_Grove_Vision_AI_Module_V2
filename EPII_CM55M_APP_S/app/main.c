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


/************************ ww_projects apps *****************/

#ifdef WW130_TEST
#include "ww130_test.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef I2C_SLAVE_APP
#include "i2c_slave_app.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef I2C_SLAVE_TEST
#include "i2c_slave_test.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef SEEED_SAMPLE
#include "seeed_sample.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif


#ifdef WW130_CLI
#include "ww130_cli.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif //WW130_CLI

#ifdef WW_TEMPLATE
#include "ww_template.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif // WW_TEMPLATE



#ifdef HELLO_WORLD_CMSIS_CV
#include "hello_world_cmsis_cv.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif

#ifdef EI_STANDALONE_INFERENCING
#include "ei_standalone_inferencing.h"

int main(void)
{
	board_init();
	ei_standalone_inferencing_app();

	return 0;
}
#endif

#ifdef EI_STANDALONE_INFERENCING_CAMERA
#include "ei_standalone_inferencing_camera.h"

int main(void)
{
	board_init();
	ei_standalone_inferencing_app();

	return 0;
}
#endif

#ifdef EDGE_IMPULSE_FIRMWARE
#include "edge_impulse_firmware.h"

int main(void)
{
	/*set pinmux init*/
	pinmux_init();
	/*platform driver init*/
	platform_driver_init();
#ifdef IP_uart
	console_setup(USE_DW_UART_0, UART_BAUDRATE_115200);
#endif
#ifdef LIB_COMMON
	xprintf_setup();
#endif
	edge_impulse_app();

	return 0;
}
#endif

#ifdef KWS_PDM_RECORD
#include "kws_pdm_record.h"

/* main entry */
int main(void)
{
	board_init();
	kws_pdm_record_app();
	return 0;
}
#endif


#ifdef TFLM_PEOPLENET
#include "tflm_peoplenet.h"
/** main entry */
int main(void)
{
	board_init();
	tflm_peoplenet_app();
	return 0;
}
#endif

