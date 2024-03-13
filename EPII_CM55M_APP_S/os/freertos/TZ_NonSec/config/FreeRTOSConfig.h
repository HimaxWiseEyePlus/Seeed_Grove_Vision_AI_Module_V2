#if !defined(HX_TFM)
/*
 * FreeRTOS Pre-Release V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/******************************************************************************
    See http://www.freertos.org/a00110.html for an explanation of the
    definitions contained in this file.
******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 * http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

//#define configENABLE_MPU                                    0
#define configENABLE_FPU                                    1
#define configENABLE_TRUSTZONE                              1
#define configRUN_FREERTOS_SECURE_ONLY                      0

/* Constants related to the behaviour or the scheduler. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION             0
#define configUSE_PREEMPTION                                1
#define configUSE_TIME_SLICING                              1
#define configMAX_PRIORITIES                                ( 5 )
#define configIDLE_SHOULD_YIELD                             1
#define configUSE_16_BIT_TICKS                              0 /* Only for 8 and 16-bit hardware. */

/* Constants that describe the hardware and memory usage. */
#define configCPU_CLOCK_HZ                                  SystemCoreClock
#define configMINIMAL_STACK_SIZE                            ( ( uint16_t )90)
#define configMINIMAL_SECURE_STACK_SIZE                     ( 256 )
#define configMAX_TASK_NAME_LEN                             ( 12 )
#define configTOTAL_HEAP_SIZE                               ( ( size_t ) ( 50 * 1024 ) )

/* Constants that build features in or out. */
#define configUSE_MUTEXES                                   1
#define configUSE_TICKLESS_IDLE                             0
#define configUSE_APPLICATION_TASK_TAG                      0
#ifdef __GNU__
#define configUSE_NEWLIB_REENTRANT            1
#else
#define configUSE_NEWLIB_REENTRANT            0
#endif
#define configUSE_CO_ROUTINES                               0
#define configUSE_COUNTING_SEMAPHORES                       1
#define configUSE_RECURSIVE_MUTEXES                         1
#define configUSE_QUEUE_SETS                                0
#define configUSE_TASK_NOTIFICATIONS                        1

/* Constants that define which hook (callback) functions should be used. */
#define configUSE_IDLE_HOOK                                 0
#define configUSE_TICK_HOOK                                 0
#define configUSE_MALLOC_FAILED_HOOK                        0

/* Constants provided for debugging and optimisation assistance. */
#define configCHECK_FOR_STACK_OVERFLOW                      0
#define configASSERT( x )                                   if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }
#define configQUEUE_REGISTRY_SIZE                           0

/* Software timer definitions. */
#define configUSE_TIMERS                                    1
#define configTIMER_TASK_PRIORITY                           ( 3 )
#define configTIMER_QUEUE_LENGTH                            5
#define configTIMER_TASK_STACK_DEPTH                        ( configMINIMAL_STACK_SIZE )

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.  NOTE:  Setting an INCLUDE_ parameter to 0 is only
 * necessary if the linker does not automatically remove functions that are not
 * referenced anyway. */
#define INCLUDE_vTaskPrioritySet                            1
#define INCLUDE_uxTaskPriorityGet                           1
#define INCLUDE_vTaskDelete                                 1
#define INCLUDE_vTaskCleanUpResources                       0
#define INCLUDE_vTaskSuspend                                1
#define INCLUDE_vTaskDelayUntil                             1
#define INCLUDE_vTaskDelay                                  1
#define INCLUDE_uxTaskGetStackHighWaterMark                 0
#define INCLUDE_xTaskGetIdleTaskHandle                      0
#define INCLUDE_eTaskGetState                               1
#define INCLUDE_xTaskResumeFromISR                          0
#define INCLUDE_xTaskGetCurrentTaskHandle                   1
#define INCLUDE_xTaskGetSchedulerState                      0
#define INCLUDE_xSemaphoreGetMutexHolder                    0
#define INCLUDE_xTimerPendFunctionCall                      1

/* This demo makes use of one or more example stats formatting functions.  These
 * format the raw data provided by the uxTaskGetSystemState() function in to human
 * readable ASCII form.  See the notes in the implementation of vTaskList() within
 * FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS                1

/* Dimensions a buffer that can be used by the FreeRTOS+CLI command
 * interpreter.  See the FreeRTOS+CLI documentation for more information:
 * http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/ */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE                   2048


/* The trace facility is turned on to make some functions available for use in
 * CLI commands. */
#define configUSE_TRACE_FACILITY                            1
/* Tasks.c additions (e.g. Thread Aware Debug capability) */
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 1

/* The #ifdef guards against the file being included from IAR assembly files. */
#ifndef __IASMARM__
        #define configGENERATE_RUN_TIME_STATS               0
        #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
        #define portGET_RUN_TIME_COUNTER_VALUE()            0
        #define configTICK_RATE_HZ                          ( ( TickType_t ) 1000 )
