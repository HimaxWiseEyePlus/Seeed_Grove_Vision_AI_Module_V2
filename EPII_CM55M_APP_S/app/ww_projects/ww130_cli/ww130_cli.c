/**
 * ww_template.c
 *
 */

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

#include "xprintf.h"
#include "printf_x.h"

// For pinmux_init()
#include "hx_drv_scu_export.h"
#include "hx_drv_scu.h"
#include "pinmux_cfg.h"

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

internal_state_t internalStates[NUMBEROFTASKS];

/**
 * Initialise GPIO pins for this application
 * // TODO move PA0 configuration here
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

	/* Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin */
	aon_gpio1_pinmux_cfg(&pinmux_cfg);

	/* Init I2C slave 0 pin mux to PA2, PA3 (SCL, SDA)*/
	i2cs0_pinmux_cfg(&pinmux_cfg);

	/* Init SPI master pin mux */
	spi_m_pinmux_cfg(&pinmux_cfg);

	// PA0 configuration for inter-board communications

	/* Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)*/
	// swd_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}

/*!
 * @brief Main function
 */
int app_main(void)
{
	UBaseType_t priority;
	TaskHandle_t task_id;
	internal_state_t internalState;
	uint8_t taskIndex = 0;

	pinmux_init();

	XP_YELLOW;
	xprintf("\n**** WW130 CLI. Built: %s %s ****\r\n\n", __TIME__, __DATE__);
	XP_WHITE;

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

	// Each task has its own file. Call these to do the task creation and initialisation
	// See here for task priorities:
	// https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/03-Task-priorities

	priority = configMAX_PRIORITIES;

	// Place highest priority task at the top. All will be allocated successively lower priorities

	// ifTask handles communications between the Seeed board and the WW130
	task_id = ifTask_createTask(--priority);
	internalState.task_id = task_id;
	internalState.getState = ifTask_getState;
	internalState.stateString = ifTask_getStateString;
	internalState.priority = priority;
	internalStates[taskIndex++] = internalState;
	xprintf("Created '%s' Priority %d\n", pcTaskGetName(task_id), priority);

	// The CLI task implements a command line interface (CLI) for use in debugging.
	// This can be extended to manage incoming messages from other hardware (as well as the console UART)
	task_id = cli_createCLITask(--priority);
	internalState.task_id = task_id;
	internalState.getState = cli_getState; // does not have states
	internalState.stateString = cli_getStateString;
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
