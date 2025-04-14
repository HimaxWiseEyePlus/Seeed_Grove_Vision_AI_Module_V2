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
#include "task1.h"

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

/**
 * Placeholder for a real inactive function
 */
static void inactive(void) {

	XP_LT_GREEN;
	xprintf("Inactive\n");
	XP_WHITE;

	app_pmu_enter_dpd(false);	// Does not return
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
	uint32_t chipid;
	uint32_t version;
	uint32_t wakeup_event;
	uint32_t wakeup_event1;
	bool coldBoot = false;

	rtc_time time = {0};
	char timeString[UTCSTRINGLENGTH];

	UBaseType_t priority;
	TaskHandle_t task_id;
	internal_state_t internalState;
	uint8_t taskIndex = 0;

	initVersionString();

	pinmux_init();

// uncomment this to test all 3 LEDS during board bring-up
//#define TEST_3_LEDS

#ifdef WW500

#ifdef TEST_3_LEDS
	// Set up PB8, PB9, PB10 as GPIO outputs,
	// TODO this should move to some pinmux code

	uint8_t pinValue;

	// PB9 = LED3 (green), SENSOR_GPIO (connects to a normally n/c pin on the sensor connector)
    hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_GPIO0, 1);
	hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_HIGH);

	// This for testing:
	hx_drv_gpio_get_in_value(GPIO0, &pinValue);

	XP_LT_GREEN;
	xprintf("Set PB9 as an output, drive to 1 (GPIO0). Read back as %d\n", pinValue);
	XP_WHITE;

	// PB10 = LED2(blue), SENSOR_ENABLE
	// This is normally the camera enable signal (active high) so would not normally be an LED output!
    hx_drv_gpio_set_output(GPIO1, GPIO_OUT_LOW);
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_GPIO1, 1);
	hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_HIGH);

	// This for testing:
	hx_drv_gpio_get_in_value(GPIO1, &pinValue);

	XP_LT_GREEN;
	xprintf("Set PB10 as an output, drive to 1 (GPIO1). Read back as %d\n", pinValue);
	XP_WHITE;

	// PB11 = LED1 (red)
	// This is normally the inter-processor interrupt pin, so it would normally be an interrupt input,
	// not an LED output!
    hx_drv_gpio_set_output(GPIO2, GPIO_OUT_LOW);
    hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_GPIO2, 1);
	hx_drv_gpio_set_out_value(GPIO2, GPIO_OUT_HIGH);

	// This for testing:
	hx_drv_gpio_get_in_value(GPIO2, &pinValue);

	XP_LT_GREEN;
	xprintf("Set PB11 as an output, drive to 1 (GPIO2). Read back as %d\n", pinValue);
	XP_WHITE;

	hx_drv_timer_cm55s_delay_ms(500, TIMER_STATE_DC);

	XP_LT_GREEN;
	xprintf("500ms delay finished. Resetting (some) GPIO pins:\n");

	hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_LOW);
	hx_drv_gpio_get_in_value(GPIO0, &pinValue);
	xprintf("Set PB9 to 0. Read back as %d\n", pinValue);

	hx_drv_timer_cm55s_delay_ms(500, TIMER_STATE_DC);

	hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_LOW);
	hx_drv_gpio_get_in_value(GPIO1, &pinValue);
	xprintf("Set PB10 to 0. Read back as %d\n", pinValue);

	hx_drv_timer_cm55s_delay_ms(500, TIMER_STATE_DC);

	hx_drv_gpio_set_out_value(GPIO2, GPIO_OUT_LOW);
	hx_drv_gpio_get_in_value(GPIO2, &pinValue);
	xprintf("Set PB11 to 0. Read back as %d\n", pinValue);

	XP_WHITE;
#else
	// Only PB9 should be used as an LED

	// PB9 = LED3 (green), SENSOR_GPIO (connects to a normally n/c pin on the sensor connector)
    hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_GPIO0, 1);

	//flash a few times
	for (uint8_t i=0; i < 3; i++) {
		hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_HIGH);
		hx_drv_timer_cm55s_delay_ms(200, TIMER_STATE_DC);
		hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_LOW);
		hx_drv_timer_cm55s_delay_ms(200, TIMER_STATE_DC);
	}

#endif	// TEST_3_LEDS
#endif	// WW500

	XP_YELLOW;
	xprintf("\n**** WW500 MD. (%s) Built: %s %s ****\r\n\n", app_get_board_name_string(), __TIME__, __DATE__);
	XP_WHITE;

	// We seem to have version D. Note that chipid & version both report 8536000d
	hx_drv_scu_get_version(&chipid, &version);
	xprintf("ChipID: 0x%08x, version 0x%08x\r\n", chipid, version);
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
		coldBoot = false;

		// Initialises clock and sets a time to be going on with...
		exif_utc_init("2025-01-01T00:00:00Z");
		//exif_utc_init("2025-01-02T03:04:05Z");
	}
	else {
		XP_LT_GREEN;
		xprintf("### Warm Boot ###\n");
		XP_WHITE;
		coldBoot = true;

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

	// Task 1 is a placeholder task that does not do anything of significance
	// It receives messages from the CLI and uses these to change state
	task_id = task1_createTask1Task(--priority);
	internalState.task_id = task_id;
	internalState.getState = task1_getState;
	internalState.stateString = task1_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	inactivity_init(4000, inactive);

	vTaskStartScheduler();

	for (;;)
	{
		// Should not get here...
	}
}
