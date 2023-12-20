/*
 * trustzone_cfg.c
 *
 *  Created on: 2021�~7��24��
 *      Author: 902447
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "xprintf.h"
#include "hx_drv_scu.h"
#include "hx_drv_mpc.h"
#include "hx_drv_ppc.h"
#include "trustzone_cfg.h"

/*
 //-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
 */
//#define __SAUREGION_PRESENT  1

/*
 // <e>Initialize Security Attribution Unit (SAU) CTRL register
 */
#define SAU_INIT_CTRL          1

/*
 //   <q> Enable SAU
 //   <i> Value for SAU->CTRL register bit ENABLE
 */
#define SAU_INIT_CTRL_ENABLE   1

/*
 //   <o> When SAU is disabled
 //     <0=> All Memory is Secure
 //     <1=> All Memory is Non-Secure
 //   <i> Value for SAU->CTRL register bit ALLNS
 //   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
 */
#define SAU_INIT_CTRL_ALLNS  0

/*
 // </e>
 */

/*
 // <h>Initialize Security Attribution Unit (SAU) Address Regions
 // <i>SAU configuration specifies regions to be one of:
 // <i> - Secure and Non-Secure Callable
 // <i> - Non-Secure
 // <i>Note: All memory regions not configured by SAU are Secure
 */
#define SAU_REGIONS_MAX   8                 /* Max. number of SAU regions */

/*
 //   <e>Initialize SAU Region 0
 //   <i> Setup SAU Region 0 memory attributes
 */
#define SAU_INIT_REGION0    1


/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START0     0/* start address of SAU region 0 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END0       (0x10000000-1)      /* end address of SAU region 0 */


/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC0       0
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 1
 //   <i> Setup SAU Region 1 memory attributes
 */
#define SAU_INIT_REGION1    1


/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START1     (CM55M_SG_BASE)//(CM55M_NS_APP_DATA_BASE)

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END1       (CM55M_SG_BASE + CM55M_SG_SIZE - 1)//(0xE0000000 - 1)//(CM55M_NS_APP_DATA_BASE + CM55M_NS_APP_DATA_SIZE - 1)


/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC1       1
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 2
 //   <i> Setup SAU Region 2 memory attributes
 */
#define SAU_INIT_REGION2    1

/*
 //     <o>Start Address <0-0xFFFFFFE0>
 */
#define SAU_INIT_START2     0x20000000//(CM55M_SG_BASE)

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END2       (0x30000000-1)//(CM55M_SG_BASE + CM55M_SG_SIZE - 1)

/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC2       0
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 3
 //   <i> Setup SAU Region 3 memory attributes
 */
#define SAU_INIT_REGION3    1

/*
 //     <o>Start Address <0-0xFFFFFFE0>
 */
#define SAU_INIT_START3     0x40000000

/*
 //     <o>End Address <0x1F-0xFFFFFFFF>
 */
#define SAU_INIT_END3       (0xE0000000 - 1)

/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC3       0
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 4
 //   <i> Setup SAU Region 4 memory attributes
 */
#define SAU_INIT_REGION4    0

/*
 //     <o>Start Address <0-0xFFFFFFE0>
 */
#define SAU_INIT_START4     0x00000000      /* start address of SAU region 4 */

/*
 //     <o>End Address <0x1F-0xFFFFFFFF>
 */
#define SAU_INIT_END4       0x00000000      /* end address of SAU region 4 */

/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC4       0
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 5
 //   <i> Setup SAU Region 5 memory attributes
 */
#define SAU_INIT_REGION5    0

/*
 //     <o>Start Address <0-0xFFFFFFE0>
 */
#define SAU_INIT_START5     0x00000000

/*
 //     <o>End Address <0x1F-0xFFFFFFFF>
 */
#define SAU_INIT_END5       0x00000000

/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC5       0
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 6
 //   <i> Setup SAU Region 6 memory attributes
 */
#define SAU_INIT_REGION6    0

/*
 //     <o>Start Address <0-0xFFFFFFE0>
 */
#define SAU_INIT_START6     0x00000000

/*
 //     <o>End Address <0x1F-0xFFFFFFFF>
 */
#define SAU_INIT_END6       0x00000000

/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC6       0
/*
 //   </e>
 */

/*
 //   <e>Initialize SAU Region 7
 //   <i> Setup SAU Region 7 memory attributes
 */
#define SAU_INIT_REGION7    0

/*
 //     <o>Start Address <0-0xFFFFFFE0>
 */
#define SAU_INIT_START7     0x00000000

/*
 //     <o>End Address <0x1F-0xFFFFFFFF>
 */
#define SAU_INIT_END7       0x00000000

/*
 //     <o>Region is
 //         <0=>Non-Secure
 //         <1=>Secure, Non-Secure Callable
 */
#define SAU_INIT_NSC7       0
/*
 //   </e>
 */

/*
 // </h>
 */

/*
 // <e>Setup behaviour of Sleep and Exception Handling
 */
#define SCB_CSR_AIRCR_INIT  1

/*
 //   <o> Deep Sleep can be enabled by
 //     <0=>Secure and Non-Secure state
 //     <1=>Secure state only
 //   <i> Value for SCB->CSR register bit DEEPSLEEPS
 */
#define SCB_CSR_DEEPSLEEPS_VAL  1

/*
 //   <o>System reset request accessible from
 //     <0=> Secure and Non-Secure state
 //     <1=> Secure state only
 //   <i> Value for SCB->AIRCR register bit SYSRESETREQS
 */
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/*
 //   <o>Priority of Non-Secure exceptions is
 //     <0=> Not altered
 //     <1=> Lowered to 0x80-0xFF
 //   <i> Value for SCB->AIRCR register bit PRIS
 */
#define SCB_AIRCR_PRIS_VAL      1

/*
 //   <o>BusFault, HardFault, and NMI target
 //     <0=> Secure state
 //     <1=> Non-Secure state
 //   <i> Value for SCB->AIRCR register bit BFHFNMINS
 */
#define SCB_AIRCR_BFHFNMINS_VAL 0

/*
 // </e>
 */

/*
 // <e>Setup behaviour of Floating Point Unit
 */
#define TZ_FPU_NS_USAGE 1

/*
 // <o>Floating Point Unit usage
 //     <0=> Secure state only
 //     <3=> Secure and Non-Secure state
 //   <i> Value for SCB->NSACR register bits CP10, CP11
 */
#define SCB_NSACR_CP10_11_VAL       3

/*
 // <o>Treat floating-point registers as Secure
 //     <0=> Disabled
 //     <1=> Enabled
 //   <i> Value for FPU->FPCCR register bit TS
 */
#define FPU_FPCCR_TS_VAL            0

/*
 // <o>Clear on return (CLRONRET) accessibility
 //     <0=> Secure and Non-Secure state
 //     <1=> Secure state only
 //   <i> Value for FPU->FPCCR register bit CLRONRETS
 */
#define FPU_FPCCR_CLRONRETS_VAL     0

/*
 // <o>Clear floating-point caller saved registers on exception return
 //     <0=> Disabled
 //     <1=> Enabled
 //   <i> Value for FPU->FPCCR register bit CLRONRET
 */
#define FPU_FPCCR_CLRONRET_VAL      1

/*
 // </e>
 */

/*
 // <h>Setup Interrupt Target
 */

/*
 //   <e>Initialize ITNS 0 (Interrupts 0..31)
 */
#define NVIC_INIT_ITNS0    1

/*
 // Interrupts 0..31
 //   <o.0>  Interrupt 0   <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 1   <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 2   <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 3   <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 4   <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 5   <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 6   <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 7   <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 8   <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 9   <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 10  <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 11  <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 12  <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 13  <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 14  <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 15  <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 16  <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 17  <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 18  <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 19  <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 20  <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 21  <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 22  <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 23  <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 24  <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 25  <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 26  <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 27  <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 28  <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 29  <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 30  <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 31  <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS0_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS0_VAL      0x1f0080
#else
#define NVIC_INIT_ITNS0_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 1 (Interrupts 32..63)
 */
#define NVIC_INIT_ITNS1    1

/*
 // Interrupts 32..63
 //   <o.0>  Interrupt 32  <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 33  <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 34  <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 35  <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 36  <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 37  <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 38  <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 39  <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 40  <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 41  <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 42  <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 43  <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 44  <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 45  <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 46  <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 47  <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 48  <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 49  <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 50  <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 51  <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 52  <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 53  <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 54  <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 55  <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 56  <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 57  <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 58  <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 59  <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 60  <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 61  <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 62  <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 63  <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS1_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS1_VAL      0xFFFFF4ff
#else
#define NVIC_INIT_ITNS1_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 2 (Interrupts 64..95)
 */
#define NVIC_INIT_ITNS2    1

/*
 // Interrupts 64..95
 //   <o.0>  Interrupt 64  <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 65  <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 66  <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 67  <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 68  <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 69  <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 70  <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 71  <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 72  <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 73  <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 74  <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 75  <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 76  <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 77  <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 78  <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 79  <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 80  <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 81  <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 82  <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 83  <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 84  <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 85  <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 86  <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 87  <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 88  <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 89  <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 90  <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 91  <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 92  <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 93  <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 94  <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 95  <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS2_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS2_VAL      0xffffffff
#else
#define NVIC_INIT_ITNS2_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 3 (Interrupts 96..127)
 */
#define NVIC_INIT_ITNS3    1

/*
 // Interrupts 96..127
 //   <o.0>  Interrupt 96  <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 97  <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 98  <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 99  <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 100 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 101 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 102 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 103 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 104 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 105 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 106 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 107 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 108 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 109 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 110 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 111 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 112 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 113 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 114 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 115 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 116 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 117 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 118 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 119 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 120 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 121 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 122 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 123 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 124 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 125 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 126 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 127 <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS3_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS3_VAL      0xFFFFFFE1
#else
#define NVIC_INIT_ITNS3_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 4 (Interrupts 128..159)
 */
#define NVIC_INIT_ITNS4    1

/*
 // Interrupts 128..159
 //   <o.0>  Interrupt 128 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 129 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 130 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 131 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 132 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 133 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 134 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 135 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 136 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 137 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 138 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 139 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 140 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 141 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 142 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 143 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 144 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 145 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 146 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 147 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 148 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 149 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 150 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 151 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 152 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 153 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 154 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 155 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 156 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 157 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 158 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 159 <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS4_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS4_VAL      0xFFFFFFc7
#else
#define NVIC_INIT_ITNS4_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 5 (Interrupts 160..191)
 */
#define NVIC_INIT_ITNS5    1

/*
 // Interrupts 160..191
 //   <o.0>  Interrupt 160 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 161 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 162 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 163 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 164 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 165 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 166 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 167 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 168 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 169 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 170 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 171 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 172 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 173 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 174 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 175 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 176 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 177 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 178 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 179 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 180 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 181 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 182 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 183 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 184 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 185 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 186 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 187 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 188 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 189 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 190 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 191 <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS5_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS5_VAL      0xF3F000C1
#else
#define NVIC_INIT_ITNS5_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 6 (Interrupts 192..223)
 */
#define NVIC_INIT_ITNS6    1

/*
 // Interrupts 192..223
 //   <o.0>  Interrupt 192 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 193 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 194 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 195 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 196 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 197 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 198 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 199 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 200 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 201 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 202 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 203 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 204 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 205 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 206 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 207 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 208 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 209 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 210 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 211 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 212 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 213 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 214 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 215 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 216 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 217 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 218 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 219 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 220 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 221 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 222 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 223 <0=> Secure state <1=> Non-Secure state
 */
#ifdef TRUSTZONE_SEC_ONLY
#define NVIC_INIT_ITNS6_VAL      0x00000000
#elif defined(TRUSTZONE_SEC)
#define NVIC_INIT_ITNS6_VAL      0x00000001
#else
#define NVIC_INIT_ITNS6_VAL      0x00000000
#endif
/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 7 (Interrupts 224..255)
 */
#define NVIC_INIT_ITNS7    0

/*
 // Interrupts 224..255
 //   <o.0>  Interrupt 224 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 225 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 226 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 227 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 228 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 229 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 230 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 231 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 232 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 233 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 234 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 235 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 236 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 237 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 238 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 239 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 240 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 241 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 242 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 243 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 244 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 245 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 246 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 247 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 248 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 249 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 250 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 251 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 252 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 253 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 254 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 255 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS7_VAL      0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 8 (Interrupts 256..287)
 */
#define NVIC_INIT_ITNS8    0

