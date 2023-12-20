/**
 * \file
 * \brief  common io implementation
 */

#if defined(HX_TFM)
#include "cmsis_compiler.h"             // Compiler agnostic definitions
//YX hook a empty xprintf_setup/xprintf_desetup/console_getchar/console_putchar for future. Only use TFM printf
__WEAK void xprintf_setup(void) { }
__WEAK void xprintf_desetup(void) { }
__WEAK int console_getchar(void) { return 0; }
__WEAK int console_putchar(unsigned char chr) { (void)chr; return 0; }
#else //#if defined(HX_TFM)

#include <stdio.h>
#include "WE2_device.h"
#ifdef IP_uart
#include "hx_drv_uart.h"
#endif
/*TODO add UART board related header file*/
#include "console_io.h"
#include "xprintf.h"
/*!< to indicate xprintf setup state(0 for not setup) */

static int xprintf_setup_flag = 0;
#ifdef IP_uart
static DEV_UART *console_uart;
#endif
/** put one char */
int console_putchar(unsigned char chr) {
#ifdef MPS2_FVP
#ifdef SEMIHOST
    putchar(chr);
#endif
#else

#ifdef SEMIHOST
    putchar(chr);
#else
#ifdef IP_uart
    if (console_uart == NULL)
    {
        return -1;
    }
    console_uart->uart_write((const void *)(&chr), 1);
#endif
#endif

#endif

	return 0;
}

/** get one char*/
int console_getchar(void) {
    unsigned char data;
#ifdef MPS2_FVP
#ifdef SEMIHOST
    data = getchar();
#endif
#else

#ifdef SEMIHOST
    data = getchar();
#else
#ifdef IP_uart
    if (console_uart == NULL)
    {
        return -1;
    }
    while(!console_uart->uart_read((void *)(&data), 1)) {}
#endif

#endif
#endif
	return (int) data;
}

/** xprintf need functions api setup */
void xprintf_setup(void) {
	if (xprintf_setup_flag) {
        return;
    }
#ifdef IP_uart
    console_uart = hx_drv_uart_get_dev(CONSOLE_UART_ID);
    console_uart->uart_open(BOARD_CONSOLE_UART_BAUD);
#endif
    xdev_in(console_getchar);
    xdev_out(console_putchar);

#ifdef IP_uart
#if defined(__GNU__)
    xprintf("Compiler Version: ARM GNU, %s\r\n", __VERSION__);
#else
    xprintf("Compiler Version: ARM CLANG, %s\r\n", __VERSION__);
#endif
#endif

    xprintf_setup_flag = 1;
}

void xprintf_desetup(void) {
	if (xprintf_setup_flag) {
#ifdef IP_uart
        hx_drv_uart_deinit(CONSOLE_UART_ID);
#endif
        xdev_in(NULL);
        xdev_out(NULL);
    	//TODO JACKY
        xprintf_setup_flag = 0;
    }
}

#endif //#if defined(HX_TFM)
