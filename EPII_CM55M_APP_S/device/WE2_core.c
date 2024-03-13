/*
 * mpu.c
 *
 *  Created on: 2021
 *      Author: 902447
 */
#include "WE2_device.h"
#include "cachel1_armv7.h"
#include "board.h"
#include "BITOPS.h"
#include "WE2_core.h"
#include "xprintf.h"
#ifdef NSC
#include "veneer_clk_ctrl.h"
#else
#include "hx_drv_scu.h"
#endif

#define MSCR_CTRL_ENABLE     1
#define MPU_QSPI_REGION    14
#define MPU_OSPI_REGION    15
/*-----------------------------------------------------------*/
/* Device memory attributes used in MPU_MAIR registers.
 *
 * 8-bit values encoded as follows:
 *  Bit[7:4] - 0000 - Device Memory
 *  Bit[3:2] - 00 --> Device-nGnRnE
 *				01 --> Device-nGnRE
 *				10 --> Device-nGRE
 *				11 --> Device-GRE
 *  Bit[1:0] - 00, Reserved.
 */
#define MPU_DEVICE_MEMORY_nGnRnE						( 0x00 ) /* 0000 0000 */
#define MPU_DEVICE_MEMORY_nGnRE							( 0x04 ) /* 0000 0100 */
#define MPU_DEVICE_MEMORY_nGRE							( 0x08 ) /* 0000 1000 */
#define MPU_DEVICE_MEMORY_GRE							( 0x0C ) /* 0000 1100 */
/* Normal memory attributes used in MPU_MAIR registers. */
#define MPU_NORMAL_MEMORY_NON_CACHEABLE					( 0x44 ) /* Non-cacheable. */
#define MPU_NORMAL_MEMORY_BUFFERABLE_CACHEABLE			( 0xFF ) /* Non-Transient, Write-back, Read-Allocate and Write-Allocate. */
/*
 * Memory region  MAIR_ATTR.Outer MAIR_ATTR.Inner Shareability
 * Flash memory 	!=0000 			0b1010 			0
 */
#define MPU_NORMAL_MEMORY_FLASH							( 0x44  )

/* Attributes used in MPU_RBAR registers. */
#define MPU_REGION_NON_SHAREABLE						( 0UL << 3UL )
#define MPU_REGION_INNER_SHAREABLE						( 1UL << 3UL )
#define MPU_REGION_OUTER_SHAREABLE						( 2UL << 3UL )

#define MPU_REGION_PRIVILEGED_READ_WRITE				( 0UL << 1UL )
#define MPU_REGION_READ_WRITE							( 1UL << 1UL )
#define MPU_REGION_PRIVILEGED_READ_ONLY					( 2UL << 1UL )
#define MPU_REGION_READ_ONLY							( 3UL << 1UL )

#define TCMCR_ENABLE_Pos                0U                                            /*!< MPU TYPE: DREGION Position */
#define TCMCR_ENABLE_Msk               (0x1UL << TCMCR_ENABLE_Pos)
#define TCMCR_ENABLE_Size               1

#define TCMCR_SIZE_Pos                3U                                            /*!< MPU TYPE: DREGION Position */
#define TCMCR_SIZE_Msk               (0xFUL << TCMCR_ENABLE_Pos)
#define TCMCR_SIZE_Size                4U

#define TGUCTRL_DBFEN_Pos                0U
#define TGUCTRL_DBFEN_Msk               (0x1UL << TGUCTRL_DBFEN_Pos)
#define TGUCTRL_DBFEN_Size                1U

#define TGUCTRL_DEREN_Pos                1U
#define TGUCTRL_DEREN_Msk               (0x1UL << TGUCTRL_DEREN_Pos)
#define TGUCTRL_DEREN_Size                1U

#define TGUCFG_PRESENT_Pos               31U
#define TGUCFG_PRESENT_Msk               (0x1UL << TGUCFG_PRESENT_Pos)
#define TGUCFG_PRESENT_Size               1U

