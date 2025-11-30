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

#include "hm0360_md.h"
#include "hm0360_regs.h"

#include "barrier.h"
#include "selfTest.h"

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

//#define INCLUDETIMERTASK
#ifdef INCLUDETIMERTASK
#include "timer_task.h"
#endif // INCLUDETIMERTASK

#ifdef WW500_C00
// defined in ww.mk
#include "pca9574.h"
#include "ledFlash.h"
#endif // WW500_C00


/*************************************** Definitions *******************************************/

// Flash time at reset
#define LED_DELAY						50

// Uncomment this to print linker stats
//#define PRINTLINKERSTATS

/*************************************** External variables *******************************************/

extern QueueHandle_t     xIfTaskQueue;
extern QueueHandle_t     xImageTaskQueue;

// This will be available to all of the tasks:
Barrier_t startupBarrier;

/*************************************** Local variables *******************************************/

internal_state_t internalStates[NUMBEROFTASKS];

static char versionString[64]; // Make sure the buffer is large enough

#ifdef USE_HM0360_MD
bool hm0360Present = false;
#endif // USE_HM0360_MD

/*************************************** Local routine prototypes  *************************************/

static void pinmux_init(void);
static void initVersionString(void);
static void ledInit(void);
static void showResetOnLeds(uint8_t numFlashes);
static void checkForCameras(void);

#ifdef PRINTLINKERSTATS
static void printLinkerStats(void);
#endif // PRINTLINKERSTATS

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
	// WW500 is defined in ww.mk
	// Init PB10 for sensor enable pin.
	// This differs from the Grove AI V2, in which sensor enable is PA1.
	// But I need PA1 to control the power supply switches

	// later: pretty sure this is not required as it is done by rp_sensor_enable()
	//rp_sensor_enable_gpio1_pinmux_cfg(&pinmux_cfg);
#else
	// For Seeed Grove Vision AI V2 only
	// Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin
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

//#ifdef WW500
//	// Sets initial state of the SENSOR_ENABLE signal
//	sensor_enable(false);
//#endif // WW500
}

/**
 * Initialise GPIO pins so they can be used to indicate activity during testing
 *
 * 	PB9  = LED3 (green), SENSOR_GPIO (connects to a normally n/c pin on the sensor connector)
 *  PB10 = LED2 (blue),  SENSOR_ENABLE (normally the RP camera enable signal)
 */
static void ledInit(void) {

#ifdef PB9ISLEDGREEN
	// PB9 = LED3 (green), SENSOR_GPIO (connects to a normally n/c pin on the sensor connector)
    hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_GPIO0, 1);
	hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_LOW);
#endif // PB9ISLEDGREEN

#ifdef PB10ISLEDBLUE
	// PB10 = LED2(blue), SENSOR_ENABLE
	// This is normally the camera enable signal (active high) so would not normally be an LED output!
    hx_drv_gpio_set_output(GPIO1, GPIO_OUT_LOW);
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_GPIO1, 1);
	hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_LOW);
#endif //PB10ISLEDBLUE
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

/**
 * See which I2C devices respond
 *
 * HM0360 should be at 0x24 but might be at 25, 34, 35
 * RP v2 should be at 0x10
 * RP v3 should be at 0x1a
 *
 * WARNING: this may fail for cameras which rely on the SENSOR_ENABLE being on.
 */