/*
 // Interrupts 256..287
 //   <o.0>  Interrupt 256 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 257 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 258 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 259 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 260 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 261 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 262 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 263 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 264 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 265 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 266 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 267 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 268 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 269 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 270 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 271 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 272 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 273 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 274 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 275 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 276 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 277 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 278 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 279 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 280 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 281 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 282 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 283 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 284 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 285 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 286 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 287 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS8_VAL      0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 9 (Interrupts 288..319)
 */
#define NVIC_INIT_ITNS9    0

/*
 // Interrupts 288..319
 //   <o.0>  Interrupt 288 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 289 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 290 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 291 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 292 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 293 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 294 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 295 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 296 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 297 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 298 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 299 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 300 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 301 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 302 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 303 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 304 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 305 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 306 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 307 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 308 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 309 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 310 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 311 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 312 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 313 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 314 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 315 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 316 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 317 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 318 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 319 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS9_VAL      0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 10 (Interrupts 320..351)
 */
#define NVIC_INIT_ITNS10   0

/*
 // Interrupts 320..351
 //   <o.0>  Interrupt 320 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 321 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 322 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 323 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 324 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 325 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 326 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 327 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 328 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 329 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 330 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 331 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 332 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 333 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 334 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 335 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 336 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 337 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 338 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 339 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 340 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 341 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 342 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 343 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 344 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 345 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 346 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 347 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 348 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 349 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 350 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 351 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS10_VAL     0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 11 (Interrupts 352..383)
 */
#define NVIC_INIT_ITNS11   0

/*
 // Interrupts 352..383
 //   <o.0>  Interrupt 352 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 353 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 354 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 355 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 356 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 357 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 358 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 359 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 360 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 361 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 362 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 363 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 364 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 365 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 366 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 367 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 368 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 369 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 370 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 371 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 372 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 373 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 374 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 375 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 376 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 377 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 378 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 379 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 380 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 381 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 382 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 383 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS11_VAL     0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 12 (Interrupts 384..415)
 */
#define NVIC_INIT_ITNS12   0

/*
 // Interrupts 384..415
 //   <o.0>  Interrupt 384 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 385 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 386 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 387 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 388 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 389 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 390 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 391 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 392 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 393 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 394 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 395 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 396 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 397 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 398 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 399 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 400 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 401 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 402 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 403 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 404 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 405 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 406 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 407 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 408 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 409 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 410 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 411 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 412 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 413 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 414 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 415 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS12_VAL     0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 13 (Interrupts 416..447)
 */
#define NVIC_INIT_ITNS13   0

/*
 // Interrupts 416..447
 //   <o.0>  Interrupt 416 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 417 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 418 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 419 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 420 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 421 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 422 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 423 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 424 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 425 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 426 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 427 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 428 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 429 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 430 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 431 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 432 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 433 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 434 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 435 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 436 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 437 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 438 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 439 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 440 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 441 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 442 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 443 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 444 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 445 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 446 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 447 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS13_VAL     0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 14 (Interrupts 448..479)
 */
#define NVIC_INIT_ITNS14   0

/*
 // Interrupts 448..479
 //   <o.0>  Interrupt 448 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 449 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 450 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 451 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 452 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 453 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 454 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 455 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 456 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 457 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 458 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 459 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 460 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 461 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 462 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 463 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 464 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 465 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 466 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 467 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 468 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 469 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 470 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 471 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 472 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 473 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 474 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 475 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 476 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 477 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 478 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 479 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS14_VAL     0x00000000

/*
 //   </e>
 */

/*
 //   <e>Initialize ITNS 15 (Interrupts 480..511)
 */
#define NVIC_INIT_ITNS15   0

/*
 // Interrupts 480..511
 //   <o.0>  Interrupt 480 <0=> Secure state <1=> Non-Secure state
 //   <o.1>  Interrupt 481 <0=> Secure state <1=> Non-Secure state
 //   <o.2>  Interrupt 482 <0=> Secure state <1=> Non-Secure state
 //   <o.3>  Interrupt 483 <0=> Secure state <1=> Non-Secure state
 //   <o.4>  Interrupt 484 <0=> Secure state <1=> Non-Secure state
 //   <o.5>  Interrupt 485 <0=> Secure state <1=> Non-Secure state
 //   <o.6>  Interrupt 486 <0=> Secure state <1=> Non-Secure state
 //   <o.7>  Interrupt 487 <0=> Secure state <1=> Non-Secure state
 //   <o.8>  Interrupt 488 <0=> Secure state <1=> Non-Secure state
 //   <o.9>  Interrupt 489 <0=> Secure state <1=> Non-Secure state
 //   <o.10> Interrupt 490 <0=> Secure state <1=> Non-Secure state
 //   <o.11> Interrupt 491 <0=> Secure state <1=> Non-Secure state
 //   <o.12> Interrupt 492 <0=> Secure state <1=> Non-Secure state
 //   <o.13> Interrupt 493 <0=> Secure state <1=> Non-Secure state
 //   <o.14> Interrupt 494 <0=> Secure state <1=> Non-Secure state
 //   <o.15> Interrupt 495 <0=> Secure state <1=> Non-Secure state
 //   <o.16> Interrupt 496 <0=> Secure state <1=> Non-Secure state
 //   <o.17> Interrupt 497 <0=> Secure state <1=> Non-Secure state
 //   <o.18> Interrupt 498 <0=> Secure state <1=> Non-Secure state
 //   <o.19> Interrupt 499 <0=> Secure state <1=> Non-Secure state
 //   <o.20> Interrupt 500 <0=> Secure state <1=> Non-Secure state
 //   <o.21> Interrupt 501 <0=> Secure state <1=> Non-Secure state
 //   <o.22> Interrupt 502 <0=> Secure state <1=> Non-Secure state
 //   <o.23> Interrupt 503 <0=> Secure state <1=> Non-Secure state
 //   <o.24> Interrupt 504 <0=> Secure state <1=> Non-Secure state
 //   <o.25> Interrupt 505 <0=> Secure state <1=> Non-Secure state
 //   <o.26> Interrupt 506 <0=> Secure state <1=> Non-Secure state
 //   <o.27> Interrupt 507 <0=> Secure state <1=> Non-Secure state
 //   <o.28> Interrupt 508 <0=> Secure state <1=> Non-Secure state
 //   <o.29> Interrupt 509 <0=> Secure state <1=> Non-Secure state
 //   <o.30> Interrupt 510 <0=> Secure state <1=> Non-Secure state
 //   <o.31> Interrupt 511 <0=> Secure state <1=> Non-Secure state
 */
#define NVIC_INIT_ITNS15_VAL     0x00000000

/*
 //   </e>
 */

/*
 // </h>
 */

/*
 max 128 SAU regions.
 SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U


#if !defined(HX_TFM)
 /**
  \brief   Setup a SAU Region
  \details Writes the region information contained in SAU_Region to the
  registers SAU_RNR, SAU_RBAR, and SAU_RLAR
  */
 void TZ_SAU_Setup(void) {

 #ifdef TRUSTZONE_SEC_ONLY
 	 SAU->CTRL = 0;
 #else
 	 SAU->CTRL = 0;
 #if defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)
 #if defined (SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
 	SAU_INIT_REGION(0);
 #endif

 #if defined (SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
 	SAU_INIT_REGION(1);
 #endif

 #if defined (SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
 	SAU_INIT_REGION(2);
 #endif

 #if defined (SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
     SAU_INIT_REGION(3);
   #endif

 #if defined (SAU_INIT_REGION4) && (SAU_INIT_REGION4 == 1U)
     SAU_INIT_REGION(4);
   #endif

 #if defined (SAU_INIT_REGION5) && (SAU_INIT_REGION5 == 1U)
     SAU_INIT_REGION(5);
   #endif

 #if defined (SAU_INIT_REGION6) && (SAU_INIT_REGION6 == 1U)
     SAU_INIT_REGION(6);
   #endif

 #if defined (SAU_INIT_REGION7) && (SAU_INIT_REGION7 == 1U)
     SAU_INIT_REGION(7);
   #endif
 	/* repeat this for all possible SAU regions */

 #endif /* defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U) */

 #if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
 	SAU->CTRL =
 			((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk)
 					| ((SAU_INIT_CTRL_ALLNS << SAU_CTRL_ALLNS_Pos)
 							& SAU_CTRL_ALLNS_Msk);
 #endif
 #endif

 #if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
 	SCB->SCR = (SCB->SCR & ~(SCB_SCR_SLEEPDEEPS_Msk))
 			| ((SCB_CSR_DEEPSLEEPS_VAL << SCB_SCR_SLEEPDEEPS_Pos)
 					& SCB_SCR_SLEEPDEEPS_Msk);

 	SCB->AIRCR = (SCB->AIRCR
 			& ~(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_SYSRESETREQS_Msk
 					| SCB_AIRCR_BFHFNMINS_Msk | SCB_AIRCR_PRIS_Msk))
 			| ((0x05FAU << SCB_AIRCR_VECTKEY_Pos) & SCB_AIRCR_VECTKEY_Msk)
 			| ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos)
 					& SCB_AIRCR_SYSRESETREQS_Msk)
 			| ((SCB_AIRCR_PRIS_VAL << SCB_AIRCR_PRIS_Pos) & SCB_AIRCR_PRIS_Msk)
 			| ((SCB_AIRCR_BFHFNMINS_VAL << SCB_AIRCR_BFHFNMINS_Pos)
 					& SCB_AIRCR_BFHFNMINS_Msk);
 #endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

 #if defined (__FPU_USED) && (__FPU_USED == 1U) && \
       defined (TZ_FPU_NS_USAGE) && (TZ_FPU_NS_USAGE == 1U)

     SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
                    ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

     FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
                    ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos       ) & FPU_FPCCR_TS_Msk       ) |
                    ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
                    ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos ) & FPU_FPCCR_CLRONRET_Msk );
   #endif

 #if defined (NVIC_INIT_ITNS0) && (NVIC_INIT_ITNS0 == 1U)
 	NVIC->ITNS[0] = NVIC_INIT_ITNS0_VAL;
 #endif

 #if defined (NVIC_INIT_ITNS1) && (NVIC_INIT_ITNS1 == 1U)
 	NVIC->ITNS[1] = NVIC_INIT_ITNS1_VAL;
 #endif

 #if defined (NVIC_INIT_ITNS2) && (NVIC_INIT_ITNS2 == 1U)
     NVIC->ITNS[2] = NVIC_INIT_ITNS2_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS3) && (NVIC_INIT_ITNS3 == 1U)
     NVIC->ITNS[3] = NVIC_INIT_ITNS3_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS4) && (NVIC_INIT_ITNS4 == 1U)
     NVIC->ITNS[4] = NVIC_INIT_ITNS4_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS5) && (NVIC_INIT_ITNS5 == 1U)
     NVIC->ITNS[5] = NVIC_INIT_ITNS5_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS6) && (NVIC_INIT_ITNS6 == 1U)
     NVIC->ITNS[6] = NVIC_INIT_ITNS6_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS7) && (NVIC_INIT_ITNS7 == 1U)
     NVIC->ITNS[7] = NVIC_INIT_ITNS7_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS8) && (NVIC_INIT_ITNS8 == 1U)
     NVIC->ITNS[8] = NVIC_INIT_ITNS8_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS9) && (NVIC_INIT_ITNS9 == 1U)
     NVIC->ITNS[9] = NVIC_INIT_ITNS9_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS10) && (NVIC_INIT_ITNS10 == 1U)
     NVIC->ITNS[10] = NVIC_INIT_ITNS10_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS11) && (NVIC_INIT_ITNS11 == 1U)
     NVIC->ITNS[11] = NVIC_INIT_ITNS11_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS12) && (NVIC_INIT_ITNS12 == 1U)
     NVIC->ITNS[12] = NVIC_INIT_ITNS12_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS13) && (NVIC_INIT_ITNS13 == 1U)
     NVIC->ITNS[13] = NVIC_INIT_ITNS13_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS14) && (NVIC_INIT_ITNS14 == 1U)
     NVIC->ITNS[14] = NVIC_INIT_ITNS14_VAL;
   #endif

 #if defined (NVIC_INIT_ITNS15) && (NVIC_INIT_ITNS15 == 1U)
     NVIC->ITNS[15] = NVIC_INIT_ITNS15_VAL;
   #endif

 	/* repeat this for all possible ITNS elements */

}
#endif

void TZ_PPC_DWC_CB(uint32_t event)
{
//	xprintf("TZ_PPC_DWC_CB\n");
	hx_drv_scu_set_dwc_ppc_irq_clear();
}

void TZ_PPC_ISP_CB(uint32_t event)
{
//	xprintf("TZ_PPC_ISP_CB\n");
	hx_drv_scu_set_isp_ppc_irq_clear();
}

