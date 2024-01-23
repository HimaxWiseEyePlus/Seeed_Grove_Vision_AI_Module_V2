/*
 * retarget_arm.c
 *
 *  Created on: 2023¦~11¤ë22¤é
 *      Author: 902447
 */
#include <stdint.h>
#include <stddef.h>
#include "cmsis_compiler.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "WE2_device.h"

static StaticSemaphore_t ctor_mutex_storage;
static SemaphoreHandle_t ctor_mutex_handle;


static int we2_os_kernel_started(void)
{
    static int rtos_started = 0;
    if (!rtos_started)
    {
        rtos_started = xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
    }
    return rtos_started;
}


static void we2_os_check_in_isr(void)
{
    if (0 != __get_IPSR())
    {
        __BKPT(0);
    }
}

void _platform_post_stackheap_init (void) {
  /* Initialize OS, memory, etc. */
  #if defined(RTE_Compiler_EventRecorder)
    EvrFreeRTOSSetup(0);
  #endif
     ctor_mutex_handle =
        xSemaphoreCreateRecursiveMutexStatic(&ctor_mutex_storage);
}

void we2_mutex_pool_acquire(SemaphoreHandle_t m)
{
    we2_os_check_in_isr();
    if (we2_os_kernel_started())
    {
        while (xSemaphoreTakeRecursive(m, 10000) != pdTRUE)
        {
            // Halt here until condition is false
        }
    }
}

void we2_mutex_pool_release(SemaphoreHandle_t m)
{
    we2_os_check_in_isr();
    if (we2_os_kernel_started())
    {
        xSemaphoreGiveRecursive(m);
    }
}


void we2_mutex_pool_destroy(SemaphoreHandle_t m)
{
    we2_os_check_in_isr();
    vSemaphoreDelete(m);
}



typedef struct
{
    uint8_t initialized;
    uint8_t acquired;
} cy_cxa_guard_object_t;


__STATIC_INLINE uint8_t we2_atomic_load(uint8_t* address)
{
    __DMB();
    uint8_t result = *address;
    __DMB();
    return result;
}


__STATIC_INLINE void we2_atomic_store(uint8_t* address, uint8_t value)
{
    __DMB();
    *address = value;
    __DMB();
}


//--------------------------------------------------------------------------------------------------
// __cxa_guard_acquire
//--------------------------------------------------------------------------------------------------
int __cxa_guard_acquire(cy_cxa_guard_object_t* guard_object)
{
    int acquired = 0;
    if (0 == we2_atomic_load(&guard_object->initialized))
    {
        we2_mutex_pool_acquire(ctor_mutex_handle);

        if (0 == we2_atomic_load(&guard_object->initialized))
        {
            acquired = 1;
            guard_object->acquired = 1;
        }
        else
        {
            we2_mutex_pool_release(ctor_mutex_handle);
        }
    }
    return acquired;
}


//--------------------------------------------------------------------------------------------------
// __cxa_guard_abort
//--------------------------------------------------------------------------------------------------
void __cxa_guard_abort(cy_cxa_guard_object_t* guard_object)
{
    if (guard_object->acquired)
    {
        guard_object->acquired = 0;
        we2_mutex_pool_release(ctor_mutex_handle);
    }
    #ifndef NDEBUG
    else
    {
        __BKPT(0);  // __cxa_guard_abort called when not acquired
    }
    #endif
}


//--------------------------------------------------------------------------------------------------
// __cxa_guard_release
//--------------------------------------------------------------------------------------------------
void __cxa_guard_release(cy_cxa_guard_object_t* guard_object)
{
    we2_atomic_store(&guard_object->initialized, 1);
    __cxa_guard_abort(guard_object);    // Release mutex
}

