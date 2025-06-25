#ifndef APP_WW_PROJECTS_WW500_MD_IMAGE_TASK_H_
#define APP_WW_PROJECTS_WW500_MD_IMAGE_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"
#include "ww500_md.h"

#include "c_api_types.h"	// Tensorflow errors

#define APP_BLOCK_FUNC()          \
	do                            \
	{                             \
		__asm volatile("b    ."); \
	} while (0)

// List of operations to set up camera subsystem
typedef enum {
	CAMERA_CONFIG_INIT_COLD,
	CAMERA_CONFIG_INIT_WARM,
	CAMERA_CONFIG_RUN,
	CAMERA_CONFIG_CONTINUE,
	CAMERA_CONFIG_STOP,
	CAMERA_CONFIG_MD,
} CAMERA_CONFIG_E;

// Possible states. Values must match imageTaskStateString[] in image_task.c
typedef enum {
	APP_IMAGE_TASK_STATE_UNINIT 	= 0x0000,
	APP_IMAGE_TASK_STATE_INIT 		= 0x0001,
	APP_IMAGE_TASK_STATE_CAPTURING 	= 0x0002,
	APP_IMAGE_TASK_STATE_NN_PROCESSING = 0x0003,
	APP_IMAGE_TASK_STATE_WAIT_FOR_TIMER = 0x0004,
	APP_IMAGE_TASK_STATE_SAVE_STATE = 0x0005,
	APP_IMAGE_TASK_STATE_NUMSTATES 	= 0x0006,
} APP_IMAGE_TASK_STATE_E;

// The number of neural network output categories
#define CATEGORIESCOUNT 2

// Sets limits for the capture command
#define MIN_IMAGE_CAPTURES 1
#define MAX_IMAGE_CAPTURES 1000
// Interval between photos (ms)
#define MIN_IMAGE_INTERVAL 0
// 15 minutes -
#define MAX_IMAGE_INTERVAL 15 * 60 * 1000

// file name: 'image_2025-02-03_1234.jpg' = 25 characters, plus trailing '\0'
#define IMAGEFILENAMELEN	26

// default 10%
#define FLASHLEDDUTY 10
// Deafult values of Operational Parameters
// todo THESE
#define NUMPICTURESTOGRAB	3
#define PICTUREINTERVAL		1500
#define	TIMELAPSEINTERVAL	1 * 60	// time in seconds

TaskHandle_t image_createTask(int8_t priority, APP_WAKE_REASON_E wakeReason);

uint16_t image_getState(void);

const char * image_getStateString(void);

const char * image_getLastImageFile(void);

// Temporary until I can make this work through the state machine
void image_hackInactive(void);

// returns true if neural network has detected target
bool image_nnDetected(void);

#endif /* APP_WW_PROJECTS_WW500_MD_IMAGE_TASK_H_ */