void TZ_PPC_AHB3_CB(uint32_t event)
{
//	xprintf("TZ_PPC_AHB3_CB\n");
	hx_drv_scu_set_ahb3_ppc_irq_clear();
}

void TZ_PPC_AHB1M4_CB(uint32_t event)
{
//	xprintf("TZ_PPC_AHB1M4_CB\n");
	hx_drv_scu_set_ahb1m4_ppc_irq_clear();
}

void TZ_PPC_APB0NS_CB(uint32_t event)
{
//	xprintf("TZ_PPC_APB0NS_CB\n");
	hx_drv_scu_set_apb0ns_ppc_irq_clear();
}

void TZ_PPC_APB0PS_CB(uint32_t event)
{
//	xprintf("TZ_PPC_APB0PS_CB\n");
	hx_drv_scu_set_apb0ps_ppc_irq_clear();
}

void TZ_PPC_APB1NS_CB(uint32_t event)
{
//	xprintf("TZ_PPC_APB1NS_CB\n");
	hx_drv_scu_set_apb1ns_ppc_irq_clear();
}

void TZ_PPC_APB1PS_CB(uint32_t event)
{
//	xprintf("TZ_PPC_APB1PS_CB\n");
	hx_drv_scu_set_apb1ps_ppc_irq_clear();
}

void TZ_PPC_APB2_CB(uint32_t event)
{
//	xprintf("TZ_PPC_APB2_CB\n");
	hx_drv_scu_set_apb2_ppc_irq_clear();
}

void TZ_PPC_BOOTROM_CB(uint32_t event)
{
//	xprintf("TZ_PPC_BOOTROM_CB\n");
	hx_drv_scu_set_rom_ppc_irq_clear();
}


void TZ_PPC_APB1_AON_CB(uint32_t event)
{
    hx_drv_scu_set_apb1_aon_ppc_irq_clear();
}


#ifdef IP_scu
void TZ_DMA0_MSC_CB(uint32_t event)
{
//	xprintf("TZ_DMA0_MSC_CB\n");
	hx_drv_scu_set_dma0_msc_irq_clear();
}

void TZ_DMA1_MSC_CB(uint32_t event)
{
//	xprintf("TZ_DMA1_MSC_CB\n");
	hx_drv_scu_set_dma1_msc_irq_clear();
}

void TZ_DMA2M1_MSC_CB(uint32_t event)
{
//	xprintf("TZ_DMA2M1_MSC_CB\n");
	hx_drv_scu_set_dma2m1_msc_irq_clear();
}

void TZ_DMA2M2_MSC_CB(uint32_t event)
{
//	xprintf("TZ_DMA2M2_MSC_CB\n");
	hx_drv_scu_set_dma2m2_msc_irq_clear();
}

void TZ_DMA3M1_MSC_CB(uint32_t event)
{
//	xprintf("TZ_DMA3M1_MSC_CB\n");
	hx_drv_scu_set_dma3m1_msc_irq_clear();
}

void TZ_DMA3M2_MSC_CB(uint32_t event)
{
//	xprintf("TZ_DMA3M2_MSC_CB\n");
	hx_drv_scu_set_dma3m2_msc_irq_clear();
}
#endif

#ifdef IP_scu
void TZ_DPDMA_MSC_CB(uint32_t event)
{
	//xprintf("TZ_DPDMA_MSC_CB\n");
	hx_drv_scu_set_dpdma_msc_irq_clear();
}
#endif

#ifdef IP_scu
void TZ_I3CH_MSC_CB(uint32_t event)
{
	//xprintf("TZ_I3CH_MSC_CB\n");
	hx_drv_scu_set_i3ch_msc_irq_clear();
}
#endif

#ifdef IP_scu
void TZ_SDIO_MSC_CB(uint32_t event)
{
	//xprintf("TZ_SDIO_MSC_CB\n");
	hx_drv_scu_set_sdio_msc_irq_clear();
}
#endif

#ifdef IP_scu
void TZ_U55M0_MSC_CB(uint32_t event)
{
	//xprintf("TZ_U55M0_MSC_CB\n");
	hx_drv_scu_set_u55_m0_msc_irq_clear();
}

void TZ_U55M1_MSC_CB(uint32_t event)
{
	//xprintf("TZ_U55M1_MSC_CB\n");
	hx_drv_scu_set_u55_m1_msc_irq_clear();
}
#endif

#ifdef IP_mpc
void TZ_MPC_SRAM0_CB(uint32_t event, uint32_t info1, uint32_t info2)
{
	//xprintf("TZ_MPC_SRAM0_CB\r\n");
	//xprintf("event =0x%x, info1= 0x%x, info2=0x%x\r\n", event,  info1, info2);
	hx_drv_mpc_set_sram0_irq_clear();
}

void TZ_MPC_SRAM1_CB(uint32_t event, uint32_t info1, uint32_t info2)
{
	//xprintf("TZ_MPC_SRAM1_CB\r\n");
	//xprintf("event =0x%x, info1= 0x%x, info2=0x%x\r\n", event,  info1, info2);
	hx_drv_mpc_set_sram1_irq_clear();
}

void TZ_MPC_SRAM2_CB(uint32_t event, uint32_t info1, uint32_t info2)
{
	//xprintf("TZ_MPC_SRAM2_CB\r\n");
	//xprintf("event =0x%x, info1= 0x%x, info2=0x%x\r\n", event,  info1, info2);
	hx_drv_mpc_set_sram2_irq_clear();
}

void TZ_MPC_FLASH1_CB(uint32_t event, uint32_t info1, uint32_t info2)
{
	//xprintf("TZ_MPC_FLASH1_CB\r\n");
//	xprintf("event =0x%x, info1= 0x%x, info2=0x%x\r\n", event,  info1, info2);
	hx_drv_mpc_set_flash1_irq_clear();
}

void TZ_MPC_FLASH2_CB(uint32_t event, uint32_t info1, uint32_t info2)
{
	//xprintf("TZ_MPC_FLASH1_CB\r\n");
//	xprintf("event =0x%x, info1= 0x%x, info2=0x%x\r\n", event,  info1, info2);
	hx_drv_mpc_set_flash2_irq_clear();
}
#endif

void TZ_set_U55M0MSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {

	SCU_U55_AXI_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_rd_enable = irq_enable;
	cfg.irq_wr_enable = irq_enable;
	if((cfg.irq_rd_enable == 1) || (cfg.irq_wr_enable ==1))
		cfg.callback = &TZ_U55M0_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_u55_m0_msc_cfg(cfg);

}

void TZ_set_U55M1MSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {

	SCU_U55_AXI_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_rd_enable = irq_enable;
	cfg.irq_wr_enable = irq_enable;
	if((cfg.irq_rd_enable == 1) || (cfg.irq_wr_enable ==1))
		cfg.callback = &TZ_U55M1_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_u55_m1_msc_cfg(cfg);

}

void TZ_set_DPDMAMSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DPDMA_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dpdma_msc_cfg(cfg);
#endif
}

void TZ_set_DMA0MSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DMA0_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dma0_msc_cfg(cfg);
#endif
}

void TZ_set_DMA1MSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DMA1_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dma1_msc_cfg(cfg);
#endif
}

void TZ_set_DMA2MSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DMA2M1_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dma2m1_msc_cfg(cfg);

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DMA2M2_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dma2m2_msc_cfg(cfg);
#endif
}

void TZ_set_DMA3MSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DMA3M1_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dma3m1_msc_cfg(cfg);

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_DMA3M2_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_dma3m2_msc_cfg(cfg);
#endif
}

void TZ_set_I3CHMSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_I3CH_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_i3ch_msc_cfg(cfg);
#endif
}


void TZ_set_SDIOMSC(uint32_t state, uint32_t resp, uint32_t irq_enable) {
#ifdef IP_scu
	SCU_AHB_MSC_CFG_T cfg;

	cfg.state = state;
	cfg.resp = resp;
	cfg.irq_enable = irq_enable;
	if(cfg.irq_enable == 1)
		cfg.callback = &TZ_SDIO_MSC_CB;
	else
		cfg.callback = NULL;
	hx_drv_scu_set_sdio_msc_cfg(cfg);
#endif
}

void TZ_mpc_register()
{
#ifdef IP_mpc
	MPC_SRAM0_CFG_T sram0_cfg;
	MPC_SRAM1_CFG_T sram1_cfg;
	MPC_SRAM2_CFG_T sram2_cfg;
	MPC_FLASH1_CFG_T flash1_cfg;
	MPC_FLASH2_CFG_T flash2_cfg;

    hx_drv_mpc_register_sram0_cb(NULL);
    hx_drv_mpc_register_sram1_cb(NULL);
    hx_drv_mpc_register_sram2_cb(NULL);
    hx_drv_mpc_register_flash1_cb(NULL);
    hx_drv_mpc_register_flash2_cb(NULL);

	hx_drv_mpc_set_sram0_irq_clear();
	hx_drv_mpc_set_sram1_irq_clear();
	hx_drv_mpc_set_sram2_irq_clear();
	hx_drv_mpc_set_flash1_irq_clear();
	hx_drv_mpc_set_flash2_irq_clear();

	hx_drv_mpc_get_sram0_cfg(&sram0_cfg);
	hx_drv_mpc_get_sram1_cfg(&sram1_cfg);
	hx_drv_mpc_get_sram2_cfg(&sram2_cfg);
	hx_drv_mpc_get_flash1_cfg(&flash1_cfg);
	hx_drv_mpc_get_flash2_cfg(&flash2_cfg);

	sram0_cfg.irq_enable = 1;
	sram0_cfg.sram0_mpc_cfg.irq_rd_enable = 1;
	sram0_cfg.sram0_mpc_cfg.irq_wr_enable = 1;
	sram0_cfg.callback = &TZ_MPC_SRAM0_CB;
	hx_drv_mpc_set_sram0_cfg(sram0_cfg);


	sram1_cfg.irq_enable = 1;
	sram1_cfg.sram1_mpc_cfg.irq_rd_enable = 1;
	sram1_cfg.sram1_mpc_cfg.irq_wr_enable = 1;
	sram1_cfg.callback = &TZ_MPC_SRAM1_CB;
	hx_drv_mpc_set_sram1_cfg(sram1_cfg);


	sram2_cfg.irq_enable = 1;
	sram2_cfg.sram2_mpc_cfg.irq_enable = 1;
	sram2_cfg.callback = &TZ_MPC_SRAM2_CB;
	hx_drv_mpc_set_sram2_cfg(sram2_cfg);


	flash1_cfg.irq_enable = 1;
	flash1_cfg.flash_mpc_cfg.irq_enable = 1;
	flash1_cfg.callback = &TZ_MPC_FLASH1_CB;
	hx_drv_mpc_set_flash1_cfg(flash1_cfg);

	flash2_cfg.irq_enable = 1;
	flash2_cfg.flash_mpc_cfg.irq_enable = 1;
	flash2_cfg.callback = &TZ_MPC_FLASH2_CB;
	hx_drv_mpc_set_flash2_cfg(flash2_cfg);
#endif
}