#endif /* __IASMARM__ */

/* Enable static allocation. */
#define configSUPPORT_STATIC_ALLOCATION                 1



#if defined(__ICCARM__)||defined(__CC_ARM)||defined(__GNUC__)
    /* Clock manager provides in this variable system core clock frequency */
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
#endif


#ifndef configENABLE_FPU
  #define configENABLE_FPU                        0
#endif
#ifndef configENABLE_MPU
  #define configENABLE_MPU                        0
#endif
#ifndef configENABLE_TRUSTZONE
  #define configENABLE_TRUSTZONE                  0
#endif
#ifndef configRUN_FREERTOS_SECURE_ONLY
  #define configRUN_FREERTOS_SECURE_ONLY          1
#endif

/* Interrupt nesting behaviour configuration. Cortex-M specific. */
#ifdef __NVIC_PRIO_BITS
/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 3 /* 8 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY ((1U << (configPRIO_BITS)) - 1)

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 2

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define vPortPendSVHandler PendSV_Handler
#define vPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

#else //#if !defined(HX_TFM)
/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * --------------------------------------------------------------------------
 *
 * $Revision:   V10.2.0
 *
 * Project:     CMSIS-FreeRTOS
 * Title:       FreeRTOS configuration definitions
 *
 * --------------------------------------------------------------------------*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#if (defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__))
#include <stdint.h>

#if 0
#include "RTE_Components.h"
#include CMSIS_device_header
#else
#include "WE2_device.h"
#endif

#endif

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//  <o>Minimal stack size [words] <0-65535>
//  <i> Stack for idle task and default task stack in words.
//  <i> Default: 128
#define configMINIMAL_STACK_SIZE                ((uint16_t)(128))

//  <o>Total heap size [bytes] <0-0xFFFFFFFF>
//  <i> Heap memory size in bytes.
//  <i> Default: 8192
//#define configTOTAL_HEAP_SIZE                   ((size_t)8192)
#define configTOTAL_HEAP_SIZE                   (( size_t )(30 * 1024 ))
//FreeRTOS TaskDelete cannot delete task itself,
//so configTOTAL_HEAP_SIZE worst case is tfm_ps_run_test TFM_NS_PS_TEST_1001 ~ 1025 x PS_TEST_TASK_STACK_SIZE
//tasks.c 
// A task is deleting itself. This cannot complete within the
// task itself, as a context switch to another task is required.
// Place the task in the termination list


//  <o>Kernel tick frequency [Hz] <0-0xFFFFFFFF>
//  <i> Kernel tick rate in Hz.
//  <i> Default: 1000
#define configTICK_RATE_HZ                      ((TickType_t)1000)

//  <o>Timer task stack depth [words] <0-65535>
//  <i> Stack for timer task in words.
//  <i> Default: 80
#define configTIMER_TASK_STACK_DEPTH            80

//  <o>Timer task priority <0-56>
//  <i> Timer task priority.
//  <i> Default: 40 (High)
#define configTIMER_TASK_PRIORITY               40

//  <o>Timer queue length <0-1024>
//  <i> Timer command queue length.
//  <i> Default: 5
#define configTIMER_QUEUE_LENGTH                5

//  <o>Preemption interrupt priority
//  <i> Maximum priority of interrupts that are safe to call FreeRTOS API.
//  <i> Default: 16
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    16

//  <q>Use time slicing
//  <i> Enable setting to use timeslicing.
//  <i> Default: 1
#define configUSE_TIME_SLICING                  1

//  <q>Idle should yield
//  <i> Control Yield behaviour of the idle task.
//  <i> Default: 1
#define configIDLE_SHOULD_YIELD                 1

//  <o>Check for stack overflow
//    <0=>Disable <1=>Method one <2=>Method two
//  <i> Enable or disable stack overflow checking.
//  <i> Callback function vApplicationStackOverflowHook implementation is required when stack checking is enabled.
//  <i> Default: 0
#define configCHECK_FOR_STACK_OVERFLOW          2

//  <q>Use idle hook
//  <i> Enable callback function call on each idle task iteration.
//  <i> Callback function vApplicationIdleHook implementation is required when idle hook is enabled.
//  <i> Default: 0
#define configUSE_IDLE_HOOK                     0

//  <q>Use tick hook
//  <i> Enable callback function call during each tick interrupt.
//  <i> Callback function vApplicationTickHook implementation is required when tick hook is enabled.
//  <i> Default: 0
#define configUSE_TICK_HOOK                     0

//  <q>Use deamon task startup hook
//  <i> Enable callback function call when timer service starts.
//  <i> Callback function vApplicationDaemonTaskStartupHook implementation is required when deamon task startup hook is enabled.
//  <i> Default: 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

//  <q>Use malloc failed hook
//  <i> Enable callback function call when out of dynamic memory.
//  <i> Callback function vApplicationMallocFailedHook implementation is required when malloc failed hook is enabled.
//  <i> Default: 0
#define configUSE_MALLOC_FAILED_HOOK            1//0

//  <o>Queue registry size
//  <i> Define maximum number of queue objects registered for debug purposes.
//  <i> The queue registry is used by kernel aware debuggers to locate queue and semaphore structures and display associated text names.
//  <i> Default: 0
#define configQUEUE_REGISTRY_SIZE               0

// <h>Event Recorder configuration
//  <i> Initialize and setup Event Recorder level filtering.
//  <i> Settings have no effect when Event Recorder is not present.

//  <q>Initialize Event Recorder
//  <i> Initialize Event Recorder before FreeRTOS kernel start.
//  <i> Default: 1
#define configEVR_INITIALIZE                    1

//  <e>Setup recording level filter
//  <i> Enable configuration of FreeRTOS events recording level
//  <i> Default: 1
#define configEVR_SETUP_LEVEL                   1

//  <o>Tasks functions
//  <i> Define event recording level bitmask for events generated from Tasks functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_TASKS                   0x05

//  <o>Queue functions
//  <i> Define event recording level bitmask for events generated from Queue functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_QUEUE                   0x05

//  <o>Timer functions
//  <i> Define event recording level bitmask for events generated from Timer functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_TIMERS                  0x05

//  <o>Event Groups functions
//  <i> Define event recording level bitmask for events generated from Event Groups functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_EVENTGROUPS             0x05

//  <o>Heap functions
//  <i> Define event recording level bitmask for events generated from Heap functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_HEAP                    0x05

//  <o>Stream Buffer functions
//  <i> Define event recording level bitmask for events generated from Stream Buffer functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_STREAMBUFFER            0x05
//  </e>
// </h>

// <h> Port Specific Features
// <i> Enable and configure port specific features.
// <i> Check FreeRTOS documentation for definitions that apply for the used port.

//  <q>Use Floating Point Unit
//  <i> Using Floating Point Unit (FPU) affects context handling.
//  <i> Enable FPU when application uses floating point operations.
//  <i> Default: 1
#define configENABLE_FPU                        0

//  <q>Use Memory Protection Unit
//  <i> Using Memory Protection Unit (MPU) requires detailed memory map definition.
//  <i> This setting is only releavant for MPU enabled ports.
//  <i> Default: 0
//#define configENABLE_MPU                      0
//YX 20230409 marked #define configENABLE_MPU for makefile os.mk OS_DEFINES += -DconfigENABLE_MPU=1

//  <q> Use TrustZone Secure Side Only
//  <i> This settings prevents FreeRTOS contex switch to Non-Secure side.
//  <i> Enable this setting when FreeRTOS runs on the Secure side only.
#define configRUN_FREERTOS_SECURE_ONLY        0

//  <q>Use TrustZone Security Extension
//  <i> Using TrustZone affects context handling.
//  <i> Enable TrustZone when FreeRTOS runs on the Non-Secure side and calls functions from the Secure side.
//  <i> Default: 1
#define configENABLE_TRUSTZONE                1

//  <o>Minimal secure stack size [words] <0-65535>
//  <i> Stack for idle task Secure side context in words.
//  <i> This setting is only relevant when TrustZone extension is enabled.
//  <i> Default: 128
#define configMINIMAL_SECURE_STACK_SIZE       ((uint32_t)256)
// </h>

//------------- <<< end of configuration section >>> ---------------------------

/* Defines needed by FreeRTOS to implement CMSIS RTOS2 API. Do not change! */
#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configUSE_PREEMPTION                    1
#define configUSE_TIMERS                        1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configMAX_PRIORITIES                    56
#define configKERNEL_INTERRUPT_PRIORITY         255

/* Defines that include FreeRTOS functions which implement CMSIS RTOS2 API. Do not change! */
#define INCLUDE_xEventGroupSetBitsFromISR       1
#define INCLUDE_xSemaphoreGetMutexHolder        1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTimerPendFunctionCall          1

/* Map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define xPortPendSVHandler                      PendSV_Handler
#define vPortSVCHandler                         SVC_Handler

/* Ensure Cortex-M port compatibility. */
#define SysTick_Handler                         xPortSysTickHandler

#if (defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__))
/* Include debug event definitions */
#include "freertos_evr.h"
#endif

//YX 20230409 add after Keil Configuration Wizard

//#define configGENERATE_RUN_TIME_STATS             0
//YX 20230409 marked #define configGENERATE_RUN_TIME_STATS for makefile os.mk OS_FREERTOS_DEFINES += -DconfigGENERATE_RUN_TIME_STATS=1

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()          0

#endif /* FREERTOS_CONFIG_H */

#endif //#if !defined(HX_TFM)