#define TGUCFG_NUMBLKS_Pos               8U
#define TGUCFG_NUMBLKS_Msk               (0xFUL << TGUCFG_NUMBLKS_Pos)
#define TGUCFG_NUMBLKS_Size               4U

#define TGUCFG_BLKSZ_Pos               0U
#define TGUCFG_BLKSZ_Msk               (0xFUL << TGUCFG_BLKSZ_Pos)
#define TGUCFG_BLKSZ_Size               4U

#define REG_MSCR_ADDR	0xE001E000
#define REG_MSCR_ICACTIVE_POS	13
#define REG_MSCR_ICACTIVE_SIZE	1
#define REG_MSCR_DCACTIVE_POS	12
#define REG_MSCR_DCACTIVE_SIZE	1

extern uint32_t SystemCoreClock;

#define EPII_SYS_TIMER_US_HZ		(1000000)

void EPII_set_reg(unsigned int addr, unsigned int val) {
	(*((volatile unsigned int*) addr)) = val;
}

unsigned int EPII_get_reg(unsigned int addr) {
	unsigned int val;
	val = (*((volatile unsigned int*) addr));
	return val;
}

__STATIC_INLINE uint32_t EPII_NVIC_GetVectorAddr(IRQn_Type IRQn)
{
	  uint32_t *vectors = (uint32_t *)SCB->VTOR;
	  return (uint32_t)&vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET];
}

/**
 \brief  Get system clock

system clock
 */
void EPII_Get_Systemclock(uint32_t *val)
{
	*val = SystemCoreClock;
}

#ifndef BOOT_USED
#if !defined(ENABLE_OS) && !defined(RTE_CMSIS_RTOS2) && !defined(RTE_RTOS_FreeRTOS_CORE)
#define EnablePrivilegedMode() __asm("SVC #0")
void SVC_Handler_Main_CORE( unsigned int *svc_args )
{
  unsigned int svc_number;

  /*
  * Stack contains:
  * r0, r1, r2, r3, r12, r14, the return address and xPSR
  * First argument (r0) is svc_args[0]
  */
  svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
  switch( svc_number )
  {
    case 0:  /* EnablePrivilegedMode */
      __set_CONTROL( __get_CONTROL( ) & ~CONTROL_nPRIV_Msk ) ;
      break;
    default:    /* unknown SVC */
      break;
  }
}

void SVC_Handler(void)
{
  __asm(
    ".global SVC_Handler_Main_CORE\n"
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r0, MSP\n"
    "MRSNE r0, PSP\n"
    "B SVC_Handler_Main_CORE\n"
  ) ;
}
#endif
#endif//#ifndef BOOT_USED

void EPII_QSPIXIP_MEM_Attribute_S(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att) {
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
	uint32_t size = 0;

	MPU->CTRL = MPU->CTRL & 0xFFFFFFFE;

	  //  using Attr 4
	ARM_MPU_SetMemAttr(4UL, MPU_NORMAL_MEMORY_FLASH);
	  //  using Attr 5
	ARM_MPU_SetMemAttr(5UL, MPU_DEVICE_MEMORY_nGnRE);

	if (mem_att == SPIXIP_MEM_XIP) {
		MPU->RNR = MPU_QSPI_REGION;
		MPU->RBAR = ARM_MPU_RBAR(start_addr, 1, 0, 1, 0);
		MPU->RLAR = ARM_MPU_RLAR(end_addr-1, 4);
	} else {
		MPU->RNR = MPU_QSPI_REGION;
		MPU->RBAR = ARM_MPU_RBAR(start_addr, 0, 0, 1, 0);
		MPU->RLAR = ARM_MPU_RLAR(end_addr-1, 5);
	}


#endif
}

