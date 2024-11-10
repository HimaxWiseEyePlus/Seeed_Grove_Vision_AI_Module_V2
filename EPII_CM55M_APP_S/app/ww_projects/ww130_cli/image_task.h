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

// The states for the image_task
// APP_IMAGE_STATE_NUMSTATES is only used to establish the number of states
// typedef enum
// {
// 	APP_IMAGE_TASK_STATE_UNINIT = 0x0000,
// 	APP_IMAGE_TASK_STATE_INIT = 0x0001,
// 	APP_IMAGE_TASK_STATE_SETUP_CAP_START = 0x0002,
// 	APP_IMAGE_TASK_STATE_SETUP_CAP_END = 0x0003,
// 	APP_IMAGE_TASK_STATE_RECAP_FRAME = 0x0004,
// 	APP_IMAGE_TASK_STATE_CAP_FRAMERDY = 0x0005,
// 	APP_IMAGE_TASK_STATE_STOP_CAP_START = 0x0006,
// 	APP_IMAGE_TASK_STATE_STOP_CAP_END = 0x0007,
// 	APP_MSG_IMAGETASK_CAP_FRAME_DONE = 0x0008,
// 	APP_IMAGE_TASK_STATE_NUMSTATES = 0x0009,
// 	APP_IMAGE_TASK_STATE_ERROR,
// } APP_IMAGE_TASK_STATE_E;

typedef enum
{
	APP_IMAGE_TASK_STATE_UNINIT = 0x0000,
	APP_IMAGE_TASK_STATE_INIT = 0x0001,
	APP_IMAGE_TASK_STATE_CAPTURING = 0x0002,
	APP_IMAGE_TASK_STATE_NN_PROCESSING = 0x0003,
	APP_IMAGE_TASK_WAIT_FOR_TIMER = 0x0004,
	APP_IMAGE_TASK_STATE_BUSY = 0x0005,
	APP_IMAGE_TASK_STATE_NUMSTATES = 0x0006,
} APP_IMAGE_TASK_STATE_E;

TaskHandle_t
image_createTask(int8_t priority);

uint16_t image_getState(void);

const char *image_getStateString(void);

#endif /* APP_WW_PROJECTS_WW130_CLI_IMAGE_TASK_H_ */