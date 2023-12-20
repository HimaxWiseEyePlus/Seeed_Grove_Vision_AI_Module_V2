#include "hello_world_freertos_tz_s_only.h"

#define FREERTOS

#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif

#ifdef TRUSTZONE_SEC
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
#ifdef NSC
#include "veneer_table.h"
#endif
/* Trustzone config. */

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif
#endif

/* Task priorities. */
#define hello_task1_PRIORITY	(configMAX_PRIORITIES - 1)
#define hello_task2_PRIORITY	(configMAX_PRIORITIES - 1)

#include "xprintf.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static void hello_task1(void *pvParameters);
static void hello_task2(void *pvParameters);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int app_main(void)
{
    printf("Task creation Hello world. No MPU\r\n");

    if ( xTaskCreate(hello_task1, "Hello_task1", 512, NULL, hello_task1_PRIORITY, NULL) != pdPASS )
    {
        printf("Hello_task1 creation failed!.\r\n");
        while (1)
        	;
    }

    if ( xTaskCreate(hello_task2, "Hello_task2", 512, NULL, hello_task2_PRIORITY, NULL) != pdPASS )
    {
        printf("Hello_task2 creation failed!.\r\n");
        while (1)
            ;
    }

    vTaskStartScheduler();

    for (;;);
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task1(void *pvParameters)
{
    for (;;)
    {
    	printf("Hello world freertos task1.\r\n");
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

static void hello_task2(void *pvParameters)
{
    for (;;)
    {
    	printf("Hello world freertos task2.\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

