#ifndef APP_WW_PROJECTS_WW130_CLI_IMAGE_TASK_H_
#define APP_WW_PROJECTS_WW130_CLI_IMAGE_TASK_H_

#include <stdbool.h>

// The states for the image_task
// APP_IMAGE_STATE_NUMSTATES is only used to establish the number of states
typedef enum
{
    APP_IMAGE_STATE_UNINIT = 0x0000,
    APP_IMAGE_STATE_IDLE = 0x0001,
    APP_IMAGE_STATE_BUSY = 0x0002,
    APP_IMAGE_STATE_NUMSTATES = 0x0003
} APP_IMAGE_STATE_E;

TaskHandle_t image_createTask(int8_t priority);

uint16_t image_getState(void);

const char *image_getStateString(void);