void TZ_Set_ALL_Secure() {
#ifdef IP_ppc
	PPC_FlashSys_DWC_CFG_T dwc_cfg;
	PPC_FlashSys_ISP_CFG_T isp_cfg;
	PPC_AHB3_CFG_T ahb3_cfg;
	PPC_AHB1M4_CFG_T ahb1m4_cfg;
	PPC_APB0NS_CFG_T apb0ns_cfg;
	PPC_APB0PS_CFG_T apb0ps_cfg;
	PPC_APB1NS_CFG_T apb1ns_cfg;
	PPC_APB1PS_CFG_T apb1ps_cfg;
	PPC_APB2_CFG_T apb2_cfg;
	PPC_APB1AON_CFG_T apb1_aon_cfg;

	hx_drv_ppc_register_dwc_cb(NULL);
	hx_drv_ppc_register_isp_cb(NULL);
	hx_drv_ppc_register_ahb3_cb(NULL);
	hx_drv_ppc_register_ahb1m4_cb(NULL);
	hx_drv_ppc_register_apb0ns_cb(NULL);
	hx_drv_ppc_register_apb0ps_cb(NULL);
	hx_drv_ppc_register_apb1ns_cb(NULL);
	hx_drv_ppc_register_apb1ps_cb(NULL);
	hx_drv_ppc_register_apb2_cb(NULL);
	hx_drv_ppc_register_bootrom_cb(NULL);
	hx_drv_ppc_register_apb1_aon_cb(NULL);

   	hx_drv_scu_set_dma0_msc_irq_clear();
    hx_drv_scu_set_dma1_msc_irq_clear();
    hx_drv_scu_set_dma2m1_msc_irq_clear();
    hx_drv_scu_set_dma2m2_msc_irq_clear();
    hx_drv_scu_set_dma3m1_msc_irq_clear();
    hx_drv_scu_set_dma3m2_msc_irq_clear();
    hx_drv_scu_set_dpdma_msc_irq_clear();
    hx_drv_scu_set_u55_m0_msc_irq_clear();
    hx_drv_scu_set_u55_m1_msc_irq_clear();

	hx_drv_scu_set_dwc_ppc_irq_clear();
    hx_drv_scu_set_isp_ppc_irq_clear();
    hx_drv_scu_set_ahb3_ppc_irq_clear();
    hx_drv_scu_set_ahb1m4_ppc_irq_clear();
    hx_drv_scu_set_apb0ns_ppc_irq_clear();
    hx_drv_scu_set_apb0ps_ppc_irq_clear();
    hx_drv_scu_set_apb1ns_ppc_irq_clear();
    hx_drv_scu_set_apb1ps_ppc_irq_clear();
    hx_drv_scu_set_apb2_ppc_irq_clear();
    hx_drv_scu_set_rom_ppc_irq_clear();
    hx_drv_scu_set_apb1_aon_ppc_irq_clear();

	hx_drv_mpc_set_sram0_irq_clear();
	hx_drv_mpc_set_sram1_irq_clear();
	hx_drv_mpc_set_sram2_irq_clear();
	hx_drv_mpc_set_flash1_irq_clear();
#endif

	TZ_mpc_register();

	hx_drv_ppc_get_dwc(&dwc_cfg);
    hx_drv_ppc_get_isp(&isp_cfg);
    hx_drv_ppc_get_ahb3(&ahb3_cfg);
    hx_drv_ppc_get_ahb1m4(&ahb1m4_cfg);
    hx_drv_ppc_get_apb0ns(&apb0ns_cfg);
	hx_drv_ppc_get_apb0ps(&apb0ps_cfg);
	hx_drv_ppc_get_apb1ns(&apb1ns_cfg);
    hx_drv_ppc_get_apb1ps(&apb1ps_cfg);
	hx_drv_ppc_get_apb2(&apb2_cfg);
	hx_drv_ppc_get_apb1_aon(&apb1_aon_cfg);

	dwc_cfg.dwc_cfg.state = 0;
	dwc_cfg.dwc_cfg.ap = 1;
	dwc_cfg.dwc_cfg.irq_enable = 1;
	dwc_cfg.callback = NULL;

	isp_cfg.isp_cfg.state = 0;
	isp_cfg.isp_cfg.ap = 1;
	isp_cfg.isp_cfg.irq_enable = 1;
	isp_cfg.callback = NULL;

    ahb3_cfg.dma2_state = 0;
    ahb3_cfg.dma3_state = 0;
    ahb3_cfg.sspi_host_state = 0;
    ahb3_cfg.sspi_slave_state = 0;
    ahb3_cfg.dma2_ap = 1;
    ahb3_cfg.dma3_ap = 1;
    ahb3_cfg.sspi_host_ap = 1;
    ahb3_cfg.sspi_slave_ap = 1;
    ahb3_cfg.irq_enable = 1;
	ahb3_cfg.callback = NULL;

    ahb1m4_cfg.dma0_state = 0;
	ahb1m4_cfg.dma1_state = 0;
	ahb1m4_cfg.isp_state = 0;
    ahb1m4_cfg.dma0_ap = 1;
	ahb1m4_cfg.dma1_ap = 1;
	ahb1m4_cfg.isp_ap = 1;
	ahb1m4_cfg.irq_enable = 1;
	ahb1m4_cfg.callback = NULL;

    apb0ns_cfg.uart1_state = 0;
    apb0ns_cfg.sculsc_state = 0;
    apb0ns_cfg.i3c_slave1_state = 0;
    apb0ns_cfg.pwm_state = 0;
    apb0ns_cfg.i2cm_sensor_state = 0;
    apb0ns_cfg.sndwire_apb_state = 0;
    apb0ns_cfg.sndwire_pdi0_state = 0;
    apb0ns_cfg.sndwire_pdi1_state = 0;
    apb0ns_cfg.sndwire_pdi2_state = 0;
    apb0ns_cfg.vad_d_state = 0;
    apb0ns_cfg.vad_a_state = 0;
    apb0ns_cfg.rop_d_state = 0;
    apb0ns_cfg.adcctrl_lv_state = 0;
    apb0ns_cfg.sw_reg_lsc_state = 0;
    apb0ns_cfg.img_sc_status_state = 0; /*!< Image Senctrl status state */

    apb0ns_cfg.uart1_ap = 1;
    apb0ns_cfg.sculsc_ap = 1;
    apb0ns_cfg.i3c_slave1_ap = 1;
    apb0ns_cfg.pwm_ap = 1;
    apb0ns_cfg.i2cm_sensor_ap = 1;
    apb0ns_cfg.sndwire_apb_ap = 1;
    apb0ns_cfg.sndwire_pdi0_ap = 1;
    apb0ns_cfg.sndwire_pdi1_ap = 1;
    apb0ns_cfg.sndwire_pdi2_ap = 1;
    apb0ns_cfg.vad_d_ap = 1;
    apb0ns_cfg.vad_a_ap = 1;
    apb0ns_cfg.rop_d_ap = 1;
    apb0ns_cfg.adcctrl_lv_ap = 1;
    apb0ns_cfg.sw_reg_lsc_ap = 1;
    apb0ns_cfg.img_sc_status_ap = 1;
    apb0ns_cfg.irq_enable = 1;
	apb0ns_cfg.callback = NULL;

    apb0ps_cfg.i2cslave_state = 0;
    apb0ps_cfg.i2chost_state = 0;
    apb0ps_cfg.i2shost_state = 0;
    apb0ps_cfg.uart0_state = 0;
    apb0ps_cfg.uart2_state = 0;
    apb0ps_cfg.pdm_state = 0;
    apb0ps_cfg.gpio0_state = 0;
    apb0ps_cfg.gpio1_state = 0;
    apb0ps_cfg.gpio2_state = 0;
    apb0ps_cfg.gpio3_state = 0;
    apb0ps_cfg.mailbox_state = 0;
    apb0ps_cfg.i2sslave_state = 0;
    apb0ps_cfg.i3cslave_state = 0;
    apb0ps_cfg.i2cslave1_state = 0;
    apb0ps_cfg.himaxpdm_state = 0;
    apb0ps_cfg.i2cslave_ap = 1;
    apb0ps_cfg.i2chost_ap = 1;
    apb0ps_cfg.i2shost_ap = 1;
    apb0ps_cfg.uart0_ap = 1;
    apb0ps_cfg.uart2_ap = 1;
    apb0ps_cfg.pdm_ap = 1;
    apb0ps_cfg.gpio0_ap = 1;
    apb0ps_cfg.gpio1_ap = 1;
    apb0ps_cfg.gpio2_ap = 1;
    apb0ps_cfg.gpio3_ap = 1;
    apb0ps_cfg.mailbox_ap = 1;
    apb0ps_cfg.i2sslave_ap = 1;
    apb0ps_cfg.i3cslave_ap = 1;
    apb0ps_cfg.i2cslave1_ap = 1;
    apb0ps_cfg.himaxpdm_ap = 1;
	apb0ps_cfg.irq_enable = 1;
	apb0ps_cfg.callback = NULL;

	apb1ns_cfg.wdt0_state = 0;
	apb1ns_cfg.wdt1_state = 0;
	apb1ns_cfg.apbtestreg_state = 0;
	apb1ns_cfg.mipitx_aip_phy_state = 0;
	apb1ns_cfg.inpdp_state = 0;
	apb1ns_cfg.senctrl_state = 0;
	apb1ns_cfg.mipirx_phy_state = 0;

	apb1ns_cfg.timer0_state = 0;
	apb1ns_cfg.timer1_state = 0;
	apb1ns_cfg.timer2_state = 0;
	apb1ns_cfg.timer3_state = 0;
	apb1ns_cfg.timer4_state = 0;
	apb1ns_cfg.timer5_state = 0;
	apb1ns_cfg.wdt0_ap = 1;
	apb1ns_cfg.wdt1_ap = 1;
	apb1ns_cfg.apbtestreg_ap = 1;
	apb1ns_cfg.mipitx_aip_phy_ap = 1;
	apb1ns_cfg.inpdp_ap = 1;
	apb1ns_cfg.senctrl_ap = 1;
	apb1ns_cfg.mipirx_phy_ap = 1;

	apb1ns_cfg.timer0_ap = 1;
	apb1ns_cfg.timer1_ap = 1;
	apb1ns_cfg.timer2_ap = 1;
	apb1ns_cfg.timer3_ap = 1;
	apb1ns_cfg.timer4_ap = 1;
	apb1ns_cfg.timer5_ap = 1;
	apb1ns_cfg.irq_enable = 1;
	apb1ns_cfg.callback = NULL;

	apb1ps_cfg.sb_gpio_state = 0; /*!< SB GPIO state*/
	apb1ps_cfg.ts_ctrl_state = 0; /*!< TS Ctrl state*/
	apb1ps_cfg.adc_ctrl_hv_state = 0; /*!< ADC CTRL HV state*/
	apb1ps_cfg.scu_sb_state = 0; /*!< SCU SB state*/
	apb1ps_cfg.i2cm_mipi_state = 0; /*!< I2C Master For MIPI state */
	apb1ps_cfg.timer6_state = 0; /*!< Timer6 state*/
	apb1ps_cfg.timer7_state = 0; /*!< Timer7 state*/
	apb1ps_cfg.timer8_state = 0; /*!< Timer8 state*/
	apb1ps_cfg.apb_testreg_state = 0; /*!< APB Test Reg state */

	apb1ps_cfg.sb_gpio_ap = 1; /*!< SB GPIO AP*/
	apb1ps_cfg.ts_ctrl_ap = 1; /*!< TS Ctrl ap*/
	apb1ps_cfg.adc_ctrl_hv_ap = 1; /*!< ADC CTRL HV ap*/
	apb1ps_cfg.scu_sb_ap = 1; /*!< SCU SB ap*/
	apb1ps_cfg.i2cm_mipi_ap = 1; /*!< I2C Master For MIPI LV */
	apb1ps_cfg.timer6_ap = 1; /*!< Timer6 ap*/
	apb1ps_cfg.timer7_ap = 1; /*!< Timer7 ap*/
	apb1ps_cfg.timer8_ap = 1; /*!< Timer8 ap*/
	apb1ps_cfg.apb_testreg_ap = 1; /*!< APB Test Reg AP */
	apb1ps_cfg.irq_enable = 1; /*!< PPC_IRQ_ENABLE */
	apb1ps_cfg.callback = NULL;

    apb2_cfg.sdio_state = 0;
    apb2_cfg.u55ctrl_state = 0;
    apb2_cfg.cc312apbs_state = 0;
    apb2_cfg.cc312apbsc_state = 0;
    apb2_cfg.pufrt_state = 0;
    apb2_cfg.mipitxctrl_state = 0;
    apb2_cfg.scu_hsc_state = 0;
    apb2_cfg.cs_dbg_state = 0;
    apb2_cfg.cs_sdc_state = 0;
    apb2_cfg.cs_apb_async_state = 0;
    apb2_cfg.mipirx_ctrl_state = 0; /*!< [11] MPIP RX CTRL*/
    apb2_cfg.i3c_hc_state = 0; /*!< [15] I3C_HC*/
    apb2_cfg.img_status_state = 0; /*!< [16] Image_Path Status*/

    apb2_cfg.sdio_ap = 1;
    apb2_cfg.u55ctrl_ap = 1;
    apb2_cfg.cc312apbs_ap = 1;
    apb2_cfg.cc312apbsc_ap = 1;
    apb2_cfg.pufrt_ap = 1;
    apb2_cfg.mipitxctrl_ap = 1;
    apb2_cfg.scu_hsc_ap = 1;
    apb2_cfg.cs_dbg_ap = 1;
    apb2_cfg.cs_sdc_ap = 1;
    apb2_cfg.cs_apb_async_ap = 1;
    apb2_cfg.mipirx_ctrl_ap = 1; /*!< [11] MPIP RX CTRL*/
    apb2_cfg.i3c_hc_ap = 1; /*!< [15] I3C_HC*/
    apb2_cfg.img_status_ap = 1; /*!< [16] Image_Path Status*/
    apb2_cfg.irq_enable = 1;
    apb2_cfg.callback = NULL;

    apb1_aon_cfg.scu_aon_state = 0; /*!< SCU AON state*/
    apb1_aon_cfg.pmu_state = 0; /*!< PMU state*/
    apb1_aon_cfg.aon_gpio_state = 0; /*!< AON GPIO state*/
    apb1_aon_cfg.rtc0_state = 0; /*!< RTC0 state*/
    apb1_aon_cfg.rtc1_state = 0; /*!< RTC1 state*/
    apb1_aon_cfg.rtc2_state = 0; /*!< RTC2 state*/
    apb1_aon_cfg.sw_reg_aon_state = 0; /*!< SW Reg AON state*/
    apb1_aon_cfg.scu_aon_ap = 1; /*!< scu AP */
    apb1_aon_cfg.pmu_ap = 1; /*!< PMU state*/
    apb1_aon_cfg.aon_gpio_ap = 1; /*!< AON GPIO AP*/
    apb1_aon_cfg.rtc0_ap = 1; /*!< RTC0 AP*/
    apb1_aon_cfg.rtc1_ap = 1; /*!< RTC1 AP*/
    apb1_aon_cfg.rtc2_ap = 1; /*!< RTC2 AP*/
    apb1_aon_cfg.sw_reg_aon_ap = 1; /*!< SW Reg AON ap*/

    apb1_aon_cfg.irq_enable = 1; /*!< PPC_IRQ_ENABLE */
    apb1_aon_cfg.callback = NULL;

	hx_drv_ppc_set_dwc(dwc_cfg);
    hx_drv_ppc_set_isp(isp_cfg);
    hx_drv_ppc_set_ahb3(ahb3_cfg);
    hx_drv_ppc_set_ahb1m4(ahb1m4_cfg);
    hx_drv_ppc_set_apb0ns(apb0ns_cfg);
	hx_drv_ppc_set_apb0ps(apb0ps_cfg);
	hx_drv_ppc_set_apb1ns(apb1ns_cfg);
    hx_drv_ppc_set_apb1ps(apb1ps_cfg);
    hx_drv_ppc_set_apb2(apb2_cfg);
    hx_drv_ppc_set_apb1_aon(apb1_aon_cfg);

	hx_drv_scu_set_dwc_ppc_irq_clear();
    hx_drv_scu_set_isp_ppc_irq_clear();
    hx_drv_scu_set_ahb3_ppc_irq_clear();
    hx_drv_scu_set_ahb1m4_ppc_irq_clear();
    hx_drv_scu_set_apb0ns_ppc_irq_clear();
    hx_drv_scu_set_apb0ps_ppc_irq_clear();
    hx_drv_scu_set_apb1ns_ppc_irq_clear();
    hx_drv_scu_set_apb1ps_ppc_irq_clear();
    hx_drv_scu_set_apb2_ppc_irq_clear();
    hx_drv_scu_set_rom_ppc_irq_clear();
    hx_drv_scu_set_apb1_aon_ppc_irq_clear();

	hx_drv_ppc_register_dwc_cb(&TZ_PPC_DWC_CB);
	hx_drv_ppc_register_isp_cb(&TZ_PPC_ISP_CB);
	hx_drv_ppc_register_ahb3_cb(&TZ_PPC_AHB3_CB);
	hx_drv_ppc_register_ahb1m4_cb(&TZ_PPC_AHB1M4_CB);
	hx_drv_ppc_register_apb0ns_cb(&TZ_PPC_APB0NS_CB);
	hx_drv_ppc_register_apb0ps_cb(&TZ_PPC_APB0PS_CB);
	hx_drv_ppc_register_apb1ns_cb(&TZ_PPC_APB1NS_CB);
	hx_drv_ppc_register_apb1ps_cb(&TZ_PPC_APB1PS_CB);
	hx_drv_ppc_register_apb2_cb(&TZ_PPC_APB2_CB);
	hx_drv_ppc_register_apb1_aon_cb(&TZ_PPC_APB1_AON_CB);
}

