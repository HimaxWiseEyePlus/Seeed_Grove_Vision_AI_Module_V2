#include "ww500_md_test_1.h"

#include "WE2_debug.h"
#include "xprintf.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "driver_interface.h"
#ifdef IP_sensorctrl
#include "hx_drv_sensorctrl.h"
#endif
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#include "sensor_dp_lib.h"
#endif
#ifdef IP_cdm
#include "hx_drv_cdm.h"
#endif
#ifdef IP_edm
#include "hx_drv_edm.h"
#endif
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#ifdef IP_swreg_aon
#include "hx_drv_swreg_aon.h"
#endif
#include "hx_drv_pmu.h"
#include "powermode.h"
#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
#include "hx_drv_CIS_common.h"
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

#include "common_config.h"
#include "app_msg.h"
#include "app_state.h"
#include "dp_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "cvapp.h"
#include "sleep_mode.h"
#include "pinmux_cfg.h"

// CGP added these:
#include "spi_fatfs.h"
#include "cis_sensor/cis_hm0360/cisdp_sensor.h"
#include "printf_x.h"	// support colour
#include "hm0360_x.h"	// change motion detection parameters
#include "hm0360_regs.h"	// Some register names

// This duplicates defines in cisdp_cfg.h and dp_task.c
//#define CIS_XSHUT_SGPIO0
//#ifdef CIS_XSHUT_SGPIO0
//#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
//#else
//#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
//#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define dp_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define comm_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define main_task_PRIORITY	(configMAX_PRIORITIES - 2)
#define algo_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define DP_TASK_QUEUE_LEN   		10
#define COMM_TASK_QUEUE_LEN   		10
#define MAIN_TASK_QUEUE_LEN   		10
#define ALGO_TASK_QUEUE_LEN   		10
#define VAD_BUFF_SIZE  				2048

volatile APP_MAIN_TASK_STATE_E g_maintask_state = APP_MAIN_TASK_STATE_UNINIT;
volatile APP_ALGO_TASK_STATE_E g_algotask_state = APP_ALGO_TASK_STATE_UNINIT;
volatile APP_DP_TASK_STATE_E g_dptask_state = APP_DP_TASK_STATE_UNINIT;
volatile APP_COMM_TASK_STATE_E g_commtask_state = APP_COMM_TASK_STATE_UNINIT;

QueueHandle_t     xMainTaskQueue;
QueueHandle_t     xDPTaskQueue;
QueueHandle_t     xCommTaskQueue;
QueueHandle_t     xAlgoTaskQueue;

uint32_t g_algo_done_frame = 0;
uint32_t g_enter_pmu_frame_cnt = 0;

// If initialised as 0 there is no SD card, or a file system error
uint16_t imageSequenceNumber = 0;


uint8_t idh;
uint8_t idl;
uint8_t sil;
uint8_t fch;
uint8_t fcl;

// CGP returns false if no camera resent
extern bool app_start_state(CAMERA_CONFIG_E state);


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void main_task(void *pvParameters);
void pinmux_init();


/*******************************************************************************
 * Code
 ******************************************************************************/
