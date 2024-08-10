/*
 *
 *
 */

/*********************************** Includes ************************************************/


#include "xprintf.h"
#include "printf_x.h"	// Print colours
#include "WE2_debug.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_CIS_common.h"
#include "driver_interface.h"
#include "ww130_test.h"

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
#include "app_state.h"
#include "dp_task.h"
#include "main_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "sleep_mode.h"
#include "pinmux_cfg.h"
#include "app_msg.h"

/*********************************** Definitions ************************************************/

#define CIS_XSHUT_SGPIO0
#ifdef CIS_XSHUT_SGPIO0
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
#else
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
#endif
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

/*********************************** Local function Declarations ************************************************/

void pinmux_init();

/*********************************** Local Variables ************************************************/

uint32_t wakeup_event;
uint32_t wakeup_event1;

volatile APP_MAIN_TASK_STATE_E g_maintask_state = APP_MAIN_TASK_STATE_UNINIT;
volatile APP_ALGO_TASK_STATE_E g_algotask_state = APP_ALGO_TASK_STATE_UNINIT;
volatile APP_DP_TASK_STATE_E g_dptask_state = APP_DP_TASK_STATE_UNINIT;
volatile APP_COMM_TASK_STATE_E g_commtask_state = APP_COMM_TASK_STATE_UNINIT;

QueueHandle_t     xMainTaskQueue;
QueueHandle_t     xDPTaskQueue;
QueueHandle_t     xCommTaskQueue;
QueueHandle_t     xAlgoTaskQueue;

static bool coldBoot;

/*********************************** Pragma Messages ************************************************/

// CGP stuff to get compiler to print values

#ifdef ENABLE_OS
#pragma message("The value of ENABLE_OS: " XSTR(ENABLE_OS))
#else
#pragma message("ENABLE_OS is not defined")
#endif	// ENABLE_OS

#ifdef OS_ID
#pragma message("The value of OS_ID: " XSTR(OS_ID))
#else
#pragma message("OS_ID is not defined")
#endif	// OS_ID

#ifdef FREERTOS
#pragma message("The value of FREERTOS: " XSTR(FREERTOS))
#else
#pragma message("FREERTOS is not defined")
#endif	// FREERTOS

#ifdef OS_SEL
#pragma message("The value of OS_SEL: " XSTR(OS_SEL))
#else
#pragma message("OS_SEL is not defined")
#endif	// OS_SEL

#ifdef FREERTOS_OSHAL
#pragma message("The value of FREERTOS_OSHAL: " XSTR(FREERTOS_OSHAL))
#else
#pragma message("FREERTOS_OSHAL is not defined")
#endif	// FREERTOS_OSHAL

#ifdef configUSE_TICKLESS_IDLE
#pragma message("The value of configUSE_TICKLESS_IDLE: " XSTR(configUSE_TICKLESS_IDLE))
#else
#pragma message("configUSE_TICKLESS_IDLE is not defined")
#endif	// configUSE_TICKLESS_IDLE

/*********************************** Local Function Definitions ************************************************/

/**
 * Initialise GPIO pins for this application
 * // TODO move PA0 configuration here
 */
void pinmux_init() {
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


	// PA0 configuration for inter-board communications

	/* Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)*/
	//swd_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}

/*********************************** Main ************************************************/

/*!
 * @brief Main function
 */
int app_main(void) {

	pinmux_init();

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE)) {
		coldBoot = true;
	}
	else {
		coldBoot = false;
	}

	XP_YELLOW;
	xprintf("\n### WW130 Test app built: %s %s ###\n", __TIME__, __DATE__);
	XP_WHITE;

	XP_LT_BLUE
	// e.g. PMU_WAKEUP_PD_SB_TIMER_INT (0x40), PMU_WAKEUP_PD_EXTGPIO (0x10)
	// Actually, these are bit positions in a 32-bit word
    xprintf("Wakeup_event = 0x%x, Wakeup_event1 = 0x%x\n\n", wakeup_event, wakeup_event1);
	XP_WHITE

	// To enable or disable tickless idle:

#if (configUSE_TICKLESS_IDLE == 1)
	xprintf("FreeRTOS Tickless Idle is active\n");
#else
	xprintf("FreeRTOS Tickless Idle is not active\n");
#endif	// configUSE_TICKLESS_IDLE

	//dbg_printf(DBG_LESS_INFO, "freertos rtos_app\r\n");

	g_maintask_state = APP_MAIN_TASK_STATE_UNINIT;
	g_algotask_state = APP_ALGO_TASK_STATE_UNINIT;
	g_dptask_state = APP_DP_TASK_STATE_UNINIT;
	g_commtask_state = APP_COMM_TASK_STATE_UNINIT;

	// Create a queue for each task. The value returned is the handle. 0 means fails
	xDPTaskQueue  = xQueueCreate( DP_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xDPTaskQueue == 0) {
        dbg_printf(DBG_LESS_INFO, "xDPTaskQueue creation failed!.\r\n");
        while (1) ;
	}

	xCommTaskQueue  = xQueueCreate( COMM_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xCommTaskQueue == 0) {
        dbg_printf(DBG_LESS_INFO, "xCommTaskQueue creation failed!.\r\n");
        while (1)  ;
	}

	xMainTaskQueue  = xQueueCreate( MAIN_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xMainTaskQueue == 0) {
        dbg_printf(DBG_LESS_INFO, "xMainTaskQueue creation failed!.\r\n");
        while (1)  ;
	}

	xAlgoTaskQueue  = xQueueCreate( ALGO_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xAlgoTaskQueue == 0) {
        dbg_printf(DBG_LESS_INFO, "xAlgoTaskQueue creation failed!.\r\n");
        while (1)  ;
	}

	// Create the tasks
    if (xTaskCreate(dp_task, "DP_task", 512, &coldBoot, dp_task_PRIORITY, NULL) !=  pdPASS)  {
        dbg_printf(DBG_LESS_INFO, "dp_task creation failed!.\r\n");
        while (1)  ;
    }

    if (xTaskCreate(comm_task, "Comm_task", 512, &coldBoot, comm_task_PRIORITY, NULL) !=  pdPASS)  {
        dbg_printf(DBG_LESS_INFO, "comm_task creation failed!.\r\n");
        while (1)  ;
    }

    if (xTaskCreate(main_task, "Main_task", 512, &coldBoot, main_task_PRIORITY, NULL) != pdPASS)  {
        dbg_printf(DBG_LESS_INFO, "main_task creation failed!.\r\n");
        while (1)  ;
    }

    if (xTaskCreate(algo_task, "Algo_task", 512, &coldBoot, algo_task_PRIORITY, NULL) !=  pdPASS) {
        dbg_printf(DBG_LESS_INFO, "algo_task creation failed!.\r\n");
        while (1)  ;
    }

	dbg_printf(DBG_LESS_INFO, "Starting FreeRTOS scheduler (coldBoot is %s)\r\n", coldBoot?"true":"false");

    vTaskStartScheduler();

    // Do we ever get here?
    //for (;;) {};
    while (1)  ;

    return 0;
}
