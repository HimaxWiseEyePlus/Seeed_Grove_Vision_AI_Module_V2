/*
 * printf_x.c
 *
 *  Created on: 29 Jul 2024
 *      Author: charl
 */

#include "printf_x.h"

// print test string in each colour
void printf_x_test(void) {
	xprintf("This text's colour is ");
	XP_NONE
	xprintf("NONE");
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

	xprintf("This text's colour is ");
	XP_WHITE
	xprintf("WHITE");
	XP_WHITE
	xprintf(" OK?\n");
}

