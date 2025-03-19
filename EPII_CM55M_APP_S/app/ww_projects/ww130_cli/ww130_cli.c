/**
 * ww_template.c
 *
 */

/*************************************** Includes *******************************************/

#include "ww130_cli.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// FreeRTOS tasks:
#include "task1.h"
#include "CLI-commands.h"
#include "if_task.h"
#include "fatfs_task.h"
#include "image_task.h"

#include "xprintf.h"
#include "printf_x.h"

// For pinmux_init()
#include "hx_drv_scu_export.h"
#include "hx_drv_scu.h"
#include "pinmux_cfg.h"
#include "hx_drv_gpio.h"

#include "hx_drv_timer.h"
#include "exif_gps.h"

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

#define CHECKGPS
//#define CHECKRTC

/*************************************** Local variables *******************************************/

internal_state_t internalStates[NUMBEROFTASKS];

/*************************************** Local routine prototypes  *************************************/

#ifdef CHECKGPS
static void check_GPS(void);
#endif // CHECKGPS

#ifdef CHECKRTC
static void check_RTC(void);
#endif // CHECKRTC

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

#ifdef CHECKGPS
/**
 * Runs some tests to chck the functionality o fthe GPS set/get
 *
 * NOTE: these tests do not extend to creating the EXIF data itself.
 * That process involves adding headers, tags etc....
 *
 * ChatGPT helped with this. See:
 * https://chatgpt.com/share/67d0aed1-2abc-8005-b2a4-2c755e47749a
 */
static void check_GPS(void) {
	extern GPS_Coordinate exif_gps_deviceLat;
	extern GPS_Coordinate exif_gps_deviceLon;
	extern GPS_Altitude exif_gps_deviceAlt;

	// Debug: test GPS parsing
	printf("\n");
	exif_gps_test_example_1(NULL, 0, NULL, 0, NULL);
    // This test should re-convert the coordinates to a string
    exif_gps_test_reconstruct(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt);
	printf("\n");

	exif_gps_test_example_1("37°48'30.50\"", 'N', "122°25'10.2\"", 'W', "20 Below");
    exif_gps_test_reconstruct(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt);
	printf("\n");

	exif_gps_test_example_2(NULL);
	printf("\n");
	exif_gps_test_example_2("36°49'55\" S 174°47'51.8\" E 31.234 Above");
    exif_gps_test_reconstruct(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt);
	printf("\n");
	exif_gps_test_example_2("36°49'55.68\" N 174°47'51.683\" W 31.2344 Below");
    exif_gps_test_reconstruct(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt);
	printf("\n");

	// Actual use will be like this: the app will send a string, and we want to get the various byte buffers:
	char * gpsString = "36°49'55\" S 174°47'51.8\" E 31.234 Above";

    uint8_t lat_buffer[26];	// 2 bytes for ref string then 6 x 32-bit words
    uint8_t lon_buffer[26];
    uint8_t alt_buffer[9];	// 1 byte for ref then 2 x 32-bit words

    printf("Parsing '%s'\n", gpsString);

    // Convert the string to GPS structures
	exif_gps_parse_full_string(&exif_gps_deviceLat, &exif_gps_deviceLon, &exif_gps_deviceAlt, gpsString);
	// Generate the byte arrays
    exif_gps_generate_byte_array(&exif_gps_deviceLat, lat_buffer);
    exif_gps_generate_byte_array(&exif_gps_deviceLon, lon_buffer);
    exif_gps_generate_altitude_byte_array(&exif_gps_deviceAlt, alt_buffer);

    // check the buffers:
    printf("Latitude Buffer:  ");
    for (size_t i = 0; i < sizeof(lat_buffer); i++) {
        printf("%02X ", lat_buffer[i]);
    }
    printf("\n");

    printf("Longitude Buffer: ");
    for (size_t i = 0; i < sizeof(lon_buffer); i++) {
        printf("%02X ", lon_buffer[i]);
    }
    printf("\n");

    printf("Altitude Buffer:  ");
    for (size_t i = 0; i < sizeof(alt_buffer); i++) {
        printf("%02X ", alt_buffer[i]);
    }
    printf("\n\n");
}
#endif	// CHECKGPS

#ifdef CHECKRTC

void app_clk_enable() {
	SCU_PDAON_CLKEN_CFG_T aonclken;

	aonclken.rtc0_clk_en = 1;/*!< RTC0 Clock enable */
	aonclken.rtc1_clk_en = 1;/*!< RTC1 Clock enable */
	aonclken.rtc2_clk_en = 1;/*!< RTC2 Clock enable */
	aonclken.pmu_clk_en = 1;/*!< PMU Clock enable */
	aonclken.aon_gpio_clk_en = 1;/*!< AON GPIO Clock enable */
	aonclken.aon_swreg_clk_en = 1;/*!< AON SW REG Clock enable */
	aonclken.antitamper_clk_en = 1;/*!< ANTI TAMPER Clock enable */
	hx_drv_scu_set_pdaon_clken_cfg(aonclken);
}

