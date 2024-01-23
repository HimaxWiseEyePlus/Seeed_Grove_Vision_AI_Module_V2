/*
 * retarget_newlib.c
 *
 *  Created on: 2023¦~11¤ë7¤é
 *      Author: 902447
 */



#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/lock.h>
#include <stdatomic.h>
#include "we2_lock.h"

/**
  * @brief Global Error_Handler
  */
__WEAK void Error_Handler(void)
{
  /* Not used if it exists in project */
  while (1);
}

/**
  * @defgroup _newlib_lock_functions newlib library locks
  * @see https://sourceware.org/newlib/libc.html
  * @{
  */
/* Private macros ------------------------------------------------------------*/
/** See struct __lock definition */
#define WE2_LOCK_PARAMETER(lock) (&(lock)->lock_data)

/* Private variables ---------------------------------------------------------*/
struct __lock
{
  LockingData_t lock_data; /**< The STM32 lock instance */
};

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___sinit_recursive_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___sfp_recursive_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___atexit_recursive_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___at_quick_exit_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___malloc_recursive_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___env_recursive_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___tz_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___dd_hash_mutex = { LOCKING_DATA_INIT };

/** Implementing mutex from <a href="https://sourceware.org/git/?p=newlib-cygwin.git;a=blob_plain;f=newlib/libc/misc/lock.c">newlib/libc/misc/lock.c</a> */
struct __lock __lock___arc4random_mutex = { LOCKING_DATA_INIT };

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Initialize lock
  * @param lock The lock
  */
void __retarget_lock_init(_LOCK_T *lock)
{
  __retarget_lock_init_recursive(lock);
}

/**
  * @brief Initialize recursive lock
  * @param lock The lock
  */
void __retarget_lock_init_recursive(_LOCK_T *lock)
{
  if (lock == NULL)
  {
    errno = EINVAL;
    return;
  }

  *lock = (_LOCK_T)malloc(sizeof(struct __lock));
  if (*lock != NULL)
  {
    we2_lock_init(WE2_LOCK_PARAMETER(*lock));
    return;
  }

  /* Unable to allocate memory */
  WE2_LOCK_BLOCK();
}

/**
  * @brief Close lock
  * @param lock The lock
  */
void __retarget_lock_close(_LOCK_T lock)
{
  __retarget_lock_close_recursive(lock);
}

/**
  * @brief Close recursive lock
  * @param lock The lock
  */
void __retarget_lock_close_recursive(_LOCK_T lock)
{
  free(lock);
}

/**
  * @brief Acquire lock
  * @param lock The lock
  */
void __retarget_lock_acquire(_LOCK_T lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
  we2_lock_acquire(WE2_LOCK_PARAMETER(lock));
}

/**
  * @brief Acquire recursive lock
  * @param lock The lock
  */
void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
  we2_lock_acquire(WE2_LOCK_PARAMETER(lock));
}

/**
  * @brief Try acquire lock
  * @param lock The lock
  * @return 0 always
  */
int __retarget_lock_try_acquire(_LOCK_T lock)
{
  __retarget_lock_acquire(lock);
  return 0;
}

/**
  * @brief Try acquire recursive lock
  * @param lock The lock
  * @return 0 always
  */
int __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
  __retarget_lock_acquire_recursive(lock);
  return 0;
}

/**
  * @brief Release lock
  * @param lock The lock
  */
void __retarget_lock_release(_LOCK_T lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
  we2_lock_release(WE2_LOCK_PARAMETER(lock));
}

/**
  * @brief Release recursive lock
  * @param lock The lock
  */
void __retarget_lock_release_recursive(_LOCK_T lock)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(lock);
  we2_lock_release(WE2_LOCK_PARAMETER(lock));
}

/**
  * @brief Acquire malloc lock
  * @param reent The reentrance struct
  */
void __malloc_lock(struct _reent *reent)
{
  WE2_LOCK_UNUSED(reent);
  we2_lock_acquire(&__lock___malloc_recursive_mutex.lock_data);
}

/**
  * @brief Release malloc lock
  * @param reent The reentrance struct
  */
void __malloc_unlock(struct _reent *reent)
{
  WE2_LOCK_UNUSED(reent);
  we2_lock_release(&__lock___malloc_recursive_mutex.lock_data);

}


/* Private typedef -----------------------------------------------------------*/
/** The guard object is created by the C++ compiler and is 32 bit for ARM EABI. */
typedef struct
{
  atomic_uchar initialized; /**< Indicate if object is initialized */
  uint8_t acquired; /**< Ensure non-recursive lock */
  uint16_t unused; /**< Padding */
} __attribute__((packed)) CxaGuardObject_t;

/* Private variables ---------------------------------------------------------*/
/** Mutex used in __cxa_guard_acquire, __cxa_guard_release and __cxa_guard_abort */
static LockingData_t __cxa_guard_mutex = LOCKING_DATA_INIT;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Acquire __cxa_guard mutex
  * @param guard_object Guard object
  * @return 0 if object is initialized, else initialization of object required
  */
int __cxa_guard_acquire(CxaGuardObject_t *guard_object)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(guard_object);

  if (atomic_load(&guard_object->initialized) == 0)
  {
    /* Object needs initialization, lock threading context */
    we2_lock_acquire(&__cxa_guard_mutex);
    if (atomic_load(&guard_object->initialized) == 0)
    {
      /* Object needs initialization */
      if (guard_object->acquired)
      {
        /* Object initialization already in progress */
        WE2_LOCK_BLOCK();
      }

      /* Lock acquired */
      guard_object->acquired = 1;
      return 1;
    }
    else
    {
      /* Object initialized in another thread */
      we2_lock_release(&__cxa_guard_mutex);
    }
  }

  /* Object already initialized */
  return 0;
}

/**
  * @brief Abort __cxa_guard mutex
  * @param guard_object Guard object
  */
void __cxa_guard_abort(CxaGuardObject_t *guard_object)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(guard_object);

  if (guard_object->acquired)
  {
    /* Release lock */
    guard_object->acquired = 0;
    we2_lock_release(&__cxa_guard_mutex);
  }
  else
  {
    /* Trying to release non-acquired lock */
    WE2_LOCK_BLOCK();
  }
}

/**
  * @brief Release __cxa_guard mutex
  * @param guard_object Guard object
  */
void __cxa_guard_release(CxaGuardObject_t *guard_object)
{
  WE2_CHECK_ARGUMENT_LOCK_BLOCK(guard_object);

  /* Object initialized */
  atomic_store(&guard_object->initialized, 1);

  /* Release lock */
  __cxa_guard_abort(guard_object);
}

