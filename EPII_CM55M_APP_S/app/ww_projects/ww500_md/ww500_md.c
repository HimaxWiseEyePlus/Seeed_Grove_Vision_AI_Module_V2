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
#include "app_msg.h"
#include "ww500_md.h"
#include "hx_drv_CIS_common.h"
#include "cisdp_sensor.h"

#ifdef USE_HM0360
#include "hm0360_regs.h"
#endif	// USE_HM0360

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

// Flash time at reset
#define LED_DELAY						50

/*************************************** External variables *******************************************/

extern QueueHandle_t     xIfTaskQueue;
extern QueueHandle_t     xImageTaskQueue;

/*************************************** Local variables *******************************************/

internal_state_t internalStates[NUMBEROFTASKS];

static char versionString[64]; // Make sure the buffer is large enough

/*************************************** Local routine prototypes  *************************************/

static void pinmux_init(void);
static void initVersionString(void);
static void ledInit(void);
static void showResetOnLeds(uint8_t numFlashes);

/*************************************** Local routine definitions  *************************************/

/**
 * Initialise GPIO pins for this application
 * // TODO move PA0 configuration here, from if_task.c aon_gpio0_interrupt_init()
 *
 * NOTE: there is a weak version of pinmux_init() in board/epii_evb/pinmux_init.c
 * that just initialises PB0 and PB1 for UART.
 */
static void pinmux_init(void) {
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

	// TODO - properly intergrate these in the pattern above
	// Activate green and blue LEDs for user feedback
	ledInit();

}

/**
 * Initialise GPIO pins so they can be used to indicate activity during testing
 *
 * 	PB9  = LED3 (green), SENSOR_GPIO (connects to a normally n/c pin on the sensor connector)
 *  PB10 = LED2 (blue),  SENSOR_ENABLE (normally the RP camera enable signal)
 */
static void ledInit(void) {

	// PB9 = LED3 (green), SENSOR_GPIO (connects to a normally n/c pin on the sensor connector)
    hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_GPIO0, 1);
	hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_LOW);

	// PB10 = LED2(blue), SENSOR_ENABLE
	// This is normally the camera enable signal (active high) so would not normally be an LED output!
    hx_drv_gpio_set_output(GPIO1, GPIO_OUT_LOW);
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_GPIO1, 1);
	hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_LOW);
}

/**
 * Flash a distinctive pattern on all LEDs at reset,
 * to show life
 *
 * TODO - use the flash timer mechanism!!!
 */
static void showResetOnLeds(uint8_t numFlashes) {

    for (uint8_t i=0; i < numFlashes; i++) {

    	app_ledGreen(true);
    	hx_drv_timer_cm55s_delay_ms(LED_DELAY, TIMER_STATE_DC);
    	app_ledBlue(true);
    	hx_drv_timer_cm55s_delay_ms(LED_DELAY, TIMER_STATE_DC);

    	app_ledGreen(false);
    	hx_drv_timer_cm55s_delay_ms(LED_DELAY, TIMER_STATE_DC);
    	app_ledBlue(false);
    	hx_drv_timer_cm55s_delay_ms(LED_DELAY, TIMER_STATE_DC);
    }
}

static void initVersionString(void) {
    snprintf(versionString, sizeof(versionString), "%s %s",__TIME__, __DATE__);
}

/**
 * Callback when all tasks have been inactive for a period
 *
 * Send a messages to whatever state machines need to know:
 *
 *  - Inform the Image Task so it can ask the FatFS task to save state, and set the
 *  	HM0360 into motion detect mode, then wait for the IF Task to complete
 *
 *  - Inform the If Task to send a final "Sleep" message to the BE processor.
 *  	It then gives a semaphore so the image task can enter DPD
 */
void app_onInactivityDetection(void) {
	APP_MSG_T send_msg;

	XP_LT_GREEN;
	xprintf("Inactive for %dms\n", fatfs_getOperationalParameter(OP_PARAMETER_INTERVAL_BEFORE_DPD));
	XP_WHITE;

	send_msg.msg_data = 0;
	send_msg.msg_parameter = 0;
	send_msg.msg_event = APP_MSG_IMAGETASK_INACTIVITY;

	// TODO - RECONSIDER SEQUENCE TO AVOID RACE CONDITIONS

	// Send a message to the image state machine to start the motion detection mechanism
	if (xQueueSend(xImageTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("send_msg=0x%x fail\r\n", send_msg.msg_event);
	}

	send_msg.msg_event = APP_MSG_IFTASK_INACTIVITY;
	// tell the interface task to send a message to the BLE processor then shut down
	if (xQueueSend(xIfTaskQueue, (void *)&send_msg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("send_msg=0x%x fail\r\n", send_msg.msg_event);
	}

	// TODO - can I send the APP_MSG_FATFSTASK_SAVE_STATE message to the fatfs task from here?
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


/**
 * activates the green LED
 * This is on PB9
 */
void app_ledGreen(bool on) {
	if (on) {
		hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_HIGH);
	}
	else {
		hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_LOW);
	}
}
/**
 * activates the blue LED
 * This is on PB10
 */
void app_ledBlue(bool on) {
	if (on) {
		hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_HIGH);
	}
	else {
		hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_LOW);
	}
}


/*************************************** Main()  *************************************/

/*!
 * @brief Main function
 *
 * Called from main.c
 */
