/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _VENEER_TABLE_H_
#define _VENEER_TABLE_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef int (*callbackptr)(char const *s1, char const *s2);
/* Callback function pointer definition. */
typedef void (*Callback_t)(void);
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Entry function for two string comparison
 *
 * This function compares two strings by using non-secure callback
 * This function is called from normal world only
 *
 * @param callback     pointer to strcmp() function
 * @param s1           String to be compared
 * @param s2           String to be compared
 * @return             result of strcmp function
 *                     >0 for s1 > s2
 *                     =0 for s1 = s2
 *                     <0 for s1 < s2
 */
uint32_t StringCompare_NSE(volatile callbackptr callback, char const *s1,
		char const *s2);

/**
 * @brief Invokes the supplied callback which is on
 *        the non-secure side.
 *
 * Returns a number which is one more than the value
 * returned in previous invocation of this function.
 * Initial invocation returns 1.
 *
 * @param pxCallback[in] The callback to invoke.
 *
 * @return A number which is one more than the value
 * returned in previous invocation of this function.
 */
uint32_t NSCFunction(Callback_t pxCallback);

uint32_t NSC_MPC_SRAM2_Cfg(uint32_t irq_enable, uint32_t lut_table);

#endif /* _VENEER_TABLE_H_ */