void TZ_Set_ITS_byIRQNo(IRQn_Type irq) {
	uint32_t idx;
	uint32_t mod_val;
	if(irq < 0)
	{
		return;
	}
	idx = irq / 32;
	mod_val = irq % 32;

 	NVIC->ITNS[idx] = NVIC->ITNS[idx] & (~(1 << mod_val));
}

void TZ_Set_ITNS_byIRQNo(IRQn_Type irq)
{
	uint32_t idx;
	uint32_t mod_val;
	if(irq < 0)
	{
		return;
	}
	idx = irq / 32;
	mod_val = irq % 32;

 	NVIC->ITNS[idx] = NVIC->ITNS[idx] | (1 << mod_val);
}

void TZ_Set_Secure_ByCFG()
{
#ifdef IP_ppc
	PPC_FlashSys_DWC_CFG_T dwc_cfg;
	PPC_FlashSys_ISP_CFG_T isp_cfg;
	PPC_AHB3_CFG_T ahb3_cfg;
	PPC_AHB1M4_CFG_T ahb1m4_cfg;
	PPC_APB0NS_CFG_T apb0ns_cfg;
	PPC_APB0PS_CFG_T apb0ps_cfg;
	PPC_APB1NS_CFG_T apb1ns_cfg;
	PPC_APB1PS_CFG_T apb1ps_cfg;
	PPC_APB2_CFG_T apb2_cfg;
	PPC_APB1AON_CFG_T apb1_aon_cfg;

	hx_drv_ppc_register_dwc_cb(NULL);
	hx_drv_ppc_register_isp_cb(NULL);
	hx_drv_ppc_register_ahb3_cb(NULL);
	hx_drv_ppc_register_ahb1m4_cb(NULL);
	hx_drv_ppc_register_apb0ns_cb(NULL);
	hx_drv_ppc_register_apb0ps_cb(NULL);
	hx_drv_ppc_register_apb1ns_cb(NULL);
	hx_drv_ppc_register_apb1ps_cb(NULL);
	hx_drv_ppc_register_apb2_cb(NULL);
	hx_drv_ppc_register_bootrom_cb(NULL);
	hx_drv_ppc_register_apb1_aon_cb(NULL);

   	hx_drv_scu_set_dma0_msc_irq_clear();
    hx_drv_scu_set_dma1_msc_irq_clear();
    hx_drv_scu_set_dma2m1_msc_irq_clear();
    hx_drv_scu_set_dma2m2_msc_irq_clear();
    hx_drv_scu_set_dma3m1_msc_irq_clear();
    hx_drv_scu_set_dma3m2_msc_irq_clear();
    hx_drv_scu_set_dpdma_msc_irq_clear();
    hx_drv_scu_set_u55_m0_msc_irq_clear();
    hx_drv_scu_set_u55_m1_msc_irq_clear();

	hx_drv_scu_set_dwc_ppc_irq_clear();
    hx_drv_scu_set_isp_ppc_irq_clear();
    hx_drv_scu_set_ahb3_ppc_irq_clear();
    hx_drv_scu_set_ahb1m4_ppc_irq_clear();
    hx_drv_scu_set_apb0ns_ppc_irq_clear();
    hx_drv_scu_set_apb0ps_ppc_irq_clear();
    hx_drv_scu_set_apb1ns_ppc_irq_clear();
    hx_drv_scu_set_apb1ps_ppc_irq_clear();
    hx_drv_scu_set_apb2_ppc_irq_clear();
    hx_drv_scu_set_rom_ppc_irq_clear();
    hx_drv_scu_set_apb1_aon_ppc_irq_clear();


	hx_drv_ppc_get_dwc(&dwc_cfg);
    hx_drv_ppc_get_isp(&isp_cfg);
    hx_drv_ppc_get_ahb3(&ahb3_cfg);
    hx_drv_ppc_get_ahb1m4(&ahb1m4_cfg);
    hx_drv_ppc_get_apb0ns(&apb0ns_cfg);
	hx_drv_ppc_get_apb0ps(&apb0ps_cfg);
	hx_drv_ppc_get_apb1ns(&apb1ns_cfg);
    hx_drv_ppc_get_apb1ps(&apb1ps_cfg);
	hx_drv_ppc_get_apb2(&apb2_cfg);
	hx_drv_ppc_get_apb1_aon(&apb1_aon_cfg);

//#ifdef IP_spi
	TZ_Set_ITS_byIRQNo(QSPI_spi_w_intp_IRQn);
	TZ_Set_ITS_byIRQNo(QSPI_ssi_intp_IRQn);
	TZ_Set_ITS_byIRQNo(OSPI_IRQn);
#ifdef TODO
	dwc_cfg.dwc_cfg.state = 0;
#endif
	dwc_cfg.dwc_cfg.ap = 1;
//#else
//	dwc_cfg.dwc_cfg.state = 1;
//#endif
	dwc_cfg.dwc_cfg.irq_enable = 1;
	dwc_cfg.callback = &TZ_PPC_DWC_CB;
#ifdef TODO
	isp_cfg.isp_cfg.state = 0;
#endif
	isp_cfg.isp_cfg.ap = 1;
	isp_cfg.isp_cfg.irq_enable = 1;
	isp_cfg.callback = &TZ_PPC_ISP_CB;

#ifdef IP_dma
#ifdef IP_INST_DMA2
#ifdef IP_INST_NS_DMA2
	TZ_set_DMA2MSC(1, 0, 1);
    ahb3_cfg.dma2_state = 1;
#else
	TZ_Set_ITS_byIRQNo(DMAC2_DMACINTERR_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC2_DMACINTTC_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC2_DMACINTR_IRQn);
	TZ_set_DMA2MSC(0, 0, 1);
    ahb3_cfg.dma2_state = 0;
#endif
#else
	TZ_set_DMA2MSC(1, 0, 1);
    ahb3_cfg.dma2_state = 1;
#endif
#else
	TZ_set_DMA2MSC(1, 0, 1);
    ahb3_cfg.dma2_state = 1;
#endif
    ahb3_cfg.dma2_ap = 1;

#ifdef IP_dma
#ifdef IP_INST_DMA3
#ifdef IP_INST_NS_DMA3
    TZ_set_DMA3MSC(1, 0, 1);
    ahb3_cfg.dma3_state = 1;
#else
	TZ_Set_ITS_byIRQNo(DMAC3_DMACINTERR_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC3_DMACINTTC_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC3_DMACINTR_IRQn);
    TZ_set_DMA3MSC(0, 0, 1);
    ahb3_cfg.dma3_state = 0;
#endif
#else
    TZ_set_DMA3MSC(1, 0, 1);
    ahb3_cfg.dma3_state = 1;
#endif
#else
    TZ_set_DMA3MSC(1, 0, 1);
    ahb3_cfg.dma3_state = 1;
#endif
    ahb3_cfg.dma3_ap = 1;
#ifdef IP_spi
#ifdef IP_INST_SSPI_HOST
#ifdef IP_INST_NS_SSPI_HOST
    ahb3_cfg.sspi_host_state = 1;
#else
	TZ_Set_ITS_byIRQNo(SSPI_HOST_IRQn);
    ahb3_cfg.sspi_host_state = 0;
#endif
#else
    ahb3_cfg.sspi_host_state = 1;
#endif
#else
    ahb3_cfg.sspi_host_state = 1;
#endif
    ahb3_cfg.sspi_host_ap = 1;

#ifdef IP_spi
#ifdef IP_INST_SSPI_SLAVE
#ifdef IP_INST_NS_SSPI_SLAVE
    ahb3_cfg.sspi_slave_state = 1;
#else
	TZ_Set_ITS_byIRQNo(SSPI_SLAVE_IRQn);
    ahb3_cfg.sspi_slave_state = 0;
#endif
#else
    ahb3_cfg.sspi_slave_state = 1;
#endif
#else
    ahb3_cfg.sspi_slave_state = 1;
#endif
    ahb3_cfg.sspi_slave_ap = 1;
    ahb3_cfg.irq_enable = 1;
	ahb3_cfg.callback = &TZ_PPC_AHB3_CB;

#ifdef IP_dma
#ifdef IP_INST_DMA0
#ifdef IP_INST_NS_DMA0
	TZ_set_DMA0MSC(1, 0, 1);
    ahb1m4_cfg.dma0_state = 1;
#else
	TZ_Set_ITS_byIRQNo(DMAC0_DMACINTERR_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC0_DMACINTTC_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC0_DMACINTR_IRQn);
	TZ_set_DMA0MSC(0, 0, 1);
    ahb1m4_cfg.dma0_state = 0;
#endif
#else
	TZ_set_DMA0MSC(1, 0, 1);
    ahb1m4_cfg.dma0_state = 1;
#endif
#else
	TZ_set_DMA0MSC(1, 0, 1);
    ahb1m4_cfg.dma0_state = 1;
#endif
    ahb1m4_cfg.dma0_ap = 1;
#ifdef IP_dma
#ifdef IP_INST_DMA1
#ifdef IP_INST_NS_DMA1
	TZ_set_DMA1MSC(1, 0, 1);
	ahb1m4_cfg.dma1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(DMAC1_DMACINTERR_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC1_DMACINTTC_IRQn);
	TZ_Set_ITS_byIRQNo(DMAC1_DMACINTR_IRQn);
	TZ_set_DMA1MSC(0, 0, 1);
	ahb1m4_cfg.dma1_state = 0;
#endif
#else
	TZ_set_DMA1MSC(1, 0, 1);
	ahb1m4_cfg.dma1_state = 1;
#endif
#else
	TZ_set_DMA1MSC(1, 0, 1);
	ahb1m4_cfg.dma1_state = 1;
#endif
	ahb1m4_cfg.dma1_ap = 1;
#ifdef TODO
	ahb1m4_cfg.isp_state = 0;
#endif
	ahb1m4_cfg.isp_ap = 1;
	ahb1m4_cfg.irq_enable = 1;
	ahb1m4_cfg.callback = &TZ_PPC_AHB1M4_CB;

#ifdef IP_uart
#ifdef IP_INST_UART1
#ifdef IP_INST_NS_UART1
    apb0ns_cfg.uart1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(UART1_intr_IRQn);
    apb0ns_cfg.uart1_state = 0;
#endif
#else
    apb0ns_cfg.uart1_state = 1;
#endif
#else
    apb0ns_cfg.uart1_state = 1;
#endif
    apb0ns_cfg.uart1_ap = 1;

    apb0ns_cfg.sculsc_state = 0;
    apb0ns_cfg.sculsc_ap = 0;
#ifdef IP_i3c_slv
#ifdef IP_INST_IIIC_SLAVE1
#ifdef IP_INST_NS_IIIC_SLAVE1
    apb0ns_cfg.i3c_slave1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I3C_Slave_1_IRQn);
    apb0ns_cfg.i3c_slave1_state = 0;
