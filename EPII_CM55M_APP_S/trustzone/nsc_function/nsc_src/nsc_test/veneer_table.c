
/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include <stdio.h>
#include "stdint.h"
#include "arm_cmse.h"
#include "veneer_table.h"
#include "hx_drv_scu.h"
#include "hx_drv_mpc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_STRING_LENGTH 0x400
typedef int (*callbackptr_NS)(char const *s1, char const *s2) __attribute__((cmse_nonsecure_call));

#define secureportNON_SECURE_CALLABLE			__attribute__((cmse_nonsecure_entry)) __attribute__((used))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Counter returned from NSCFunction. */
static uint32_t ulSecureCounter = 0;

/**
 * @brief typedef for non-secure callback.
 */
#if defined(__IAR_SYSTEMS_ICC__)
typedef __cmse_nonsecure_call void (*NonSecureCallback_t)(void);
#else
typedef void (*NonSecureCallback_t)(void) __attribute__((cmse_nonsecure_call));
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/
/* strnlen function implementation for arm compiler */
#if defined(__arm__)
size_t strnlen(const char *s, size_t maxLength) {
	size_t length = 0;
	while ((length <= maxLength) && (*s)) {
		s++;
		length++;
	}
	return length;
}
#endif

/* Non-secure callable (entry) function, calling a non-secure callback function */
__attribute__((cmse_nonsecure_entry))  uint32_t StringCompare_NSE(
		volatile callbackptr callback, char const *s1, char const *s2) {
	callbackptr_NS callback_NS;
	size_t string_length;
	int result;

	/* Input parameters check */
	/* Check whether function pointer is located in non-secure memory */
	callback_NS = (callbackptr_NS) cmse_nsfptr_create(callback);
	if (cmse_check_pointed_object((int*) callback_NS, CMSE_NONSECURE) == NULL) {
		printf("The callback is not located in normal world!\r\n");
		//       while (1);
	}
	/* Check whether string is properly terminated */
	string_length = strnlen(s1, MAX_STRING_LENGTH);
	if ((string_length == MAX_STRING_LENGTH) && (s1[string_length] != '\0')) {
		printf("First string too long or invalid string termination!\r\n");
		//while (1);
	}
	/* Check whether string is properly terminated */
	string_length = strnlen(s2, MAX_STRING_LENGTH);
	if ((string_length == MAX_STRING_LENGTH) && (s2[string_length] != '\0')) {
		printf("Second string too long or invalid string termination!\r\n");
		// while (1);
	}
	//  printf("Comparing two string as a callback to normal world\r\n");
	//   printf("String 1: ");
	//  printf(s1);
	//  printf("String 2: ");
	//  printf(s2);
	result = callback_NS(s1, s2);
	return result;
}

secureportNON_SECURE_CALLABLE uint32_t NSCFunction(Callback_t pxCallback) {
	NonSecureCallback_t pxNonSecureCallback;

	/* Return function pointer with cleared LSB. */
	pxNonSecureCallback = (NonSecureCallback_t) cmse_nsfptr_create(pxCallback);

	/* Invoke the supplied callback. */
	pxNonSecureCallback();

	/* Increment the secure side counter. */
	ulSecureCounter += 1;

	/* Return the secure side counter. */
	return ulSecureCounter;
}

__attribute__((cmse_nonsecure_entry))  uint32_t NSC_MPC_SRAM2_Cfg(uint32_t irq_enable, uint32_t lut_table)
{
	MPC_SRAM2_CFG_T cfg;
	cfg.irq_enable = irq_enable;
	cfg.lut_table = lut_table;
	cfg.sram2_mpc_cfg.init_state = 0;
	cfg.sram2_mpc_cfg.irq_enable = irq_enable;

	hx_drv_mpc_set_sram2_cfg(cfg);
	return 0;
}