extern void EPII_OSPIXIP_MEM_Attribute_S(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att)
{
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
	uint32_t size = 0;

	MPU->CTRL = MPU->CTRL & 0xFFFFFFFE;

	  //  using Attr 4
	ARM_MPU_SetMemAttr(4UL, MPU_NORMAL_MEMORY_FLASH);
	  //  using Attr 5
	ARM_MPU_SetMemAttr(5UL, MPU_DEVICE_MEMORY_nGnRE);
	if (mem_att == SPIXIP_MEM_XIP) {
		MPU->RNR = MPU_OSPI_REGION;
		MPU->RBAR = ARM_MPU_RBAR(start_addr, 1, 0, 1, 0);
		MPU->RLAR = ARM_MPU_RLAR(end_addr-1, 4);
	} else {
		MPU->RNR = MPU_OSPI_REGION;
		MPU->RBAR = ARM_MPU_RBAR(start_addr, 0, 0, 1, 0);
		MPU->RLAR = ARM_MPU_RLAR(end_addr-1, 5);
	}

#endif
}


void EPII_QSPIXIP_MEM_Attribute_NS(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att) {
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
#ifdef TRUSTZONE_SEC

	MPU_NS->CTRL = MPU_NS->CTRL & 0xFFFFFFFE;

	  //  using Attr 4
	ARM_MPU_SetMemAttr_NS(4UL, MPU_NORMAL_MEMORY_FLASH);
	  //  using Attr 5
	ARM_MPU_SetMemAttr_NS(5UL, MPU_DEVICE_MEMORY_nGnRE);

	if (mem_att == SPIXIP_MEM_XIP) {
		MPU_NS->RNR = MPU_QSPI_REGION;
		MPU_NS->RBAR = ARM_MPU_RBAR(start_addr, 1, 0, 1, 0);
		MPU_NS->RLAR = ARM_MPU_RLAR(end_addr-1, 4);
	} else {
		MPU_NS->RNR = MPU_QSPI_REGION;
		MPU_NS->RBAR = ARM_MPU_RBAR(start_addr, 0, 0, 1, 0);
		MPU_NS->RLAR = ARM_MPU_RLAR(end_addr-1, 5);
	}


#endif
#endif
}

extern void EPII_OSPIXIP_MEM_Attribute_NS(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att)
{
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
#ifdef TRUSTZONE_SEC

	MPU_NS->CTRL = MPU_NS->CTRL & 0xFFFFFFFE;

	  //  using Attr 4
	ARM_MPU_SetMemAttr_NS(4UL, MPU_NORMAL_MEMORY_FLASH);
	  //  using Attr 5
	ARM_MPU_SetMemAttr_NS(5UL, MPU_DEVICE_MEMORY_nGnRE);
	if (mem_att == SPIXIP_MEM_XIP) {
		MPU_NS->RNR = MPU_OSPI_REGION;
		MPU_NS->RBAR = ARM_MPU_RBAR(start_addr, 1, 0, 1, 0);
		MPU_NS->RLAR = ARM_MPU_RLAR(end_addr-1, 4);
	} else {
		MPU_NS->RNR = MPU_OSPI_REGION;
		MPU_NS->RBAR = ARM_MPU_RBAR(start_addr, 0, 0, 1, 0);
		MPU_NS->RLAR = ARM_MPU_RLAR(end_addr-1, 5);
	}
#endif
#endif
}

/**
 \brief  Set MPU enable

 */
extern void EPII_MPU_Enable(uint32_t enable, uint32_t HFNMIENA_enable, uint32_t PRIVDEFENA_enable)
{
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
	uint32_t ctrl;
	ctrl = enable | (HFNMIENA_enable << MPU_CTRL_HFNMIENA_Pos) | (PRIVDEFENA_enable << MPU_CTRL_PRIVDEFENA_Pos);
	if(enable == 0)
	{
		ARM_MPU_Disable();
	}else{
		ARM_MPU_Enable(ctrl);
	}
#endif
}


extern void EPII_MPU_Enable_NS(uint32_t enable, uint32_t HFNMIENA_enable, uint32_t PRIVDEFENA_enable)
{
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
#ifdef TRUSTZONE_SEC
	uint32_t ctrl;
	ctrl = enable | (HFNMIENA_enable << MPU_CTRL_HFNMIENA_Pos) | (PRIVDEFENA_enable << MPU_CTRL_PRIVDEFENA_Pos);
	if(enable == 0)
	{
		ARM_MPU_Disable_NS();
	}else{
		ARM_MPU_Enable_NS(ctrl);
	}
#endif
#endif
}

