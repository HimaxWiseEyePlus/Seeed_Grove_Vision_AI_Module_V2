/**
 * ww500_md.c
 *
 * Main entry point for the 'ww500_md' app.
 * Execution begins at 'app_main()'
 *
 */

/*************************************** Includes *******************************************/


// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "CLI-commands.h"
#include "exif_gps.h"
#include "exif_utc.h"

// FreeRTOS tasks:
#include "fatfs_task.h"
#include "if_task.h"
#include "image_task.h"

#include "inactivity.h"
#include "pinmux_cfg.h"
#include "printf_x.h"
#include "sleep_mode.h"

#include "xprintf.h"
#include "hx_drv_scu_export.h"
#include "hx_drv_scu.h"
#include "hx_drv_gpio.h"

#include "hx_drv_timer.h"
#include "exif_utc.h"

#include "hx_drv_pmu.h"
#include "ww500_md.h"

#ifdef TRUSTZONE_SEC

#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include "arm_cmse.h"
#ifdef NSC
#include "veneer_table.h"
#endif
/* Trustzone config. */

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif
#endif

/*************************************** Definitions *******************************************/


/*************************************** Local variables *******************************************/

internal_state_t internalStates[NUMBEROFTASKS];

static char versionString[64]; // Make sure the buffer is large enough
static bool coldBoot = false;

/*************************************** Local routine prototypes  *************************************/


/*************************************** Local routine definitions  *************************************/

/**
 * Initialise GPIO pins for this application
 * // TODO move PA0 configuration here, from if_task.c aon_gpio0_interrupt_init()
 *
 * NOTE: there is a weak version of pinmux_init() in board/epii_evb/pinmux_init.c
 * that just initialises PB0 and PB1 for UART.
 */
void pinmux_init(void)
{
	SCU_PINMUX_CFG_T pinmux_cfg;

	hx_drv_scu_get_all_pinmux_cfg(&pinmux_cfg);

	/* Init UART0 pin mux to PB0 and PB1 */
	uart0_pinmux_cfg(&pinmux_cfg);

#ifdef WW500
	// WW500 is defined in ww130_cli.h, but I should probably move this...
	// Init PB10 for sensor enable pin.
	// This differs from the Grove AI V2, in which sensor enable is PA1.
	// But I need PA1 to control the power supply switches
	sensor_enable_gpio1_pinmux_cfg(&pinmux_cfg);

#else
	/* Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin */
	aon_gpio1_pinmux_cfg(&pinmux_cfg);
#endif	// WW500

	/* Init I2C slave 0 pin mux to PA2, PA3 (SCL, SDA)*/
	i2cs0_pinmux_cfg(&pinmux_cfg);

	/* Init SPI master pin mux */
	spi_m_pinmux_cfg(&pinmux_cfg);

	// PA0 configuration for inter-board communications

	/* Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)*/
	// swd_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}

void initVersionString(void) {
    snprintf(versionString, sizeof(versionString), "%s %s",__TIME__, __DATE__);
}


/*************************************** Public function definitions *************************************/

// So the version can be reported
char * app_get_version_string(void) {
	return versionString;
}

char * app_get_board_name_string(void) {
	static char * boardString = BOARD_NAME_STRING;
	return boardString;
}

/*************************************** Main()  *************************************/

/*!
 * @brief Main function
 */
int app_main(void){
//	uint32_t chipid;
//	uint32_t version;
	uint32_t wakeup_event;
	uint32_t wakeup_event1;

	rtc_time time = {0};
	char timeString[UTCSTRINGLENGTH];

	UBaseType_t priority;
	TaskHandle_t task_id;
	internal_state_t internalState;
	uint8_t taskIndex = 0;

	initVersionString();

	pinmux_init();

	XP_YELLOW;
	xprintf("\n**** WW500 MD. (%s) Built: %s %s ****\r\n\n", app_get_board_name_string(), __TIME__, __DATE__);
	XP_WHITE;

//	// We seem to have version D. Note that chipid & version both report 8536000d
//	hx_drv_scu_get_version(&chipid, &version);
//	xprintf("ChipID: 0x%08x, version 0x%08x\r\n", chipid, version);

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

	XP_CYAN;
    xprintf("Wakeup_event=0x%04x, WakeupEvt1=0x%04x\n", wakeup_event, wakeup_event1);
    print_wakeup_event(wakeup_event, wakeup_event1);	// print descriptive string
    XP_WHITE;

	if (configUSE_TICKLESS_IDLE) {
		xprintf("FreeRTOS tickless idle is enabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
	}
	else {
		XP_RED;
		xprintf("FreeRTOS tickless idle is disabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
		XP_WHITE;
	}

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		XP_LT_BLUE;
		xprintf("\n### Cold Boot ###\n");
		XP_WHITE;
		coldBoot = true;

		// Initialises clock and sets a time to be going on with...
		exif_utc_init("2025-01-01T00:00:00Z");
	}
	else {
		XP_LT_GREEN;
		xprintf("### Warm Boot ###\n");
		XP_WHITE;
		coldBoot = false;

		// Call when exiting DPD
		exif_utc_clk_enable();

		//exif_utc_get_rtc_as_time_dpd(&time);	// This takes time! c. 900ms
		exif_utc_get_rtc_as_time(&time);
		exif_utc_time_to_utc_string(&time, timeString, sizeof(timeString));
		xprintf("Woke at %s\n", timeString);
	}

	// Each task has its own file. Call these to do the task creation and initialisation
	// See here for task priorities:
	// https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/03-Task-priorities

	priority = configMAX_PRIORITIES;

	// Place highest priority task at the top. All will be allocated successively lower priorities

	// The CLI task implements a command line interface (CLI) for use in debugging.
	// This can be extended to manage incoming messages from other hardware (as well as the console UART)
	task_id = cli_createCLITask(--priority);
	internalState.task_id = task_id;
	internalState.getState = cli_getState; // does not have states
	internalState.stateString = cli_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// ifTask handles communications between the Seeed board and the WW130
	task_id = ifTask_createTask(--priority);
	internalState.task_id = task_id;
	internalState.getState = ifTask_getState;
	internalState.stateString = ifTask_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// This tasks provides a CLI interface to the FatFs
	task_id = fatfs_createTask(--priority);
	internalState.task_id = task_id;
	internalState.getState = fatfs_getState;
	internalState.stateString = fatfs_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// Image task for camera init & image capture and processing
	task_id = image_createTask(--priority, coldBoot);
	internalState.task_id = task_id;
	internalState.getState = image_getState;
	internalState.stateString = image_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// for now, 30s
	inactivity_init(30000, image_hackInactive);

	vTaskStartScheduler();

	for (;;) {
		// Should not get here...
	}
	return 0;
}
