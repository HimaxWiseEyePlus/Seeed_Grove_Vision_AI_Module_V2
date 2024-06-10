/*
 * timed_interrupt_fatfs.h
 *
 *  Created on: June 1st, 2024
 *      Author: Tobyn
 */

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#ifndef SCENARIO_APP_TIMED_INTERRUPT_H_
#define SCENARIO_APP_TIMED_INTERRUPT_H_

#define APP_BLOCK_FUNC()          \
    do                            \
    {                             \
        __asm volatile("b    ."); \
    } while (0)

typedef enum
{
    APP_STATE_ALLON,
    APP_STATE_RESTART,
    APP_STATE_STOP,
} APP_STATE_E;

int app_main(void);

#endif /* SCENARIO_APP_TIMED_INTERRUPT_H_ */