/**
 \brief  Get MPU Region in Secure

 Get MPU Region in Secure.
 */
void EPII_Get_MPU_Region_S(uint8_t *mpu_region) {
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
	*mpu_region = (MPU->TYPE >> MPU_TYPE_DREGION_Pos) & 0xFF;
#endif
}

/**
 \brief  Get MPU Region in NS

 Get MPU Region in Secure.
 */
void EPII_Get_MPU_Region_NS(uint8_t *mpu_region) {
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
	*mpu_region = (MPU_NS->TYPE >> MPU_TYPE_DREGION_Pos) & 0xFF;
#endif
#endif
}

#ifndef BOOT_USED
/**
 \brief  Get ITCM Size

 Get ITCM Size.
 */
void EPII_Get_ITCM_Size(TCM_SZ_E *size) {
	*size = HX_BIT_GET(ITCMCR_REG, TCMCR_SIZE_Size, TCMCR_SIZE_Pos);
}

/**
 \brief  Set ITCM Enable

 Set ITCM enable.
 */
void EPII_Set_ITCM_En(uint8_t enable) {
	HX_BIT_SET(ITCMCR_REG, TCMCR_ENABLE_Size, TCMCR_ENABLE_Pos, enable);
}

/**
 \brief  Get ITCM Enable

 Get ITCM enable.
 */
void EPII_Get_ITCM_En(uint8_t *enable) {
	*enable = HX_BIT_GET(ITCMCR_REG, TCMCR_ENABLE_Size, TCMCR_ENABLE_Pos);
}
#endif
/**
 \brief  Set ITGU Error enable

 Set ITGU Error enable.
 */
void EPII_Set_ITGU_Err_En(uint8_t DBFEN, uint8_t DEREN) {
	HX_BIT_SET(ITGU_CTRL_REG, TGUCTRL_DBFEN_Size, TGUCTRL_DBFEN_Pos, DBFEN);

	HX_BIT_SET(ITGU_CTRL_REG, TGUCTRL_DEREN_Size, TGUCTRL_DEREN_Pos, DEREN);
}

/**
 \brief  Get ITGU Error enable

 Get ITGU Error enable.
 */
void EPII_Get_ITGU_Err_En(uint8_t *DBFEN, uint8_t *DEREN) {
	*DBFEN = HX_BIT_GET(ITGU_CTRL_REG, TGUCTRL_DBFEN_Size, TGUCTRL_DBFEN_Pos);
	*DEREN = HX_BIT_GET(ITGU_CTRL_REG, TGUCTRL_DEREN_Size, TGUCTRL_DEREN_Pos);
}

/**
 \brief  Get ITGU CFG

 Get ITGU Configuration
 */
void EPII_Get_ITGU_CFG(TGU_PRESENT_E *present, uint8_t *NUMBLKS, uint8_t *BLKSZ) {
	*present = HX_BIT_GET(ITGU_CFG_REG, TGUCFG_PRESENT_Size,
			TGUCFG_PRESENT_Pos);

	*NUMBLKS = HX_BIT_GET(ITGU_CFG_REG, TGUCFG_NUMBLKS_Size,
			TGUCFG_NUMBLKS_Pos);

	*BLKSZ = HX_BIT_GET(ITGU_CFG_REG, TGUCFG_BLKSZ_Size, TGUCFG_BLKSZ_Pos);
}

/**
 \brief  Set ITGU LUT configuration

 Set ITGU LUT configuration
 */
void EPII_Set_ITGU_LUT(uint8_t block_no, TGU_LUT_E lut) {
	HX_BIT_SET(ITGU_LUT0_CFG_REG, 1, block_no, lut);
}
/**
 \brief  Get ITGU LUT configuration

 Get ITGU LUT configuration
 */
