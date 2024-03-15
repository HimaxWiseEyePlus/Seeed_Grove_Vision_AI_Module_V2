#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
//#include "veneer_table.h"
//
#endif
#endif

#include "WE2_device.h"
#include "board.h"
#include "xprintf.h"
#include "set_internal_pull.h"
#include "board.h"
#include "WE2_core.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_pdm_rx.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int app_main(void) {
    SCU_PAD_PULL_LIST_T pull_cfg;

    xprintf("Internal GPIO pull high/low example\n");

    hx_drv_scu_get_all_pull_cfg(&pull_cfg);

    //select dedicated pin to set high/low status
    //set PAD_AON_GPIO_0 as example to internal pull high
	pull_cfg.pa0.pull_en = SCU_PAD_PULL_EN;
    pull_cfg.pa0.pull_sel = SCU_PAD_PULL_UP;
				
	hx_drv_scu_set_all_pull_cfg(&pull_cfg);                


    xprintf("Internal GPIO pull high/low example done\n");


	return 0;
}