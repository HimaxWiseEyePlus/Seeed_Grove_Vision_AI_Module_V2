/*
 * inactivity.c
 *
 * This code detects inactivity and performs a callback when inactivity is detected.
 * The callback would typically switch to DPD.
 *
 * There are two approaches - only one of which I have tested (and probably I won't ever try the other)
 *
 * (1) USEIDLETASK  This relies on:
 * 		(a) a function inactivity_on_task_switched_in() called by FreeRTOS on every task switch.
 * 		(b) vApplicationIdleHook()
 *
 * (2) USETIMER  This relies on:
 * 		(a)	A timer set at init() which will call teh callback when it expires
 * 		(b) Every task calling inactivity_reset() to reset the timer, when it is active
 *
 *  Created on: 13 Apr 2025
 *      Author: charl
 */

/********************************** Includes ******************************************/


#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#include "inactivity.h"
#include "xprintf.h"

/**************************************** Definitions  *************************************/

// Uncomment one of these only
#define USEIDLETASK
//#define USETIMER

/**************************************** Local routine declarations  *************************************/

#ifdef USETIMER
static void inactivity_timer_callback(TimerHandle_t xTimer);
#endif // USETIMER


/**************************************** Local Variables **************************************/

// Static state
#ifdef USETIMER
static TimerHandle_t inactivity_timer = NULL;
#endif // USETIMER

static void (*inactivity_callback)(void) = NULL;
static TickType_t inactivity_timeout_ticks = 0;
static BaseType_t inactivity_enabled = pdFALSE;

// Idle hook state
static TickType_t idle_start_tick = 0;
static BaseType_t inactivity_triggered = pdFALSE;

/**************************************** Local function definitions  *************************************/

#ifdef USETIMER

/**
 * This is the local callback that executes when the timer expires
 *
 * It calls the function that was registered in the initialisation process.
 *
 */
static void inactivity_timer_callback(TimerHandle_t xTimer) {
    if (inactivity_callback) {
    	xprintf("Timer has timed out. ");
        inactivity_callback();
    }
}
#endif // USETIMER


/**************************************** Global function definitions  *************************************/

/**
 * Initialise a one-shot timer and start it.
 *
 * @param timeout_ms the duration of the timer.
 * @param callback - the function to call when it expires.
 */
void inactivity_init(uint32_t timeout_ms, void (*callback)(void)) {

    if (callback == NULL || timeout_ms == 0) {
        return; // Invalid input
    }

    inactivity_timeout_ticks = pdMS_TO_TICKS(timeout_ms);

    inactivity_callback = callback;
    inactivity_triggered = pdFALSE;
    idle_start_tick = 0;
    inactivity_enabled = pdTRUE;

#ifdef USETIMER

    if (inactivity_timer == NULL) {
        inactivity_timer = xTimerCreate("InactivityTimer",
                                        inactivity_timeout_ticks,
                                        pdFALSE,
                                        NULL,
                                        inactivity_timer_callback);
    }

    if (inactivity_timer != NULL) {
        xTimerStart(inactivity_timer, 0);
    }
#endif // USETIMER

}

#ifdef USETIMER

/**
 * Function called by each task to indicate that they are active.
 */
void inactivity_reset(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!inactivity_enabled) {
    	return;
    }

    // Reset idle tracking
    idle_start_tick = 0;
    inactivity_triggered = pdFALSE;

    if (xPortIsInsideInterrupt()) {
        xTimerResetFromISR(inactivity_timer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else {
        xTimerReset(inactivity_timer, 0);
    }
}
#else
void inactivity_reset(void) {
	// do nothing
}
#endif //USETIMER

#ifdef USEIDLETASK

/**
 * Called when FreeRTOS enters the idle state.
 *
 * This is called via freertos_app.c (more portable that way)
 *
 * If used this must be defined in FreeRTOSConfig.h as follows:
 * #define configUSE_IDLE_HOOK 1
 */
void inactivity_IdleHook(void) {
    TickType_t now;

    if (!inactivity_enabled || inactivity_timeout_ticks == 0) {
    	return;
    }

    now = xTaskGetTickCount();

    if (idle_start_tick == 0) {
        idle_start_tick = now;
    }

    if (!inactivity_triggered && (now - idle_start_tick >= inactivity_timeout_ticks)) {
        inactivity_triggered = pdTRUE;

        if (inactivity_callback) {
        	//xprintf("Idle hook. ");
            inactivity_callback();
        }
    }
}
#else
void inactivity_IdleHook(void) {
	// Do nothing
}
#endif	//USEIDLETASK


/**
 * Auto-reset idle tracking on any non-idle task switch-in
 *
 * If used this must be defined in FreeRTOSConfig.h as follows:
 *
 * #define traceTASK_SWITCHED_IN() inactivity_ on_task_switched_in()
 *
 */
#ifdef USEIDLETASK

void inactivity_on_task_switched_in(void) {
    if (!inactivity_enabled) {
    	return;
    }

    if (xTaskGetCurrentTaskHandle() != xTaskGetIdleTaskHandle()) {
        idle_start_tick = 0;
        inactivity_triggered = pdFALSE;
    }
}

#else
void inactivity_on_task_switched_in(void) {
	// do nothing
}
#endif	// USEIDLETASK