void EPII_Get_ITGU_LUT(uint8_t block_no, TGU_LUT_E *lut) {
	*lut = HX_BIT_GET(ITGU_LUT0_CFG_REG, 1, block_no);
}
#ifndef BOOT_USED
/**
 \brief  Get DTCM Size

 Get DTCM Size.
 */
void EPII_Get_DTCM_Size(TCM_SZ_E *size) {
	*size = HX_BIT_GET(DTCMCR_REG, TCMCR_SIZE_Size, TCMCR_SIZE_Pos);
}

/**
 \brief  Set DTCM Enable

 Set DTCM enable.
 */
void EPII_Set_DTCM_En(uint8_t enable) {
	HX_BIT_SET(DTCMCR_REG, TCMCR_ENABLE_Size, TCMCR_ENABLE_Pos, enable);
}

/**
 \brief  Get DTCM Enable

 Get DTCM enable.
 */
void EPII_Get_DTCM_En(uint8_t *enable) {
	*enable = HX_BIT_GET(DTCMCR_REG, TCMCR_ENABLE_Size, TCMCR_ENABLE_Pos);
}
#endif
/**
 \brief  Set DTGU Error enable

 Set DTGU Error enable.
 */
void EPII_Set_DTGU_Err_En(uint8_t DBFEN, uint8_t DEREN) {
	HX_BIT_SET(DTGU_CTRL_REG, TGUCTRL_DBFEN_Size, TGUCTRL_DBFEN_Pos, DBFEN);

	HX_BIT_SET(DTGU_CTRL_REG, TGUCTRL_DEREN_Size, TGUCTRL_DEREN_Pos, DEREN);
}

/**
 \brief  Get DTGU Error enable

 Get DTGU Error enable.
 */
void EPII_Get_DTGU_Err_En(uint8_t *DBFEN, uint8_t *DEREN) {
	*DBFEN = HX_BIT_GET(DTGU_CTRL_REG, TGUCTRL_DBFEN_Size, TGUCTRL_DBFEN_Pos);
	*DEREN = HX_BIT_GET(DTGU_CTRL_REG, TGUCTRL_DEREN_Size, TGUCTRL_DEREN_Pos);
}

/**
 \brief  Get DTGU CFG

 Get DTGU Configuration
 */
void EPII_Get_DTGU_CFG(TGU_PRESENT_E *present, uint8_t *NUMBLKS, uint8_t *BLKSZ) {
	*present = HX_BIT_GET(DTGU_CFG_REG, TGUCFG_PRESENT_Size,
			TGUCFG_PRESENT_Pos);

	*NUMBLKS = HX_BIT_GET(DTGU_CFG_REG, TGUCFG_NUMBLKS_Size,
			TGUCFG_NUMBLKS_Pos);

	*BLKSZ = HX_BIT_GET(DTGU_CFG_REG, TGUCFG_BLKSZ_Size, TGUCFG_BLKSZ_Pos);
}

/**
 \brief  Set DTGU LUT configuration

 Set DTGU LUT configuration
 */
void EPII_Set_DTGU_LUT(uint8_t block_no, TGU_LUT_E lut) {
	HX_BIT_SET(DTGU_LUT0_CFG_REG, 1, block_no, lut);
}
/**
 \brief  Get DTGU LUT configuration

 Get DTGU LUT configuration
 */
void EPII_Get_DTGU_LUT(uint8_t block_no, TGU_LUT_E *lut) {
	*lut = HX_BIT_GET(DTGU_LUT0_CFG_REG, 1, block_no);
}
#ifndef BOOT_USED
/**
 \brief  Set systick load

 Set systick load
 */
void EPII_Set_Systick_load(uint32_t tick) {
	SysTick->LOAD = (uint32_t) tick;
	SysTick->VAL = 0UL;
}

/**
 \brief  Set systick load

 GSet systick load
 */
void EPII_Get_Systick_load(uint32_t *load) {
	*load = SysTick->LOAD;
}

/**
 \brief  Get systick val

 Get systick val
 */
void EPII_Get_Systick_val(uint32_t *val) {
	*val = SysTick->VAL;
}
#endif
/**
 \brief  Set systick load

 Set systick load
 */
