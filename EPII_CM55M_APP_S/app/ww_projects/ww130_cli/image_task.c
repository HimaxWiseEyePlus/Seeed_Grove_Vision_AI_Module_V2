
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "image_task.h"
#include "app_msg.h"
#include "CLI-commands.h"
#include "ww130_cli.h"
#include "ff.h"

/*************************************** Definitions *******************************************/

// TODO sort out how to allocate priorities
#define image_task_PRIORITY (configMAX_PRIORITIES - 3)

#define IMAGE_TASK_QUEUE_LEN 10

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vImageTask(void *pvParameters);

static FRESULT imageInit(void);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T handleEventForIdle(APP_MSG_T rxMessage);
static APP_MSG_DEST_T handleEventForCapturing(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T rxMessage);

/*************************************** External variables *******************************************/

extern SemaphoreHandle_t xI2CTxSemaphore;

/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t image_task_id;
QueueHandle_t xImageTaskQueue;

volatile APP_IMAGE_TASK_STATE_E image_task_state = APP_IMAGE_TASK_STATE_UNINIT;

static uint8_t g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t g_spi_master_initial_status;
uint32_t jpeg_addr, jpeg_sz;
uint32_t g_img_data = 0;

// Strings for each of these states. Values must match APP_IMAGE_TASK_STATE_E in image_task.h
const char *imageTaskStateString[APP_IMAGE_TASK_STATE_NUMSTATES] = {
    "Uninitialised",
    "Init",
    "Setup Capture Start",
    "Setup Capture End",
    "Recapture Frame",
    "Capture Frame Ready",
    "Stop Capture Start",
    "Stop Capture End",
};

/********************************** Local Functions  *************************************/

/**
 * Initialises image processing variables
 */
void image_var_int(void)
{
    g_frame_ready = 0;
    g_cur_jpegenc_frame = 0;
    g_spi_master_initial_status = 0;
}
/********************************** Public Functions  *************************************/

/**
 * Creates the image task.
 *
 * The task itself initialises the Image sensor and then manages requests to access it.
 *
 * Not sure how big the stack needs to be...
 */
TaskHandle_t image_createTask(int8_t priority)
{
    if (priority < 0)
    {
        priority = 0;
    }

    xImageTaskQueue = xQueueCreate(IMAGE_TASK_QUEUE_LEN, sizeof(APP_MSG_T));
    if (xImageTaskQueue == 0)
    {
        xprintf("Failed to create xImageTaskQueue\n");
        configASSERT(0); // TODO add debug messages?
    }

    if (xTaskCreate(vImageTask, (const char *)"IMAGE",
                    3 * configMINIMAL_STACK_SIZE + CLI_CMD_LINE_BUF_SIZE + CLI_OUTPUT_BUF_SIZE,
                    NULL, priority,
                    &image_task_id) != pdPASS)
    {
        xprintf("Failed to create vImageTask\n");
        configASSERT(0); // TODO add debug messages?
    }

    return image_task_id;
}

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

/********************************** Private Function definitions  *************************************/

/**
 * FreeRTOS task responsible for handling interactions with the Image sensor.
 */
static void vImageTask(void *pvParameters)
{
    return 0;
}