static void checkForCameras(void) {

#if defined (USE_RP2) || defined (USE_RP3)
	// Only needed if using a RP camera
	rp_sensor_enable(true);
#endif

 	// Can't use vTaskDelay() since FreeRTOS scheduler has not started yet
    hx_drv_timer_cm55x_delay_ms(CIS_POWERUP_DELAY, TIMER_STATE_DC);

	// This should be called in platform_driver_init(), called by board_init(), in main() before app_main()
	// But Himax wrote:
	// After HM0360 enters the motion detection I2C low-speed mode, WE2 needs to change the I2C master clock to 100K hz low-speed mode to avoid HM0360 register reading errors.
	hx_drv_i2cm_init(USE_DW_IIC_1, HX_I2C_HOST_MST_1_BASE, DW_IIC_SPEED_STANDARD);

	XP_LT_GREY;

#ifdef USE_HM0360_MD
	// Test for the HM0360, if it is in use for motion detection
	hm0360Present = hm0360_md_isSensorPresent(HM0360_SENSOR_I2CID);
	if (hm0360Present) {
		xprintf("HM0360 present at 0x%02x\n", HM0360_SENSOR_I2CID);
	}
	else {
		xprintf("HM0360 not present at 0x%02x\n",  HM0360_SENSOR_I2CID);
		// expect a driver error message as well...

		selfTest_setErrorBits(1 << SELF_TEST_AI_NO_MD);
	}
#endif // USE_HM0360_MD

	// Test for the main camera, whose address is managed in the cis_sensor folders
	if (hm0360_md_isSensorPresent(CIS_I2C_ID)) {
		xprintf("Main camera present at 0x%02x\n", CIS_I2C_ID);
	}
	else {
		xprintf("Main camera not present at 0x%02x\n",  CIS_I2C_ID);
		// expect a driver error message as well...
		selfTest_setErrorBits(1 << SELF_TEST_AI_NO_CAM);
	}

#ifdef WW500_C00
	// Test for the I2C extender
	if (hm0360_md_isSensorPresent(PCA9574_I2C_ADDRESS_0)) {
		xprintf("PCA9574 present at 0x%02x\n", PCA9574_I2C_ADDRESS_0);
		//pca9574_readWriteTests(PCA9574_I2C_ADDRESS_0);
	}
	else {
		xprintf("PCA9574 not present at 0x%02x\n", PCA9574_I2C_ADDRESS_0);
		// expect a driver error message as well...

		selfTest_setErrorBits(1 << SELF_TEST_AI_NO_FLASH);
	}
#endif // WW500_C00

	XP_WHITE;
	// Disable even if not using RP camera so this pin is set to output, 0
 	rp_sensor_enable(false);	// Negate SENSOR_ENABLE

}

/**
 *
 */
static void initVersionString(void) {
    snprintf(versionString, sizeof(versionString), "%s %s",__TIME__, __DATE__);
}


#ifdef PRINTLINKERSTATS
/**
 *
 */
extern uint32_t __RAM_Start;
extern uint32_t __RAM_End;
extern uint32_t __RAM_Size;

extern uint32_t __Rodata_Start;
extern uint32_t __Rodata_End;
extern uint32_t __Rodata_Size;

extern uint32_t __CopyTable_Start;
extern uint32_t __CopyTable_End;
extern uint32_t __CopyTable_Size;

extern uint32_t __ZeroTable_Start;
extern uint32_t __ZeroTable_End;
extern uint32_t __ZeroTable_Size;

extern uint32_t __ARM_Extab_Size;
extern uint32_t __ARM_Exidx_Size;

extern uint32_t __PrivilegedData_Start;
extern uint32_t __PrivilegedData_End;
extern uint32_t __PrivilegedData_Size;
extern uint32_t __PrivilegedSRAM_Start;
extern uint32_t __PrivilegedSRAM_End;

extern uint32_t __Data_Start;
extern uint32_t __Data_End;
extern uint32_t __Data_Size;

extern uint32_t __BSS_Start;
extern uint32_t __BSS_End;
extern uint32_t __BSS_Size;

extern uint32_t __start_noinit_SRAM;
extern uint32_t __end_noinit_SRAM;
extern uint32_t __noinit_SRAM_Size;

extern uint32_t __HeapSize;
extern uint32_t __StackSize;
extern uint32_t __HeapStackMargin;

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;
extern uint32_t __StackTop;
extern uint32_t __StackLimit;