extern void EPII_Set_Systick_enable(uint32_t enable) {
	if (enable == 0) {
		SysTick->VAL = 0;
		SysTick->CTRL = 0;
	} else {
		SysTick->VAL = 0;
		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
		SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
	}
}
#ifndef BOOT_USED
/**
 \brief  Set CPU Mode

 Set CPU Mode
 */
extern EPII_CORE_RET_E EPII_Set_CPU_Mode(CPU_MODE_E mode)
{
	uint32_t current_mode, val;
	val = __get_CONTROL();

	current_mode = val & CONTROL_nPRIV_Msk;
	if(current_mode == mode)
	{
		return EPII_CORE_RET_SUCCESS;
	}else{
		if(mode == CPU_MODE_USER)
		{
			val = val | mode;
			__set_CONTROL(val);
		}else{
#if !defined(ENABLE_OS) && !defined(RTE_CMSIS_RTOS2) && !defined(RTE_RTOS_FreeRTOS_CORE)
			EnablePrivilegedMode();
#else
			return EPII_CORE_RET_FAIL;
#endif
		}
	}
	return EPII_CORE_RET_SUCCESS;
}

/**
 \brief  Get CPU Mode

 Get CPU Mode
 */
extern void EPII_Get_CPU_Mode(CPU_MODE_E *mode)
{
	uint32_t current_mode;
	current_mode = __get_CONTROL();
	*mode = current_mode & CONTROL_nPRIV_Msk;
}

/**
 \brief  Set Stack Sel

 Set CPU Mode
 */
extern EPII_CORE_RET_E EPII_Set_Stack_Sel(STACK_MODE_E mode)
{
	uint32_t current_mode, val;
	val = __get_CONTROL();
#if 0
	current_mode = val & CONTROL_nPRIV_Msk;
	if(current_mode == mode)
	{
		return EPII_CORE_RET_SUCCESS;
	}else{
		if(mode == CPU_MODE_USER)
		{
			val = val | mode;
			__set_CONTROL(val);
		}else{
			g_ori_EPIIcore_svcaddr = __NVIC_GetVector(SVCall_IRQn);
			EPII_NVIC_SetVector(SVCall_IRQn, &SVC_Handler_Core);

			EnablePrivilegedMode();
		}
	}
#else
	current_mode = val & (uint32_t)(~CONTROL_SPSEL_Msk);
	val = val | (mode << CONTROL_SPSEL_Pos);
	__set_CONTROL(val);
#endif
	return EPII_CORE_RET_SUCCESS;
}

/**
 \brief  Get Stack Sel

 Get CPU Mode
 */
extern void EPII_Get_Stack_Sel(STACK_MODE_E *mode)
{
	uint32_t current_mode;
	current_mode = __get_CONTROL();
	*mode = (current_mode & CONTROL_SPSEL_Msk) >> CONTROL_SPSEL_Pos;
}
#endif//#ifndef BOOT_USED
/**
 \brief  NVIC Set Vector

 Set NVIC Vector
 */
extern void EPII_NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
#ifdef BOOTROM
	return;
#else
	uint32_t addr;
	NVIC_SetVector(IRQn, vector);
	addr = EPII_NVIC_GetVectorAddr(IRQn);
	if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
	{
		SCB_CleanDCache_by_Addr((volatile void *)addr, 4);
	}
	if ((SCB->CCR & SCB_CCR_IC_Msk) != 0U)
	{
		SCB_InvalidateICache_by_Addr((volatile void *)addr, 4);
	}
#endif
}



void hx_set_memory(unsigned int addr, unsigned int val) {
	(*((volatile unsigned int*) addr)) = val;
}

unsigned int hx_get_memory(unsigned int addr) {
	unsigned int val;
	val = (*((volatile unsigned int*) addr));
	return val;
}

