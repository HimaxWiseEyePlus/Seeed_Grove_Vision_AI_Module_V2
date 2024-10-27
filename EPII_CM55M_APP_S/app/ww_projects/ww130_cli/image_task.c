
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "image_task.h"
volatile APP_IMAGE_STATE_E image_task_state = APP_IMAGE_STATE_UNINIT;

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char *imageTaskStateString[APP_IMAGE_STATE_NUMSTATES] = {
    "Uninitialised",
    "Idle",
    "Busy"};

/**
 * Returns the internal state as a number
 */
uint16_t image_getState(void)
{
    return image_task_state;
}

/**
 * Returns the internal state as a string
 */
const char *image_getStateString(void)
{
    return *&imageTaskStateString[image_task_state];
}