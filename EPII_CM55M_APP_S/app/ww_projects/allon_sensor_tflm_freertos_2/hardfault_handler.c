/*
 Copyright (c) 2020 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of
 and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"

#if 0
/* HardFault handler implementation that prints a message
   then exits the program early.
 */
void HardFault_Handler(void)
{
#if 0
    printf("HardFault occurred!\n");
#endif
    for(;;) {}
}
#endif
void HardFault_Handler(void) {
	/* Handling SAU related secure faults */
	printf("\r\nEntering HardFault interrupt!\r\n");
	if (SAU->SFSR != 0) {
		if (SAU->SFSR & SAU_SFSR_INVEP_Msk) {
			/* Invalid Secure state entry point */
			printf(
					"SAU->SFSR:INVEP fault: Invalid entry point to secure world.\r\n");
		} else if (SAU->SFSR & SAU_SFSR_AUVIOL_Msk) {
			/* AUVIOL: SAU violation  */
			printf(
					"SAU->SFSR:AUVIOL fault: SAU violation. Access to secure memory from normal world.\r\n");
		} else if (SAU->SFSR & SAU_SFSR_INVTRAN_Msk) {
			/* INVTRAN: Invalid transition from secure to normal world  */
			printf(
					"SAU->SFSR:INVTRAN fault: Invalid transition from secure to normal world.\r\n");
		} else {
			printf("Another SAU error.\r\n");
		}
		if (SAU->SFSR & SAU_SFSR_SFARVALID_Msk) {
			/* SFARVALID: SFAR contain valid address that caused secure violation */
			printf("Address that caused SAU violation is 0x%X.\r\n", SAU->SFAR);
		}
	}

	/* Handling secure bus related faults */
	if (SCB->CFSR != 0) {
		if (SCB->CFSR & SCB_CFSR_IBUSERR_Msk) {
			/* IBUSERR: Instruction bus error on an instruction prefetch */
			printf(
					"SCB->BFSR:IBUSERR fault: Instruction bus error on an instruction prefetch.\r\n");
		} else if (SCB->CFSR & SCB_CFSR_PRECISERR_Msk) {
			/* PRECISERR: Instruction bus error on an instruction prefetch */
			printf("SCB->BFSR:PRECISERR fault: Precise data access error.\r\n");
		} else {
			printf("Security Another secure bus error 1.\r\n");
		}
		if (SCB->CFSR & SCB_CFSR_BFARVALID_Msk) {
			/* BFARVALID: BFAR contain valid address that caused secure violation */
			printf("Address that caused secure bus violation is 0x%X.\r\n",
					SCB->BFAR);
		}
	}

	/* Handling non-secure bus related faults */
	if (SCB_NS->CFSR != 0) {
		if (SCB_NS->CFSR & SCB_CFSR_IBUSERR_Msk) {
			/* IBUSERR: Instruction bus error on an instruction prefetch */
			printf(
					"SCB_NS->BFSR:IBUSERR fault: Instruction bus error on an instruction prefetch.\r\n");
		} else if (SCB_NS->CFSR & SCB_CFSR_PRECISERR_Msk) {
			/* PRECISERR: Data bus error on an data read/write */
			printf(
					"SCB_NS->BFSR:PRECISERR fault: Precise data access error.\r\n");
		} else {
			printf("Security Another secure bus error 2.\r\n");
		}
		if (SCB_NS->CFSR & SCB_CFSR_BFARVALID_Msk) {
			/* BFARVALID: BFAR contain valid address that caused secure violation */
			printf("Address that caused secure bus violation is 0x%X.\r\n",
					SCB_NS->BFAR);
		}
	}

#if 0
    /* Perform system RESET */
    SCB->AIRCR =
        (SCB->AIRCR & ~SCB_AIRCR_VECTKEY_Msk) | (0x05FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
#else
	printf("SCB->CFSR:0x%08x\n", SCB->CFSR);
	printf("SCB->BFAR:0x%08x\n", SCB->BFAR);
	printf("SCB->HFSR:0x%08x\n", SCB->HFSR);
	for (;;) {
	}
#endif
}

void NMI_Handler(void) {
	printf("\r\nEntering NMI_Handler interrupt!\r\n");
	for (;;) {
	}
}

void MemManage_Handler(void) {
	printf("\r\nEntering MemManage_Handler interrupt!\r\n");
	for (;;) {
	}
}
void BusFault_Handler(void) {
	printf("\r\nEntering BusFault_Handler interrupt!\r\n");
	printf("SCB->CFSR:0x%08x\n", SCB->CFSR);
	printf("SCB->BFAR:0x%08x\n", SCB->BFAR);
	printf("SCB->HFSR:0x%08x\n", SCB->HFSR);
	for (;;) {
	}
}
void UsageFault_Handler(void) {
	printf("\r\nEntering UsageFault_Handler interrupt!\r\n");
	for (;;) {
	}
}
void SecureFault_Handler(void) {
	printf("\r\nEntering SecureFault_Handler interrupt!\r\n");
	for (;;) {
	}
}

