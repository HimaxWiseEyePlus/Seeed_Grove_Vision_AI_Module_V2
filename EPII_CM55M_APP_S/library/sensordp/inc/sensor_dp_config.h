/**
 ********************************************************************************************
 *  @file      sensor_dp_config.h
 *  @details   This file contains all sensor datapath configuration
 *  @author    himax/902447
 *  @version   V1.0.0
 *  @date      14-May-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/


#ifndef SENSOR_DP_CONFIG_H_
#define SENSOR_DP_CONFIG_H_

#include "WE2_device.h"

#ifdef TRUSTZONE_SEC
#define EXT_RAM_START   BASE_ADDR_SRAM0_ALIAS
#else
#ifndef TRUSTZONE
#define EXT_RAM_START   BASE_ADDR_SRAM0_ALIAS
#else
#define EXT_RAM_START   BASE_ADDR_SRAM0
#endif
#endif

/**
 * \defgroup	SENSOR_DP_LIB_MACRO	Sensor Datapath Macro
 * \ingroup	SENSOR_DP_LIB
 * \brief	Defines some macros of Sensor Datapath Library need.
 * @{
 */
#define WMDA2_YUV420_TARGET_LOOP_CNT         6	/*!< JPEG YUV420 default buffer count*/
#define WMDA2_YUV422_TARGET_LOOP_CNT         3  /*!< JPEG YUV422 default buffer count*/
#define WMDA2_YUV400_TARGET_LOOP_CNT         10	/*!< JPEG YUV400 default buffer count*/

/*
 * \def WE1_DP_WDMA1_OUT_SRAM_ADDR  WDMA1 base memory address
 * \def WE1_DP_WDMA2_OUT_SRAM_ADDR  WDMA2 base memory address
 * \def WE1_DP_WDMA3_OUT_SRAM_ADDR  WDMA3 base memory address
*/
#if defined(CLI_ENABLE) || defined(PMU_LIB_TEST)
#define WE1_DP_WDMA1_OUT_SRAM_ADDR   	(EXT_RAM_START+0x00080000)//(EXT_RAM_START + 0)/*!< WDMA1 default base memory address*/
//#define WE1_DP_WDMA1_OUT_SRAM_ADDR      (EXT_RAM_START+0x00000000)//(EXT_RAM_START + 0)/*!< WDMA1 default base memory address*/
#define WE1_DP_WDMA2_OUT_SRAM_ADDR    	(EXT_RAM_START+0x000DF400)//(EXT_RAM_START + 0xA0800)/*!< WDMA2 default base memory address*/
#define WE1_DP_WDMA3_OUT_SRAM_ADDR      (EXT_RAM_START+0x0012A400)
//#ifdef NSC
//#define WE1_DP_WDMA3_OUT_SRAM_ADDR    	(EXT_RAM_START+0x00180000)//(EXT_RAM_START + 0xEB800)/*!< WDMA3 default base memory address*/
//#else
//#define WE1_DP_WDMA3_OUT_SRAM_ADDR    	(EXT_RAM_START+0x0012A400)//(EXT_RAM_START + 0xEB800)/*!< WDMA3 default base memory address*/
//#endif
#elif defined(HWACCBYTPG_AUTO_TEST)
#define WE1_DP_WDMA1_OUT_SRAM_ADDR   	(EXT_RAM_START+0x00000000)//(EXT_RAM_START + 0)/*!< WDMA1 default base memory address*/
#define WE1_DP_WDMA2_OUT_SRAM_ADDR    	(EXT_RAM_START+0x000A0800)//(EXT_RAM_START + 0xA0800)/*!< WDMA2 default base memory address*/
#define WE1_DP_WDMA3_OUT_SRAM_ADDR    	(EXT_RAM_START+0x000EB800)//(EXT_RAM_START + 0xEB800)/*!< WDMA3 default base memory address*/
#else
#define WE1_DP_WDMA1_OUT_SRAM_ADDR   	(EXT_RAM_START + 0x00080000)/*!< WDMA1 default base memory address*/ //300KB////(WE1_DP_BASE_ADDR + 0x34000)//
#define WE1_DP_WDMA2_OUT_SRAM_ADDR    	(EXT_RAM_START + 0x000DF400)/*!< WDMA2 default base memory address*///300KB//(WE1_DP_BASE_ADDR + 0xA4800)
#define WE1_DP_WDMA3_OUT_SRAM_ADDR      (EXT_RAM_START + 0x12A400)
//#ifdef NSC
//#define WE1_DP_WDMA3_OUT_SRAM_ADDR    	(EXT_RAM_START+0x00180000)//(EXT_RAM_START + 0xEB800)/*!< WDMA3 default base memory address*/
//#else
//#define WE1_DP_WDMA3_OUT_SRAM_ADDR    	(EXT_RAM_START+0x12A400)//(EXT_RAM_START + 0xEB800)/*!< WDMA3 default base memory address*/
//#endif
#endif
//#define WE1_DP_YUV422_WDMA3_OUT_SRAM_ADDR    	(EXT_RAM_START + 0x0)/*!< WDMA3 YUV422 format base memory address*/
#define WE1_DP_YUV422_WDMA3_OUT_SRAM_ADDR       (WE1_DP_WDMA3_OUT_SRAM_ADDR)/*!< WDMA3 YUV422 format base memory address*/

#ifdef NSC
#if defined(CLI_ENABLE) || defined(PMU_LIB_TEST)
#define WE1_DP_JPEG_HWAUTOFILL_SRAM_ADDR    (EXT_RAM_START + 0x175400)  /*!< JPEG HW auto fill memory start address*/
#else
#define WE1_DP_JPEG_HWAUTOFILL_SRAM_ADDR    (EXT_RAM_START + 0x175400)  /*!< JPEG HW auto fill memory start address*/
#endif
#else
#define WE1_DP_JPEG_HWAUTOFILL_SRAM_ADDR    (EXT_RAM_START + 0x175400)  /*!< JPEG HW auto fill memory start address*/
#endif


#define SUPPORT_EDM_PULSE_TIMING_DETECT	/*!< EDM after INP Timing detect compiler flag(enable/disable) debug purpose*/

#define SUPPORT_EDM_CONV_TIMING_DETECT	/*!< EDM Conventional Timing detect compiler flag(enable/disable) debug purpose*/
//#define SUPPORT_EDM_PULSE_TIMING_DETECT_ISR
//#define SUPPORT_EDM_CONV_TIMING_DETECT_ISR

#define SUPPORT_EDM_WDT_DETECT  /*!< EDM WDT compiler flag for Datapath abnormal to avoid system hang up */
#ifdef SUPPORT_EDM_WDT_DETECT
#define WDT_TIMEOUT_PERIOD    5000 /*!< EDM WDT fire period */
#endif
/** @} */
#endif /* SENSOR_DP_CONFIG_H_ */
