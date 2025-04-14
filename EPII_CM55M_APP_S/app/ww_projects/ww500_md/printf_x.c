/*
 * printf_x.c
 *
 *  Created on: 29 Jul 2024
 *      Author: charl
 */

#include "printf_x.h"
#include "xprintf.h"
#include "ctype.h"

#include <stdint.h>

#define IN_LINE_PRINT_CNT   (16u)   //!< Number of data bytes printed in a single line.


// print test string in each colour
void printf_x_test(void) {
	xprintf("This text's colour is ");
	XP_WHITE
	xprintf("WHITE");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_BR_WHITE
	xprintf("BR_WHITE");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_RED
	xprintf("RED");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_LT_RED
	xprintf("LT_RED");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_GREEN
	xprintf("GREEN");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_LT_GREEN
	xprintf("LT_GREEN");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_BROWN
	xprintf("BROWN");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_YELLOW
	xprintf("YELLOW");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_BLUE
	xprintf("BLUE");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_LT_BLUE
	xprintf("LT_BLUE");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_PURPLE
	xprintf("PURPLE");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_MAGENTA
	xprintf("MAGENTA");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_CYAN
	xprintf("CYAN");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_LT_CYAN
	xprintf("LT_CYAN");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_GREY
	xprintf("GREY");
	XP_WHITE
	xprintf(" OK?\n");

	xprintf("This text's colour is ");
	XP_LT_GREY
	xprintf("LT_GREY");
	XP_WHITE
	xprintf(" OK?\n");
}

/**
 * Utility to print the contents of a buffer
 */
void printf_x_printBuffer(uint8_t * buff, uint16_t length) {
    uint8_t lineBuff[IN_LINE_PRINT_CNT + 1];	  // +1 for '\0'

    for (uint16_t addr = 0; addr < length; addr += IN_LINE_PRINT_CNT)  {
    	memcpy(lineBuff, buff + addr, IN_LINE_PRINT_CNT);
        lineBuff[IN_LINE_PRINT_CNT] = '\0';

        xprintf("%03x: ", addr);
        for (uint8_t i = 0; i < IN_LINE_PRINT_CNT; i++)  {
        	xprintf("%02x ", lineBuff[i]);
            if (i == 7) {
            	// Extra space to separate 1st and 2nd 8 bytes
            	xprintf(" ");
            }
            if (!isprint((int)lineBuff[i]))  {
                lineBuff[i] = '.';
            }
        }
        // Now the string version
        xprintf("%s\n", lineBuff);
    }
}