void app_clk_disable() {
	SCU_PDAON_CLKEN_CFG_T aonclken;

	aonclken.rtc0_clk_en = 1;/*!< RTC0 Clock enable */
	aonclken.rtc1_clk_en = 0;/*!< RTC1 Clock enable */
	aonclken.rtc2_clk_en = 0;/*!< RTC2 Clock enable */
	aonclken.pmu_clk_en = 1;/*!< PMU Clock enable */
	aonclken.aon_gpio_clk_en = 0;/*!< AON GPIO Clock enable */
	aonclken.aon_swreg_clk_en = 1;/*!< AON SW REG Clock enable */
	aonclken.antitamper_clk_en = 0;/*!< ANTI TAMPER Clock enable */
	hx_drv_scu_set_pdaon_clken_cfg(aonclken);
}

/**
 * DEBUG code to see if I can get the RTC to work.
 *
 * It looks like the clock runs and the hx_drv_rtc_read_time() returns plausible data,
 * In practise, the clock seems to run 16000x too fast
 *
 * Looks like we have to call hx_drv_rtc_enable() to get the RTC incrementing...
 *
 */
#if 0
static void check_RTC(void) {
	RTC_ERROR_E ret;
	rtc_time time;
	uint32_t timeCounter;

	ret = hx_drv_rtc_enable(RTC_ID_0, 1);

	if (ret == RTC_NO_ERROR) {
		uint32_t freq;
		hx_drv_rtc_get_clk(RTC_ID_0, &freq);
		xprintf("RTC 0 enabled. %d Hz\n", freq);
	}
	else {
		xprintf("Time 0 enable error%d\n", ret);
	}

	ret = hx_drv_rtc_enable(RTC_ID_1, 1);
	if (ret == RTC_NO_ERROR) {
		uint32_t freq;
		hx_drv_rtc_get_clk(RTC_ID_1, &freq);
		xprintf("RTC 1 enabled. %d Hz\n", freq);
	}
	else {
		xprintf("Time 1 enable error%d\n", ret);
	}

	ret = hx_drv_rtc_enable(RTC_ID_2, 1);
	if (ret == RTC_NO_ERROR) {
		uint32_t freq;
		hx_drv_rtc_get_clk(RTC_ID_2, &freq);
		xprintf("RTC 2 enabled. %d Hz\n", freq);
	}
	else {
		xprintf("Time 2 enable error%d\n", ret);
	}

	// I expect this would be a calendar time
	ret = hx_drv_rtc_read_time(RTC_ID_0, &time, RTC_TIME_AFTER_DPD_1ST_READ_NO);
	if (ret == RTC_NO_ERROR) {
		xprintf("Time might be: %d:%d:%d %d/%d/%d\n",
				time.tm_hour, time.tm_min, time.tm_sec,
				time.tm_mday, time.tm_mon, time.tm_year);
	}
	else {
		xprintf("Time error %d\n", ret);
	}

	// I expect this would be an integer
	ret = hx_drv_rtc_read_val(RTC_ID_0, &timeCounter, RTC_TIME_AFTER_DPD_1ST_READ_NO);
	if (ret == RTC_NO_ERROR) {
		xprintf("Time counter: %d\n", timeCounter);
	}
	else {
		xprintf("Time val error %d\n", ret);
	}

	// I expect this would be a calendar time
	hx_drv_rtc_cm55m_read_time(&time, RTC_TIME_AFTER_DPD_1ST_READ_NO);
	if (ret == RTC_NO_ERROR) {
		xprintf("CM55M Time might be: %d:%d:%d %d/%d/%d\n",
				time.tm_hour, time.tm_min, time.tm_sec,
				time.tm_mday, time.tm_mon, time.tm_year);
	}
	else {
		xprintf("Time CM55M error %d\n", ret);
	}
}

#else
static void check_RTC(void) {
	RTC_ERROR_E ret;
	rtc_time time;
	uint32_t timeCounter;

	rtc_time tm;

	ret = hx_drv_rtc_enable(RTC_ID_0, 1);

	// Set a default time and date (1/1/25 0:0:0)
	tm.tm_year = 2025;
	tm.tm_mon = 1;
	tm.tm_mday = 1;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;

	RTC_SetTime(&tm);

	app_clk_enable();
		RTC_GetTime(&tm);
		xprintf("RTC GetTime : %d/%02d/%02d %02d:%02d:%02d\r\n",
			tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}

#endif // 0

#endif	// CHECKRTC

/*************************************** Main()  *************************************/

/*!
 * @brief Main function
 */
int app_main(void){
	uint32_t chipid;
	uint32_t version;

	UBaseType_t priority;
	TaskHandle_t task_id;
	internal_state_t internalState;
	uint8_t taskIndex = 0;

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
	xprintf("\n**** WW130 CLI. Built: %s %s ****\r\n\n", __TIME__, __DATE__);
	XP_WHITE;

	// We seem to have version D. Note that chipid & version both report 8536000d
	hx_drv_scu_get_version(&chipid, &version);
	xprintf("ChipID: 0x%08x, version 0x%08x\r\n", chipid, version);

	if (configUSE_TICKLESS_IDLE)
	{
		xprintf("FreeRTOS tickless idle is enabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
	}
	else
	{
		XP_RED;
		xprintf("FreeRTOS tickless idle is disabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
		XP_WHITE;
	}


#ifdef CHECKGPS
	check_GPS();	// test of GPS functions



#endif	// CHECKGPS


#ifdef CHECKRTC
	check_RTC();	// DEBUG: see if the RTC works
#endif	// CHECKRTC

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
	task_id = image_createTask(--priority);
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

	vTaskStartScheduler();

	for (;;)
	{
		// Should not get here...
	}
}
