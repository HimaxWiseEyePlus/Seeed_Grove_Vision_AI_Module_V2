#ifndef APP_WW_PROJECTS_WW130_CLI_IMAGE_TASK_H_
#define APP_WW_PROJECTS_WW130_CLI_IMAGE_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#define APP_BLOCK_FUNC()          \
	do                            \
	{                             \
		__asm volatile("b    ."); \
	} while (0)

// The states for the image
typedef enum
{
	APP_STATE_ALLON,
	APP_STATE_RESTART,
	APP_STATE_STOP,
} APP_STATE_E;

// Possible states. Values must match imageTaskStateString[] in image_task.c
typedef enum {
	APP_IMAGE_TASK_STATE_UNINIT = 0x0000,
	APP_IMAGE_TASK_STATE_INIT = 0x0001,
	APP_IMAGE_TASK_STATE_CAPTURING = 0x0002,
	APP_IMAGE_TASK_STATE_NN_PROCESSING = 0x0003,
	APP_IMAGE_TASK_WAIT_FOR_TIMER = 0x0004,
	APP_IMAGE_TASK_STATE_BUSY = 0x0005,
	APP_IMAGE_TASK_STATE_NUMSTATES = 0x0006,
} APP_IMAGE_TASK_STATE_E;

// Sets limits for the capture command
#define MIN_IMAGE_CAPTURES 1
#define MAX_IMAGE_CAPTURES 1000
#define MIN_IMAGE_INTERVAL 0
#define MAX_IMAGE_INTERVAL 1000

// file name: 'image_2025-02-03_1234.jpg' = 25 characters, plus trailing '\0'
#define IMAGEFILENAMELEN	26

TaskHandle_t image_createTask(int8_t priority);

uint16_t image_getState(void);

const char * image_getStateString(void);

const char * image_getLastImageFile(void);

#endif /* APP_WW_PROJECTS_WW500_MD_IMAGE_TASK_H_ */