#endif
#else
    apb0ns_cfg.i3c_slave1_state = 1;
#endif
#else
    apb0ns_cfg.i3c_slave1_state = 1;
#endif
    apb0ns_cfg.i3c_slave1_ap = 1;
#ifdef IP_pwm
#ifdef IP_INST_NS_pwm
    apb0ns_cfg.pwm_state = 1;
#else
	TZ_Set_ITS_byIRQNo(pwm0_int_out_IRQn);
	TZ_Set_ITS_byIRQNo(pwm1_int_out_IRQn);
	TZ_Set_ITS_byIRQNo(pwm2_int_out_IRQn);
	apb0ns_cfg.pwm_state = 0;
#endif
#else
    apb0ns_cfg.pwm_state = 1;
#endif
    apb0ns_cfg.pwm_ap = 1;
#ifdef IP_iic
#ifdef IP_INST_IIC_HOST_SENSOR
#ifdef IP_INST_NS_IIC_HOST_SENSOR
    apb0ns_cfg.i2cm_sensor_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I2C_MST_1_intr_IRQn);
    apb0ns_cfg.i2cm_sensor_state = 0;
#endif
#else
    apb0ns_cfg.i2cm_sensor_state = 1;
#endif
#else
    apb0ns_cfg.i2cm_sensor_state = 1;
#endif
    apb0ns_cfg.i2cm_sensor_ap = 1;
#ifdef IP_sndwire
#ifdef IP_INST_NS_sndwire
    apb0ns_cfg.sndwire_apb_state = 1;
    apb0ns_cfg.sndwire_pdi0_state = 1;
    apb0ns_cfg.sndwire_pdi1_state = 1;
    apb0ns_cfg.sndwire_pdi2_state = 1;
#else
	TZ_Set_ITS_byIRQNo(Soundwire_intp_IRQn);
	TZ_Set_ITS_byIRQNo(sw_pd0_int_IRQn);
	TZ_Set_ITS_byIRQNo(sw_pd1_int_IRQn);
	TZ_Set_ITS_byIRQNo(sw_pd2_int_IRQn);
    apb0ns_cfg.sndwire_apb_state = 0;
    apb0ns_cfg.sndwire_pdi0_state = 0;
    apb0ns_cfg.sndwire_pdi1_state = 0;
    apb0ns_cfg.sndwire_pdi2_state = 0;
#endif
#else
    apb0ns_cfg.sndwire_apb_state = 1;
    apb0ns_cfg.sndwire_pdi0_state = 1;
    apb0ns_cfg.sndwire_pdi1_state = 1;
    apb0ns_cfg.sndwire_pdi2_state = 1;
#endif
    apb0ns_cfg.sndwire_apb_ap = 1;
    apb0ns_cfg.sndwire_pdi0_ap = 1;
    apb0ns_cfg.sndwire_pdi1_ap = 1;
    apb0ns_cfg.sndwire_pdi2_ap = 1;
#ifdef IP_vad
#ifdef IP_INST_NS_VAD
    apb0ns_cfg.vad_d_state = 1;
#else
	TZ_Set_ITS_byIRQNo(VAD_D_IRQn);
    apb0ns_cfg.vad_d_state = 0;
#endif
#else
    apb0ns_cfg.vad_d_state = 1;
#endif
    apb0ns_cfg.vad_d_ap = 1;
#ifdef TODO
    apb0ns_cfg.vad_a_state = 0;
#endif
#ifdef IP_ropd
    apb0ns_cfg.rop_d_state = 0;
#else
    apb0ns_cfg.rop_d_state = 1;
#endif
    apb0ns_cfg.rop_d_ap = 0;
#ifdef IP_adcc
#ifdef IP_INST_NS_ADCC
    apb0ns_cfg.adcctrl_lv_state = 1;
#else
	TZ_Set_ITS_byIRQNo(adcc_lv_int_out_IRQn);
    apb0ns_cfg.adcctrl_lv_state = 0;
#endif
#else
    apb0ns_cfg.adcctrl_lv_state = 1;
#endif
    apb0ns_cfg.adcctrl_lv_ap = 1;

#ifdef IP_swreg_lsc
#ifdef IP_INST_NS_swreg_lsc
    apb0ns_cfg.sw_reg_lsc_state = 1;
#else
    apb0ns_cfg.sw_reg_lsc_state = 0;
#endif
#else
    apb0ns_cfg.sw_reg_lsc_state = 1;
#endif

#ifdef IP_sensorctrl
#ifdef IP_INST_NS_sensorctrl
    apb0ns_cfg.img_sc_status_state = 1; /*!< Image Senctrl status state */
#else
    apb0ns_cfg.img_sc_status_state = 0; /*!< Image Senctrl status state */
#endif
#else
    apb0ns_cfg.img_sc_status_state = 1; /*!< Image Senctrl status state */
#endif
    apb0ns_cfg.img_sc_status_ap = 1;
    apb0ns_cfg.sw_reg_lsc_ap = 1;
    apb0ns_cfg.irq_enable = 1;
	apb0ns_cfg.callback = &TZ_PPC_APB0NS_CB;

#ifdef IP_iic
#ifdef IP_INST_IIC_SLAVE0
#ifdef IP_INST_NS_IIC_SLAVE0
    apb0ps_cfg.i2cslave_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I2C_SLAVE_intr_IRQn);
    apb0ps_cfg.i2cslave_state = 0;
#endif
#else
    apb0ps_cfg.i2cslave_state = 1;
#endif
#else
    apb0ps_cfg.i2cslave_state = 1;
#endif
    apb0ps_cfg.i2cslave_ap = 1;
#ifdef IP_iic
#ifdef IP_INST_IIC_HOST
#ifdef IP_INST_NS_IIC_HOST
    apb0ps_cfg.i2chost_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I2C_MST_0_intr_IRQn);
    apb0ps_cfg.i2chost_state = 0;
#endif
#else
    apb0ps_cfg.i2chost_state = 1;
#endif
#else
    apb0ps_cfg.i2chost_state = 1;
#endif
    apb0ps_cfg.i2chost_ap = 1;
#ifdef IP_i2s
#ifdef IP_INST_I2S_HOST
#ifdef IP_INST_NS_I2S_HOST
    apb0ps_cfg.i2shost_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I2S_HOST_rx_da_0_intr_IRQn);
	TZ_Set_ITS_byIRQNo(I2S_HOST_rx_or_0_intr_IRQn);
	TZ_Set_ITS_byIRQNo(I2S_HOST_tx_emp_0_intr_IRQn);
	TZ_Set_ITS_byIRQNo(I2S_HOST_tx_or_0_intr_IRQn);
    apb0ps_cfg.i2shost_state = 0;
#endif
#else
    apb0ps_cfg.i2shost_state = 1;
#endif
#else
    apb0ps_cfg.i2shost_state = 1;
#endif
    apb0ps_cfg.i2shost_ap = 1;
#ifdef IP_uart
#ifdef IP_INST_UART0
#ifdef IP_INST_NS_UART0
    apb0ps_cfg.uart0_state = 1;
#else
	TZ_Set_ITS_byIRQNo(UART0_intr_IRQn);
    apb0ps_cfg.uart0_state = 0;
#endif
#else
    apb0ps_cfg.uart0_state = 1;
#endif
#else
    apb0ps_cfg.uart0_state = 1;
#endif
    apb0ps_cfg.uart0_ap = 1;


#ifdef IP_uart
#ifdef IP_INST_UART2
#ifdef IP_INST_NS_UART2
    apb0ps_cfg.uart2_state = 1;
#else
	TZ_Set_ITS_byIRQNo(UART2_intr_IRQn);
    apb0ps_cfg.uart2_state = 0;
#endif
#else
    apb0ps_cfg.uart2_state = 1;
#endif
#else
    apb0ps_cfg.uart2_state = 1;
#endif
    apb0ps_cfg.uart2_ap = 1;


#ifdef IP_pdm
#ifdef IP_INST_NS_PDM
    apb0ps_cfg.pdm_state = 1;
#else
	TZ_Set_ITS_byIRQNo(PDM_int_rx_avail_IRQn);
	TZ_Set_ITS_byIRQNo(PDM_int_err_IRQn);
	TZ_Set_ITS_byIRQNo(PDM_int_clp_IRQn);
    apb0ps_cfg.pdm_state = 0;
#endif
#else
    apb0ps_cfg.pdm_state = 1;
#endif
    apb0ps_cfg.pdm_ap = 1;
#ifdef IP_gpio
#ifdef IP_INST_GPIO_G0
#ifdef IP_INST_NS_GPIO_G0
    apb0ps_cfg.gpio0_state = 1;
#else
	TZ_Set_ITS_byIRQNo(GPIO_GROUP_0_IRQn);
    apb0ps_cfg.gpio0_state = 0;
#endif
#else
    apb0ps_cfg.gpio0_state = 1;
#endif
#else
    apb0ps_cfg.gpio0_state = 1;
#endif
    apb0ps_cfg.gpio0_ap = 1;

#ifdef IP_gpio
#ifdef IP_INST_GPIO_G1
#ifdef IP_INST_NS_GPIO_G1
    apb0ps_cfg.gpio1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(GPIO_GROUP_1_IRQn);
    apb0ps_cfg.gpio1_state = 0;
#endif
#else
    apb0ps_cfg.gpio1_state = 1;
#endif
#else
    apb0ps_cfg.gpio1_state = 1;
#endif
    apb0ps_cfg.gpio1_ap = 1;
#ifdef IP_gpio
#ifdef IP_INST_GPIO_G2
#ifdef IP_INST_NS_GPIO_G2
    apb0ps_cfg.gpio2_state = 1;
#else
	TZ_Set_ITS_byIRQNo(GPIO_GROUP_2_IRQn);
    apb0ps_cfg.gpio2_state = 0;
#endif
#else
    apb0ps_cfg.gpio2_state = 1;
#endif
#else
    apb0ps_cfg.gpio2_state = 1;
#endif
    apb0ps_cfg.gpio2_ap = 1;
#ifdef IP_gpio
#ifdef IP_INST_GPIO_G3
#ifdef IP_INST_NS_GPIO_G3
    apb0ps_cfg.gpio3_state = 1;
#else
	TZ_Set_ITS_byIRQNo(GPIO_GROUP_3_IRQn);
    apb0ps_cfg.gpio3_state = 0;
#endif
#else
    apb0ps_cfg.gpio3_state = 1;
#endif
#else
    apb0ps_cfg.gpio3_state = 1;
#endif
    apb0ps_cfg.gpio3_ap = 1;

#ifdef IP_mb
#ifdef IP_INST_NS_mb
    apb0ps_cfg.mailbox_state = 1;
#else
	TZ_Set_ITS_byIRQNo(MBOX_IRQn);
    apb0ps_cfg.mailbox_state = 0;
