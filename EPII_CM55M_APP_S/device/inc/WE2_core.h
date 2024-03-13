/*
 * mpu.h
 *
 *  Created on: 2021�~3��30��
 *      Author: 902447
 */

#ifndef DEVICE_INC_EPII_CORE_H_
#define DEVICE_INC_EPII_CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"


#ifdef __cplusplus
extern "C" {
#endif

#define  INT_PRIGROUP_0     0   /*[7:1] [0] */
#define  INT_PRIGROUP_1     1   /*[7:2] [1:0] */
#define  INT_PRIGROUP_2     2   /*[7:3] [2:0] */
#define  INT_PRIGROUP_3     3   /*[7:4] [3:0] */
#define  INT_PRIGROUP_4     4   /*[7:5] [4:0] */
#define  INT_PRIGROUP_5     5   /*[7:6] [5:0] */
#define  INT_PRIGROUP_6     6   /*[7] [6:0] */
#define  INT_PRIGROUP_7     7   /*None [7:0] */

#define BASE_PRI_POS	5

typedef enum SPIXIP_MEM_ATT_S {
	SPIXIP_MEM_XIP, SPIXIP_MEM_SPI,
} SPIXIP_MEM_ATT_E;

typedef enum TCM_SZ_S {
	TCM_SZ_NO_TCM = 0x00,
	TCM_SZ_4KB = 0x03,
	TCM_SZ_8KB = 0x04,
	TCM_SZ_16KB = 0x05,
	TCM_SZ_32KB = 0x06,
	TCM_SZ_64KB = 0x07,
	TCM_SZ_128KB = 0x08,
	TCM_SZ_256KB = 0x09,
	TCM_SZ_512KB = 0x0A,
	TCM_SZ_1MB = 0x0B,
	TCM_SZ_2MB = 0x0C,
	TCM_SZ_4MB = 0x0D,
	TCM_SZ_8MB = 0x0E,
	TCM_SZ_16MB = 0x0F,
} TCM_SZ_E;

typedef enum TGU_PRESENT_S {
	TGU_PRESENT_NO = 0x00, TGU_PRESENT_Yes,
} TGU_PRESENT_E;

typedef enum TGU_LUT_S {
	TGU_LUT_SECURE = 0x00, TGU_LUT_NS,
} TGU_LUT_E;

typedef enum CPU_MODE_S {
	CPU_MODE_PRIV, CPU_MODE_USER,
} CPU_MODE_E;

typedef enum STACK_MODE_S {
	STACK_MODE_MSP, STACK_MODE_PSP,
} STACK_MODE_E;

typedef enum EPII_CORE_RET_S {
	EPII_CORE_RET_SUCCESS,
	EPII_CORE_RET_FAIL,
} EPII_CORE_RET_E;

/**
 * @brief Constants required to manipulate the MPU.
 */
#define MPU_RLAR_ATTR_INDEX0			( 0UL << 1UL )
#define MPU_RLAR_ATTR_INDEX1			( 1UL << 1UL )
#define MPU_RLAR_ATTR_INDEX2			( 2UL << 1UL )
#define MPU_RLAR_ATTR_INDEX3			( 3UL << 1UL )
#define MPU_RLAR_ATTR_INDEX4			( 4UL << 1UL )
#define MPU_RLAR_ATTR_INDEX5			( 5UL << 1UL )
#define MPU_RLAR_ATTR_INDEX6			( 6UL << 1UL )
#define MPU_RLAR_ATTR_INDEX7			( 7UL << 1UL )

#define MPU_RLAR_REGION_ENABLE			( 1UL )

/* Enable privileged access to unmapped region. */
#define MPU_PRIV_BACKGROUND_ENABLE		( 1UL << 2UL )

/* Enable MPU. */
#define MPU_ENABLE						( 1UL << 0UL )

#define ITCMCR_REG					( * ( ( volatile uint32_t * ) 0xE001E010 ) )
#define ITGU_CTRL_REG					( * ( ( volatile uint32_t * ) 0xE001E500 ) )
#define ITGU_CFG_REG					( * ( ( volatile uint32_t * ) 0xE001E504 ) )

#define DTCMCR_REG					( * ( ( volatile uint32_t * ) 0xE001E014 ) )
#define DTGU_CTRL_REG					( * ( ( volatile uint32_t * ) 0xE001E600 ) )
#define DTGU_CFG_REG					( * ( ( volatile uint32_t * ) 0xE001E604 ) )

#define ITGU_LUT0_CFG_REG					( * ( ( volatile uint32_t * ) 0xE001E510 ) )
#define DTGU_LUT0_CFG_REG					( * ( ( volatile uint32_t * ) 0xE001E610 ) )


#define ITGU_BLK_SIZE	0x8000   //32KB
#define DTGU_BLK_SIZE	0x8000   //32KB
#define SRAM0_BLK_SIZE	0x10000   //64KB
#define SRAM1_BLK_SIZE	0x10000	//64KB
#define SRAM2_BLK_SIZE	0x8000	//32KB
#define FLASH1_BLK_SIZE	0x8000	//32KB
#define FLASH2_BLK_SIZE	0x20000	//128KB

/**
 \brief  Set QSPI/XIP to device/normal memory in Secure

 Set QSPI/XIP to device/normal memory in Secure.
 */
extern void EPII_QSPIXIP_MEM_Attribute_S(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att);

/**
 \brief  Set OSPI/XIP to device/normal memory in Secure

 Set OSPI/XIP to device/normal memory in Secure.
 */
extern void EPII_OSPIXIP_MEM_Attribute_S(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att);


/**
 \brief  Set QSPI/XIP to device/normal memory in NS

 Set QSPI/XIP to device/normal memory in NS.
 */
extern void EPII_QSPIXIP_MEM_Attribute_NS(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att);

/**
 \brief  Set OSPI/XIP to device/normal memory in NS

 Set OSPI/XIP to device/normal memory in NS.
 */
extern void EPII_OSPIXIP_MEM_Attribute_NS(uint32_t start_addr, uint32_t end_addr,
		SPIXIP_MEM_ATT_E mem_att);

/**
 \brief  Set MPU enable

 */
extern void EPII_MPU_Enable(uint32_t enable, uint32_t HFNMIENA_enable, uint32_t PRIVDEFENA_enable);

/**
 \brief  Set MPU enable NS

 */
extern void EPII_MPU_Enable_NS(uint32_t enable, uint32_t HFNMIENA_enable, uint32_t PRIVDEFENA_enable);


/**
 \brief  Get MPU Region in Secure

 Get MPU Region in Secure.
 */
extern void EPII_Get_MPU_Region_S(uint8_t *mpu_region);

/**
 \brief  Get MPU Region in NS

 Get MPU Region in Secure.
 */
extern void EPII_Get_MPU_Region_NS(uint8_t *mpu_region);
#ifndef BOOT_USED
/**
 \brief  Get ITCM Size

 Get ITCM Size.
 */
extern void EPII_Get_ITCM_Size(TCM_SZ_E *size);

/**
 \brief  Set ITCM Enable

 Set ITCM enable.
 */
extern void EPII_Set_ITCM_En(uint8_t enable);

/**
 \brief  Get ITCM Enable

 Get ITCM enable.
 */
extern void EPII_Get_ITCM_En(uint8_t *enable);
#endif
/**
 \brief  Set ITGU Error enable

 Set ITGU Error enable.
 */
extern void EPII_Set_ITGU_Err_En(uint8_t DBFEN, uint8_t DEREN);

/**
 \brief  Get ITGU Error enable

 Get ITGU Error enable.
 */
extern void EPII_Get_ITGU_Err_En(uint8_t *DBFEN, uint8_t *DEREN);

/**
 \brief  Get ITGU CFG

 Get ITGU Configuration
 */
extern void EPII_Get_ITGU_CFG(TGU_PRESENT_E *present, uint8_t *NUMBLKS,
		uint8_t *BLKSZ);

/**
 \brief  Set ITGU LUT configuration

 Set ITGU LUT configuration
 */
extern void EPII_Set_ITGU_LUT(uint8_t block_no, TGU_LUT_E lut);
/**
 \brief  Get ITGU LUT configuration

 Get ITGU LUT configuration
 */
extern void EPII_Get_ITGU_LUT(uint8_t block_no, TGU_LUT_E *lut);
#ifndef BOOT_USED
/**
 \brief  Get DTCM Size

 Get DTCM Size.
 */
extern void EPII_Get_DTCM_Size(TCM_SZ_E *size);

/**
 \brief  Set DTCM Enable

 Set DTCM enable.
 */
extern void EPII_Set_DTCM_En(uint8_t enable);

/**
 \brief  Get DTCM Enable

 Get DTCM enable.
 */
extern void EPII_Get_DTCM_En(uint8_t *enable);
#endif
/**
 \brief  Set DTGU Error enable

 Set DTGU Error enable.
 */
extern void EPII_Set_DTGU_Err_En(uint8_t DBFEN, uint8_t DEREN);

/**
 \brief  Get DTGU Error enable

 Get DTGU Error enable.
 */
extern void EPII_Get_DTGU_Err_En(uint8_t *DBFEN, uint8_t *DEREN);

/**
 \brief  Get DTGU CFG

 Get DTGU Configuration
 */
extern void EPII_Get_DTGU_CFG(TGU_PRESENT_E *present, uint8_t *NUMBLKS,
		uint8_t *BLKSZ);

/**
 \brief  Set DTGU LUT configuration

 Set DTGU LUT configuration
 */
extern void EPII_Set_DTGU_LUT(uint8_t block_no, TGU_LUT_E lut);
/**
 \brief  Get DTGU LUT configuration

 Get DTGU LUT configuration
 */
extern void EPII_Get_DTGU_LUT(uint8_t block_no, TGU_LUT_E *lut);
#ifndef BOOT_USED
/**
 \brief  Set systick load

 Set systick load
 */
extern void EPII_Set_Systick_load(uint32_t tick);

/**
 \brief  Set systick load

 Get systick load
 */
extern void EPII_Get_Systick_load(uint32_t *tick);

/**
 \brief  Get systick val

 Get systick val
 */
extern void EPII_Get_Systick_val(uint32_t *val);
#endif
/**
 \brief  Set systick load

 Set systick load
 */
extern void EPII_Set_Systick_enable(uint32_t enable);
#ifndef BOOT_USED
/**
 \brief  Set CPU Mode

 Set CPU Mode
 */
extern EPII_CORE_RET_E EPII_Set_CPU_Mode(CPU_MODE_E mode);

/**
 \brief  Get CPU Mode

 Get CPU Mode
 */
extern void EPII_Get_CPU_Mode(CPU_MODE_E *mode);


/**
 \brief  Set Stack Sel

 Set CPU Mode
 */
extern EPII_CORE_RET_E EPII_Set_Stack_Sel(STACK_MODE_E mode);

/**
 \brief  Get Stack Sel

 Get CPU Mode
 */
extern void EPII_Get_Stack_Sel(STACK_MODE_E *mode);
#endif
/**
 \brief  NVIC Set Vector

 Set NVIC Vector
 */
extern void EPII_NVIC_SetVector(IRQn_Type IRQn, uint32_t vector);

/**
 \brief  Get system clock

system clock
 */
extern void EPII_Get_Systemclock(uint32_t *val);

/**
 \brief  Set Memory value

 */
extern void hx_set_memory(unsigned int addr, unsigned int val);

/**
 \brief  Get Memory value

 */
extern unsigned int hx_get_memory(unsigned int addr);


extern void EPII_cpu_nop_us(uint32_t period_us);


/**
 \brief  Enable/Disable I Cache

 Enable/Disable I Cache
 */
extern void hx_enable_ICache(uint32_t enable);

/**
 \brief  Enable/Disable D Cache

 Enable/Disable D Cache
 */
extern void hx_enable_DCache(uint32_t enable);

/**
 \brief  D-Cache clean by address

 D-Cache clean by address
 */
extern void hx_CleanDCache_by_Addr(volatile void *addr, int32_t dsize);


/**
 \brief  D-Cache Invalidate by address

 D-Cache Invalidate by address
 */
extern void hx_InvalidateDCache_by_Addr(volatile void *addr, int32_t dsize);

/**
 \brief  I-Cache Invalidate by address

 I-Cache Invalidate by address
 */
extern void hx_InvalidateICache_by_Addr(volatile void *addr, int32_t dsize);

/**
 \brief  D-Cache Clean Invalidate by address

 D-Cache Invalidate by address
 */
extern void hx_CleanInvalidateDCache_by_Addr(volatile void *addr, int32_t dsize);

#define EPII_set_memory hx_set_memory
#define EPII_get_memory hx_get_memory


#ifdef __cplusplus
}
#endif

#endif /* DEVICE_INC_EPII_CORE_H_ */