/**
 * Prints values determined by the linker.
 *
 * We can run this with different compilers to understand linker error.
 *
 * For 14.3:
 * Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      154872 B       256 KB     59.08%
CM55M_S_APP_DATA:      137688 B       256 KB     52.52%
    CM55M_S_SRAM:      983072 B      1924 KB     49.90%
***** Linker Stats *****
RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x3000df38 (57144 bytes)
.copy.table     0x10025ce4-0x10025cf0 (12 bytes)
.zero.table     0x10025cf0-0x10025cf8 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x3000df38-0x3000df37 (4294967295 bytes)
Privileged SRAM 0x3000df40-0x3000df3f
Data            0x3000df38-0x3000f780 (6216 bytes)
BSS             0x3000f780-0x300219d8 (74328 bytes)
Heap Base:      0x300219d8
Heap Limit:     0x3002b9d8
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 42536 bytes

With 10.3 compiler:
Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      140368 B       256 KB     53.55%
CM55M_S_APP_DATA:      110792 B       256 KB     42.26%
    CM55M_S_SRAM:      983296 B      1924 KB     49.91%
***** Linker Stats *****
RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x30007396 (29590 bytes)
.copy.table     0x1002243c-0x10022448 (12 bytes)
.zero.table     0x10022448-0x10022450 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x30007398-0x30007397 (4294967295 bytes)
Privileged SRAM 0x300073a0-0x3000739f
Data            0x30007398-0x30008d10 (6520 bytes)
BSS             0x30008d10-0x3001b0c8 (74680 bytes)
Heap Base:      0x3001b0c8
Heap Limit:     0x300250c8
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 69432 bytes

 */
