/**
 * \file
 * \brief  common io implementation
 */
#ifndef _CONSOLE_IO_H_
#define _CONSOLE_IO_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "WE2_device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int console_putchar(unsigned char chr);
extern int console_getchar(void);
extern void console_setup(uint32_t uart_id, uint32_t uart_baud_rate);
extern void console_desetup(uint32_t uart_id);

#ifdef __cplusplus
}
#endif

#endif /* _CONSOLE_IO_H_ */
