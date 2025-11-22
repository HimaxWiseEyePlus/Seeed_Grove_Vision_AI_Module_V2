/*
 * barrier.h - FreeRTOS Task Barrier
 *
 *  Created on: 19 Nov 2025
 *      Author: charl
 *
 *
 * This module provides a simple barrier mechanism for FreeRTOS tasks.
 *
 * Concept:
 * ----------
 * A barrier allows multiple tasks to synchronize at a known point in their
 * execution. Each task calls Barrier_Ready() when it has reached the "ready"
 * point (typically after initialization but before entering the main task loop).
 *
 * The barrier tracks how many tasks have reached this point using an internal
 * counter and a mutex to ensure thread safety.
 *
 * Features:
 * ----------
 * - The barrier supports any number of tasks.
 * - The last task to reach the barrier automatically calls a user-provided
 *   callback function (e.g., systemAllTasksReady()) exactly once.
 * - Mutex-protected counter ensures safe concurrent access from multiple tasks.
 * - The callback is called **outside** the mutex to prevent deadlocks.
 * - Fully portable across FreeRTOS ports and MCU architectures.
 *
 * Usage:
 * -------
 * 1. Define a global Barrier_t variable:
 *       Barrier_t startupBarrier;
 *
 * 2. Initialize the barrier before starting tasks:
 *       Barrier_Init(&startupBarrier, TOTAL_TASKS, systemAllTasksReady);
 *
 * 3. In each task, after completing initialization, call:
 *       Barrier_Ready(&startupBarrier);
 *
 *    This signals the task has reached the ready point. The last task to call
 *    Barrier_Ready() will trigger the callback.
 *
 * Notes:
 * -------
 * - Only **one definition** of the Barrier_t variable should exist; other
 *   files can declare it with `extern`.
 * - No dedicated supervisor task is required; the barrier is fully handled
 *   by the tasks themselves.
 * - Ensure TOTAL_TASKS matches the number of tasks that will call Barrier_Ready().
 *
 * Example:
 * ----------
 *   // main.c
 *   Barrier_t startupBarrier;
 *   Barrier_Init(&startupBarrier, 5, systemAllTasksReady);
 *
 *   // In each task
 *   Barrier_Ready(&startupBarrier);
 *
 */


#ifndef BARRIER_H
#define BARRIER_H

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*BarrierCallback_t)(void);

typedef struct {
    uint32_t totalTasks;
    uint32_t readyCount;
    bool callbackCalled;
    SemaphoreHandle_t mutex;
    BarrierCallback_t callback;
} Barrier_t;

void barrier_init(Barrier_t *b, uint32_t totalTasks, BarrierCallback_t cb);
void barrier_ready(Barrier_t *b);

#endif /* BARRIER_H */