static void printLinkerStats(void) {
	XP_LT_CYAN;
	xprintf("***** Linker Stats *****\n");
    xprintf("RAM 		0x%08x-0x%08x (%lu bytes)\n",
    		(unsigned long)&__RAM_Start, (unsigned long)&__RAM_End, (unsigned long)&__RAM_Size);
	/* .rodata diagnostics */
    xprintf(".rodata 0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__Rodata_Start, (unsigned long)&__Rodata_End, (unsigned long)&__Rodata_Size);
	/* .CopyTable diagnostics */
    xprintf(".copy.table 	0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__CopyTable_Start, (unsigned long)&__CopyTable_End, (unsigned long)&__CopyTable_Size);
	/* .ZeroTable diagnostics */
    xprintf(".zero.table 	0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__ZeroTable_Start, (unsigned long)&__ZeroTable_End, (unsigned long)&__ZeroTable_Size);
	/* Exception table diagnostics */
	xprintf(".ARM.extab_Size: 0x%08x\n", (unsigned long)&__ARM_Extab_Size);
	xprintf(".ARM.exidx_Size: 0x%08x\n", (unsigned long)&__ARM_Exidx_Size);
	/* .PrivilegedData diagnostics */
    xprintf("PrivilegedData	0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__PrivilegedData_Start, (unsigned long)&__PrivilegedData_End, (unsigned long)&__PrivilegedData_Size);
    xprintf("Privileged SRAM 0x%08x-0x%08x\n",
    	    (unsigned long)&__PrivilegedSRAM_Start, (unsigned long)&__PrivilegedSRAM_End);

    xprintf("Data 		0x%08x-0x%08x (%lu bytes)\n",
    		(unsigned long)&__Data_Start, (unsigned long)&__Data_End, (unsigned long)&__Data_Size);
    xprintf("BSS 		0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__BSS_Start, (unsigned long)&__BSS_End, (unsigned long)&__BSS_Size);
    xprintf("No init SRAM 		0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__start_noinit_SRAM, (unsigned long)&__end_noinit_SRAM, (unsigned long)&__noinit_SRAM_Size);

	xprintf("Heap Base: 	0x%08x\n", (unsigned long)&__HeapBase);
	xprintf("Heap Limit: 	0x%08x\n", (unsigned long)&__HeapLimit);
	xprintf("Stack Top: 	0x%08x\n", (unsigned long)&__StackTop);
	xprintf("Stack Limit: 	0x%08x\n", (unsigned long)&__StackLimit);

	xprintf("Heap size: 	%lu bytes\n", (unsigned long)&__HeapSize);
	xprintf("Stack size: 	%lu bytes\n", (unsigned long)&__StackSize);
	xprintf("Heap/Stack margin: %lu bytes\n\n", (unsigned long)&__HeapStackMargin);
	XP_WHITE;
}
#endif // PRINTLINKERSTATS


/*************************************** Public function definitions *************************************/

/**
 * Callback when all tasks have been inactive for a period
 *
 * Send a messages to whatever state machines need to know:
 *
 *  - Inform the Image Task so it can ask the FatFS task to save state, and set the
 *  	HM0360 into motion detect mode, then wait for the IF Task to complete
 *
 *  - Inform the If Task to send a final "Sleep" message to the BLE processor.
 *  	It then gives a semaphore so the image task can enter DPD
 */
void app_onInactivityDetection(void) {
	APP_MSG_T send_msg;

	XP_LT_GREEN;
	xprintf("Inactive for %dms\n", inactivity_getPeriod());
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
#ifdef PB9ISLEDGREEN
	if (on) {
		hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_HIGH);
	}
	else {
		hx_drv_gpio_set_out_value(GPIO0, GPIO_OUT_LOW);
	}
#endif // PB9ISLEDGREEN
}
/**
 * activates the blue LED
 * This is on PB10
 */
void app_ledBlue(bool on) {
#ifdef PB10ISLEDBLUE
	if (on) {
		hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_HIGH);
	}
	else {
		hx_drv_gpio_set_out_value(GPIO1, GPIO_OUT_LOW);
	}
#endif // PB10ISLEDBLUE
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


#if defined(USE_HM0360) || defined(USE_HM0360_MD)
	// These are typically defined in the makefile:
	// USE_HM0360 define if the HM0360 is the main camera:
	//		APPL_DEFINES += -DUSE_HM0360
	// USE_HM0360_MD defined if the HM0360 is to be used for motion detection
	// 		APPL_DEFINES += -DUSE_HM0360_MD
	uint8_t hm0360_interrupt_status;
#endif	// USE_HM0360

	initVersionString();

	pinmux_init();

	selfTest_init();

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

	// set I2C clock to 100K Hz
	// Otherwise I2C speed is initialised in platform_driver_init() as DW_IIC_SPEED_FAST = 400kHz

	// This should be called with DW_IIC_SPEED_FAST = 400kHz in platform_driver_init(),
	// called by board_init(), in main() before app_main(). But Himax wrote:
	// After HM0360 enters the motion detection I2C low-speed mode, WE2 needs to change the I2C master clock to 100K hz low-speed mode to avoid HM0360 register reading errors.
	hx_drv_i2cm_init(USE_DW_IIC_1, HX_I2C_HOST_MST_1_BASE, DW_IIC_SPEED_STANDARD);
	// TODO consider restoring DW_IIC_SPEED_FAST

#ifdef USE_HM0360
#pragma message "Compiling for HM0360"
	xprintf("Camera: HM0360\n");
#elif defined (USE_RP2)
#pragma message "Compiling for IMX219"
	xprintf("Camera: RP v2 (IMX219)\n");
#elif defined (USE_RP3)
#pragma message "Compiling for IMX708"
	xprintf("Camera: RP v3 (IMX708)\n");
#else
#pragma message "Compiling for unknown camera"
	xprintf("Camera: Unknown\n");
#endif	// USE_HM0360

	checkForCameras();	// see which I2C devices respond

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		showResetOnLeds(3);	// pattern on LEDs to show cold boot

		XP_LT_BLUE;
		xprintf("\n### Cold Boot ###\n");
		XP_WHITE;
		wakeReason = APP_WAKE_REASON_COLD;
		if (configUSE_TICKLESS_IDLE) {
			xprintf("FreeRTOS tickless idle is enabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
		}
		else {
			XP_RED;
			xprintf("FreeRTOS tickless idle is disabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
			XP_WHITE;
		}
#ifdef configUSE_NEWLIB_REENTRANT
		// Guards against issues when using FreeRTOS and nano-lib?
		xprintf("configUSE_NEWLIB_REENTRANT is defined\n");
#endif	// configUSE_NEWLIB_REENTRANT

		// Initialises clock and sets a time to be going on with...
		// A date prior to 2025 flags "not set"
		exif_utc_init("2024-01-01T00:00:00Z");
#ifdef PRINTLINKERSTATS
		printLinkerStats();
#endif // PRINTLINKERSTATS
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
		// TODO sort out getting time efficiently after warm boot
		exif_utc_get_rtc_as_time(&time);
		exif_utc_time_to_exif_string(&time, timeString, sizeof(timeString));

		xprintf("Woke at %s \n", timeString);

#ifdef USE_HM0360_MD
		// Test for the HM0360 (to be used for motion detection)
		hm0360Present = hm0360_md_isSensorPresent(HM0360_SENSOR_I2CID);
#endif // USE_HM0360_MD

#if defined(USE_HM0360)
		// HM0360 is main camera
		hm0360_md_getInterruptStatus(&hm0360_interrupt_status);
		// Wait till image_task before clearing the interrupt as this allows simple measurement of latency
    	hm0360_md_disableInterrupt();	/// stop further MD activity

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
			wakeReason = APP_WAKE_REASON_TIMER;
		}
		else {
			// else I don't know! Add more reason in the future
			wakeReason = APP_WAKE_REASON_UNKNOWN;
		}
		XP_WHITE;

#elif defined(USE_HM0360_MD)
		// HM0360 is used for MD with a RP camera
		if (hm0360Present) {
			hm0360_md_getInterruptStatus(&hm0360_interrupt_status);
			// Wait till image_task before clearing the interrupt as this allows simple measurement of latency
	    	hm0360_md_disableInterrupt();	/// stop further MD activity
		}

		XP_YELLOW;
		if (wakeup_event1 == PMU_WAKEUPEVENT1_DPD_PAD_AON_GPIO_0) {
			// The WAKE pin has woken us...
			if (hm0360Present && ((hm0360_interrupt_status & MD_INT) == MD_INT)) {
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
			wakeReason = APP_WAKE_REASON_TIMER;
		}
		else {
			// else I don't know! Add more reason in the future
			wakeReason = APP_WAKE_REASON_UNKNOWN;
		}
		XP_WHITE;

#else
		// RP camera alone - no HM0360
		XP_YELLOW;
		if (wakeup_event1 == PMU_WAKEUPEVENT1_DPD_PAD_AON_GPIO_0) {
			xprintf("BLE wake\n");
			wakeReason = APP_WAKE_REASON_BLE;
		}
		else if (wakeup_event == PMU_WAKEUP_DPD_RTC_INT) {
			xprintf("Timer wake\n");
			wakeReason = APP_WAKE_REASON_TIMER;
		}
		else {
			// else I don't know! Add more reason in the future
			wakeReason = APP_WAKE_REASON_UNKNOWN;
		}
		XP_WHITE;
#endif	// USE_HM0360
	}

#ifdef WW500_C00
	// TODO remove this. It is present in CLI-commands.
	// Need to enable it based on the operational parameters setting.
		// The CLI 'flash n m" command allows testing
	if (ledFlashInit()) {
		xprintf("Initialised LED Flash\n");
	}
	else {
		xprintf("Can't initialise LED Flash\n");
	}
#endif // WW500_C00

	xprintf("Initialising FreeRTOS tasks\n");

	// Each task has its own file. Call these to do the task creation and initialisation
	// See here for task priorities:
	// https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/03-Task-priorities

	priority = configMAX_PRIORITIES;

	// Place highest priority task at the top. All will be allocated successively lower priorities

#ifdef INCLUDETIMERTASK
	// Simple task to do something at regular intervals, such as print the time
	task_id = timerTask_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = timerTask_getState;
	internalState.stateString = timerTask_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created task '%s' Priority %d\n", pcTaskGetName(task_id), priority);

#endif	// INCLUDETIMERTASK

	// The CLI task implements a command line interface (CLI) for use in debugging.
	// This can be extended to manage incoming messages from other hardware (as well as the console UART)
	task_id = cli_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = cli_getState; // does not have states
	internalState.stateString = cli_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created task '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// ifTask handles communications between the Seeed board and the WW130
	task_id = ifTask_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = ifTask_getState;
	internalState.stateString = ifTask_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created task '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// This tasks provides a CLI interface to the FatFs
	task_id = fatfs_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = fatfs_getState;
	internalState.stateString = fatfs_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created task '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// Image task for camera init & image capture and processing
	task_id = image_createTask(--priority, wakeReason);
	internalState.task_id = task_id;
	internalState.getState = image_getState;
	internalState.stateString = image_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created task '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// Now create a barrier entity so that a function is called when all tasks are ready in their
	// for(;;) loop
	barrier_init(&startupBarrier, taskIndex, ifTask_allTasksReady);

	xprintf("FreeRTOS scheduler started.\n");
	vTaskStartScheduler();

	for (;;) {
		// Should not get here...
	}
	return 0;
}