void EPII_cpu_nop_us(uint32_t period_us)
{
	uint32_t freq, delay_nop_cnt;

#ifdef CM55_BIG
#ifdef NSC
	freq = veneer_clk_get_freq(SCU_CLK_FREQ_TYPE_HSC_CM55M);
#else
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_HSC_CM55M, &freq);
#endif
#else
#ifdef NSC
	freq = veneer_clk_get_freq(SCU_CLK_FREQ_TYPE_LSC_CM55S);
#else
	hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_LSC_CM55S, &freq);
#endif
#endif
	delay_nop_cnt = (freq/EPII_SYS_TIMER_US_HZ) * period_us;

	for(uint32_t idx = 0; idx < delay_nop_cnt; idx++)
	{
	    __NOP();
	}
}


/**
 \brief  Enable/Disable I Cache

 Enable/Disable I Cache
 */
extern void hx_enable_ICache(uint32_t enable)
{
	uint32_t val;
	if(enable == 0)
	{
		SCB_DisableICache();
#ifdef BOOTROM
		val = EPII_get_reg(REG_MSCR_ADDR);
		HX_BIT_SET(val, REG_MSCR_ICACTIVE_SIZE, REG_MSCR_ICACTIVE_POS, 1);
		EPII_set_reg(REG_MSCR_ADDR, val);
#else
#if MSCR_CTRL_ENABLE
		val = EPII_get_reg(REG_MSCR_ADDR);
		HX_BIT_SET(val, REG_MSCR_ICACTIVE_SIZE, REG_MSCR_ICACTIVE_POS, 0);
		EPII_set_reg(REG_MSCR_ADDR, val);
#endif
#endif
	}else{
#if MSCR_CTRL_ENABLE
		val = EPII_get_reg(REG_MSCR_ADDR);
		HX_BIT_SET(val, REG_MSCR_ICACTIVE_SIZE, REG_MSCR_ICACTIVE_POS, 1);
		EPII_set_reg(REG_MSCR_ADDR, val);
#endif
		SCB_EnableICache();
	}
}

/**
 \brief  Enable/Disable D Cache

 Enable/Disable D Cache
 */
extern void hx_enable_DCache(uint32_t enable)
{
	uint32_t val;
	if(enable == 0)
	{
		SCB_DisableDCache();
#if MSCR_CTRL_ENABLE
#ifdef BOOTROM
		val = EPII_get_reg(REG_MSCR_ADDR);
		HX_BIT_SET(val, REG_MSCR_DCACTIVE_SIZE, REG_MSCR_DCACTIVE_POS, 1);
		EPII_set_reg(REG_MSCR_ADDR, val);
#else
		val = EPII_get_reg(REG_MSCR_ADDR);
		HX_BIT_SET(val, REG_MSCR_DCACTIVE_SIZE, REG_MSCR_DCACTIVE_POS, 0);
		EPII_set_reg(REG_MSCR_ADDR, val);
#endif
#endif
	}else{
#if MSCR_CTRL_ENABLE
		val = EPII_get_reg(REG_MSCR_ADDR);
		HX_BIT_SET(val, REG_MSCR_DCACTIVE_SIZE, REG_MSCR_DCACTIVE_POS, 1);
		EPII_set_reg(REG_MSCR_ADDR, val);
#endif
		SCB_EnableDCache();
	}
}

/**
 \brief  D-Cache clean by address

 D-Cache clean by address
 */
extern void hx_CleanDCache_by_Addr(volatile void *addr, int32_t dsize)
{
	uint32_t dtcm_start;
	uint32_t dtcm_end;
	uint32_t itcm_start;
	uint32_t itcm_end;
#ifdef TRUSTZONE_SEC
	dtcm_start = BASE_ADDR_DTCM_ALIAS;
	dtcm_end = BASE_ADDR_DTCM_ALIAS + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM_ALIAS;
	itcm_end = BASE_ADDR_ITCM_ALIAS + ITCM_SIZE;
#else
	dtcm_start = BASE_ADDR_DTCM;
	dtcm_end = BASE_ADDR_DTCM + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM;
	itcm_end = BASE_ADDR_ITCM + ITCM_SIZE;
#endif

	if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
	{
		if(((uint32_t)addr >= dtcm_start) && ((uint32_t)addr <= dtcm_end))
		{
			return;
		}
		if(((uint32_t)addr >= itcm_start) && ((uint32_t)addr <= itcm_end))
		{
			return;
		}
		SCB_CleanDCache_by_Addr(addr, dsize);
	}
}

