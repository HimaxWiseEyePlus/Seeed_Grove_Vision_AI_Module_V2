/*
 * barrier.c
 *
 * This module provides a simple barrier mechanism for FreeRTOS tasks.
 * See barrier.h for a description
 *
 *  Created on: 19 Nov 2025
 *      Author: charl
 */


#include "barrier.h"

/**
 * Called once to initialise the barrier mechanism
 *
 * @param b - pointer to a Barrier_t object
 * @param totalTasks - the number of tasks that must call barrier_ready() before the callback is invoked.
 * @param cb - function to call.
 */
void barrier_init(Barrier_t *b, uint32_t totalTasks, BarrierCallback_t cb) {
    b->totalTasks = totalTasks;
    b->readyCount = 0;
    b->callbackCalled = false;
    b->callback = cb;

    b->mutex = xSemaphoreCreateMutex();
    configASSERT(b->mutex != NULL);
}

/**
 * Called by each task when it is ready.
 *
 * When all tasks have called this function, the callback is executed.
 */
void barrier_ready(Barrier_t *b) {
    xSemaphoreTake(b->mutex, portMAX_DELAY);

    b->readyCount++;

    bool callNow =
        (b->readyCount == b->totalTasks) &&
        (!b->callbackCalled);

    if (callNow) {
        b->callbackCalled = true;
    }

    xSemaphoreGive(b->mutex);

    /* Call outside the mutex to avoid deadlocks or priority inversions. */
    if (callNow && b->callback) {
        b->callback();
    }
}