int app_main(void){
	uint32_t wakeup_event;
	uint32_t wakeup_event1;
	APP_WAKE_REASON_E wakeReason;

	rtc_time time = {0};
	char timeString[UTCSTRINGLENGTH];

	UBaseType_t priority;
	TaskHandle_t task_id;
	internal_state_t internalState;
	uint8_t taskIndex = 0;

#ifdef USE_HM0360
	uint8_t hm0360_interrupt_status;
#endif	// USE_HM0360

	initVersionString();

	pinmux_init();

	app_ledGreen(false);	// On to show camera activity
	app_ledBlue(true);		// On to show processor is active (not in DPD)

	XP_YELLOW;
	xprintf("\n**** WW500 MD. (%s) Built: %s %s ****\r\n\n", app_get_board_name_string(), __TIME__, __DATE__);
	XP_WHITE;

	//	// We seem to have version D. Note that chipid & version both report 8536000d
	//	hx_drv_scu_get_version(&chipid, &version);
	//	xprintf("ChipID: 0x%08x, version 0x%08x\r\n", chipid, version);

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

	XP_CYAN;
	sleep_mode_print_event(wakeup_event, wakeup_event1);	// print descriptive string
	XP_WHITE;

#ifdef USE_HM0360
#pragma message "Compiling for HM0360"
	xprintf("Camera: HM0360\n");
#else
#pragma message "Compiling for IMX219"
	xprintf("Camera: RP v2 (IMX219)\n");
#endif	// USE_HM0360

	if (configUSE_TICKLESS_IDLE) {
		xprintf("FreeRTOS tickless idle is enabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
	}
	else {
		XP_RED;
		xprintf("FreeRTOS tickless idle is disabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
		XP_WHITE;
	}

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		showResetOnLeds(3);	// pattern on LEDs to show reset

		XP_LT_BLUE;
		xprintf("\n### Cold Boot ###\n");
		XP_WHITE;
		wakeReason = APP_WAKE_REASON_COLD;

		// Initialises clock and sets a time to be going on with...
		exif_utc_init("2025-01-01T00:00:00Z");
	}
	else {
		XP_LT_GREEN;
		xprintf("### Warm Boot ###\n");
		XP_WHITE;

		// We need to clear the RTC interrupt or it will trigger immediately
		hx_drv_rtc_clear_alarm_int_status(RTC_ID_0);

		// Call when exiting DPD
		exif_utc_clk_enable();

		//exif_utc_get_rtc_as_time_dpd(&time);	// This takes time! c. 900ms
		exif_utc_get_rtc_as_time(&time);
		exif_utc_time_to_exif_string(&time, timeString, sizeof(timeString));

		xprintf("Woke at %s: \n", timeString);

		// Determine the cause of the wakeup by reading the HM0360 HM0360_INT_INDC_REG register
		// set I2C clock to 100K Hz
		// Otherwise I2C speed is initialised in platform_driver_init() as DW_IIC_SPEED_FAST = 400kHz
		hx_drv_i2cm_init(USE_DW_IIC_1, HX_I2C_HOST_MST_1_BASE, DW_IIC_SPEED_STANDARD);

#ifdef USE_HM0360
		cisdp_sensor_get_int_status(&hm0360_interrupt_status);
		cisdp_sensor_clear_interrupt(0xff);		// clear all bits

		XP_YELLOW;
		if (wakeup_event1 == PMU_WAKEUPEVENT1_DPD_PAD_AON_GPIO_0) {
			// The WAKE pin has woken us...
			if ((hm0360_interrupt_status & MD_INT) == MD_INT) {
				xprintf("Motion detected INT_INDIC = 0x%02x\n", hm0360_interrupt_status);
				wakeReason = APP_WAKE_REASON_MD;
			}
			else {
				xprintf("BLE wake\n");
				wakeReason = APP_WAKE_REASON_BLE;
			}
		}
		else if (wakeup_event == PMU_WAKEUP_DPD_RTC_INT) {
			xprintf("Timer wake\n");
			wakeReason = APP_WAKE_REASON_BLE;
		}
		else {
			// else I don't know! Add more reason in the future
			wakeReason = APP_WAKE_REASON_UNKNOWN;
		}
		XP_WHITE;
#else
		XP_YELLOW;
		if (wakeup_event1 == PMU_WAKEUPEVENT1_DPD_PAD_AON_GPIO_0) {
			xprintf("BLE wake\n");
			wakeReason = APP_WAKE_REASON_BLE;
		}
		else if (wakeup_event == PMU_WAKEUP_DPD_RTC_INT) {
			xprintf("Timer wake\n");
			wakeReason = APP_WAKE_REASON_BLE;
		}
		else {
			// else I don't know! Add more reason in the future
			wakeReason = APP_WAKE_REASON_UNKNOWN;
		}
		XP_WHITE;
#endif	// USE_HM0360
	}

	// Each task has its own file. Call these to do the task creation and initialisation
	// See here for task priorities:
	// https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/03-Task-priorities

	priority = configMAX_PRIORITIES;

	// Place highest priority task at the top. All will be allocated successively lower priorities

	// The CLI task implements a command line interface (CLI) for use in debugging.
	// This can be extended to manage incoming messages from other hardware (as well as the console UART)
	task_id = cli_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = cli_getState; // does not have states
	internalState.stateString = cli_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// ifTask handles communications between the Seeed board and the WW130
	task_id = ifTask_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = ifTask_getState;
	internalState.stateString = ifTask_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// This tasks provides a CLI interface to the FatFs
	task_id = fatfs_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = fatfs_getState;
	internalState.stateString = fatfs_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// Image task for camera init & image capture and processing
	task_id = image_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = image_getState;
	internalState.stateString = image_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	vTaskStartScheduler();

	for (;;) {
		// Should not get here...
	}
	return 0;
}