/**
 \brief  D-Cache Invalidate by address

 D-Cache Invalidate by address
 */
extern void hx_InvalidateDCache_by_Addr(volatile void *addr, int32_t dsize)
{
	uint32_t dtcm_start;
	uint32_t dtcm_end;
	uint32_t itcm_start;
	uint32_t itcm_end;
#ifdef TRUSTZONE_SEC
	dtcm_start = BASE_ADDR_DTCM_ALIAS;
	dtcm_end = BASE_ADDR_DTCM_ALIAS + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM_ALIAS;
	itcm_end = BASE_ADDR_ITCM_ALIAS + ITCM_SIZE;
#else
	dtcm_start = BASE_ADDR_DTCM;
	dtcm_end = BASE_ADDR_DTCM + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM;
	itcm_end = BASE_ADDR_ITCM + ITCM_SIZE;
#endif
	if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
	{
		if(((uint32_t)addr >= dtcm_start) && ((uint32_t)addr <= dtcm_end))
		{
			return;
		}
		if(((uint32_t)addr >= itcm_start) && ((uint32_t)addr <= itcm_end))
		{
			return;
		}
		SCB_InvalidateDCache_by_Addr(addr, dsize);
	}
}

extern void hx_InvalidateICache_by_Addr(volatile void *addr, int32_t isize)
{
	uint32_t itcm_start;
	uint32_t itcm_end;
	uint32_t dtcm_start;
	uint32_t dtcm_end;
#ifdef TRUSTZONE_SEC
	dtcm_start = BASE_ADDR_DTCM_ALIAS;
	dtcm_end = BASE_ADDR_DTCM_ALIAS + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM_ALIAS;
	itcm_end = BASE_ADDR_ITCM_ALIAS + ITCM_SIZE;
#else
	dtcm_start = BASE_ADDR_DTCM;
	dtcm_end = BASE_ADDR_DTCM + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM;
	itcm_end = BASE_ADDR_ITCM + ITCM_SIZE;
#endif
	if ((SCB->CCR & SCB_CCR_IC_Msk) != 0U)
	{
		if(((uint32_t)addr >= itcm_start) && ((uint32_t)addr <= itcm_end))
		{
			return;
		}
		if(((uint32_t)addr >= dtcm_start) && ((uint32_t)addr <= dtcm_end))
		{
			return;
		}
		SCB_InvalidateICache_by_Addr(addr, isize);
	}
}

/**
 \brief  D-Cache Clean Invalidate by address

 D-Cache Invalidate by address
 */
extern void hx_CleanInvalidateDCache_by_Addr(volatile void *addr, int32_t dsize)
{
	uint32_t mod_val;
	uint32_t dtcm_start;
	uint32_t dtcm_end;
	uint32_t itcm_start;
	uint32_t itcm_end;
#ifdef TRUSTZONE_SEC
	dtcm_start = BASE_ADDR_DTCM_ALIAS;
	dtcm_end = BASE_ADDR_DTCM_ALIAS + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM_ALIAS;
	itcm_end = BASE_ADDR_ITCM_ALIAS + ITCM_SIZE;
#else
	dtcm_start = BASE_ADDR_DTCM;
	dtcm_end = BASE_ADDR_DTCM + DTCM_SIZE;
	itcm_start = BASE_ADDR_ITCM;
	itcm_end = BASE_ADDR_ITCM + ITCM_SIZE;
#endif
	if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
	{
		if(((uint32_t)addr >= dtcm_start) && ((uint32_t)addr <= dtcm_end))
		{
			return;
		}
		if(((uint32_t)addr >= itcm_start) && ((uint32_t)addr <= itcm_end))
		{
			return;
		}
		SCB_CleanInvalidateDCache_by_Addr(addr, dsize);
	}
}