void pinmux_init()
{
	SCU_PINMUX_CFG_T pinmux_cfg;

	hx_drv_scu_get_all_pinmux_cfg(&pinmux_cfg);

	/* Init UART0 pin mux to PB0 and PB1 */
	uart0_pinmux_cfg(&pinmux_cfg);

	/* Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin */
	aon_gpio1_pinmux_cfg(&pinmux_cfg);

	/* Init I2C slave 0 pin mux to PA2, PA3 (SCL, SDA)*/
	i2cs0_pinmux_cfg(&pinmux_cfg);

	/* Init SPI master pin mux */
	spi_m_pinmux_cfg(&pinmux_cfg);

	/* Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)*/
	//swd_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
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


/*!
 * @brief Main function
 *
 * Here for both cold and warm boots.
 *
 * This initialises the FreeRTOS tasks and starts the scheduler. It never returns.
 * Expect some activity right away from the main task (in this file).
 */
int app_main(void)
{
	pinmux_init();

	// Activate green and blue LEDs for user feedback
	ledInit();
	app_ledGreen(false);	// On to show camera activity
	app_ledBlue(true);		// On to show processor is active (not in DPD)

	XP_YELLOW;
	dbg_printf(DBG_LESS_INFO, "\n*** WW500_MD_Test_1 Built: %s %s\r\n", __TIME__, __DATE__);
	XP_WHITE;
	dbg_printf(DBG_LESS_INFO, "Testing HM0360 wake from sleep\n");

	g_maintask_state = APP_MAIN_TASK_STATE_UNINIT;
	g_algotask_state = APP_ALGO_TASK_STATE_UNINIT;
	g_dptask_state = APP_DP_TASK_STATE_UNINIT;
	g_commtask_state = APP_COMM_TASK_STATE_UNINIT;

	xDPTaskQueue  = xQueueCreate( DP_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xDPTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xDPTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}
	xCommTaskQueue  = xQueueCreate( COMM_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xCommTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xCommTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}

	xMainTaskQueue  = xQueueCreate( MAIN_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xMainTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xMainTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}

	xAlgoTaskQueue  = xQueueCreate( ALGO_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xAlgoTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xAlgoTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}

    if (xTaskCreate(dp_task, "DP_task", 1024, NULL, dp_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "dp_task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(comm_task, "Comm_task", 1024, NULL, comm_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "comm_task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(main_task, "Main_task", 1024, NULL, main_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "main_task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(algo_task, "Algo_task", 1024, NULL, algo_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "algo_task creation failed!.\r\n");
        while (1)
            ;
    }

	if (configUSE_TICKLESS_IDLE) {
		xprintf("FreeRTOS tickless idle is enabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
	}
	else {
		XP_RED;
		xprintf("FreeRTOS tickless idle is disabled. configMAX_PRIORITIES = %d\n", configMAX_PRIORITIES);
		XP_WHITE;
	}

    vTaskStartScheduler();

    for (;;) {
    	// Should never get here...
    }

    return 0;
}


/**
 * Prepare for DPD, then enter DPD
 *
 * Does not return.
 */
static void enterDPD(void) {
	app_start_state(APP_STATE_STOP);
	cisdp_sensor_md_init();

	// Do some configuration of MD parameters

	hm0360_x_set_threshold(10);

	// Save the new value of imageSequenceNumber
	if (imageSequenceNumber > 0) {
		fatfs_saveSequenceNumber(imageSequenceNumber);
	}

	dbg_printf(DBG_LESS_INFO, "\nEnter DPD mode!\n\n");
	app_ledBlue(false);

	app_pmu_enter_dpd();	// Does not return
}

/**
 * Reads HM0360 registers that contain device ID etc.
 *
 * Because of noise on the SCL pin the results are sometimes unexpected - hence the error checking.
 *
 * There are two ways of doing this. Both options produce the same result.
 */
static void checkIds(void) {
	XP_LT_CYAN;
#if 1
	//
	hx_drv_cis_get_reg(MODEL_ID_H, &idh);	// 0x03
	if (idh != 0x03) {
		dbg_printf(DBG_LESS_INFO, "MODEL_ID_H was 0x%02x should be 0x03\n", idh);
		app_pmu_enter_dpd();	// Does not return
	}
	hx_drv_cis_get_reg(MODEL_ID_L, &idl);	// 0x60
	if (idl != 0x60) {
		dbg_printf(DBG_LESS_INFO, "MODEL_ID_L was 0x%02x should be 0x60\n", idl);
		app_pmu_enter_dpd();	// Does not return
	}
	hx_drv_cis_get_reg(SILICON_REV, &sil);	// 0x03
	if (sil != 0x03) {
		dbg_printf(DBG_LESS_INFO, "SILICON_REV was 0x%02x should be 0x03\n", sil);
		app_pmu_enter_dpd();	// Does not return
	}
	hx_drv_cis_get_reg(FRAME_COUNT_H, &fch);	// 0xff
		if (fch != 0xff) {
			dbg_printf(DBG_LESS_INFO, "FRAME_COUNT_H was 0x%02x should be 0xff\n", fch);
			app_pmu_enter_dpd();	// Does not return
		}
	hx_drv_cis_get_reg(FRAME_COUNT_L, &fcl);	// 0xff
		if (fcl != 0xff) {
			dbg_printf(DBG_LESS_INFO, "FRAME_COUNT_L was 0x%02x should be 0xff\n", fcl);
			app_pmu_enter_dpd();	// Does not return
		}
#else
	hm0360_x_get_reg(MODEL_ID_H, &idh);
	hm0360_x_get_reg(MODEL_ID_L, &idl);
	hm0360_x_get_reg(SILICON_REV, &sil);
	hm0360_x_get_reg(FRAME_COUNT_H, &fch);
	hm0360_x_get_reg(FRAME_COUNT_L, &fcl);
#endif


	dbg_printf(DBG_LESS_INFO, "HM0360 ID: 0x%02x%02x Rev 0x%02x Frames: %d\n\n", idh, idl, sil, (fch << 8) + fcl);
	XP_WHITE;
}

static void print_md_output(void) {
	uint8_t roiOut[ROIOUTENTRIES];

	// read the RoI_OUT registers
	xprintf("> ");
	hm0360_x_get_roi_out(roiOut, ROIOUTENTRIES);
	for (uint8_t i=0; i < ROIOUTENTRIES; i++) {
		xprintf("%02x ", roiOut[i]);
		//            	if ((i % 8) == 7) {
		//            		// space after 8 bytes
		//            		xprintf("\n");
		//            	}
	}
	xprintf("\n");
}

/**
 * FreeRTOS task - entered on cold and warm boots.
 *
 * After initialisation the FreeRTOS starts.
 *
 * Observed behaviour is as follows:

	After initialisation the next thing that happens is that the os_app_dplib_cb() in dp_task.c
	receives a SENSORDPLIB_STATUS_XDMA_FRAME_READY event (5) which it
	sends to the dp_task queue. This updates counters and flags and sends a APP_MSG_MAINEVENT_CAP_FRAME_DONE to the main_task.

	The main task sends a APP_MSG_VISIONALGOEVENT_START_ALGO message to the algo task.
	The algo task writes a JPEG file and runs the neural network processor.
	The algo task then sends a APP_MSG_MAINEVENT_VISIONALGO_STARTDONE message to the main task.

	This is the end of one picture and there is a FreeRTOS vTaskDelay() delay here (if there are more images to capture_.
	If enough images have been processed the code prepares to enter DPD.
	Otherwise a APP_MSG_DPEVENT_RECAPTURE message is sent to the dp task
	which calls sensordplib_retrigger_capture() to take another image.
 *
 */
void main_task(void *pvParameters) {
	APP_MSG_T main_recv_msg;
	APP_MSG_T algo_send_msg;
	APP_MSG_T dp_send_msg;
	bool cameraPresent = false;

	TickType_t xLastWakeTime = 0;
	uint16_t frameCount;
	int16_t deltaFrames;
	uint16_t lastFrameCount = 0;

	uint8_t interruptStatus;

#ifdef TODO
    uint8_t main_motion_detect = 0;
#endif	// TODO
    uint8_t main_waitstart_cap = 0;
    uint8_t gpioValue;
	uint32_t wakeup_event;
	uint32_t wakeup_event1;
	rtc_time tm;

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

	XP_CYAN;
    xprintf("Wakeup_event=0x%04x, WakeupEvt1=0x%04x\n", wakeup_event, wakeup_event1);
    print_wakeup_event(wakeup_event, wakeup_event1);	// print descriptive string
    XP_WHITE;

#if (FLASH_XIP_MODEL == 1)
    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
#endif

    if(cv_init(true, true)<0) {
    	xprintf("cv init fail\n");
        while (1) {
        	// hangs up
        }
    }

	g_maintask_state = APP_MAIN_TASK_STATE_INIT;

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		/*Cold Boot*/
		XP_LT_BLUE;
		xprintf("\n### Cold Boot ###\n");
		XP_WHITE;

		// Set a default time and date (1/1/25 0:0:0)
		tm.tm_year = 2025;
		tm.tm_mon = 1;
		tm.tm_mday = 1;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;

		RTC_SetTime(&tm);

		#if ( SUPPORT_FATFS == 1 )
		imageSequenceNumber = fatfs_init();
		#endif
		// The name 'SENSOR_AE_STABLE_CNT' implies something to do with "Automatic Exposure"
		// There are many AE registers in the HM0360
		g_enter_pmu_frame_cnt = SENSOR_AE_STABLE_CNT;
		// The following will start the first image capture
		cameraPresent = app_start_state(APP_STATE_ALLON);

    	if (cameraPresent) {
    		// The following confirms I can read registers
    		checkIds();
    		cisdp_sensor_get_int_status(&interruptStatus);
    		dbg_printf(DBG_LESS_INFO, "HM0360 interrupt status 0x%02x\n", interruptStatus);
    		if ((interruptStatus & MD_INT) == MD_INT) {
        		dbg_printf(DBG_LESS_INFO, "Motion detected\n");
    		}
    		cisdp_sensor_clear_interrupt(0xff);		// clear all bits
    		cisdp_sensor_get_int_status(&interruptStatus);		// check this worked
    		dbg_printf(DBG_LESS_INFO, "HM0360 interrupt status 0x%02x\n", interruptStatus);
    	}

		XP_CYAN;
		xprintf("Cold boot initialisation done\n\n");
		XP_WHITE;
		xprintf("Taking %d pictures at %dms intervals before sleeping starting at image %d\n",
				SENSOR_AE_STABLE_CNT, DELAYBETWEENPICS, imageSequenceNumber);
#ifdef MCLKUSESRC36M
		xprintf("HM0360 uses RC36M clock\n");
#else
		xprintf("HM0360 uses 24MHz xtal clock\n");
#endif	// MCLKUSESRC36M
	}
	else {
		/*Warm Boot*/
		XP_LT_GREEN;
		xprintf("### Warm Boot ###\n");
		XP_WHITE;

		xprintf("Wakeup Events = %d, %d\n", wakeup_event, wakeup_event1);

		app_clk_enable();
		RTC_GetTime(&tm);
		xprintf("RTC GetTime : %d/%02d/%02d %02d:%02d:%02d\r\n",
			tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

		#if ( SUPPORT_FATFS == 1 )
		imageSequenceNumber = fatfs_init();
		#endif
		g_enter_pmu_frame_cnt = ENTER_PMU_MODE_FRAME_CNT;
		drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_CPU);
        sensordplib_csirx_disable();

        // With APP_STATE_RESTART - most camera init is skipped
        cameraPresent = app_start_state(APP_STATE_RESTART);

    	if (cameraPresent) {
    		// The following confirms I can read registers
    		// Both options produce the same result, but both appear to read 16 bits of data (pruambly discarding the second byte)
    		checkIds();
    		cisdp_sensor_get_int_status(&interruptStatus);
    		dbg_printf(DBG_LESS_INFO, "HM0360 interrupt status 0x%02x\n", interruptStatus);
    		if ((interruptStatus & MD_INT) == MD_INT) {
        		dbg_printf(DBG_LESS_INFO, "Motion detected\n");
    		}
    		cisdp_sensor_clear_interrupt(0xff);		// clear all bits
    		cisdp_sensor_get_int_status(&interruptStatus);		// check this worked
    		dbg_printf(DBG_LESS_INFO, "HM0360 interrupt status 0x%02x\n", interruptStatus);

    		// According to hx_drv_CIS_common.h this should have a value 0, 1,2, 3 or ff - actually return 0x52
    		dbg_printf(DBG_LESS_INFO, "DEAULT_XHSUTDOWN_PIN = 0x%02x\n", DEAULT_XHSUTDOWN_PIN);

    		// read the RoI_OUT registers
    		xprintf("Motion detected:\n");
    		print_md_output();
    	}

        XP_CYAN;
        xprintf("Warm boot initialisation done\n");
		XP_WHITE;
		xprintf("Taking %d pictures at %dms intervals before sleeping starting at image %d\n",
				ENTER_PMU_MODE_FRAME_CNT, DELAYBETWEENPICS, imageSequenceNumber);
	}

#ifdef SUPPORT_DUAL_CORE
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
	hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);
#endif

#if ( ENTER_SLEEP_MODE == 1 )
	if (cameraPresent) {
		// The first image capture has started
		app_ledGreen(true);

#ifdef DELAYBETWEENPICS
		xLastWakeTime = xTaskGetTickCount(); // Initialize wake time
#endif	// DELAYBETWEENPICS

	}
	else {
		dbg_printf(DBG_LESS_INFO, "\nNo camera. Enter DPD mode!\n\n");
		app_pmu_enter_dpd();	// Does not return
	}
#endif	// ENTER_SLEEP_MODE


	// Process messages from other tasks.

	for (;;)
    {
    	if (xQueueReceive ( xMainTaskQueue , &(main_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE )
    	{
    	   	dbg_printf(DBG_LESS_INFO, "main_recv_msg=0x%x\r\n", main_recv_msg.msg_event);
    	   	switch(main_recv_msg.msg_event)
    	   	{
    	   	case APP_MSG_MAINEVENT_CAP_FRAME_DONE:
    	   		algo_send_msg.msg_data = 0;
    	   		algo_send_msg.msg_event = APP_MSG_VISIONALGOEVENT_START_ALGO;
    	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_SENSOR_TIMER:
    	   		break;
    	   	case APP_MSG_MAINEVENT_DP_ERROR:
    	   		g_dptask_state = APP_DP_TASK_STATE_ERROR;
    	   	    dbg_printf(DBG_LESS_INFO, "main_recv_msg.msg_data=0x%x fail\r\n", main_recv_msg.msg_data);
#ifdef TODO
    	   	    app_dump_dplib_edminfo();
    	   		main_motion_detect = 0;
    	   	    app_setup_dplib();
#endif
    	   	    break;
    	   	case APP_MSG_MAINEVENT_SENSOR_DP_ERROR:
    	   		g_dptask_state = APP_DP_TASK_STATE_ERROR;
    	   	    dbg_printf(DBG_LESS_INFO, "main_recv_msg.msg_data=0x%x fail\r\n", main_recv_msg.msg_data);
#ifdef TODO
    	   	    app_dump_dplib_edminfo();
    	   		main_motion_detect = 0;
    	   	    app_setup_dplib();
#endif
    	   	    break;
    	   	case APP_MSG_MAINEVENT_START_CAPTURE:
	   			dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_START_CAPTURE\n");
	   			main_waitstart_cap = 0;
	   			g_dptask_state = APP_DP_TASK_STATE_SETUP_CAP_END;
    	   		break;
    	   	case APP_MSG_MAINEVENT_STOP_CAPTURE:
	   			dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_STOP_CAPTURE main_waitstart_cap=%d\n", main_waitstart_cap);
    	   		xQueueReset(xDPTaskQueue);
    	   		g_dptask_state = APP_DP_TASK_STATE_STOP_CAP_END;
    	   		if(main_waitstart_cap == 1)
    	   		{
        	   		if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))
        	   		{
        	   			main_waitstart_cap = 0;
            	   		dp_send_msg.msg_data = 0;
            	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
            	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
            	   		{
            	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
            	   		}
        	   		}else{
        	   			dbg_printf(DBG_LESS_INFO, "wait start g_dptask_state=x%x, g_visionalgotask_state=0x%x\n", g_dptask_state, g_algotask_state);
        	   		}
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_MOTION_DETECT:
#ifdef TODO
    	   		main_motion_detect = 1;
#endif	// TODO
    	   		break;
    	   	case APP_MSG_MAINEVENT_AON_GPIO0_INT:
    	   	    hx_drv_gpio_get_in_value(AON_GPIO0, &gpioValue);
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_AON_GPIO0_INT %d AON_GPIO0=%d\n", main_recv_msg.msg_data, gpioValue);
#if 0
    	   	    if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_START) || (g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END))
    	   	    {
    	   	    	dbg_printf(DBG_LESS_INFO, "g_dptask_state=0x%x no send STOPCAPTURE\r\n", g_dptask_state);
    	   	    }else{
        	   		dp_send_msg.msg_data = 0;
        	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STOPCAPTURE;
        	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
        	   		{
        	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
        	   		}
    	   	    }
    	   	    if((g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO) || (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))
    	   	    {
    	   	    	dbg_printf(DBG_LESS_INFO, "g_algotask_state=0x%x no send STOP_ALGO\r\n", g_algotask_state);
    	   	    }else{
        	   		algo_send_msg.msg_data = 0;
        	   		algo_send_msg.msg_event = APP_MSG_VISIONALGOEVENT_STOP_ALGO;
        	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE)
        	   		{
        	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
        	   		}
    	   	    }
#endif
    	   		break;
    	   	case APP_MSG_MAINEVENT_AON_GPIO1_INT:
    	   	    hx_drv_gpio_get_in_value(AON_GPIO1, &gpioValue);
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_AON_GPIO1_INT %d gpioValue=%d\n", main_recv_msg.msg_data, gpioValue);
#if 0
    	   		if(((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)) || \
    	   				((g_dptask_state == APP_DP_TASK_STATE_ERROR) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)))
    	   		{
    	   			main_waitstart_cap = 0;
        	   		dp_send_msg.msg_data = 0;
        	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
        	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
        	   		{
        	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
        	   		}
    	   		}else{
    	   			dbg_printf(DBG_LESS_INFO, "wait start g_dptask_state=x%x, g_algotask_state=0x%x\n", g_dptask_state, g_algotask_state);
    	   			if((g_dptask_state == APP_DP_TASK_STATE_SETUP_CAP_START) || (g_dptask_state == APP_DP_TASK_STATE_SETUP_CAP_END))
    	   			{
    	   				dbg_printf(DBG_LESS_INFO, "already in start cap g_dptask_state=%d\r\n", g_dptask_state);
    	   			}else{
        	   			main_waitstart_cap = 1;
    	   			}
    	   		}
#endif
    	   		break;
    	   	case APP_MSG_MAINEVENT_SB_GPIO0_INT:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_SB_GPIO0_INT\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_SB_GPIO1_INT:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_SB_GPIO1_INT\n");
    	   		break;

    	   	case APP_MSG_MAINEVENT_VISIONALGO_STARTDONE:

    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VISIONALGO_STARTDONE\n");
    	   		g_algotask_state = APP_ALGO_TASK_STATE_DOALGO_DONE;
				g_algo_done_frame++;

				// It seems that the frame counter increments.
				// Both of these options give the same result
#if 0
				hx_drv_cis_get_reg(FRAME_COUNT_H, &fch);
				hx_drv_cis_get_reg(FRAME_COUNT_L, &fcl);
#else
				hm0360_x_get_reg(FRAME_COUNT_H, &fch);
				hm0360_x_get_reg(FRAME_COUNT_L, &fcl);
#endif
				dbg_printf(DBG_LESS_INFO, "g_algo_done_frame = %d \n", g_algo_done_frame);

				// Print some statistics concerning frame rate
				frameCount = (fch << 8) + fcl;
				deltaFrames = frameCount - lastFrameCount;
				lastFrameCount = frameCount;
				XP_CYAN;
				dbg_printf(DBG_LESS_INFO, "Frame count = %d (%d + %d) (delta = %d, %dHz)\n",
						frameCount, (fch << 8), fcl, deltaFrames, (deltaFrames * 1000) / DELAYBETWEENPICS);
				XP_WHITE;
				dbg_printf(DBG_LESS_INFO, "\n");

				print_md_output();

				#if ( ENTER_SLEEP_MODE == 1 )
				if ( g_algo_done_frame == g_enter_pmu_frame_cnt ) {
					// Move all the "prepare for DPD" to a separate function

					enterDPD();
				}
				#endif	// ENTER_SLEEP_MODE

#ifdef DELAYBETWEENPICS
                // Delay between pictures
				vTaskDelayUntil(&xLastWakeTime, DELAYBETWEENPICS);
#endif	// DELAYBETWEENPICS

				// Take another pic:
				app_ledGreen(true);

    	   		dp_send_msg.msg_data = 0;
    	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_RECAPTURE;
    	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_VISIONALGO_STOPDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VISIONALGO_STOPDONE\n");
    	   		g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO_DONE;

    	   		if(main_waitstart_cap == 1)
    	   		{
        	   		if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))
        	   		{
        	   			main_waitstart_cap = 0;
            	   		dp_send_msg.msg_data = 0;
            	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
            	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
            	   		{
            	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
            	   		}
        	   		}else{
        	   			dbg_printf(DBG_LESS_INFO, "wait start g_dptask_state=x%x, g_algotask_state=0x%x\n", g_dptask_state, g_algotask_state);
        	   		}
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_I2CCOMM:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_I2CCOMM\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_VADBUF1_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VADBUF1_NOTIFY size=0x%x\n", main_recv_msg.msg_data);
    	   		break;
    	   	case APP_MSG_MAINEVENT_VADBUF2_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VADBUF2_NOTIFY size=0x%x\n", main_recv_msg.msg_data);
    	   		break;
    	   	case APP_MSG_MAINEVENT_CM55SRDY_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_CM55SRDY_NOTIFY\n");
    	   		break;
    	   	default:
    	   		break;
    	   	}

    	}
    }
}
