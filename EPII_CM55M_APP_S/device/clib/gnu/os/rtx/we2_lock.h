#ifndef __WE2_LOCK_H__
#define __WE2_LOCK_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <cmsis_compiler.h>
#include "WE2_device.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Function prototypes -------------------------------------------------------*/
void Error_Handler(void);

static int rtx_kernel_started(void)
{
	  static uint8_t os_kernel_active = 0U;

	  if (os_kernel_active == 0U) {
	    if (osKernelGetState() > osKernelReady) {
	      os_kernel_active = 1U;
	    }
	  }
	  return (uint32_t)os_kernel_active;
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
  if (rtx_kernel_started())
  {
	  osKernelLock();
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
  if (rtx_kernel_started())
  {
	  osKernelUnlock();
  }
}



#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __WE2_LOCK_H__ */
