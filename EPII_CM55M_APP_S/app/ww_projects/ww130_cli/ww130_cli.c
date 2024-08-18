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

	pinmux_init();

	XP_YELLOW;
	printf("\n**** WW130 CLI. Built: %s %s ****\r\n\n", __TIME__, __DATE__);
	XP_WHITE;

	// Each task has its own file. Call these to do the task creation and initialisation

	// Task 1 is a placeholder task that does not do anything of significance
	// It receives messages from the CLI and uses these to change state
	task1_createTask1Task();

	// Task 2 is a placeholder task that does not do anything of significance
	// It receives messages from the CLI and uses these to change state
	ifTask_createTask();

	// The CLI task implements a command line interface (CLI) for use in debugging.
	// This can be extended to manage incoming messages from other hardware (as well as the console UART)
	cli_createCLITask();

	// This tasks provides a CLI interface to the FatFs
	fatfs_createTask();

	vTaskStartScheduler();

	for (;;)
		;
}
