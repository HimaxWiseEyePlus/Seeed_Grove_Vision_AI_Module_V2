/*
 * retarget_arm.c
 *
 *  Created on: 2023¦~11¤ë22¤é
 *      Author: 902447
 */
#include <stdint.h>
#include <stddef.h>
#include "cmsis_compiler.h"
#include "rtx_os.h"
#include "cmsis_os2.h"
#include "WE2_device.h"

static osRtxMutex_t singleton_mutex_obj;
static osMutexId_t singleton_mutex_id;


static int we2_os_kernel_started(void)
{
	  static uint8_t os_kernel_active = 0U;

	  if (os_kernel_active == 0U) {
	    if (osKernelGetState() > osKernelReady) {
	      os_kernel_active = 1U;
	    }
	  }
	  return (uint32_t)os_kernel_active;
}


static void we2_os_check_in_isr(void)
{
    if (0 != __get_IPSR())
    {
        __BKPT(0);
    }
}


void _platform_post_stackheap_init (void) {
  (void)osKernelInitialize();

  const osMutexAttr_t singleton_mutex_attr = {
      .name = "singleton_mutex",
      .attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust,
      .cb_size = sizeof(singleton_mutex_obj),
      .cb_mem = &singleton_mutex_obj
  };
  singleton_mutex_id = osMutexNew(&singleton_mutex_attr);
}



void we2_mutex_pool_acquire()
{
    we2_os_check_in_isr();
    if (we2_os_kernel_started())
    {
        if (!singleton_mutex_id) {
            // RTOS has not booted yet so no mutex is needed
            return;
        }
        osMutexAcquire(singleton_mutex_id, osWaitForever);
    }
}

void we2_mutex_pool_release()
{
    we2_os_check_in_isr();
    if (we2_os_kernel_started())
    {
        if (!singleton_mutex_id) {
            // RTOS has not booted yet so no mutex is needed
            return;
        }
        osMutexRelease(singleton_mutex_id);
    }
}


void we2_mutex_pool_destroy()
{
    we2_os_check_in_isr();
    osMutexDelete(singleton_mutex_id);
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
        we2_mutex_pool_acquire();

        if (0 == we2_atomic_load(&guard_object->initialized))
        {
            acquired = 1;
            guard_object->acquired = 1;
        }
        else
        {
            we2_mutex_pool_release();
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
        we2_mutex_pool_release();
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

