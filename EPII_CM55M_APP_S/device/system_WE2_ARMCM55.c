/**************************************************************************//**
 * @file     system_ARMCM55.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM55 Device
 * @version  V1.0.0
 * @date     30. March 2020
 ******************************************************************************/
/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
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
 */

#if defined (ARMCM55)
#include "WE2_device.h"
#else
#error device not specified!
#endif

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
#ifdef TRUSTZONE_SEC
   
   #include "trustzone_cfg.h"
#endif
#ifndef TRUSTZONE_SEC_ONLY
   #include "hx_drv_scu.h"
#endif
#endif

#include "board.h"

/*----------------------------------------------------------------------------
 Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
extern const VECTOR_TABLE_Type __VECTOR_TABLE[496];

/*----------------------------------------------------------------------------
 System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;

volatile unsigned long g_time_loop = 0;
#if defined(SIM_PER) || defined(SIM_DHRY)
#else
#if !defined(ENABLE_OS) && !defined(RTE_CMSIS_RTOS2) && !defined(RTE_RTOS_FreeRTOS_CORE)
void SysTick_Handler(void) {
	if (g_time_loop >= 0xFFFFFFFF)
		g_time_loop = 0;
	else
		g_time_loop++;
}
#endif
#endif
/*----------------------------------------------------------------------------
 System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockInit() {
	uint32_t val = SysTick_LOAD_RELOAD_Msk+1;
	SystemCoreClock = SYSTEM_CLOCK;
	g_time_loop = 0;
#if !defined(ENABLE_OS) && !defined(RTE_CMSIS_RTOS2) && !defined(RTE_RTOS_FreeRTOS_CORE)
	//SYSTICK MAX
	if (SysTick_Config(val)) {

		while (1)
			;
	}
#endif
}

void SystemCoreClockUpdate(uint32_t clock) {
	uint32_t val = SysTick_LOAD_RELOAD_Msk+1;
	SystemCoreClock = clock;
	g_time_loop = 0;
#if !defined(ENABLE_OS) && !defined(RTE_CMSIS_RTOS2) && !defined(RTE_RTOS_FreeRTOS_CORE)
	//SYSTICK MAX
	if (SysTick_Config(val)) {

		while (1)
			;
	}
#endif
}

/**
 \brief  Get Current Tick.

 */
void SystemGetTick(uint32_t *systick, uint32_t *loop_cnt)
{
	*systick = SysTick->VAL;
	*loop_cnt = g_time_loop;
}

#if !defined(HX_TFM)
/*----------------------------------------------------------------------------
 System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit(void) {

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t)(&__VECTOR_TABLE[0]);
#endif

#if (defined (__FPU_USED) && (__FPU_USED == 1U)) || \
    (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
  SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */
#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  SCB_NS->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */
#endif
#endif
	SCB->SHCSR |= (1 << SCB_SHCSR_MEMFAULTENA_Pos);
	SCB->SHCSR |= (1 << SCB_SHCSR_BUSFAULTENA_Pos);
	SCB->SHCSR |= (1 << SCB_SHCSR_USGFAULTENA_Pos);
#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  SCB->SHCSR |= (1 << SCB_SHCSR_SECUREFAULTENA_Pos);
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

// Enable Loop and branch info cache
	SCB->CCR |= SCB_CCR_LOB_Msk;
	__ISB();


	for(IRQn_Type idx = 0; idx <=200; idx++)
	{
		NVIC_DisableIRQ(idx);
	}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#ifdef TRUSTZONE_SEC
  TZ_SAU_Setup();
#endif
#ifndef TRUSTZONE_SEC_ONLY
  hx_drv_scu_set_CM55M_IDAU(3, 0, 0, 1);
#endif
#endif

  SystemCoreClockInit();
}

#endif //#if !defined(HX_TFM)