#endif
#else
    apb0ps_cfg.mailbox_state = 1;
#endif
    apb0ps_cfg.mailbox_ap = 1;

#ifdef IP_i2s
#ifdef IP_INST_I2S_SLAVE
#ifdef IP_INST_NS_I2S_SLAVE
    apb0ps_cfg.i2sslave_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I2S_SLAVE_rx_da_0_intr_IRQn);
	TZ_Set_ITS_byIRQNo(I2S_SLAVE_rx_or_0_intr_IRQn);
	TZ_Set_ITS_byIRQNo(I2S_SLAVE_tx_emp_0_intr_IRQn);
	TZ_Set_ITS_byIRQNo(I2S_SLAVE_tx_or_0_intr_IRQn);
    apb0ps_cfg.i2sslave_state = 0;
#endif
#else
    apb0ps_cfg.i2sslave_state = 1;
#endif
#else
    apb0ps_cfg.i2sslave_state = 1;
#endif
    apb0ps_cfg.i2sslave_ap = 1;

#ifdef IP_i3c_slv
#ifdef IP_INST_IIIC_SLAVE0
#ifdef IP_INST_NS_IIIC_SLAVE0
    apb0ps_cfg.i3cslave_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I3C_SLAVE_intr_IRQn);
    apb0ps_cfg.i3cslave_state = 0;
#endif
#else
    apb0ps_cfg.i3cslave_state = 1;
#endif
#else
    apb0ps_cfg.i3cslave_state = 1;
#endif
    apb0ps_cfg.i3cslave_ap = 1;


#ifdef IP_iic
#ifdef IP_INST_IIC_SLAVE1
#ifdef IP_INST_NS_IIC_SLAVE1
    apb0ps_cfg.i2cslave1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(I2C_Slave_1_IRQn);
    apb0ps_cfg.i2cslave1_state = 0;
#endif
#else
    apb0ps_cfg.i2cslave1_state = 1;
#endif
#else
    apb0ps_cfg.i2cslave1_state = 1;
#endif
    apb0ps_cfg.i2cslave1_ap = 1;

#ifdef IP_pdm
#ifdef IP_INST_NS_PDM
    apb0ps_cfg.himaxpdm_state = 1;
#else
	TZ_Set_ITS_byIRQNo(HimaxPDM_int_rx_avail_IRQn);
	TZ_Set_ITS_byIRQNo(HimaxPDM_int_err_IRQn);
	TZ_Set_ITS_byIRQNo(HimaxPDM_int_clp_IRQn);
    apb0ps_cfg.himaxpdm_state = 0;
#endif
#else
    apb0ps_cfg.himaxpdm_state = 1;
#endif
    apb0ps_cfg.himaxpdm_ap = 1;
	apb0ps_cfg.irq_enable = 1;
	apb0ps_cfg.callback = &TZ_PPC_APB0PS_CB;
#ifdef IP_watchdog
#ifdef IP_INST_WDT0
#ifdef IP_INST_NS_WDT0
	apb1ns_cfg.wdt0_state = 1;
#else
	TZ_Set_ITS_byIRQNo(WDOG0INT_IRQn);
	apb1ns_cfg.wdt0_state = 0;
#endif
#else
	apb1ns_cfg.wdt0_state = 1;
#endif
#else
	apb1ns_cfg.wdt0_state = 1;
#endif
	apb1ns_cfg.wdt0_ap = 1;
#ifdef IP_watchdog
#ifdef IP_INST_WDT1
#ifdef IP_INST_NS_WDT1
	apb1ns_cfg.wdt1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(WDOG1INT_IRQn);
	apb1ns_cfg.wdt1_state = 0;
#endif
#else
	apb1ns_cfg.wdt1_state = 1;
#endif
#else
	apb1ns_cfg.wdt1_state = 1;
#endif
	apb1ns_cfg.wdt1_ap = 1;
#ifdef TODO
	apb1ns_cfg.apbtestreg_state = 0;
#endif
	apb1ns_cfg.apbtestreg_ap = 0;
#ifdef IP_csitx
#ifdef IP_INST_NS_csitx
	apb1ns_cfg.mipitx_aip_phy_state = 1;
#else
	apb1ns_cfg.mipitx_aip_phy_state = 0;
#endif
#else
	apb1ns_cfg.mipitx_aip_phy_state = 1;
#endif
	apb1ns_cfg.mipitx_aip_phy_ap = 1;
