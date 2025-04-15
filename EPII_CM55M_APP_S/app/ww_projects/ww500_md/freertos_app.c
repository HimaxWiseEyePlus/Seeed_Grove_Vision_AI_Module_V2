#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"

#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif

#include "xprintf.h"
#include "inactivity.h"

/**
 * Function called whenever a task is enabled.
 *
 * see FreeRTOSConfig.h
 */
void vApplicationTaskSwitchedIn(void) {
	inactivity_on_task_switched_in();
}

/**
 * Called when FreeRTOS enters the idle state.
 *
 * Call the user defined function from within the idle task.  This
 * allows the application designer to add background functionality
 * without the overhead of a separate task.
 *
 * NOTE: vApplicationIdleHook() MUST NOT, UNDER ANY CIRCUMSTANCES, CALL A FUNCTION THAT MIGHT BLOCK.
 *
 * If used this must be defined in FreeRTOSConfig.h as follows:
 * #define configUSE_IDLE_HOOK 1
 *
 * NOTE - NOT USED IN ww130_cli app, but it must be here if configUSE_IDLE_HOOK is defined
 */
void vApplicationIdleHook(void) {
	// For the ww500_md app this calls a function in inactivity.c
	inactivity_IdleHook();
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
	/* If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE + 100];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE + 100;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
		StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
	/* If the buffers to be provided to the Timer task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	 * task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

/**
 * Used if configCHECK_FOR_STACK_OVERFLOW is set to 1 or 2 in FreeRTOSConfig.h
 *
 * See https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/09-Memory-management/02-Stack-usage-and-stack-overflow-checking
 *
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	/* Silence warning about unused parameters. */
	(void) xTask;

	xprintf("Stack overflow in %s\n", pcTaskName);

	/* Force an assert. */
	configASSERT(pcTaskName == 0);
}

/*-----------------------------------------------------------*/

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress) {
	/* These are volatile to try and prevent the compiler/linker optimising them
	 * away as the variables never actually get used.  If the debugger won't show the
	 * values of the variables, make them global my moving their declaration outside
	 * of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr; /* Program status register. */

	r0 = pulFaultStackAddress[0];
	r1 = pulFaultStackAddress[1];
	r2 = pulFaultStackAddress[2];
	r3 = pulFaultStackAddress[3];

	r12 = pulFaultStackAddress[4];
	lr = pulFaultStackAddress[5];
	pc = pulFaultStackAddress[6];
	psr = pulFaultStackAddress[7];

	/* Remove compiler warnings about the variables not being used. */
	(void) r0;
	(void) r1;
	(void) r2;
	(void) r3;
	(void) r12;
	(void) lr; /* Link register. */
	(void) pc; /* Program counter. */
	(void) psr; /* Program status register. */

	/* When the following line is hit, the variables contain the register values. */
	for (;;) {
	}
}
/*-----------------------------------------------------------*/

#if defined(__GNUC)
/**
 * @brief The fault handler implementation calls a function called
 * prvGetRegistersFromStack().
 */
void MemManage_Handler(void)
{
    __asm volatile(
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, handler2_address_const                            \n"
        " bx r1                                                     \n"
        "                                                           \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n");
}
/*-----------------------------------------------------------*/
#endif
