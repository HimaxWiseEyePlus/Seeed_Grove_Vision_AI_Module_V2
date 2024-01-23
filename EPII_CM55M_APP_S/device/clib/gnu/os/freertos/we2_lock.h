#ifndef __WE2_LOCK_H__
#define __WE2_LOCK_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <cmsis_compiler.h>
#include "WE2_device.h"
#include <FreeRTOS.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Function prototypes -------------------------------------------------------*/
void Error_Handler(void);

static int freertos_kernel_started(void)
{
    static int rtos_started = 0;
    if (!rtos_started)
    {
        rtos_started = xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
    }
    return rtos_started;
}


/* Public macros -------------------------------------------------------------*/
/** Blocks execution */
#define WE2_LOCK_BLOCK()                      \
  do                                            \
  {                                             \
    __disable_irq();                            \
    Error_Handler();                            \
    while (1);                                  \
  } while (0)

/** Blocks execution if argument is NULL */
#define WE2_CHECK_ARGUMENT_LOCK_BLOCK(x)    \
  do                                            \
  {                                             \
    if ((x) == NULL)                            \
    {                                           \
      WE2_LOCK_BLOCK();                       \
    }                                           \
  } while (0)

/** Blocks execution if in interrupt context */
#define WE2_INTERRUPT_CONTEXT_LOCK_BLOCK() \
  do                                            \
  {                                             \
    if (__get_IPSR())                           \
    {                                           \
      WE2_LOCK_BLOCK();                       \
    }                                           \
  } while (0)

/** Hide unused parameter warning from compiler */
#define WE2_LOCK_UNUSED(var) (void)var

/* Includes ----------------------------------------------------------------*/
#if defined (__GNUC__) && !defined (__CC_ARM) && configUSE_NEWLIB_REENTRANT == 0
#warning Please set configUSE_NEWLIB_REENTRANT to 1 in FreeRTOSConfig.h, otherwise newlib will not be thread-safe
#endif /* defined (__GNUC__) && !defined (__CC_ARM) && configUSE_NEWLIB_REENTRANT == 0 */

/* Private defines ---------------------------------------------------------*/
/** Initialize members in instance of <code>LockingData_t</code> structure */
#define LOCKING_DATA_INIT 0

/* Private typedef ---------------------------------------------------------*/
typedef uint8_t LockingData_t;  /**< Unused */

/** Hide unused parameter warning from compiler */
#define WE2_LOCK_UNUSED(var) (void)var

/* Private functions -------------------------------------------------------*/

/**
  * @brief Initialize WE2 lock
  * @param lock The lock to init
  */
static inline void we2_lock_init(LockingData_t *lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
}

/**
  * @brief Acquire WE2 lock
  * @param lock The lock to acquire
  */
static inline void we2_lock_acquire(LockingData_t *lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
  WE2_INTERRUPT_CONTEXT_LOCK_BLOCK();
  if (freertos_kernel_started())
  {
	  vTaskSuspendAll();
  }
}

/**
  * @brief Release WE2 lock
  * @param lock The lock to release
  */
static inline void we2_lock_release(LockingData_t *lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
  WE2_INTERRUPT_CONTEXT_LOCK_BLOCK();
  if (freertos_kernel_started())
  {
	  xTaskResumeAll();
  }
}



#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __WE2_LOCK_H__ */