#ifdef IP_xdma
	TZ_Set_ITS_byIRQNo(SC_sen_int_IRQn);
	TZ_Set_ITS_byIRQNo(inpparser_error_int_out_IRQn);
	TZ_Set_ITS_byIRQNo(SC_trigger_timeout_IRQn);
	TZ_Set_ITS_byIRQNo(SC_sen_nframe_end_IRQn);
	TZ_Set_ITS_byIRQNo(inp_vsync_in_IRQn);
	TZ_Set_ITS_byIRQNo(inp_hsync_in_IRQn);
	TZ_Set_ITS_byIRQNo(inp_vsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(inp_hsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(REG_INP_S2P_ERROR_INT_IRQn);
	TZ_Set_ITS_byIRQNo(DP_CDM_MOTION_INT_IRQn);
	TZ_Set_ITS_byIRQNo(DP_ABNORMAL_INT_IRQn);
	TZ_Set_ITS_byIRQNo(edm_int_out_IRQn);
	TZ_Set_ITS_byIRQNo(wdma1_vsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(wdma1_hsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(wdma2_vsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(wdma2_hsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(wdma3_vsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(wdma3_hsync_out_IRQn);
	TZ_Set_ITS_byIRQNo(INP_RX_DE_HCNT_ERROR_INT_IRQn);
	TZ_Set_ITS_byIRQNo(INP_RX2OUT_DE_HCNT_ERROR_INT_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA1_int_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA1_abnormal_int_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA2_int_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA2_abnormal_int_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA3_int_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA3_abnormal_int_IRQn);
	TZ_Set_ITS_byIRQNo(RDMA_int_IRQn);
	TZ_Set_ITS_byIRQNo(RDMA_abnormal_int_IRQn);
#ifdef IP_INST_NS_xdma
	TZ_set_DPDMAMSC(1, 0, 1);
	apb1ns_cfg.inpdp_state = 1;
	apb1ns_cfg.senctrl_state = 1;
#else
	TZ_set_DPDMAMSC(0, 0, 1);
	apb1ns_cfg.inpdp_state = 0;
	apb1ns_cfg.senctrl_state = 0;
#endif
#else
	TZ_set_DPDMAMSC(1, 0, 1);
	apb1ns_cfg.inpdp_state = 1;
	apb1ns_cfg.senctrl_state = 1;
#endif
	apb1ns_cfg.inpdp_ap = 1;
	apb1ns_cfg.senctrl_ap = 1;

#ifdef IP_csirx
#ifdef IP_INST_NS_csirx
	apb1ns_cfg.mipirx_phy_state = 1;
#else
	apb1ns_cfg.mipirx_phy_state = 0;
#endif
#else
	apb1ns_cfg.mipirx_phy_state = 1;
#endif
	apb1ns_cfg.mipirx_phy_ap = 1;

#ifdef IP_timer
#ifdef IP_INST_TIMER0
#ifdef IP_INST_NS_TIMER0
	apb1ns_cfg.timer0_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER0INT_IRQn);
	apb1ns_cfg.timer0_state = 0;
#endif
#else
	apb1ns_cfg.timer0_state = 1;
#endif
#else
	apb1ns_cfg.timer0_state = 1;
#endif
	apb1ns_cfg.timer0_ap = 1;
#ifdef IP_timer
#ifdef IP_INST_TIMER1
#ifdef IP_INST_NS_TIMER1
	apb1ns_cfg.timer1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER1INT_IRQn);
	apb1ns_cfg.timer1_state = 0;
#endif
#else
	apb1ns_cfg.timer1_state = 1;
#endif
#else
	apb1ns_cfg.timer1_state = 1;
#endif
	apb1ns_cfg.timer1_ap = 1;
#ifdef IP_timer
#ifdef IP_INST_TIMER2
#ifdef IP_INST_NS_TIMER2
	apb1ns_cfg.timer2_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER2INT_IRQn);
	apb1ns_cfg.timer2_state = 0;
#endif
#else
	apb1ns_cfg.timer2_state = 1;
#endif
#else
	apb1ns_cfg.timer2_state = 1;
#endif
	apb1ns_cfg.timer2_ap = 1;
#ifdef IP_timer
#ifdef IP_INST_TIMER3
#ifdef IP_INST_NS_TIMER3
	apb1ns_cfg.timer3_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER3INT_IRQn);
	apb1ns_cfg.timer3_state = 0;
#endif
#else
	apb1ns_cfg.timer3_state = 1;
#endif
#else
	apb1ns_cfg.timer3_state = 1;
#endif
	apb1ns_cfg.timer3_ap = 1;
#ifdef IP_timer
#ifdef IP_INST_TIMER4
#ifdef IP_INST_NS_TIMER4
	apb1ns_cfg.timer4_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER4INT_IRQn);
	apb1ns_cfg.timer4_state = 0;
#endif
#else
	apb1ns_cfg.timer4_state = 1;
#endif
#else
	apb1ns_cfg.timer4_state = 1;
#endif
	apb1ns_cfg.timer4_ap = 1;
#ifdef IP_timer
#ifdef IP_INST_TIMER5
#ifdef IP_INST_NS_TIMER5
	apb1ns_cfg.timer5_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER5INT_IRQn);
	apb1ns_cfg.timer5_state = 0;
#endif
#else
	apb1ns_cfg.timer5_state = 1;
#endif
#else
	apb1ns_cfg.timer5_state = 1;
#endif
	apb1ns_cfg.timer5_ap = 1;
	apb1ns_cfg.irq_enable = 1;
	apb1ns_cfg.callback = &TZ_PPC_APB1NS_CB;

	TZ_Set_ITS_byIRQNo(APB_1_NS_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(APB_1_PS_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(AHB_SRAM2_MPC_IRQn);
	TZ_Set_ITS_byIRQNo(AHB_3_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(APB0_NS_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(APB0_PS_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(DMA2_M1_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(DMA2_M2_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(DMA3_M1_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(DMA3_M2_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(DMA_1_AHB_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(APB_2_APB_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(qspi_cache_Cache_MPC_IRQn);
	TZ_Set_ITS_byIRQNo(qspi_cache_DWC_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(qspi_cache_ISP_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(OSPI_MPC_IRQn);
	TZ_Set_ITS_byIRQNo(AXI_ROM_AXI_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(AXI_SRAM0_AXI_MPC_IRQn);
	TZ_Set_ITS_byIRQNo(AXI_SRAM1_AXI_MPC_IRQn);
	TZ_Set_ITS_byIRQNo(AHB1M4_PPC_IRQn);
	TZ_Set_ITS_byIRQNo(DPDMA_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(DMA0_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(WDMA3_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(U55M0_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(U55M1_MSC_IRQn);
	TZ_Set_ITS_byIRQNo(anti_tamp_int_IRQn);


	apb1ps_cfg.scu_sb_state = 0; /*!< SCU SB*/
#ifdef IP_gpio
#ifdef IP_INST_SB_GPIO
#ifdef IP_INST_NS_SB_GPIO
	apb1ps_cfg.sb_gpio_state = 1;
#else
	TZ_Set_ITS_byIRQNo(GPIO_GROUP_4_SB_IRQn);
	apb1ps_cfg.sb_gpio_state = 0;
#endif
#else
	apb1ps_cfg.sb_gpio_state = 1;
#endif
#else
	apb1ps_cfg.sb_gpio_state = 1;
#endif

#ifdef TODO
	apb1ps_cfg.apb_testreg_state = 0;
#endif
#ifdef IP_ts
#ifdef IP_INST_NS_ts
	apb1ps_cfg.ts_ctrl_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TS_CTRL_IRQn);
	apb1ps_cfg.ts_ctrl_state = 0;
#endif
#else
	apb1ps_cfg.ts_ctrl_state = 1;
#endif

#ifdef IP_adcc_hv
#ifdef IP_INST_NS_ADCC_HV
	apb1ps_cfg.adc_ctrl_hv_state = 1;
#else
	TZ_Set_ITS_byIRQNo(ADC_CTRL_HV_IRQn);
	apb1ps_cfg.adc_ctrl_hv_state = 0;
#endif
#else
	apb1ps_cfg.adc_ctrl_hv_state = 1;
#endif
#ifdef IP_hxautoi2c_mst
#ifdef IP_INST_NS_hxautoi2c_mst
	apb1ps_cfg.i2cm_mipi_state = 1;
#else
	TZ_Set_ITS_byIRQNo(mipi_i2c_mst_noack_int_IRQn);
	TZ_Set_ITS_byIRQNo(mipi_i2c_mst_trig_int_IRQn);
	TZ_Set_ITS_byIRQNo(mipi_i2c_mst_stop_int_IRQn);
	apb1ps_cfg.i2cm_mipi_state = 0;
#endif
#else
	apb1ps_cfg.i2cm_mipi_state = 1;
#endif
#ifdef IP_timer
#ifdef IP_INST_TIMER6
#ifdef IP_INST_NS_TIMER6
	apb1ps_cfg.timer6_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER6INT_IRQn);
	apb1ps_cfg.timer6_state = 0;
#endif
#else
	apb1ps_cfg.timer6_state = 1;
#endif
#else
	apb1ps_cfg.timer6_state = 1;
#endif
#ifdef IP_timer
#ifdef IP_INST_TIMER7
#ifdef IP_INST_NS_TIMER7
	apb1ps_cfg.timer7_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER7INT_IRQn);
	apb1ps_cfg.timer7_state = 0;
#endif
#else
	apb1ps_cfg.timer7_state = 1;
#endif
#else
	apb1ps_cfg.timer7_state = 1;
#endif
#ifdef IP_timer
#ifdef IP_INST_TIMER8
#ifdef IP_INST_NS_TIMER8
	apb1ps_cfg.timer8_state = 1;
#else
	TZ_Set_ITS_byIRQNo(TIMER8INT_IRQn);
	apb1ps_cfg.timer8_state = 0;
#endif
#else
	apb1ps_cfg.timer8_state = 1;
#endif
#else
	apb1ps_cfg.timer8_state = 1;
#endif
	apb1ps_cfg.sb_gpio_ap = 1; /*!< SB GPIO AP*/
	apb1ps_cfg.ts_ctrl_ap = 1; /*!< TS Ctrl ap*/
	apb1ps_cfg.adc_ctrl_hv_ap = 1; /*!< ADC CTRL HV ap*/
	apb1ps_cfg.scu_sb_ap = 1; /*!< SCU SB ap*/
	apb1ps_cfg.i2cm_mipi_ap = 1; /*!< I2C Master For MIPI LV */
	apb1ps_cfg.timer6_ap = 1; /*!< Timer6 ap*/
	apb1ps_cfg.timer7_ap = 1; /*!< Timer7 ap*/
	apb1ps_cfg.timer8_ap = 1; /*!< Timer8 ap*/
	apb1ps_cfg.apb_testreg_ap = 1; /*!< APB Test Reg AP */
	apb1ps_cfg.irq_enable = 1;
	apb1ps_cfg.callback = &TZ_PPC_APB1PS_CB;

#ifdef IP_sdio
#ifdef IP_INST_NS_sdio
    apb2_cfg.sdio_state = 1;
	TZ_set_SDIOMSC(1, 0, 1);
#else
	TZ_Set_ITS_byIRQNo(SDIO_IRQn);
	TZ_Set_ITS_byIRQNo(SDIO_WAKEUP_IRQn);
    apb2_cfg.sdio_state = 0;
	TZ_set_SDIOMSC(0, 0, 1);
#endif
#else
    apb2_cfg.sdio_state = 1;
	TZ_set_SDIOMSC(1, 0, 1);
#endif
    apb2_cfg.sdio_ap = 1;
#ifdef IP_u55
#ifdef IP_INST_NS_u55
    apb2_cfg.u55ctrl_state = 1;
    TZ_set_U55M0MSC(1, 0, 1);
    TZ_set_U55M1MSC(1, 0, 1);
	hx_drv_scu_set_U55_PORSL(SCU_U55_PORSL_NSECURE);
	hx_drv_scu_set_U55_PORPL(SCU_U55_PORPL_PRIVILEGED);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_RESET);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_NORMAL);
#else
	TZ_Set_ITS_byIRQNo(U55_IRQn);
    apb2_cfg.u55ctrl_state = 0;
    TZ_set_U55M0MSC(0, 0, 1);
    TZ_set_U55M1MSC(0, 0, 1);
	hx_drv_scu_set_U55_PORSL(SCU_U55_PORSL_SECURE);
	hx_drv_scu_set_U55_PORPL(SCU_U55_PORPL_PRIVILEGED);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_RESET);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_NORMAL);
#endif
#else
    apb2_cfg.u55ctrl_state = 1;
    TZ_set_U55M0MSC(1, 0, 1);
    TZ_set_U55M1MSC(1, 0, 1);
	hx_drv_scu_set_U55_PORSL(SCU_U55_PORSL_NSECURE);
	hx_drv_scu_set_U55_PORPL(SCU_U55_PORPL_PRIVILEGED);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_RESET);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_NORMAL);
#endif
    apb2_cfg.u55ctrl_ap = 1;
    apb2_cfg.cc312apbs_state = 0;
    apb2_cfg.cc312apbsc_state = 0;
    apb2_cfg.pufrt_state = 0;
    apb2_cfg.cc312apbs_ap = 1;
    apb2_cfg.cc312apbsc_ap = 1;
    apb2_cfg.pufrt_ap = 1;
#ifdef IP_csitx
#ifdef IP_INST_NS_csitx
    apb2_cfg.mipitxctrl_state = 1;
#else
	TZ_Set_ITS_byIRQNo(MIPI_TX_IRQn);
    apb2_cfg.mipitxctrl_state = 0;
#endif
#else
    apb2_cfg.mipitxctrl_state = 1;
#endif
    apb2_cfg.mipitxctrl_ap = 1;
    apb2_cfg.scu_hsc_state = 0;
    apb2_cfg.cs_dbg_state = 0;
    apb2_cfg.cs_sdc_state = 0;
    apb2_cfg.cs_apb_async_state = 0;
    apb2_cfg.scu_hsc_ap = 1;
    apb2_cfg.cs_dbg_ap = 1;
    apb2_cfg.cs_sdc_ap = 1;
    apb2_cfg.cs_apb_async_ap = 1;
#ifdef IP_csirx
#ifdef IP_INST_NS_csirx
    apb2_cfg.mipirx_ctrl_state = 1; /*!< [11] MPIP RX CTRL*/
#else
	TZ_Set_ITS_byIRQNo(MIPI_RX_Interrupt0_IRQn);
	TZ_Set_ITS_byIRQNo(MIPI_RX_Interrupt1_IRQn);
	TZ_Set_ITS_byIRQNo(MIPI_RX_Interrupt2_IRQn);
	TZ_Set_ITS_byIRQNo(MIPI_RX_Interrupt3_IRQn);
    apb2_cfg.mipirx_ctrl_state = 0; /*!< [11] MPIP RX CTRL*/
#endif
#else
    apb2_cfg.mipirx_ctrl_state = 1; /*!< [11] MPIP RX CTRL*/
#endif
    apb2_cfg.mipirx_ctrl_ap = 1; /*!< [11] MPIP RX CTRL*/
#ifdef IP_i3c_mst
#ifdef IP_INST_NS_i3c_mst
	TZ_set_I3CHMSC(1, 0, 1);
    apb2_cfg.i3c_hc_state = 1; /*!< [15] I3C_HC*/
#else
	TZ_Set_ITS_byIRQNo(I3C_HC_MSC_IRQ_IRQn);
	TZ_Set_ITS_byIRQNo(I3C_INT_IRQn);
	TZ_set_I3CHMSC(0, 0, 1);
    apb2_cfg.i3c_hc_state = 0; /*!< [15] I3C_HC*/
#endif
#else
	TZ_set_I3CHMSC(1, 0, 1);
    apb2_cfg.i3c_hc_state = 1; /*!< [15] I3C_HC*/
#endif
    apb2_cfg.i3c_hc_ap = 1; /*!< [15] I3C_HC*/

#ifdef IP_xdma
#ifdef IP_INST_NS_xdma
    apb2_cfg.img_status_state = 1;
#else
    apb2_cfg.img_status_state = 0;
#endif
#else
    apb2_cfg.img_status_state = 1;
#endif
    apb2_cfg.img_status_ap = 1;
    apb2_cfg.irq_enable = 1;
    apb2_cfg.callback = &TZ_PPC_APB2_CB;
	TZ_Set_ITS_byIRQNo(pufrt_top_IRQn);

	TZ_Set_ITS_byIRQNo(APB1_AON_PPC_IRQn);
	apb1_aon_cfg.scu_aon_state = 0; /*!< SCU AON state*/
#ifdef IP_pmu
#ifdef IP_INST_NS_pmu
	apb1_aon_cfg.pmu_state = 1;
#else
	TZ_Set_ITS_byIRQNo(pmu_wdg_timeout_int_IRQn);
	apb1_aon_cfg.pmu_state = 0;
#endif
#else
	apb1_aon_cfg.pmu_state = 1;
#endif

#ifdef IP_gpio
#ifdef IP_INST_AON_GPIO
#ifdef IP_INST_NS_AON_GPIO
	apb1_aon_cfg.aon_gpio_state = 1;
#else
	TZ_Set_ITS_byIRQNo(GPIO_GROUP_5_AON_IRQn);
	apb1_aon_cfg.aon_gpio_state = 0;
#endif
#else
	apb1_aon_cfg.aon_gpio_state = 1;
#endif
#else
	apb1_aon_cfg.aon_gpio_state = 1;
#endif

#ifdef IP_rtc
#ifdef IP_INST_RTC0
#ifdef IP_INST_NS_RTC0
	apb1_aon_cfg.rtc0_state = 1;
#else
	TZ_Set_ITS_byIRQNo(RTC0INTR_IRQn);
	apb1_aon_cfg.rtc0_state = 0;
#endif
#else
	apb1_aon_cfg.rtc0_state = 1;
#endif
#else
	apb1_aon_cfg.rtc0_state = 1;
#endif

#ifdef IP_rtc
#ifdef IP_INST_RTC1
#ifdef IP_INST_NS_RTC1
	apb1_aon_cfg.rtc1_state = 1;
#else
	TZ_Set_ITS_byIRQNo(RTC1INTR_IRQn);
	apb1_aon_cfg.rtc1_state = 0;
#endif
#else
	apb1_aon_cfg.rtc1_state = 1;
#endif
#else
	apb1_aon_cfg.rtc1_state = 1;
#endif

#ifdef IP_rtc
#ifdef IP_INST_RTC2
#ifdef IP_INST_NS_RTC2
	apb1_aon_cfg.rtc2_state = 1;
#else
	TZ_Set_ITS_byIRQNo(RTC2INTR_IRQn);
	apb1_aon_cfg.rtc2_state = 0;
#endif
#else
	apb1_aon_cfg.rtc2_state = 1;
#endif
#else
	apb1_aon_cfg.rtc2_state = 1;
#endif

#ifdef IP_swreg_aon
#ifdef IP_INST_NS_swreg_aon
	apb1_aon_cfg.sw_reg_aon_state = 1;
#else
	apb1_aon_cfg.sw_reg_aon_state = 0;
#endif
#else
	apb1_aon_cfg.sw_reg_aon_state = 1;
#endif
	apb1_aon_cfg.scu_aon_ap = 1; /*!< scu AP */
	apb1_aon_cfg.pmu_ap = 1; /*!< PMU state*/
	apb1_aon_cfg.aon_gpio_ap = 1; /*!< AON GPIO AP*/
	apb1_aon_cfg.rtc0_ap = 1; /*!< RTC0 AP*/
	apb1_aon_cfg.rtc1_ap = 1; /*!< RTC1 AP*/
	apb1_aon_cfg.rtc2_ap = 1; /*!< RTC2 AP*/
	apb1_aon_cfg. sw_reg_aon_ap = 1; /*!< SW Reg AON ap*/
	apb1_aon_cfg.irq_enable = 1; /*!< PPC_IRQ_ENABLE */
	apb1_aon_cfg.callback = &TZ_PPC_APB1_AON_CB;

	hx_drv_ppc_set_dwc(dwc_cfg);
    hx_drv_ppc_set_isp(isp_cfg);
    hx_drv_ppc_set_ahb3(ahb3_cfg);
    hx_drv_ppc_set_ahb1m4(ahb1m4_cfg);
    hx_drv_ppc_set_apb0ns(apb0ns_cfg);
	hx_drv_ppc_set_apb0ps(apb0ps_cfg);
	hx_drv_ppc_set_apb1ns(apb1ns_cfg);
    hx_drv_ppc_set_apb1ps(apb1ps_cfg);
    hx_drv_ppc_set_apb2(apb2_cfg);
    hx_drv_ppc_set_apb1_aon(apb1_aon_cfg);
#endif

	TZ_mpc_register();
}

