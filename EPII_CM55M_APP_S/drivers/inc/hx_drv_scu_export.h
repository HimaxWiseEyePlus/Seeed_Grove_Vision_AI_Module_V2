/*
 * hx_drv_scu.h
 *
 *  Created on: 2021/4/21
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SCU_EXPORT_H_
#define DRIVERS_INC_HX_DRV_SCU_EXPORT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#ifdef IP_timer
#include "hx_drv_timer.h"
#endif
#ifndef BOOT_USED
#include "hx_drv_rtc.h"
#include "hx_drv_watchdog.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EPII_FPGA
#define CHIPC_VERSION	0x20230428
#define CHIPD_VERSION	0x20230913
#else
#define CHIPC_VERSION	0x8538000C
#define CHIPD_VERSION	0x8538000D
#endif

#define SUPPORT_CKGEN		1
#define SUPPORT_SWRESET		1

#define FREQ_1K  	1000
#define FREQ_32K  	32000
#define XTAL_FREQ_32K  	32768
#define FREQ_1M   	1000000
#define FREQ_24M    24000000
#define FREQ_48M    48000000
#define FREQ_96M    96000000
#define FREQ_100M    100000000
#define FREQ_200M    200000000
#define FREQ_300M    300000000
#define FREQ_400M    400000000
#define FREQ_450M    450000000
#define FREQ_150M    150000000

/**
 * \defgroup	SCU_DRV	SCU Driver
 * \ingroup		SCU_DRV
 * \brief	SCU Driver Declaration
 * \details Application can use SCU Structure to control Hardware\n
 * <pre>
 * For Example
 * Ex1: SCU_CFG_T: All SCU configuration for application to control SCU
 * </pre>
 */
/**
 * \defgroup	SCU_DRV_ENUM	SCU Driver Enumeration
 * \ingroup	SCU_DRV
 * \brief	Defines some enumeration of SCU Driver need.
 * @{
 */
/**
 * \enum SCU_ERROR_E
 * \brief SCU Errors Type
 */
typedef enum {
	SCU_NO_ERROR = 0, /**< No ERROR */
	SCU_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	SCU_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
	SCU_ILLEGAL_MEMATT, /**< ERROR MSG: illegal Memory Attribute*/
} SCU_ERROR_E;

/**
 * \enum SCU_CM55S_STATE_E
 * \brief SCU CM55S State
 */
typedef enum {
	SCU_CM55S_STATE_RESET = 0, /**< CM55S State Reset */
	SCU_CM55S_STATE_NORMAL, /**< CM55S State Normal */
} SCU_CM55S_STATE_E;

/**
 * \enum SCU_U55_STATE_E
 * \brief SCU U55 State
 */
typedef enum {
	SCU_U55_STATE_RESET = 0, /**< U55 State Reset */
	SCU_U55_STATE_NORMAL, /**< U55 State Normal */
} SCU_U55_STATE_E;



/**
 * \enum SCU_RTC_STATE_E
 * \brief SCU RTC State
 */
typedef enum {
	SCU_RTC_STATE_RESET = 0, /**< RTC State Reset */
	SCU_RTC_STATE_NORMAL, /**< RTC State Normal */
} SCU_RTC_STATE_E;

/**
 * \enum SCU_I2S_MODE_E
 * \brief SCU I2S Mode
 */
typedef enum {
	SCU_I2S_MODE_MASTER = 0, /**< I2S Mode Master */
	SCU_I2S_MODE_SLAVE = 1, /**< I2S Mode Slave */
} SCU_I2S_MODE_E;

/**
 * \enum SCU_PDM_IP_E
 * \brief SCU PDM IP Selection
 */
typedef enum {
	SCU_PDM_IP_SNPS = 0, /**< PDM SNPS */
	SCU_PDM_IP_HIMAX = 1, /**< PDM Himax */
} SCU_PDM_IP_E;

/**
 * \enum SCU_CM55_CPUWAIT_E
 * \brief SCU Asserting this signal stops the processor fetching or executing instructions after reset. When the CPUWAIT signal is deasserted the processor starts fetching instructions from the reset vector address in the normal way.
 */
typedef enum {
	SCU_CM55_CPU_RUN = 0, /**< CM55 start to fetch instruction*/
	SCU_CM55_CPU_WAIT, /**< CM55 stop to fetch instruction */
} SCU_CM55_CPUWAIT_E;

/**
 * \enum SCU_U55_PORPL_E
 * \brief SCU U55 The Power-On-Reset Privilege Level (PORPL).
 */
typedef enum {
	SCU_U55_PORPL_USER = 0, /**< U55 User Level */
	SCU_U55_PORPL_PRIVILEGED, /**< U55 Privileged level */
} SCU_U55_PORPL_E;

/**
 * \enum SCU_BOOT_OPT_E
 * \brief SCU Boot Option.
 */
typedef enum {
	SCU_BOOT_QSPI= 0, /**<  BOOT QSPI*/
	SCU_BOOT_I2C_S, /**< BOOT I2C_S 1*/
	SCU_BOOT_UART, /**< BOOT UART 2*/
	SCU_BOOT_UNDEF, /**< BOOT UNDEF 3*/
	SCU_BOOT_SPI_S, /**< BOOT SPI_S 4*/
	SCU_BOOT_UNKNOWN, /**< BOOT UNKNOWN */
} SCU_BOOT_OPT_E;

/**
 * \enum SCU_DP_WAKEUPCTRL_SEL_E
 * \brief SCU DP Wakeup control Selection.
 */
typedef enum {
	SCU_DP_WAKEUPCTRL_SEL_CPU= 0, /**<  DP Ctrl by CPU*/
	SCU_DP_WAKEUPCTRL_SEL_PMU, /**< DP Ctrl by PMU */
} SCU_DP_WAKEUPCTRL_SEL_E;

/**
 * \enum SCU_DP_WAKEUPCTRL_SEL_E
 * \brief SCU DP Wakeup control Selection.
 */
typedef enum {
	SCU_MCLKCTRL_SEL_CPU= 0, /**<  MCLK Ctrl by CPU*/
	SCU_MCLKCTRL_SEL_PMU, /**< MCLK Ctrl by PMU */
} SCU_MCLKCTRL_SEL_E;

/**
 * \enum SCU_WDTCTRL_E
 * \brief SCU WDT CTRL
 */
typedef enum {
	SCU_WDTCTRL_CPU = 0, /**< CPU control */
	SCU_WDTCTRL_PMU = 1, /**PMU control*/
} SCU_WDTCTRL_E;

/**
 * \enum SCU_TIMERCTRL_E
 * \brief SCU TIMER CTRL
 */
typedef enum {
	SCU_TIMERCTRL_CPU = 0, /**< CPU control */
	SCU_TIMERCTRL_PMU = 1, /**PMU control*/
} SCU_TIMERCTRL_E;

#ifdef SUPPORT_CKGEN
/**
 * \enum SCU_SYSCLKSRC_HW_E
 * \brief SCU SYSCLK SRC HW Selection.
 */
typedef enum {
	SCU_SYSCLKSRC_HW_RC24MRC32K, /**< CLK Source HW RC(RC24M and RC32K) */
	SCU_SYSCLKSRC_HW_RC24MRC48MRC32K, /**< CLK Source HW RC24M RC48M and RC32K)  */
	SCU_SYSCLKSRC_HW_RC24MRC96MRC32K, /**< CLK Source HW RC24M RC96M and RC32K)  */
	SCU_SYSCLKSRC_HW_XTAL24MXTAL32K, /**< CLK Source HW ALL XTAL */
	SCU_SYSCLKSRC_HW_XTAL24MRC48MXTAL32K, /**< CLK Source HW ALL XTAL RC48M*/
	SCU_SYSCLKSRC_HW_XTAL24MRC96MXTAL32K, /**< CLK Source HW ALL XTAL RC96M*/
	SCU_SYSCLKSRC_HW_XTAL24MRC32K, /**< CLK Source XTAL24M and RC32K */
	SCU_SYSCLKSRC_HW_XTAL24MRC48MRC32K, /**< CLK Source XTAL24M, RC48M and RC32K */
	SCU_SYSCLKSRC_HW_XTAL24MRC96MRC32K, /**< CLK Source XTAL24M, RC96M and RC32K */
	SCU_SYSCLKSRC_HW_RC24M_XTAL32K, /**< CLK Source HW RC(RC24M and XTAL32K) */
	SCU_SYSCLKSRC_HW_RC24MRC48M_XTAL32K, /**< CLK Source HW RC(RC48M and XTAL32K)  */
	SCU_SYSCLKSRC_HW_RC24MRC96M_XTAL32K, /**< CLK Source HW RC(RC96M and XTAL32K)  */
} SCU_SYSCLKSRC_HW_E;


/**
 * \enum SCU_XTAL24MSEL_E
 * \brief SCU XTAL24M Selection.
 */
typedef enum {
	SCU_XTAL24MSEL_1M_4M, /**< 1MHz ~ 4MHz */
	SCU_XTAL24MSEL_12P1M_24M, /**< 12.1MHz ~ 24MHZ */
	SCU_XTAL24MSEL_4P1M_12M, /**< 4.1MHz ~ 12MHZ */
	SCU_XTAL24MSEL_24P1M_48M, /**< 24.1MHz ~ 48MHZ */
} SCU_XTAL24MSEL_E;

/**
 * \enum SCU_RC24M1MSEL_E
 * \brief SCU RC24M1M Selection.
 */
typedef enum {
	SCU_RC24M1MSEL_RC24M, /**< CLK Source RC24M */
	SCU_RC24M1MSEL_RC1M_SLOW001, /**< CLK Source RC1M */
	SCU_RC24M1MSEL_RC1M_SLOW010, /**< CLK Source RC1M */
	SCU_RC24M1MSEL_RC1M_SLOW011, /**< CLK Source RC1M */
	SCU_RC24M1MSEL_RC1M_DEF100, /**< CLK Source RC1M */
	SCU_RC24M1MSEL_RC1M_FAST101, /**< CLK Source RC1M */
	SCU_RC24M1MSEL_RC1M_FAST110, /**< CLK Source RC1M */
	SCU_RC24M1MSEL_RC1M_FAST111, /**< CLK Source RC1M */
} SCU_RC24M1MSEL_E;

/**
 * \enum SCU_RC96M48MSEL_E
 * \brief SCU RC96M48M Selection.
 */
typedef enum {
	SCU_RC96M48MSEL_RC48M, /**< CLK Source RC48M */
	SCU_RC96M48MSEL_RC96M, /**< CLK Source RC96M */
} SCU_RC96M48MSEL_E;

/**
 * \enum SCU_RC32K1KSEL_E
 * \brief SCU RC32K1K Selection.
 */
typedef enum {
	SCU_RC32K1KSEL_RC1K, /**< CLK Source RC32K */
	SCU_RC32K1KSEL_RC32K, /**< CLK Source RC1K */
} SCU_RC32K1KSEL_E;

/**
 * \enum SCU_CLKSRC_E
 * \brief SCU CLK Source.
 */
typedef enum {
	SCU_CLKSRC_RC24M, /**< CLK Source RC24M */
	SCU_CLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_CLKSRC_RC1M, /**< CLK Source RC1M */
	SCU_CLKSRC_RC48M, /**< CLK Source RC48M */
	SCU_CLKSRC_RC96M, /**< CLK Source RC96M */
	SCU_CLKSRC_XTAL32K, /**<  CLK Source XTAL32K*/
	SCU_CLKSRC_RC1K, /**< CLK Source RC1K */
	SCU_CLKSRC_RC32K, /**< CLK Source RC32K */
	SCU_CLKSRC_PLL /**< CLK Source PLL */
} SCU_CLKSRC_E;

/**
 * \enum SCU_PLLCLKSRC_E
 * \brief SCU PLL CLK Source.
 */
typedef enum {
	SCU_PLLCLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_PLLCLKSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_PLLCLKSRC_XTAL24M /**<  CLK Source XTAL24M*/
} SCU_PLLCLKSRC_E;

/**
 * \enum SCU_PLL_FREQ_E
 * \brief SCU PLL Disable or Enable.
 */
typedef enum {
	SCU_PLL_FREQ_DISABLE, /**< CLK Source PLL Disable */
	SCU_PLL_FREQ_ENABLE, /**< CLK Source PLL Enable */
} SCU_PLL_FREQ_E;


/**
 * \enum SCU_MCLK_FREQ_E
 * \brief SCU MCLK FREQ.
 */
typedef enum {
	SCU_MCLK_FREQ_12M, /**< MCLK 12M */
	SCU_MCLK_FREQ_24M, /**< MCLK 24M */
	SCU_MCLK_FREQ_32M, /**< MCLK 32M */
	SCU_MCLK_FREQ_48M, /**< MCLK 48M */
	SCU_MCLK_FREQ_96M, /**< MCLK 96M */
} SCU_MCLK_FREQ_E;

/**
 * \enum SCU_HSCCLKSRC_E
 * \brief SCU HSC CLK Source.
 */
typedef enum {
	SCU_HSCCLKSRC_RC24M1M, /**< CLK Source RC24M1M 3'b0x0*/
	SCU_HSCCLKSRC_XTAL24M, /**<  CLK Source XTAL24M 3'b0x1*/
	SCU_HSCCLKSRC_RC96M48M, /**< CLK Source RC96M48M 3'b10x*/
	SCU_HSCCLKSRC_PLL /**< CLK Source PLL 3'b11x : PLL_CLK*/
} SCU_HSCCLKSRC_E;
/**
 * \enum SCU_HSCCLKDIV_E
 * \brief SCU HSC CLK DIV.
 */
typedef enum {
	SCU_HSCCLKDIV_1, /**< CLK Source DIV1 */
	SCU_HSCCLKDIV_2, /**< CLK Source DIV2 */
	SCU_HSCCLKDIV_3, /**< CLK Source DIV3 */
	SCU_HSCCLKDIV_4, /**< CLK Source DIV4 */
	SCU_HSCCLKDIV_5, /**< CLK Source DIV5 */
	SCU_HSCCLKDIV_6, /**< CLK Source DIV6 */
	SCU_HSCCLKDIV_7, /**< CLK Source DIV7 */
	SCU_HSCCLKDIV_8, /**< CLK Source DIV8 */
	SCU_HSCCLKDIV_9, /**< CLK Source DIV9 */
	SCU_HSCCLKDIV_10, /**< CLK Source DIV10 */
	SCU_HSCCLKDIV_11, /**< CLK Source DIV11 */
	SCU_HSCCLKDIV_12, /**< CLK Source DIV12 */
	SCU_HSCCLKDIV_13, /**< CLK Source DIV13 */
	SCU_HSCCLKDIV_14, /**< CLK Source DIV14 */
	SCU_HSCCLKDIV_15, /**< CLK Source DIV15 */
	SCU_HSCCLKDIV_16, /**< CLK Source DIV16 */
} SCU_HSCCLKDIV_E;

/**
 * \enum SCU_HSCD12CLKSRC_E
 * \brief SCU HSC CLK Source.
 */
typedef enum {
	SCU_HSCD12CLKSRC_HSC, /**< CLK Source HSC */
	SCU_HSCD12CLKSRC_HSCDIV2, /**< CLK Source HSC DIV2 */
} SCU_HSCD12CLKSRC_E;

/**
 * \enum SCU_QSPICLKSRC_E
 * \brief SCU QSPI CLK Source.
 */
typedef enum {
	SCU_QSPICLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_QSPICLKSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_QSPICLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_QSPICLKSRC_PLL /**< CLK Source PLL */
} SCU_QSPICLKSRC_E;

/**
 * \enum SCU_OSPICLKSRC_E
 * \brief SCU OSPI CLK Source.
 */
typedef enum {
	SCU_OSPICLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_OSPICLKSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_OSPICLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_OSPICLKSRC_PLL /**< CLK Source PLL */
} SCU_OSPICLKSRC_E;

/**
 * \enum SCU_HSC32KCLKSRC_E
 * \brief SCU HSC 32K CLK Source.
 */
typedef enum {
	SCU_HSC32KCLKSRC_RC32K1K, /**< CLK Source RC32K1K */
	SCU_HSC32KCLKSRC_XTAL32K /**<  CLK Source XTAL32K*/
} SCU_HSC32KCLKSRC_E;


/**
 * \enum SCU_HSCI3CHCLKDIV_E
 * \brief SCU HSC I3CH CLK DIV.
 */
typedef enum {
	SCU_HSCI3CHCLKDIV_1, /**< CLK Source DIV1 */
	SCU_HSCI3CHCLKDIV_2, /**< CLK Source DIV2 */
	SCU_HSCI3CHCLKDIV_3, /**< CLK Source DIV3 */
	SCU_HSCI3CHCLKDIV_4, /**< CLK Source DIV4 */
	SCU_HSCI3CHCLKDIV_5, /**< CLK Source DIV5 */
	SCU_HSCI3CHCLKDIV_6, /**< CLK Source DIV6 */
	SCU_HSCI3CHCLKDIV_7, /**< CLK Source DIV7 */
	SCU_HSCI3CHCLKDIV_8, /**< CLK Source DIV8 */
	SCU_HSCI3CHCLKDIV_9, /**< CLK Source DIV9 */
	SCU_HSCI3CHCLKDIV_10, /**< CLK Source DIV10 */
	SCU_HSCI3CHCLKDIV_11, /**< CLK Source DIV11 */
	SCU_HSCI3CHCLKDIV_12, /**< CLK Source DIV12 */
	SCU_HSCI3CHCLKDIV_13, /**< CLK Source DIV13 */
	SCU_HSCI3CHCLKDIV_14, /**< CLK Source DIV14 */
	SCU_HSCI3CHCLKDIV_15, /**< CLK Source DIV15 */
	SCU_HSCI3CHCLKDIV_16, /**< CLK Source DIV16 */
} SCU_HSCI3CHCLKDIV_E;

/**
 * \enum SCU_HSCPUFCLKSRC_E
 * \brief SCU HSC PUF CLK Source.
 */
typedef enum {
	SCU_HSCPUFCLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_HSCPUFCLKSRC_XTAL24M /**<  CLK Source XTAL24M*/
} SCU_HSCPUFCLKSRC_E;

/**
 * \enum SCU_HSCDPCLKSRC_E
 * \brief SCU HSC DP CLK Source.
 */
typedef enum {
	SCU_HSCDPCLKSRC_RC24M1M, /**< CLK Source RC24M */
	SCU_HSCDPCLKSRC_RC96M48M, /**< CLK Source RC96M or 48M */
	SCU_HSCDPCLKSRC_RC96M48M_DIV2, /**< CLK Source RC96M or 48M div2 */
	SCU_HSCDPCLKSRC_XTAL24M /**<  CLK Source XTAL24M*/
} SCU_HSCDPCLKSRC_E;

/**
 * \enum SCU_HSCRXCLKSRC_E
 * \brief SCU HSC RX CLK Source.
 */
typedef enum {
	SCU_HSCRXCLKSRC_DP, /**< CLK Source DP */
	SCU_HSCRXCLKSRC_MIPIRX, /**< CLK Source MIPIRX */
	SCU_HSCRXCLKSRC_SENPARA /**<  CLK Source SENPARA*/
} SCU_HSCRXCLKSRC_E;

/**
 * \enum SCU_HSCMIPICLKSRC_E
 * \brief SCU HSC MIPI CLK Source.
 */
typedef enum {
	SCU_HSCMIPICLKSRC_RC24M, /**< CLK Source RC24M */
	SCU_HSCMIPICLKSRC_RC96M48M, /**< CLK Source RC96M or 48M */
	SCU_HSCMIPICLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
#if(IC_VERSION >= 30)
	SCU_HSCMIPICLKSRC_PLL /**<  CLK Source PLL*/
#endif
} SCU_HSCMIPICLKSRC_E;

/**
 * \enum SCU_HSCPPITXCLKSRC_E
 * \brief SCU HSC PPITX CLK Source.
 */
typedef enum {
	SCU_HSCPPITXCLKSRC_RC24M, /**< CLK Source RC24M1M */
	SCU_HSCPPITXCLKSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_HSCPPITXCLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_HSCPPITXCLKSRC_PLL /**<  CLK Source PLL*/
} SCU_HSCPPITXCLKSRC_E;

/**
 * \enum SCU_HSCSDIOCLKDIV_E
 * \brief SCU HSC SDIO CLK DIV.
 */
typedef enum {
	SCU_HSCSDIOCLKDIV_1, /**< CLK Source DIV1 */
	SCU_HSCSDIOCLKDIV_2, /**< CLK Source DIV2 */
	SCU_HSCSDIOCLKDIV_3, /**< CLK Source DIV3 */
	SCU_HSCSDIOCLKDIV_4, /**< CLK Source DIV4 */
	SCU_HSCSDIOCLKDIV_5, /**< CLK Source DIV5 */
	SCU_HSCSDIOCLKDIV_6, /**< CLK Source DIV6 */
	SCU_HSCSDIOCLKDIV_7, /**< CLK Source DIV7 */
	SCU_HSCSDIOCLKDIV_8, /**< CLK Source DIV8 */
	SCU_HSCSDIOCLKDIV_9, /**< CLK Source DIV9 */
	SCU_HSCSDIOCLKDIV_10, /**< CLK Source DIV10 */
	SCU_HSCSDIOCLKDIV_11, /**< CLK Source DIV11 */
	SCU_HSCSDIOCLKDIV_12, /**< CLK Source DIV12 */
	SCU_HSCSDIOCLKDIV_13, /**< CLK Source DIV13 */
	SCU_HSCSDIOCLKDIV_14, /**< CLK Source DIV14 */
	SCU_HSCSDIOCLKDIV_15, /**< CLK Source DIV15 */
	SCU_HSCSDIOCLKDIV_16, /**< CLK Source DIV16 */
} SCU_HSCSDIOCLKDIV_E;

/**
 * \enum SCU_LSCCLKSRC_E
 * \brief SCU LSC CLK Source.
 */
typedef enum {
	SCU_LSCCLKSRC_RC24M1M, /**< CLK Source RC24M1M 3'b0x0*/
	SCU_LSCCLKSRC_XTAL24M, /**<  CLK Source XTAL24M 3'b0x1*/
	SCU_LSCCLKSRC_RC96M48M, /**< CLK Source RC96M48M 3'b10x*/
	SCU_LSCCLKSRC_PLL /**< CLK Source PLL 3'b11x*/
} SCU_LSCCLKSRC_E;

/**
 * \enum SCU_LSCCLKDIV_E
 * \brief SCU LSC CLK DIV.
 */
typedef enum {
	SCU_LSCCLKDIV_1, /**< CLK Source DIV1 */
	SCU_LSCCLKDIV_2, /**< CLK Source DIV2 */
	SCU_LSCCLKDIV_3, /**< CLK Source DIV3 */
	SCU_LSCCLKDIV_4, /**< CLK Source DIV4 */
	SCU_LSCCLKDIV_5, /**< CLK Source DIV5 */
	SCU_LSCCLKDIV_6, /**< CLK Source DIV6 */
	SCU_LSCCLKDIV_7, /**< CLK Source DIV7 */
	SCU_LSCCLKDIV_8, /**< CLK Source DIV8 */
	SCU_LSCCLKDIV_9, /**< CLK Source DIV9 */
	SCU_LSCCLKDIV_10, /**< CLK Source DIV10 */
	SCU_LSCCLKDIV_11, /**< CLK Source DIV11 */
	SCU_LSCCLKDIV_12, /**< CLK Source DIV12 */
	SCU_LSCCLKDIV_13, /**< CLK Source DIV13 */
	SCU_LSCCLKDIV_14, /**< CLK Source DIV14 */
	SCU_LSCCLKDIV_15, /**< CLK Source DIV15 */
	SCU_LSCCLKDIV_16, /**< CLK Source DIV16 */
} SCU_LSCCLKDIV_E;

/**
 * \enum SCU_LSC32KCLKSRC_E
 * \brief SCU LSC 32K CLK Source.
 */
typedef enum {
	SCU_LSC32KCLKSRC_RC32K1K, /**< CLK Source RC32K1K */
	SCU_LSC32KCLKSRC_XTAL32K /**<  CLK Source XTAL32K*/
} SCU_LSC32KCLKSRC_E;

/**
 * \enum SCU_LSCREFCLKSRC_E
 * \brief SCU LSC Ref CLK Source.
 */
typedef enum {
	SCU_LSCREFCLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_LSCREFCLKSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_LSCREFCLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_LSCREFCLKSRC_PLL /**< CLK Source PLL */
} SCU_LSCREFCLKSRC_E;

/**
 * \enum SCU_LSCSCCLKSRC_E
 * \brief SCU LSC SC CLK Source.
 */
typedef enum {
	SCU_LSCSCCLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_LSCSCCLKSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_LSCSCCLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
} SCU_LSCSCCLKSRC_E;

/**
 * \enum SCU_LSCSENMCLKDIV_E
 * \brief SCU LSC Sensor MCLK DIV.
 */
typedef enum {
	SCU_LSCSENMCLKDIV_1, /**< CLK Source DIV1 */
	SCU_LSCSENMCLKDIV_2, /**< CLK Source DIV2 */
	SCU_LSCSENMCLKDIV_3, /**< CLK Source DIV3 */
	SCU_LSCSENMCLKDIV_4, /**< CLK Source DIV4 */
	SCU_LSCSENMCLKDIV_5, /**< CLK Source DIV5 */
	SCU_LSCSENMCLKDIV_6, /**< CLK Source DIV6 */
	SCU_LSCSENMCLKDIV_7, /**< CLK Source DIV7 */
	SCU_LSCSENMCLKDIV_8, /**< CLK Source DIV8 */
	SCU_LSCSENMCLKDIV_9, /**< CLK Source DIV9 */
	SCU_LSCSENMCLKDIV_10, /**< CLK Source DIV10 */
	SCU_LSCSENMCLKDIV_11, /**< CLK Source DIV11 */
	SCU_LSCSENMCLKDIV_12, /**< CLK Source DIV12 */
	SCU_LSCSENMCLKDIV_13, /**< CLK Source DIV13 */
	SCU_LSCSENMCLKDIV_14, /**< CLK Source DIV14 */
	SCU_LSCSENMCLKDIV_15, /**< CLK Source DIV15 */
	SCU_LSCSENMCLKDIV_16, /**< CLK Source DIV16 */
} SCU_LSCSENMCLKDIV_E;

/**
 * \enum SCU_LSCADCCKINSRC_E
 * \brief SCU LSC ADC CK IN Source.
 */
typedef enum {
	SCU_LSCVADDCLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_LSCVADDCLKSRC_XTAL24M, /**< CLK Source XTAL24M */
} SCU_LSCADCCKINSRC_E;

/**
 * \enum SCU_LSCADCCKINDIV_E
 * \brief SCU LSC ADC CK IN DIV.
 */
typedef enum {
	SCU_LSCADCCKINDIV_1, /**< CLK Source DIV1 */
	SCU_LSCADCCKINDIV_2, /**< CLK Source DIV2 */
	SCU_LSCADCCKINDIV_3, /**< CLK Source DIV3 */
	SCU_LSCADCCKINDIV_4, /**< CLK Source DIV4 */
	SCU_LSCADCCKINDIV_5, /**< CLK Source DIV5 */
	SCU_LSCADCCKINDIV_6, /**< CLK Source DIV6 */
	SCU_LSCADCCKINDIV_7, /**< CLK Source DIV7 */
	SCU_LSCADCCKINDIV_8, /**< CLK Source DIV8 */
	SCU_LSCADCCKINDIV_9, /**< CLK Source DIV9 */
	SCU_LSCADCCKINDIV_10, /**< CLK Source DIV10 */
	SCU_LSCADCCKINDIV_11, /**< CLK Source DIV11 */
	SCU_LSCADCCKINDIV_12, /**< CLK Source DIV12 */
	SCU_LSCADCCKINDIV_13, /**< CLK Source DIV13 */
	SCU_LSCADCCKINDIV_14, /**< CLK Source DIV14 */
	SCU_LSCADCCKINDIV_15, /**< CLK Source DIV15 */
	SCU_LSCADCCKINDIV_16, /**< CLK Source DIV16 */
} SCU_LSCADCCKINDIV_E;

/**
 * \enum SCU_LSCSCCLKSRC_E
 * \brief SCU LSC SC CLK Source.
 */
typedef enum {
	SCU_LSCVADDCLKSRC_PDMPCM, /**< CLK Source PDMPCM */
	SCU_LSCVADDCLKSRC_I2SHOSTPCM, /**< CLK Source I2SHOSTPCM */
	SCU_LSCVADDCLKSRC_I2SSLAVEPCM, /**<  CLK Source I2SSLAVEPCM*/
} SCU_LSCVADDCLKSRC_E;


/**
 * \enum SCU_LSCCKMONSRC_E
 * \brief SCU LSC Clock Monitor Source.
 */
typedef enum {
	SCU_LSCCKMONSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_LSCCKMONSRC_XTAL24M, /**< CLK Source XTAL24M */
} SCU_LSCCKMONSRC_E;

/**
 * \enum SCU_LSCSSPIMSRC_E
 * \brief SCU LSC SSPIM CLK Source.
 */
typedef enum {
	SCU_LSCSSPIMSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_LSCSSPIMSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_LSCSSPIMSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_LSCSSPIMSRC_PLL, /**<  CLK Source PLL*/
} SCU_LSCSSPIMSRC_E;

/**
 * \enum SCU_LSCSSPISSRC_E
 * \brief SCU LSC SSPIS CLK Source.
 */
typedef enum {
	SCU_LSCSSPISSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_LSCSSPISSRC_RC96M48M, /**< CLK Source RC96M48M */
	SCU_LSCSSPISSRC_XTAL24M, /**<  CLK Source XTAL24M*/
	SCU_LSCSSPISSRC_PLL, /**<  CLK Source PLL*/
} SCU_LSCSSPISSRC_E;

/**
 * \enum SCU_LSCPDMCLKSRC_E
 * \brief SCU LSC PDM CLK Source.
 */
typedef enum {
	SCU_LSCPDMCLKSRC_LSCREF, /**< CLK Source LSC Ref */
	SCU_LSCPDMCLKSRC_EXT, /**< CLK Source External PDM Clock */
} SCU_LSCPDMCLKSRC_E;

/**
 * \enum SCU_SBCLKSRC_E
 * \brief SCU SB CLK Source.
 */
typedef enum {
	SCU_SBCLKSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_SBCLKSRC_XTAL24M, /**<  CLK Source XTAL24M*/
} SCU_SBCLKSRC_E;


/**
 * \enum SCU_SBCLKDIV_E
 * \brief SCU SB CLK DIV.
 */
typedef enum {
	SCU_SBCLKDIV_1, /**< CLK Source DIV1 */
	SCU_SBCLKDIV_2, /**< CLK Source DIV2 */
	SCU_SBCLKDIV_3, /**< CLK Source DIV3 */
	SCU_SBCLKDIV_4, /**< CLK Source DIV4 */
	SCU_SBCLKDIV_5, /**< CLK Source DIV5 */
	SCU_SBCLKDIV_6, /**< CLK Source DIV6 */
	SCU_SBCLKDIV_7, /**< CLK Source DIV7 */
	SCU_SBCLKDIV_8, /**< CLK Source DIV8 */
	SCU_SBCLKDIV_9, /**< CLK Source DIV9 */
	SCU_SBCLKDIV_10, /**< CLK Source DIV10 */
	SCU_SBCLKDIV_11, /**< CLK Source DIV11 */
	SCU_SBCLKDIV_12, /**< CLK Source DIV12 */
	SCU_SBCLKDIV_13, /**< CLK Source DIV13 */
	SCU_SBCLKDIV_14, /**< CLK Source DIV14 */
	SCU_SBCLKDIV_15, /**< CLK Source DIV15 */
	SCU_SBCLKDIV_16, /**< CLK Source DIV16 */
} SCU_SBCLKDIV_E;


/**
 * \enum SCU_SB_32KSRC_E
 * \brief SCU SB 32K CLK Source.
 */
typedef enum {
	SCU_SB_32KSRC_RC32K1K, /**< CLK Source RC32K1K */
	SCU_SB_32KSRC_XTAL32K /**<  CLK Source XTAL32K*/
} SCU_SB_32KSRC_E;

/**
 * \enum SCU_SB_AHB4_HCLK_SEL_E
 * \brief SCU SB AHB4 HCLK Sel.
 */
typedef enum {
	SCU_SB_AHB4_HCLK_SBCLK, /**< CLK Source SB_CLK */
	SCU_SB_AHB4_HCLK_SB32KCLK /**<  CLK Source SB_CLK 32K*/
} SCU_SB_AHB4_HCLK_SEL_E;

/**
 * \enum SCU_SB_APB1AHB4_CLK_SEL_E
 * \brief SCU SB APB1 and AHB4 CLK Sel.
 */
typedef enum {
	SCU_SB_APB1SBCLKD4_AHB4SBCLK, /**< CLK Source APB1=SB_CLK Div 4, AHB4=SB_CLK */
	SCU_SB_APB1SB32K_AHB4SB32K /**<  CLK Source SB_CLK 32K*/
} SCU_SB_APB1AHB4_CLK_SEL_E;

/**
 * \enum SCU_SB_HMXI2CMSRC_E
 * \brief SCU SB HIMAX I2CM Source.
 */
typedef enum {
	SCU_SB_HMXI2CMSRC_RC24M1M, /**< CLK Source RC24M1M */
	SCU_SB_HMXI2CMSRC_XTAL24M, /**< CLK Source XTAL24M */
} SCU_SB_HMXI2CMSRC_E;


/**
 * \enum SCU_AON CLKSRC_E
 * \brief SCU AON CLK Source.
 */
typedef enum {
	SCU_AONCLKSRC_APB1_RC32K1K, /**<  CLK Source SB APB1 RC32K1K*/
	SCU_AONCLKSRC_APB1_XTAL32K, /**<  CLK Source SB APB1 XTAL32K*/
	SCU_AONCLKSRC_RC32K1K, /**< CLK Source RC32K1K */
	SCU_AONCLKSRC_XTAL32K, /**<  CLK Source XTAL32K*/
} SCU_AONCLKSRC_E;

/**
 * \enum SCU_AON SCU_ANTITAMP_SRC_E
 * \brief SCU ANTITAMP CLK Source.
 */
typedef enum {
	SCU_ANTITAMP_RC32K1K, /**< CLK Source RC32K1K */
	SCU_ANTITAMP_XTAL32K /**<  CLK Source XTAL32K*/
} SCU_ANTITAMP_SRC_E;
/**
 * \enum SCU_AON CLKSRC_E
 * \brief SCU AON CLK Source.
 */
typedef enum {
	SCU_AONCLK32KSRC_RC32K1K, /**< CLK Source RC32K1K */
	SCU_AONCLK32KSRC_XTAL32K /**<  CLK Source XTAL32K*/
} SCU_AONCLK32KSRC_E;

/**
 * \enum SCU_CLK_FREQ_TYPE_E
 * \brief SCU CLK TYPE Frequency.
 */
typedef enum {
	SCU_CLK_FREQ_TYPE_PLL, /**< PLL Frequency */
	SCU_CLK_FREQ_TYPE_HSC_CLK, /**< HSC CLK Frequency */
	SCU_CLK_FREQ_TYPE_HSC_CLK_D1_2, /**< HSC_CLK_D1_2 Frequency */
	SCU_CLK_FREQ_TYPE_HSC_32K_CLK, /**< HSC_32K_CLK Frequency */
	SCU_CLK_FREQ_TYPE_HSC_CM55M, /**< CM55M Frequency */
	SCU_CLK_FREQ_TYPE_HSC_U55, /**< U55 Frequency */
	SCU_CLK_FREQ_TYPE_HSC_AXI, /**< AXI Frequency */
	SCU_CLK_FREQ_TYPE_HSC_AHB_0, /**< AHB_0 Frequency */
	SCU_CLK_FREQ_TYPE_HSC_AHB_5, /**< AHB_5 Frequency */
	SCU_CLK_FREQ_TYPE_HSC_AHB_1, /**< AHB_1 Frequency */
	SCU_CLK_FREQ_TYPE_HSC_APB_2, /**< APB_2 Frequency */
	SCU_CLK_FREQ_TYPE_HSC_XDMA, /**< XDMA Frequency */
	SCU_CLK_FREQ_TYPE_HSC_I3CHC, /**< I3CHC Frequency */
	SCU_CLK_FREQ_TYPE_HSC_PUF, /**< PUF Frequency */
	SCU_CLK_FREQ_TYPE_HSC_DP_PCLK, /**< DP PCLK Frequency */
	SCU_CLK_FREQ_TYPE_HSC_MIPI_RXCLK, /**< MIPI RX TX Frequency */
	SCU_CLK_FREQ_TYPE_HSC_PPI_TXCLK, /**< MIPI PPI TX Frequency */
	SCU_CLK_FREQ_TYPE_HSC_SDIO_CLK, /**< SDIO Frequency */
	SCU_CLK_FREQ_TYPE_HSC_QSPI_CLK, /**< QSPI Frequency */
	SCU_CLK_FREQ_TYPE_HSC_OSPI_CLK, /**< OSPI Frequency */
	SCU_CLK_FREQ_TYPE_LSC_CLK, /**< LSC_CLK Frequency */
	SCU_CLK_FREQ_TYPE_LSC_CLK_D4, /**< LSC_CLK_D4 Frequency */
	SCU_CLK_FREQ_TYPE_LSC_32K_CLK, /**< LSC_32K_CLK Frequency */
	SCU_CLK_FREQ_TYPE_LSC_CM55S, /**< LSC CM55S Frequency */
	SCU_CLK_FREQ_TYPE_LSC_AHB_M, /**< LSC AHB_M Frequency */
	SCU_CLK_FREQ_TYPE_LSC_AHB_2, /**< LSC AHB_2 Frequency */
	SCU_CLK_FREQ_TYPE_LSC_AHB_3, /**< LSC AHB_3 Frequency */
	SCU_CLK_FREQ_TYPE_LSC_APB_0, /**< LSC APB_0 Frequency */
	SCU_CLK_FREQ_TYPE_LSC_REF_CLK, /**< LSC_REF_CLK Frequency */
	SCU_CLK_FREQ_TYPE_LSC_I2S_HOST, /**< LSC I2S Host Frequency */
	SCU_CLK_FREQ_TYPE_LSC_PDM_HOST, /**< LSC PDM Host Frequency */
	SCU_CLK_FREQ_TYPE_LSC_UART, /**< LSC UART Frequency */
	SCU_CLK_FREQ_TYPE_LSC_I3C_SLAVE, /**< LSC I3C Slave Frequency */
	SCU_CLK_FREQ_TYPE_LSC_PWM, /**< LSC PWM Frequency */
	SCU_CLK_FREQ_TYPE_LSC_RO_PD_CK_IN, /**< LSC RO_PD_CK_IN Frequency */
	SCU_CLK_FREQ_TYPE_LSC_I2C, /**< LSC I2C Frequency */
	SCU_CLK_FREQ_TYPE_LSC_SCCLK, /**< LSC SC CLK Frequency */
	SCU_CLK_FREQ_TYPE_LSC_MCLK, /**< LSC MCLK Frequency */
	SCU_CLK_FREQ_TYPE_LSC_ADC_CKIN, /**< LSC ADC_CKIN Frequency */
	SCU_CLK_FREQ_TYPE_LSC_CKMONITOR, /**< LSC CLK Monitor Frequency */
	SCU_CLK_FREQ_TYPE_LSC_SSPIM, /**< LSC SSPIM Frequency */
	SCU_CLK_FREQ_TYPE_LSC_SSPIS, /**< LSC SSPIS Frequency */
	SCU_CLK_FREQ_TYPE_SB_SB_CLK, /**< SB SB_CLK Frequency */
	SCU_CLK_FREQ_TYPE_SB_SB_CLK_D4, /**< SB SB_CLK_D4 Frequency */
	SCU_CLK_FREQ_TYPE_SB_APB_1_CLK, /**< SB APB_1_CLK Frequency */
	SCU_CLK_FREQ_TYPE_SB_AHB_4_CLK, /**< SB AHB_4_CLK Frequency */
	SCU_CLK_FREQ_TYPE_SB_APB_1_CLK_PMU, /**< SB APB_1_CLK PMU Frequency */
	SCU_CLK_FREQ_TYPE_SB_TS_CLK, /**< SB TS_CLK Frequency */
	SCU_CLK_FREQ_TYPE_SB_ADC_LP_CK_IN_HV, /**< SB ADC_LP_CK_IN_HV Frequency */
	SCU_CLK_FREQ_TYPE_SB_32K, /**< SB 32K Frequency */
	SCU_CLK_FREQ_TYPE_HMXI2CM, /**< SB HMXI2CM Frequency */

	SCU_CLK_FREQ_TYPE_AON_AON_CLK, /**< AON AON_CLK Frequency */
	SCU_CLK_FREQ_TYPE_AON_32K_CLK, /**< AON 32K_CLK Frequency */
	SCU_CLK_FREQ_TYPE_AON_PMU_CLK, /**< AON PMU Frequency */
	SCU_CLK_FREQ_TYPE_AON_ANTITAMP, /**< AON ANTI TAMP Frequency */
} SCU_CLK_FREQ_TYPE_E;

#endif


/**
 * \enum SCU_REG_APBBRG_AUD_DMA_E
 * \brief SCU APBBRG_AUD_DMA
 */
typedef enum {
	SCU_REG_APBBRG_AUD_DMA_RES = 1, /**< SEN reserved */
	SCU_REG_APBBRG_AUD_DMA_I2S_H = 0x2, /**< APBBRG_AUD_DMA of I2S Host */
	SCU_REG_APBBRG_AUD_DMA_I2S_S = 0x4, /**< APBBRG_AUD_DMA of I2S Slave */
	SCU_REG_APBBRG_AUD_DMA_HXPDM = 0x8 /**< APBBRG_AUD_DMA of HXPDM */
} SCU_REG_APBBRG_AUD_DMA_E;

/**
 * \enum SCU_AUD_CH_SEL_E
 * \brief SCU AUD_CH_SEL
 */
typedef enum {
	SCU_AUD_CH_SEL_RES = 1, /**< SEN reserved */
	SCU_AUD_CH_SEL_I2S_H = 0x2, /**< AUD_CH_SEL of I2S Host */
	SCU_AUD_CH_SEL_I2S_S = 0x4, /**< AUD_CH_SEL of I2S Slave */
	SCU_AUD_CH_SEL_HXPDM = 0x8 /**< AUD_CH_SEL of HXPDM */
} SCU_AUD_CH_SEL_E;


/**
 * \enum SCU_SEN_TRIG_PINMUX_E
 * \brief SCU SEN TRIG PinMux
 */
typedef enum {
	SCU_SEN_TRIG_PINMUX_TRIG0 = 0, /**< SEN TRIG */
	SCU_SEN_TRIG_PINMUX_TRIG1 = 1, /**< SEN TRIG */
	SCU_SEN_TRIG_PINMUX_GPIO7 = 2, /**< GPIO7 */
	SCU_SEN_TRIG_PINMUX_SEND9 = 3 /**< SEN_D9 */
} SCU_SEN_TRIG_PINMUX_E;

/**
 * \enum SCU_SEN_FAE_PINMUX_E
 * \brief SCU SEN FAE PinMux
 */
typedef enum {
	SCU_SEN_FAE_PINMUX_FAE0 = 0, /**< SEN FAE */
	SCU_SEN_FAE_PINMUX_FAE1 = 1, /**< SEN FAE */
	SCU_SEN_FAE_PINMUX_GPIO6 = 2, /**< GPIO6 */
	SCU_SEN_FAE_PINMUX_SEND8 = 3 /**< SEN_D8 */
} SCU_SEN_FAE_PINMUX_E;

/**
 * \enum SCU_SEN_CSW0_PINMUX_E
 * \brief SCU SEN CSW0 PinMux
 */
typedef enum {
	SCU_SEN_CSW0_PINMUX_CSW0_0 = 0, /**< SEN CSW0 */
	SCU_SEN_CSW0_PINMUX_CSW0_1 = 1, /**< SEN CSW0 */
	SCU_SEN_CSW0_PINMUX_GPIO8 = 2 /**< GPIO8 */
} SCU_SEN_CSW0_PINMUX_E;

/**
 * \enum SCU_SEN_CSW1_PINMUX_E
 * \brief SCU SEN CSW1 PinMux
 */
typedef enum {
	SCU_SEN_CSW1_PINMUX_CSW1_0 = 0, /**< SEN CSW0 */
	SCU_SEN_CSW1_PINMUX_CSW1_1 = 1, /**< SEN CSW0 */
	SCU_SEN_CSW1_PINMUX_GPIO9 = 2 /**< GPIO9 */
} SCU_SEN_CSW1_PINMUX_E;

/**
 * \enum SCU_SEN_INT_PINMUX_E
 * \brief SCU SEN INT PinMux
 */
typedef enum {
	SCU_SEN_INT_PINMUX_INT0 = 0, /**< SEN INT */
	SCU_SEN_INT_PINMUX_INT1 = 1, /**< SEN INT */
	SCU_SEN_INT_PINMUX_GPIO10 = 2, /**< GPIO10 */
	SCU_SEN_INT_PINMUX_FVALID = 3 /**< FVALID */
} SCU_SEN_INT_PINMUX_E;

/**
 * \enum SCU_SEN_GPIO_PINMUX_E
 * \brief SCU SEN GPIO PinMux
 */
typedef enum {
	SCU_SEN_GPIO_PINMUX_SENAONGPIO2_0 = 0, /**< SEN_AON_GPIO2 */
	SCU_SEN_GPIO_PINMUX_SENAONGPIO2_1 = 1, /**< SEN_AON_GPIO2 */
	SCU_SEN_GPIO_PINMUX_SENGPIO3 = 2, /**< SENGPIO3 */
	SCU_SEN_GPIO_PINMUX_LVALID = 3 /**< LVALID */
} SCU_SEN_GPIO_PINMUX_E;

/**
 * \enum SCU_SEN_I2CM_SDA_PINMUX_E
 * \brief SCU SEN I2CM_SDA PinMux
 */
typedef enum {
	SCU_SEN_I2CM_SDA_PINMUX_I2CM_SDA_0 = 0, /**< SEN I2CM_SDA */
	SCU_SEN_I2CM_SDA_PINMUX_I2CM_SDA_1 = 1, /**< SEN I2CM_SDA */
	SCU_SEN_I2CM_SDA_PINMUX_GPIO4 = 2, /**< GPIO4 */
	SCU_SEN_I2CM_SDA_PINMUX_MIPI_I2C_M_SDA = 6 /**< MIPI_I2C_M_SDA */
} SCU_SEN_I2CM_SDA_PINMUX_E;

/**
 * \enum SCU_SEN_I2CM_SCL_PINMUX_E
 * \brief SCU SEN I2CM_SCL PinMux
 */
typedef enum {
	SCU_SEN_I2CM_SCL_PINMUX_I2CM_SCL_0 = 0, /**< SEN I2CM_SCL */
	SCU_SEN_I2CM_SCL_PINMUX_I2CM_SCL_1 = 1, /**< SEN I2CM_SCL */
	SCU_SEN_I2CM_SCL_PINMUX_GPIO5 = 2, /**< GPIO5 */
	SCU_SEN_I2CM_SCL_PINMUX_RC24M1M_CLK_DIV256 = 3, /**< RC_24M1M_CLK/256 */
	SCU_SEN_I2CM_SCL_PINMUX_RC96M48M_CLK_DIV256 = 4, /**< RC_96M48M_CLK/256 */
	SCU_SEN_I2CM_SCL_PINMUX_XTAL24M_CLK_DIV256 = 5, /**< XTAL_24M_CLK/256 */
	SCU_SEN_I2CM_SCL_PINMUX_MIPI_I2C_M_SCL = 6, /**< MIPI_I2C_M_SCL */
	SCU_SEN_I2CM_SCL_PINMUX_PLL_CLK_DIV256 = 7 /**< PLL_CLK/256 */
} SCU_SEN_I2CM_SCL_PINMUX_E;


/**
 * \enum SCU_PA0_PINMUX_E
 * \brief SCU PA0 PinMux
 */
typedef enum {
	SCU_PA0_PINMUX_AON_GPIO0_0 = 0, /**< AON_GPIO0 (input mode only)*/
	SCU_PA0_PINMUX_PMU_SINT0 = 1, /**< PMU_SINT0 */
	SCU_PA0_PINMUX_AON_GPIO0_2 = 2, /**< AON_GPIO0 (input/output mode)*/
	SCU_PA0_PINMUX_AON_XTAL32K_CLK_DIV256 = 3, /**< XTAL_32K_CLK/256 */
	SCU_PA0_PINMUX_PMU_DCDC_EN = 5, /**< PMU_DCDC_EN */
	SCU_PA0_PINMUX_TAM_IN_0 = 6, /**< TAM_IN_0 */
	SCU_PA0_PINMUX_EN_OSC_32K1K = 7 /**< EN_OSC_32K1K */
} SCU_PA0_PINMUX_E;

/**
 * \enum SCU_PA1_PINMUX_E
 * \brief SCU PA1 PinMux
 */
typedef enum {
	SCU_PA1_PINMUX_VMUTE_0 = 0, /**< VMUTE */
	SCU_PA1_PINMUX_VMUTE_1 = 1, /**< VMUTE */
	SCU_PA1_PINMUX_AON_GPIO1 = 2, /**< AON_GPIO1 */
	SCU_PA1_PINMUX_AON_PMU_SINT1 = 3, /**< PMU_SINT1 */
	SCU_PA1_PINMUX_PMU_DCDC_EN = 5, /**< PMU_DCDC_EN */
	SCU_PA1_PINMUX_TAM_OUT_0 = 6, /**< TAM_OUT_0 */
	SCU_PA1_PINMUX_OSC_OUT_32K1K = 7 /**< OSC_OUT_32K1K */
} SCU_PA1_PINMUX_E;


/**
 * \enum SCU_PA2_PINMUX_E
 * \brief SCU PA2 PinMux
 */
typedef enum {
	SCU_PA2_PINMUX_SB_I2C_S_SCL_0 = 0, /**< SB_I2C_S_SCL */
	SCU_PA2_PINMUX_SB_I2C_S_SCL_1 = 1, /**< SB_I2C_S_SCL */
	SCU_PA2_PINMUX_SB_GPIO0 = 2, /**< SB_GPIO0 */
	SCU_PA2_PINMUX_SWCLK = 3, /**< SWCLK */
	SCU_PA2_PINMUX_SB_I2C_S_SCL_4 = 4, /**< SB_I2C_S_SCL */
	SCU_PA2_PINMUX_SB_I3C_S_SCL = 5, /**< SB_I3C_S_SCL */
	SCU_PA2_PINMUX_I2C_M_SCL = 6, /**< I2C_M_SCL */
	SCU_PA2_PINMUX_I3C_M_SCL = 7, /**< I3C_M_SCL */
	SCU_PA2_PINMUX_TAM_IN_1 = 9, /**< TAM_IN_1 */
	SCU_PA2_PINMUX_UART2_RX = 10 /**< UART2_RX */
} SCU_PA2_PINMUX_E;

/**
 * \enum SCU_PA3_PINMUX_E
 * \brief SCU PA3 PinMux
 */
typedef enum {
	SCU_PA3_PINMUX_SB_I2C_S_SDA_0 = 0, /**< I2C_S_SDA */
	SCU_PA3_PINMUX_SB_I2C_S_SDA_1 = 1, /**< I2C_S_SDA */
	SCU_PA3_PINMUX_SB_GPIO1 = 2, /**< SB_GPIO1 */
	SCU_PA3_PINMUX_SWDIO = 3, /**< SWDIO */
	SCU_PA3_PINMUX_SB_I2C_S_SDA_4 = 4, /**< SB_I2C_S_SDA */
	SCU_PA3_PINMUX_SB_I3C_S_SDA = 5, /**< SB_I3C_S_SDA */
	SCU_PA3_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA */
	SCU_PA3_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA */
	SCU_PA3_PINMUX_SB_32K_CLK_DIV256 = 8, /**< SB_32K_CLK_DIV256 */
	SCU_PA3_PINMUX_TAM_OUT_1 = 9, /**< TAM_OUT_1 */
	SCU_PA3_PINMUX_UART2_TX = 10 /**< UART2_TX */
} SCU_PA3_PINMUX_E;

/**
 * \enum SCU_UART_PINMUX_E
 * \brief SCU UART PinMux
 */
typedef enum {
	SCU_UART_PINMUX_NA = 0, /**< NA */
	SCU_UART_PINMUX_UART1 = 1 /**< UART1 TODO*/
} SCU_UART_PINMUX_E;

/**
 * \enum SCU_PB0_PINMUX_E
 * \brief SCU PB0 PinMux
 */
typedef enum {
	SCU_PB0_PINMUX_NA = 0, /**< NA */
	SCU_PB0_PINMUX_UART0_RX_1 = 1, /**< UART0_RX*/
	SCU_PB0_PINMUX_GPIO11 = 2, /**< GPIO11*/
	SCU_PB0_PINMUX_SWCLK = 3, /**< SWCLK*/
	SCU_PB0_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB0_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB0_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB0_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB0_PINMUX_SPI_M_CLK = 8, /**< SPI_M_CLK*/
	SCU_PB0_PINMUX_SPI_S_CLK = 9, /**< SPI_S_CLK*/
#if(IC_VERSION >= 30)
	SCU_PB0_PINMUX_UART1_RX_10 = 10, /**< UART1_RX*/
#else
	SCU_PB0_PINMUX_UART0_RX_10 = 10, /**< UART0_RX*/
#endif
	SCU_PB0_PINMUX_PWM0 = 11, /**< PWM0*/
	SCU_PB0_PINMUX_SDIO_CMD = 12, /**< SDIO_CMD*/
	SCU_PB0_PINMUX_PDM_DATA0 = 13, /**< PDM_DATA0*/
	SCU_PB0_PINMUX_I2S_M_SCLK = 14, /**< I2S_M_SCLK*/
	SCU_PB0_PINMUX_I2S_S_SDI = 15, /**< I2S_S_SDI*/
	SCU_PB0_PINMUX_SW_DATA = 16, /**< SW_DATA*/
	SCU_PB0_PINMUX_TAM_IN_2 = 17 /**< TAM_IN_2*/
} SCU_PB0_PINMUX_E;

/**
 * \enum SCU_PB1_PINMUX_E
 * \brief SCU PB1 PinMux
 */
typedef enum {
	SCU_PB1_PINMUX_NA = 0, /**< NA */
	SCU_PB1_PINMUX_UART0_TX_1 = 1, /**< UART0_TX*/
	SCU_PB1_PINMUX_GPIO12 = 2, /**< GPIO12*/
	SCU_PB1_PINMUX_SWDIO = 3, /**< SWDIO*/
	SCU_PB1_PINMUX_I2C_S1_SCL = 4, /**< I2C_S1_SCL*/
	SCU_PB1_PINMUX_I3C_S1_SCL = 5, /**< I3C_S1_SCL*/
	SCU_PB1_PINMUX_I2C_M_SCL = 6, /**< I2C_M_SCL*/
	SCU_PB1_PINMUX_I3C_M_SCL = 7, /**< I3C_M_SCL*/
	SCU_PB1_PINMUX_SPI_M_CS = 8, /**< SPI_M_CS*/
	SCU_PB1_PINMUX_SPI_S_CS = 9, /**< SPI_S_CS*/
#if(IC_VERSION >= 30)
	SCU_PB1_PINMUX_UART1_TX_10 = 10, /**< UART1_TX*/
#else
	SCU_PB1_PINMUX_UART0_TX_10 = 10, /**< UART0_TX*/
#endif
	SCU_PB1_PINMUX_PWM1 = 11, /**< PWM1*/
	SCU_PB1_PINMUX_SDIO_SDCLK = 12, /**< SDIO_SDCLK*/
	SCU_PB1_PINMUX_PDM_CLK = 13, /**< PDM_CLK*/
	SCU_PB1_PINMUX_I2S_M_WS = 14, /**< I2S_M_WS*/
	SCU_PB1_PINMUX_I2S_S_WS = 15, /**< I2S_S_WS*/
	SCU_PB1_PINMUX_SW_CLK = 16, /**< SW_CLK*/
	SCU_PB1_PINMUX_TAM_OUT_2 = 17 /**< TAM_OUT_2*/
} SCU_PB1_PINMUX_E;


/**
 * \enum SCU_PB9_PINMUX_E
 * \brief SCU PB9 PinMux
 */
typedef enum {
#if(IC_VERSION >= 30)
	SCU_PB9_PINMUX_MCLK_IN = 0, /**< MCLK in */
#else
	SCU_PB9_PINMUX_NA = 0, /**< NA */
#endif
	SCU_PB9_PINMUX_PDM_CLK_1 = 1, /**< PDM_CLK*/
	SCU_PB9_PINMUX_GPIO0 = 2, /**< GPIO0*/
	SCU_PB9_PINMUX_SWCLK = 3, /**< SWCLK*/
	SCU_PB9_PINMUX_I2C_S1_SCL = 4, /**< I2C_S1_SCL*/
	SCU_PB9_PINMUX_I3C_S1_SCL = 5, /**< I3C_S1_SCL*/
	SCU_PB9_PINMUX_I2C_M_SCL = 6, /**< I2C_M_SCL*/
	SCU_PB9_PINMUX_I3C_M_SCL = 7, /**< I3C_M_SCL*/
	SCU_PB9_PINMUX_SPI_M_DO = 8, /**< SPI_M_DO*/
	SCU_PB9_PINMUX_SPI_S_DO = 9, /**< SPI_S_DO*/
	SCU_PB9_PINMUX_UART2_RX = 10, /**< UART2_RX*/
	SCU_PB9_PINMUX_PWM1 = 11, /**< PWM1*/
	SCU_PB9_PINMUX_SDIO_SDCLK = 12, /**< SDIO_SDCLK*/
	SCU_PB9_PINMUX_PDM_CLK_13 = 13, /**< PDM_CLK*/
	SCU_PB9_PINMUX_I2S_M_CLK = 14, /**< I2S_M_CLK*/
	SCU_PB9_PINMUX_I2S_S_CLK = 15, /**< I2S_S_CLK*/
	SCU_PB9_PINMUX_SW_DATA = 16 /**< SW_DATA*/
} SCU_PB9_PINMUX_E;

/**
 * \enum SCU_PB10_PINMUX_E
 * \brief SCU PB10 PinMux
 */
typedef enum {
#if(IC_VERSION >= 30)
	SCU_PB10_PINMUX_XSHUTDOWN_IN = 0, /**< XSHUTDOWN in */
#else
	SCU_PB10_PINMUX_NA = 0, /**< NA */
#endif
	SCU_PB10_PINMUX_PDM_DATA0_1 = 1, /**< PDM_DATA0*/
	SCU_PB10_PINMUX_GPIO1 = 2, /**< GPIO1*/
	SCU_PB10_PINMUX_SWDIO = 3, /**< SWDIO*/
	SCU_PB10_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB10_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB10_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB10_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB10_PINMUX_SPI_M_CLK = 8, /**< SPI_M_CLK*/
	SCU_PB10_PINMUX_SPI_S_CLK = 9, /**< SPI_S_CLK*/
	SCU_PB10_PINMUX_UART2_TX = 10, /**< UART2_TX*/
	SCU_PB10_PINMUX_PWM2 = 11, /**< PWM2*/
	SCU_PB10_PINMUX_SDIO_DAT3 = 12, /**< SDIO_DAT3*/
	SCU_PB10_PINMUX_PDM_DATA0_13 = 13, /**< PDM_DATA0*/
	SCU_PB10_PINMUX_I2S_M_WS = 14, /**< I2S_M_WS*/
	SCU_PB10_PINMUX_I2S_S_WS = 15, /**< I2S_S_WS*/
	SCU_PB10_PINMUX_SW_CLK = 16 /**< SW_CLK*/
} SCU_PB10_PINMUX_E;

/**
 * \enum SCU_PB11_PINMUX_E
 * \brief SCU PB11 PinMux
 */
typedef enum {
#if(IC_VERSION >= 30)
	SCU_PB11_PINMUX_ISPSEL = 0, /**< ISP Sel */
#else
	SCU_PB11_PINMUX_NA = 0, /**< NA */
#endif
	SCU_PB11_PINMUX_PDM_DATA1_1 = 1, /**< PDM_DATA1*/
	SCU_PB11_PINMUX_GPIO2 = 2, /**< GPIO2*/
	SCU_PB11_PINMUX_SRSTN = 3, /**< SRSTN*/
	SCU_PB11_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB11_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB11_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB11_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB11_PINMUX_SPI_M_CS = 8, /**< SPI_M_CS*/
	SCU_PB11_PINMUX_SPI_S_CS = 9, /**< SPI_S_CS*/
	SCU_PB11_PINMUX_UART1_RTS = 10, /**< UART1_RTS*/
	SCU_PB11_PINMUX_PWM0 = 11, /**< PWM0*/
	SCU_PB11_PINMUX_SDIO_DAT2 = 12, /**< SDIO_DAT2*/
	SCU_PB11_PINMUX_PDM_DATA1_13 = 13, /**< PDM_DATA1*/
	SCU_PB11_PINMUX_I2S_M_SDI = 14, /**< I2S_M_SDI*/
	SCU_PB11_PINMUX_I2S_S_SDI = 15, /**< I2S_S_SDI*/
	SCU_PB11_PINMUX_SW_DATA = 16 /**< SW_DATA*/
} SCU_PB11_PINMUX_E;

/**
 * \enum SCU_PC3_PINMUX_E
 * \brief SCU PC3 PinMux
 */
typedef enum {
	SCU_PC3_PINMUX_NA = 0, /**< NA */
	SCU_PC3_PINMUX_PDM_DATA2 = 1, /**< PDM_DATA2*/
	SCU_PC3_PINMUX_I2S_M_SDO = 14, /**< I2S_M_SDO*/
	SCU_PC3_PINMUX_I2S_S_SDO = 15 /**< I2S_S_SDO*/
} SCU_PC3_PINMUX_E;

/**
 * \enum SCU_PC4_PINMUX_E
 * \brief SCU PC4 PinMux
 */
typedef enum {
	SCU_PC4_PINMUX_NA = 0, /**< NA */
	SCU_PC4_PINMUX_PDM_DATA3 = 1 /**< PDM_DATA3*/
} SCU_PC4_PINMUX_E;

/**
 * \enum SCU_SW_DATA_PINMUX_E
 * \brief SCU SW_DATA PinMux
 */
typedef enum {
	SCU_SW_DATA_PINMUX_NA = 0, /**< NA */
	SCU_SW_DATA_PINMUX_SW_DATA = 1, /**< SW_DATA*/
	SCU_SW_DATA_PINMUX_RC24M1M_CLKDIV256 = 2, /**< RC24M1M_CLKDIV256*/
	SCU_SW_DATA_PINMUX_RC96M48M_CLKDIV256 = 3, /**< RC96M48M_CLKDIV256*/
	SCU_SW_DATA_PINMUX_XTAL24M_CLKDIV256 = 4, /**< XTAL24M_CLKDIV256*/
	SCU_SW_DATA_PINMUX_PLLCLK_DIV256 = 5, /**< PLLCLK_DIV256*/
	SCU_SW_DATA_PINMUX_UART2_TX = 10 /**< UART2_TX*/
} SCU_SW_DATA_PINMUX_E;

/**
 * \enum SCU_SW_CLK_PINMUX_E
 * \brief SCU SW_CLK PinMux
 */
typedef enum {
	SCU_SW_CLK_PINMUX_NA = 0, /**< NA */
	SCU_SW_CLK_PINMUX_SW_CLK = 1, /**< SW_CLK*/
	SCU_SW_CLK_PINMUX_UART2_RX = 10 /**< UART2_RX*/
} SCU_SW_CLK_PINMUX_E;

/**
 * \enum SCU_I2CM_SCLSDA_PINMUX_E
 * \brief SCU I2CM_SCLSDA PinMux
 */
typedef enum {
	SCU_I2CM_SCLSDA_PINMUX_NA = 0, /**< NA */
	SCU_I2CM_SCLSDA_PINMUX_I2CM_SCLSDA = 1, /**< SCLSDA*/
} SCU_I2CM_SCLSDA_PINMUX_E;


/**
 * \enum SCU_PB2_PINMUX_E
 * \brief SCU PB2 PinMux
 */
typedef enum {
	SCU_PB2_PINMUX_SPI2AHB_DO = 0, /**< SPI2AHB_DO */
	SCU_PB2_PINMUX_SPI_M_DO_1 = 1, /**< SPI_M_DO*/
	SCU_PB2_PINMUX_GPIO13 = 2, /**< GPIO13*/
	SCU_PB2_PINMUX_SWDIO = 3, /**< SWDIO*/
	SCU_PB2_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB2_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB2_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB2_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB2_PINMUX_SPI_M_DO_8 = 8, /**< SPI_M_DO*/
	SCU_PB2_PINMUX_SPI_S_DO = 9, /**< SPI_S_DO*/
	SCU_PB2_PINMUX_UART1_RTS = 10, /**< UART1_RTS*/
	SCU_PB2_PINMUX_PWM2 = 11, /**< PWM2*/
	SCU_PB2_PINMUX_SDIO_DAT1 = 12, /**< SDIO_DAT1*/
	SCU_PB2_PINMUX_PDM_DATA2 = 13, /**< PDM_DATA2*/
	SCU_PB2_PINMUX_I2S_M_SDO = 14, /**< I2S_M_SDO*/
	SCU_PB2_PINMUX_I2S_S_SDO = 15, /**< I2S_S_SDO*/
	SCU_PB2_PINMUX_SW_CLK = 16, /**< SW_CLK*/
	SCU_PB2_PINMUX_TAM_IN_3 = 17, /**< TAM_IN_3*/
#if(IC_VERSION >= 30)	
	SCU_PB2_PINMUX_PDM_CLK_IN = 18, /**< PDM_CLK_IN*/
#endif	
} SCU_PB2_PINMUX_E;

/**
 * \enum SCU_PB3_PINMUX_E
 * \brief SCU PB3 PinMux
 */
typedef enum {
	SCU_PB3_PINMUX_SPI2AHB_DI = 0, /**< SPI2AHB_DI */
	SCU_PB3_PINMUX_SPI_M_DI_1 = 1, /**< SPI_M_DI*/
	SCU_PB3_PINMUX_GPIO14 = 2, /**< GPIO14*/
	SCU_PB3_PINMUX_SRSTN = 3, /**< SRSTN*/
	SCU_PB3_PINMUX_I2C_S1_SCL = 4, /**< I2C_S1_SCL*/
	SCU_PB3_PINMUX_I3C_S1_SCL = 5, /**< I3C_S1_SCL*/
	SCU_PB3_PINMUX_I2C_M_SCL = 6, /**< I2C_M_SCL*/
	SCU_PB3_PINMUX_I3C_M_SCL = 7, /**< I3C_M_SCL*/
	SCU_PB3_PINMUX_SPI_M_DI_8 = 8, /**< SPI_M_DI*/
	SCU_PB3_PINMUX_SPI_S_DI = 9, /**< SPI_S_DI*/
	SCU_PB3_PINMUX_UART1_CTS = 10, /**< UART1_CTS*/
	SCU_PB3_PINMUX_PWM0 = 11, /**< PWM0*/
	SCU_PB3_PINMUX_SDIO_DAT0 = 12, /**< SDIO_DAT0*/
	SCU_PB3_PINMUX_PDM_DATA1 = 13, /**< PDM_DATA1*/
	SCU_PB3_PINMUX_I2S_S_SDI = 14, /**< I2S_M_SDI*/
	SCU_PB3_PINMUX_I2S_S_SDO = 15, /**< I2S_S_SDO*/
	SCU_PB3_PINMUX_SW_DATA = 16, /**< SW_DATA*/
	SCU_PB3_PINMUX_TAM_OUT_3 = 17, /**< TAM_OUT_3*/
#if(IC_VERSION >= 30)
	SCU_PB3_PINMUX_PDM_DATA0_OUT = 18, /**< PDM_DATA0_OUT*/
#endif
} SCU_PB3_PINMUX_E;

/**
 * \enum SCU_PB4_PINMUX_E
 * \brief SCU PB4 PinMux
 */
typedef enum {
	SCU_PB4_PINMUX_SPI2AHB_SCLK = 0, /**< SPI2AHB_SCLK */
	SCU_PB4_PINMUX_SPI_M_SCLK_1 = 1, /**< SPI_M_SCLK*/
	SCU_PB4_PINMUX_GPIO15 = 2, /**< GPIO15*/
	SCU_PB4_PINMUX_SWCLK = 3, /**< SWCLK*/
	SCU_PB4_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB4_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB4_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB4_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB4_PINMUX_SPI_M_SCLK_8 = 8, /**< SPI_M_SCLK*/
	SCU_PB4_PINMUX_SPI_S_CLK = 9, /**< SPI_S_CLK*/
	SCU_PB4_PINMUX_UART1_RX = 10, /**< UART1_RX*/
	SCU_PB4_PINMUX_PWM1 = 11, /**< PWM1*/
	SCU_PB4_PINMUX_SDIO_DAT2 = 12, /**< SDIO_DAT2*/
	SCU_PB4_PINMUX_PDM_CLK = 13, /**< PDM_CLK*/
	SCU_PB4_PINMUX_I2S_M_SCLK = 14, /**< I2S_M_SCLK*/
	SCU_PB4_PINMUX_I2S_S_SCLK = 15, /**< I2S_S_SCLK*/
	SCU_PB4_PINMUX_SW_CLK = 16, /**< SW_CLK*/
	SCU_PB4_PINMUX_TAM_IN_4 = 17, /**< TAM_IN_4*/
#if(IC_VERSION >= 30)
	SCU_PB4_PINMUX_NB_PDM_DATA0_OUT = 18, /**< NB_PDM_DATA0_OUT*/
#endif
} SCU_PB4_PINMUX_E;

/**
 * \enum SCU_PB5_PINMUX_E
 * \brief SCU PB5 PinMux
 */
typedef enum {
	SCU_PB5_PINMUX_SPI2AHB_CS = 0, /**< SPI2AHB_CS */
	SCU_PB5_PINMUX_SPI_M_CS_1 = 1, /**< SPI_M_CS*/
	SCU_PB5_PINMUX_GPIO16 = 2, /**< GPIO16*/
	SCU_PB5_PINMUX_SWDIO = 3, /**< SWDIO*/
	SCU_PB5_PINMUX_I2C_S1_SCL = 4, /**< I2C_S1_SCL*/
	SCU_PB5_PINMUX_I3C_S1_SCL = 5, /**< I3C_S1_SCL*/
	SCU_PB5_PINMUX_I2C_M_SCL = 6, /**< I2C_M_SCL*/
	SCU_PB5_PINMUX_I3C_M_SCL = 7, /**< I3C_M_SCL*/
	SCU_PB5_PINMUX_SPI_M_CS_8 = 8, /**< SPI_M_CS*/
	SCU_PB5_PINMUX_SPI_S_CS = 9, /**< SPI_S_CS*/
	SCU_PB5_PINMUX_UART1_TX = 10, /**< UART1_TX*/
	SCU_PB5_PINMUX_PWM2 = 11, /**< PWM2*/
	SCU_PB5_PINMUX_SDIO_DAT3 = 12, /**< SDIO_DAT3*/
	SCU_PB5_PINMUX_PDM_DATA0 = 13, /**< PDM_DATA0*/
	SCU_PB5_PINMUX_I2S_M_WS = 14, /**< I2S_M_WS*/
	SCU_PB5_PINMUX_I2S_S_WS = 15, /**< I2S_S_WS*/
	SCU_PB5_PINMUX_SW_DATA = 16, /**< SW_DATA*/
	SCU_PB5_PINMUX_TAM_OUT_4 = 17, /**< TAM_OUT_4*/
} SCU_PB5_PINMUX_E;


/**
 * \enum SCU_SPIS_DO_PINMUX_E
 * \brief SCU SPIS_DO PinMux
 */
typedef enum {
	SCU_SPIS_DO_PINMUX_NA = 0, /**< NA */
	SCU_SPIS_DO_PINMUX_SPI_S_DO = 1, /**< SPI_S_DO*/
	SCU_SPIS_DO_PINMUX_TDO = 3, /**< TDO*/
} SCU_SPIS_DO_PINMUX_E;

/**
 * \enum SCU_SPIS_DI_PINMUX_E
 * \brief SCU SPIS_DI PinMux
 */
typedef enum {
	SCU_SPIS_DI_PINMUX_NA = 0, /**< NA */
	SCU_SPIS_DI_PINMUX_SPI_S_DI = 1, /**< SPI_S_DI*/
	SCU_SPIS_DI_PINMUX_TDI = 3, /**< TDI*/
} SCU_SPIS_DI_PINMUX_E;

/**
 * \enum SCU_SPIS_SCLK_PINMUX_E
 * \brief SCU SPIS_SCLK PinMux
 */
typedef enum {
	SCU_SPIS_SCLK_PINMUX_NA = 0, /**< NA */
	SCU_SPIS_SCLK_PINMUX_SPI_S_SCLK = 1, /**< SPI_S_SCLK*/
} SCU_SPIS_SCLK_PINMUX_E;

/**
 * \enum SCU_SPIS_CS_PINMUX_E
 * \brief SCU SPIS CS PinMux
 */
typedef enum {
	SCU_SPIS_CS_PINMUX_NA = 0, /**< NA */
	SCU_SPIS_CS_PINMUX_SPI_S_CS = 1, /**< SPI_S_CS*/
	SCU_SPIS_CS_PINMUX_TRSTN = 3, /**< TRSTN*/
} SCU_SPIS_CS_PINMUX_E;

/**
 * \enum SCU_PAD_SWCLK_TCK_PINMUX_E
 * \brief SCU PAD_SWCLK_TCK PinMux
 */
typedef enum {
	SCU_PAD_SWCLK_TCK_PINMUX_NA = 0, /**< NA */
	SCU_PAD_SWCLK_TCK_PINMUX_SWCLK = 1, /**< SWCLK*/
	SCU_PAD_SWCLK_TCK_PINMUX_GPIO1 = 2, /**< GPIO1*/
} SCU_PAD_SWCLK_TCK_PINMUX_E;

/**
 * \enum SCU_PAD_SWDIO_TMS_PINMUX_E
 * \brief SCU PAD_SWDIO_TMS PinMux
 */
typedef enum {
	SCU_PAD_SWDIO_TMS_PINMUX_NA = 0, /**< NA */
	SCU_PAD_SWDIO_TMS_PINMUX_SWDIO = 1, /**< SWDIO*/
	SCU_PAD_SWDIO_TMS_PINMUX_GPIO2 = 2, /**< GPIO2*/
} SCU_PAD_SWDIO_TMS_PINMUX_E;

/**
 * \enum SCU_PAD_SRSTN_PINMUX_E
 * \brief SCU PAD_SRSTN PinMux
 */
typedef enum {
	SCU_PAD_SRSTN_PINMUX_NA = 0, /**< NA */
	SCU_PAD_SRSTN_PINMUX_SRSTN = 1, /**< SRSTN*/
	SCU_PAD_SRSTN_PINMUX_GPIO0 = 2, /**< GPIO0*/
} SCU_PAD_SRSTN_PINMUX_E;

/**
 * \enum SCU_SDIO_SDCLK_PINMUX_E
 * \brief SCU SDIO_SDCLK PinMux
 */
typedef enum {
	SCU_SDIO_SDCLK_PINMUX_NA = 0, /**< NA */
	SCU_SDIO_SDCLK_PINMUX_SDIO_SDCLK = 1, /**< SDIO_SDCLK*/
	SCU_SDIO_SDCLK_PINMUX_TRACECLK = 3, /**< TRACECLK*/
} SCU_SDIO_SDCLK_PINMUX_E;

/**
 * \enum SCU_SDIO_CMD_PINMUX_E
 * \brief SCU SDIO_CMD PinMux
 */
typedef enum {
	SCU_SDIO_CMD_PINMUX_NA = 0, /**< NA */
	SCU_SDIO_CMD_PINMUX_SDIO_CMD = 1, /**< SDIO_CMD*/
	SCU_SDIO_CMD_PINMUX_TRACECLK = 3, /**< TRACECLK*/
} SCU_SDIO_CMD_PINMUX_E;

/**
 * \enum SCU_SDIO_DATA0_PINMUX_E
 * \brief SCU SDIO_DATA0 PinMux
 */
typedef enum {
	SCU_SDIO_DATA0_PINMUX_NA = 0, /**< NA */
	SCU_SDIO_DATA0_PINMUX_SDIO_DATA0 = 1, /**< SDIO_DATA0*/
	SCU_SDIO_DATA0_PINMUX_TRACEDATA0 = 3, /**< TRACEDATA0*/
} SCU_SDIO_DATA0_PINMUX_E;

/**
 * \enum SCU_SDIO_DATA1_PINMUX_E
 * \brief SCU SDIO_DATA1 PinMux
 */
typedef enum {
	SCU_SDIO_DATA1_PINMUX_NA = 0, /**< NA */
	SCU_SDIO_DATA1_PINMUX_SDIO_DATA1 = 1, /**< SDIO_DATA1*/
	SCU_SDIO_DATA1_PINMUX_TRACEDATA1 = 3, /**< TRACEDATA1*/
} SCU_SDIO_DATA1_PINMUX_E;

/**
 * \enum SCU_SDIO_DATA2_PINMUX_E
 * \brief SCU SDIO_DATA2 PinMux
 */
typedef enum {
	SCU_SDIO_DATA2_PINMUX_NA = 0, /**< NA */
	SCU_SDIO_DATA2_PINMUX_SDIO_DATA2 = 1, /**< SDIO_DATA2*/
	SCU_SDIO_DATA2_PINMUX_TRACEDATA2 = 3, /**< TRACEDATA2*/
} SCU_SDIO_DATA2_PINMUX_E;

/**
 * \enum SCU_SDIO_DATA3_PINMUX_E
 * \brief SCU SDIO_DATA3 PinMux
 */
typedef enum {
	SCU_SDIO_DATA3_PINMUX_NA = 0, /**< NA */
	SCU_SDIO_DATA3_PINMUX_SDIO_DATA3 = 1, /**< SDIO_DATA3*/
	SCU_SDIO_DATA3_PINMUX_TRACEDATA3 = 3, /**< TRACEDATA3*/
} SCU_SDIO_DATA3_PINMUX_E;


/**
 * \enum SCU_PB6_PINMUX_E
 * \brief SCU PB6 PinMux
 */
typedef enum {
	SCU_PB6_PINMUX_NA = 0, /**< NA */
	SCU_PB6_PINMUX_GPIO0_1 = 1, /**< GPIO0*/
	SCU_PB6_PINMUX_GPIO0_2 = 2, /**< GPIO0*/
	SCU_PB6_PINMUX_SRSTN = 3, /**< SRSTN*/
	SCU_PB6_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB6_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB6_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB6_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB6_PINMUX_SPI_M_DI = 8, /**< SPI_M_DI*/
	SCU_PB6_PINMUX_SPI_S_DI = 9, /**< SPI_S_DI*/
	SCU_PB6_PINMUX_UART1_RX = 10, /**< UART1_RX*/
	SCU_PB6_PINMUX_PWM0 = 11, /**< PWM0*/
	SCU_PB6_PINMUX_SDIO_CMD = 12, /**< SDIO_CMD*/
	SCU_PB6_PINMUX_PDM_DATA3 = 13, /**< PDM_DATA3*/
	SCU_PB6_PINMUX_I2S_M_SCLK = 14, /**< I2S_M_SCLK*/
	SCU_PB6_PINMUX_I2S_S_SCLK = 15, /**< I2S_S_SCLK*/
	SCU_PB6_PINMUX_SW_CLK = 16, /**< SW_CLK*/
} SCU_PB6_PINMUX_E;

/**
 * \enum SCU_PB7_PINMUX_E
 * \brief SCU PB7 PinMux
 */
typedef enum {
	SCU_PB7_PINMUX_SWCLK_0 = 0, /**< SWCLK */
	SCU_PB7_PINMUX_GPIO1_1 = 1, /**< GPIO1*/
	SCU_PB7_PINMUX_GPIO1_2 = 2, /**< GPIO1*/
	SCU_PB7_PINMUX_SWCLK_3 = 3, /**< SWCLK*/
	SCU_PB7_PINMUX_I2C_S1_SCL = 4, /**< I2C_S1_SCL*/
	SCU_PB7_PINMUX_I3C_S1_SCL = 5, /**< I3C_S1_SCL*/
	SCU_PB7_PINMUX_I2C_M_SCL = 6, /**< I2C_M_SCL*/
	SCU_PB7_PINMUX_I3C_M_SCL = 7, /**< I3C_M_SCL*/
	SCU_PB7_PINMUX_SPI_M_DO = 8, /**< SPI_M_DO*/
	SCU_PB7_PINMUX_SPI_S_DO = 9, /**< SPI_S_DO*/
	SCU_PB7_PINMUX_UART1_TX = 10, /**< UART1_TX*/
	SCU_PB7_PINMUX_PWM1 = 11, /**< PWM1*/
	SCU_PB7_PINMUX_SDIO_SDCLK = 12, /**< SDIO_SDCLK*/
	SCU_PB7_PINMUX_PDM_DATA3 = 13, /**< PDM_DATA3*/
	SCU_PB7_PINMUX_I2S_M_WS = 14, /**< I2S_M_WS*/
	SCU_PB7_PINMUX_I2S_S_WS = 15, /**< I2S_S_WS*/
	SCU_PB7_PINMUX_SW_DATA = 16, /**< SW_DATA*/
} SCU_PB7_PINMUX_E;

/**
 * \enum SCU_PB8_PINMUX_E
 * \brief SCU PB8 PinMux
 */
typedef enum {
	SCU_PB8_PINMUX_SWDIO_0 = 0, /**< SWDIO */
	SCU_PB8_PINMUX_GPIO2_1 = 1, /**< GPIO2*/
	SCU_PB8_PINMUX_GPIO2_2 = 2, /**< GPIO2*/
	SCU_PB8_PINMUX_SWDIO_3 = 3, /**< SWDIO*/
	SCU_PB8_PINMUX_I2C_S1_SDA = 4, /**< I2C_S1_SDA*/
	SCU_PB8_PINMUX_I3C_S1_SDA = 5, /**< I3C_S1_SDA*/
	SCU_PB8_PINMUX_I2C_M_SDA = 6, /**< I2C_M_SDA*/
	SCU_PB8_PINMUX_I3C_M_SDA = 7, /**< I3C_M_SDA*/
	SCU_PB8_PINMUX_SPI_M_CLK = 8, /**< SPI_M_CLK*/
	SCU_PB8_PINMUX_SPI_S_CLK = 9, /**< SPI_S_CLK*/
	SCU_PB8_PINMUX_UART1_CTS = 10, /**< UART1_CTS*/
	SCU_PB8_PINMUX_PWM2 = 11, /**< PWM2*/
	SCU_PB8_PINMUX_SDIO_DAT0 = 12, /**< SDIO_DAT0*/
	SCU_PB8_PINMUX_PDM_DATA3 = 13, /**< PDM_DATA3*/
	SCU_PB8_PINMUX_I2S_M_SDO = 14, /**< I2S_M_SDO*/
	SCU_PB8_PINMUX_I2S_S_SDO = 15, /**< I2S_S_SDO*/
	SCU_PB8_PINMUX_SW_CLK = 16, /**< SW_CLK*/
} SCU_PB8_PINMUX_E;

/**
 * \enum SCU_OSPI_PINMUX_E
 * \brief SCU OSPI PinMux
 */
typedef enum {
	SCU_OSPI_PINMUX_NA = 0, /**< NA */
	SCU_OSPI_PINMUX_OSPI = 1 /**< OSPI*/
} SCU_OSPI_PINMUX_E;

/**
 * \enum SCU_SEN_XSLEEP_PINMUX_E
 * \brief SCU SEN_XSLEEP PinMux
 */
typedef enum {
	SCU_SEN_XSLEEP_PINMUX_SEN_XSLEEP_0 = 0, /**< SEN_XSLEEP */
	SCU_SEN_XSLEEP_PINMUX_SENGPIO = 1, /**< SENGPIO AON GPIO2*/
	SCU_SEN_XSLEEP_PINMUX_GPIO28 = 2, /**< GPIO28*/
	SCU_SEN_XSLEEP_PINMUX_TRIG = 3 /**< TRIG*/
} SCU_SEN_XSLEEP_PINMUX_E;

/**
 * \enum SCU_SEN_PCLK_PINMUX_E
 * \brief SCU PCLK PinMux
 */
typedef enum {
	SCU_SEN_PCLK_PINMUX_SEN_PCLK_0 = 0, /**< SEN_PCLK */
#if(IC_VERSION >= 30)
	SCU_SEN_PCLK_PINMUX_SEN_PCLK_INV = 1, /**< SEN_PCLK INV*/
#else
	SCU_SEN_PCLK_PINMUX_SEN_PCLK_1 = 1, /**< SEN_PCLK*/
#endif
	SCU_SEN_PCLK_PINMUX_GPIO17 = 2, /**< GPIO17*/
} SCU_SEN_PCLK_PINMUX_E;

/**
 * \enum SCU_SEN_D0_PINMUX_E
 * \brief SCU D0 PinMux
 */
typedef enum {
	SCU_SEN_D0_PINMUX_SEN_D0_0 = 0, /**< D0 */
	SCU_SEN_D0_PINMUX_SEN_D0_1 = 1, /**< D0*/
	SCU_SEN_D0_PINMUX_GPIO18 = 2, /**< GPIO18*/
} SCU_SEN_D0_PINMUX_E;

/**
 * \enum SCU_SEN_D1_PINMUX_E
 * \brief SCU D1 PinMux
 */
typedef enum {
	SCU_SEN_D1_PINMUX_SEN_D1_0 = 0, /**< D1 */
	SCU_SEN_D1_PINMUX_SEN_D1_1 = 1, /**< D1*/
	SCU_SEN_D1_PINMUX_GPIO19 = 2, /**< GPIO19*/
} SCU_SEN_D1_PINMUX_E;

/**
 * \enum SCU_SEN_D2_PINMUX_E
 * \brief SCU D2 PinMux
 */
typedef enum {
	SCU_SEN_D2_PINMUX_SEN_D2_0 = 0, /**< D2 */
	SCU_SEN_D2_PINMUX_SEN_D2_1 = 1, /**< D2*/
	SCU_SEN_D2_PINMUX_GPIO20 = 2, /**< GPIO20*/
} SCU_SEN_D2_PINMUX_E;

/**
 * \enum SCU_SEN_D3_PINMUX_E
 * \brief SCU D3 PinMux
 */
typedef enum {
	SCU_SEN_D3_PINMUX_SEN_D3_0 = 0, /**< D3 */
	SCU_SEN_D3_PINMUX_SEN_D3_1 = 1, /**< D3*/
	SCU_SEN_D3_PINMUX_GPIO21 = 2, /**< GPIO21*/
} SCU_SEN_D3_PINMUX_E;

/**
 * \enum SCU_SEN_D4_PINMUX_E
 * \brief SCU D4 PinMux
 */
typedef enum {
	SCU_SEN_D4_PINMUX_SEN_D4_0 = 0, /**< D4 */
	SCU_SEN_D4_PINMUX_SEN_D4_1 = 1, /**< D4*/
	SCU_SEN_D4_PINMUX_GPIO22 = 2, /**< GPIO22*/
} SCU_SEN_D4_PINMUX_E;

/**
 * \enum SCU_SEN_MCLK_PINMUX_E
 * \brief SCU MCLK PinMux
 */
typedef enum {
	SCU_SEN_MCLK_PINMUX_SEN_MCLK_0 = 0, /**< MCLK */
	SCU_SEN_MCLK_PINMUX_SENGPIO = 1, /**< SENGPIO AON GPIO2*/
	SCU_SEN_MCLK_PINMUX_GPIO29 = 2, /**< GPIO29*/
	SCU_SEN_MCLK_PINMUX_XSLEEP = 3, /**< XSLEEP*/
} SCU_SEN_MCLK_PINMUX_E;

/**
 * \enum SCU_SEN_D5_PINMUX_E
 * \brief SCU D5 PinMux
 */
typedef enum {
	SCU_SEN_D5_PINMUX_SEN_D5_0 = 0, /**< D5 */
	SCU_SEN_D5_PINMUX_SEN_D5_1 = 1, /**< D5*/
	SCU_SEN_D5_PINMUX_GPIO23 = 2, /**< GPIO23*/
} SCU_SEN_D5_PINMUX_E;

/**
 * \enum SCU_SEN_D6_PINMUX_E
 * \brief SCU D6 PinMux
 */
typedef enum {
	SCU_SEN_D6_PINMUX_SEN_D6_0 = 0, /**< D6 */
	SCU_SEN_D6_PINMUX_SEN_D6_1 = 1, /**< D6*/
	SCU_SEN_D6_PINMUX_GPIO24 = 2, /**< GPIO24*/
} SCU_SEN_D6_PINMUX_E;

/**
 * \enum SCU_SEN_D7_PINMUX_E
 * \brief SCU D7 PinMux
 */
typedef enum {
	SCU_SEN_D7_PINMUX_SEN_D7_0 = 0, /**< D7 */
	SCU_SEN_D7_PINMUX_SEN_D7_1 = 1, /**< D7*/
	SCU_SEN_D7_PINMUX_GPIO25 = 2, /**< GPIO25*/
} SCU_SEN_D7_PINMUX_E;

/**
 * \enum SCU_SEN_FVALID_PINMUX_E
 * \brief SCU FVALID PinMux
 */
typedef enum {
	SCU_SEN_FVALID_PINMUX_SEN_FVALID_0 = 0, /**< FVALID */
	SCU_SEN_FVALID_PINMUX_SEN_FVALID_1 = 1, /**< FVALID*/
	SCU_SEN_FVALID_PINMUX_GPIO26 = 2, /**< GPIO26*/
} SCU_SEN_FVALID_PINMUX_E;

/**
 * \enum SCU_SEN_LVALID_PINMUX_E
 * \brief SCU LVALID PinMux
 */
typedef enum {
	SCU_SEN_LVALID_PINMUX_SEN_LVALID_0 = 0, /**< LVALID */
	SCU_SEN_LVALID_PINMUX_SEN_LVALID_1 = 1, /**< LVALID*/
	SCU_SEN_LVALID_PINMUX_GPIO27 = 2, /**< GPIO27*/
} SCU_SEN_LVALID_PINMUX_E;


/**
 * \enum SCU_I2CSLV_SHARE_E
 * \brief SCU I2C Slave Share
 */
typedef enum {
	SCU_I2CSLV_SHARE_DISABLE = 0, /**< Disable I2C Slave0 and Slave1 share same pin */
	SCU_I2CSLV_SHARE_ENDISABLE = 1, /**Both I2C slave share at same pin (PAD_I2C2AHB_SCL and PAD_I2C2AHB_SDA)*/
} SCU_I2CSLV_SHARE_E;

/**
 * \enum SCU_I3CSLV_SHARE_E
 * \brief SCU I3C Slave Share
 */
typedef enum {
	SCU_I3CSLV_SHARE_DISABLE = 0, /**< Disable I3C Slave0 and Slave1 share same pin */
	SCU_I3CSLV_SHARE_ENDISABLE = 1, /**Both I3C slave share at same pin (PAD_I2C2AHB_SCL and PAD_I2C2AHB_SDA)*/
} SCU_I3CSLV_SHARE_E;



/**
 * \enum SCU_SWRESET_E
 * \brief SCU SWRESET
 */
typedef enum {
	SCU_SWRESET_RESET = 0, /**< Reset */
	SCU_SWRESET_NORMAL = 1, /**Normal*/
} SCU_SWRESET_E;


/**
 * \enum SCU_AUDIO_HWTYPE_E
 * \brief SCU Audio Harware Type
 */
typedef enum {
	SCU_AUDIO_HWTYPE_HXPDM = 0, /**< Himax PDM */
	SCU_AUDIO_HWTYPE_I2S_HOST, /**< I2S Host */
	SCU_AUDIO_HWTYPE_I2S_SLAVE, /**< I2S Slave */
} SCU_AUDIO_HWTYPE_E;


/**
 * \enum SCU_AUDIO_CAPTURE_CHANNEL_E
 * \brief SCU Audio Type
 */
typedef enum {
	SCU_AUDIO_CAPTURE_CHANNEL_LEFT_ONLY = 0, /**< Capture Right Only */
	SCU_AUDIO_CAPTURE_CHANNEL_RIGHT_ONLY, /**< Capture Right Only*/
	SCU_AUDIO_CAPTURE_CHANNEL_STEREO, /**< Capture Stereo */
} SCU_AUDIO_CAPTURE_CHANNEL_E;

#ifndef BOOT_USED
#if(IC_VERSION >= 30)
/**
 * \enum SCU_REG_SEN_SEL_SRC_E
 * \brief SCU REG_SEN_SEL SRC
 */
typedef enum {
	SCU_REG_SEN_SEL_SRC_REG = 0, /**< From register (REG_SEL_SEL) */
	SCU_REG_SEN_SEL_SRC_EXT_ISP, /**< From External ISP_SEL (Pin PB11 input)*/
} SCU_REG_SEN_SEL_SRC_E;

/**
 * \enum SCU_REG_SEN_SEL_E
 * \brief SCU REG_SEN_SEL MCLK_OUT and XSHUTDOWN_OUT source selection register
 */
typedef enum {
	SCU_REG_SEN_SEL_INTERNAL = 0, /**< From Internal Input */
	SCU_REG_SEN_SEL_EXTERNAL, /**< From External Input (Pin PB9 and PB10)*/
} SCU_REG_SEN_SEL_E;
#endif
#endif

/**
 * \enum VAD_PCM_DATA_SEL_E
 * \brief VAD PDM Data Selection
 */
typedef enum {
	SCU_VAD_PCM_DATA_R_CH = 0,
	SCU_VAD_PCM_DATA_L_CH = 1,
} SCU_VAD_PCM_DATA_SEL_E;

/**
 * \enum SCU_VMUTE_CTRL_TIMING_SRC_E
 * \brief VMUTE CTRL TIMING SRC Selection
 */
typedef enum {
	SCU_VMUTE_CTRL_TIMING_SRC_RX = 0,
	SCU_VMUTE_CTRL_TIMING_SRC_VMUTE = 1,
} SCU_VMUTE_CTRL_TIMING_SRC_E;

/**
 * \enum SCU_VMUTE_CTRL_TXPHY_PWR_E
 * \brief VMUTE CTRL TXPHY Power Gating
 */
typedef enum {
	SCU_VMUTE_CTRL_TXPHY_PWR_DISABLE = 0,
	SCU_VMUTE_CTRL_TXPHY_PWR_ENABLE = 1,
} SCU_VMUTE_CTRL_TXPHY_PWR_E;

/**
 * \enum SCU_VMUTE_CTRL_SRC_E
 * \brief VMUTE CTRL Source
 */
typedef enum {
	SCU_VMUTE_CTRL_SRC_PAD = 0,
	SCU_VMUTE_CTRL_SRC_SW = 1,
} SCU_VMUTE_CTRL_SRC_E;

/**
 * \enum SCU_VMUTE_CTRL_SW_E
 * \brief Software set VMUTE
 */
typedef enum {
	SCU_VMUTE_CTRL_SW_DISABLE = 0,
	SCU_VMUTE_CTRL_SW_ENABLE = 1,
} SCU_VMUTE_CTRL_SW_E;

/**
 * \enum SCU_COREVOLT_CLKTREE_E
 * \brief Clock tree run in Core Voltage 0.9V or 0.8V
 */
typedef enum {
	SCU_COREVOLT_CLKTREE_0P9V = 0,
	SCU_COREVOLT_CLKTREE_0P8V = 1,
} SCU_COREVOLT_CLKTREE_E;

/**
 * \enum SCU_COREVOLT_SRAM_E
 * \brief SRAM run in Core Voltage 0.9V or 0.8V
 */
typedef enum {
	SCU_COREVOLT_SRAM_0P9V = 0,
	SCU_COREVOLT_SRAM_0P8V = 1,
} SCU_COREVOLT_SRAM_E;

/**
 * \enum SCU_PA01_PULLENABLE_E
 * \brief PA0 and PA1 Pull enable/disable
 */
typedef enum {
	SCU_PA01_PULL_DISABLE = 0,
	SCU_PA01_PULL_ENABLE = 1,
} SCU_PA01_PULLENABLE_E;

/**
 * \enum SCU_PAD_PULL_EN_E
 * \brief PAD Pull enable/disable
 */
typedef enum {
    SCU_PAD_PULL_DIS = 0,
    SCU_PAD_PULL_EN  = 1,
} SCU_PAD_PULL_EN_E;

/**
 * \enum SCU_PAD_PULL_SEL_E
 * \brief PAD Pull up/down
 */
typedef enum {
	SCU_PAD_PULL_DOWN = 0,
	SCU_PAD_PULL_UP = 1,
} SCU_PAD_PULL_SEL_E;

/**
 * \enum SCU_MEM_ATTR
 * \brief MEM ARRT
 */
typedef enum {
	SCU_MEM_ATTR_SECURE = 0,
	SCU_MEM_ATTR_NONSECURE = 1,
} SCU_MEM_ATTR_E;

/**
 * \enum SCU_MIPI_CTRL
 * \brief MIPI Control by CPU or PMU
 */
typedef enum {
	SCU_MIPI_CTRL_CPU = 0,
	SCU_MIPI_CTRL_PMU = 1,
} SCU_MIPI_CTRL_E;


/**
 * \enum SCU_TXPHY_RCLK_SRC_SEL_E
 * \brief TX RX Clock Source selection
 */
typedef enum {
	SCU_TXPHY_RCLK_SRC_SEL_NORMAL = 0, /**<  Normal Mode*/
	SCU_TXPHY_RCLK_SRC_SEL_TEST_RC24M =2, /**<  Test Mode, from RC_24M1M_CLK*/
	SCU_TXPHY_RCLK_SRC_SEL_TEST_XTAL24M = 3, /**<  Test Mode, from XTAL_24M_CLK*/
} SCU_TXPHY_RCLK_SRC_SEL_E;
/** @} */

/**
 * \struct SCU_PAD_PULL_CFG_T
 * \brief PAD Pull configuration
 */
typedef struct {
    SCU_PAD_PULL_EN_E       pull_en;
    SCU_PAD_PULL_SEL_E      pull_sel;
} SCU_PAD_PULL_CFG_T;


/**
 * \struct SCU_PAD_PULL_LIST_T
 * \brief List PAD Pull configuration
 */
typedef struct {
	// AON
    SCU_PAD_PULL_CFG_T       pa0;			// PAD_AON_GPIO_0
	SCU_PAD_PULL_CFG_T       pa1;			// PAD_VMUTE
	// SB
	SCU_PAD_PULL_CFG_T       pa2;			// PAD_I2C2AHB_SCL
	SCU_PAD_PULL_CFG_T       pa3;			// PAD_I2C2AHB_SDA
	// LSC
    SCU_PAD_PULL_CFG_T       pb0;			// PAD_UART0_RX
	SCU_PAD_PULL_CFG_T       pb1;			// PAD_UART0_TX
	SCU_PAD_PULL_CFG_T       pb2;			// PAD_SSPIM_DO
	SCU_PAD_PULL_CFG_T       pb3;			// PAD_SSPIM_DI
    SCU_PAD_PULL_CFG_T       pb4;			// PAD_SSPIM_SCLK
	SCU_PAD_PULL_CFG_T       pb5;			// PAD_SSPIM_CS
	SCU_PAD_PULL_CFG_T       pb6;			// PAD_GPIO_0
	SCU_PAD_PULL_CFG_T       pb7;			// PAD_GPIO_1
    SCU_PAD_PULL_CFG_T       pb8;			// PAD_GPIO_2
	SCU_PAD_PULL_CFG_T       pb9;			// PAD_PDM_CLK
	SCU_PAD_PULL_CFG_T       pb10;			// PAD_PDM_DATA_IN_0
	SCU_PAD_PULL_CFG_T       pb11;			// PAD_PDM_DATA_IN_1
	SCU_PAD_PULL_CFG_T       pc3;			// PAD_PDM_DATA_IN_2
	SCU_PAD_PULL_CFG_T       pc4;			// PAD_PDM_DATA_IN_3
	SCU_PAD_PULL_CFG_T       uart1_rx;
	SCU_PAD_PULL_CFG_T       uart1_tx;
	SCU_PAD_PULL_CFG_T       uart1_cts;
	SCU_PAD_PULL_CFG_T       uart1_rts;
	SCU_PAD_PULL_CFG_T       sw_data;
	SCU_PAD_PULL_CFG_T       sw_clk;
	SCU_PAD_PULL_CFG_T       i2cm_sda;		// PAD_I2C_MST_SDA (LQFP128)
	SCU_PAD_PULL_CFG_T       i2cm_scl;		// PAD_I2C_MST_SCL (LQFP128)
	SCU_PAD_PULL_CFG_T       spis_do;
	SCU_PAD_PULL_CFG_T       spis_di;
	SCU_PAD_PULL_CFG_T       spis_sclk;
	SCU_PAD_PULL_CFG_T       spis_cs;		
	SCU_PAD_PULL_CFG_T       sdio_clk;
	SCU_PAD_PULL_CFG_T       sdio_cmd;
	SCU_PAD_PULL_CFG_T       sdio_dat0;
	SCU_PAD_PULL_CFG_T       sdio_dat1;
	SCU_PAD_PULL_CFG_T       sdio_dat2;
	SCU_PAD_PULL_CFG_T       sdio_dat3;
} SCU_PAD_PULL_LIST_T;
/** @} */

/**
 * \defgroup	SCU_DRV_MACRO	SCU Driver MACRO
 * \ingroup	SCU_DRV
 * \brief	contains definitions of SCU Driver MACRO.
 * @{
 */
#ifdef SUPPORT_CKGEN
/**
 * \struct SCU_HSC_CLK_CFG_T
 * \brief HSC CLK configuration Header
 */
typedef struct {
	SCU_HSCCLKSRC_E hscclksrc; /*!< HSC CLK Src */
	SCU_HSCCLKDIV_E hscclkdiv; /*!< HSC CLK div*/
} SCU_HSC_CLK_CFG_T;

#ifndef BOOTROM
/**
 * \struct SCU_HSC_CLK_MIPI_CFG_T
 * \brief HSC MIPI CLK configuration Header
 */
typedef struct {
	SCU_HSCMIPICLKSRC_E hscmipiclksrc; /*!< HSC MIPI CLK Src */
	uint8_t hscmipiclkdiv; /*!< HSC MIPI CLK div (0 ~ 63)*/
} SCU_HSC_CLK_MIPI_CFG_T;

/**
 * \struct SCU_HSC_CLK_PPITX_CFG_T
 * \brief HSC PPI TX CLK configuration Header
 */
typedef struct {
	SCU_HSCPPITXCLKSRC_E hscppitxclksrc; /*!< HSC PPI TX CLK Src */
	uint8_t hscppitxclkdiv; /*!< HSC PPI TX CLK div (0 ~ 63)*/
} SCU_HSC_CLK_PPITX_CFG_T;
#endif
/**
 * \struct SCU_LSC_CLK_CFG_T
 * \brief LSC CLK configuration Header
 */
typedef struct {
	SCU_LSCCLKSRC_E lscclksrc; /*!< LSC CLK Src */
	SCU_LSCCLKDIV_E lscclkdiv; /*!< LSC CLK div*/
} SCU_LSC_CLK_CFG_T;


/**
 * \struct SCU_LSC_SENMCLK_CFG_T
 * \brief LSC Sensor MCLK configuration Header
 */
typedef struct {
	SCU_LSCSCCLKSRC_E lscscmclksrc; /*!< LSC SC and MCLK Src */
	SCU_LSCCLKDIV_E lscmclkdiv; /*!< LSC MCLK div*/
} SCU_LSC_SCMCLK_CFG_T;


/**
 * \struct SCU_SB_CLK_CFG_T
 * \brief SB CLK configuration Header
 */
typedef struct {
	SCU_SBCLKSRC_E sbclksrc; /*!< SB CLK Src */
	SCU_SBCLKDIV_E sbclkdiv; /*!< SB CLK div*/
} SCU_SB_CLK_CFG_T;

#ifndef BOOTROM
/**
 * \struct SCU_PDHSC_DPCLK_CFG_T
 * \brief PD_HSC Image CLK configuration Header
 */
typedef struct {
	SCU_HSCDPCLKSRC_E dpclk;/*!< HSC DP CLK*/
	SCU_HSCRXCLKSRC_E rxclk;/*!< HSC RX CLK*/
	SCU_HSC_CLK_MIPI_CFG_T mipiclk;/*!< HSC MIPI CLK*/
	SCU_HSC_CLK_PPITX_CFG_T ppitxclk;/*!< HSC PPI TX CLK*/
} SCU_PDHSC_DPCLK_CFG_T;
#endif

/**
 * \struct SCU_PDHSC_HSCCLK_CFG_T
 * \brief PD_HSC HSC_CLK related configuration Header
 */
typedef struct {
	SCU_HSC_CLK_CFG_T hscclk; /*!< HSC CLK Src */
	SCU_HSCD12CLKSRC_E hscd12clksrc; /*!< HSC D1 2 CLK Src */
	SCU_HSCI3CHCLKDIV_E i3chcdiv;/*!< HSC I3CH DIV CLK*/
	SCU_HSCSDIOCLKDIV_E sdiodiv;/*!< HSC SDIO DIV*/
} SCU_PDHSC_HSCCLK_CFG_T;


/**
 * \struct SCU_PDHSC_CLK_CFG_T
 * \brief PD_HSC CLK configuration Header
 */
typedef struct {
	SCU_PDHSC_HSCCLK_CFG_T hscclk_related; /*!< HSC CLK Src */
	SCU_QSPICLKSRC_E qspiclksrc;/*!< QSPI CLK Src */
	uint8_t qspiclkdiv;/*!< QSPI CLK div */
#ifndef BOOTROM
	SCU_OSPICLKSRC_E ospiclksrc;/*!< OSPI CLK Src */
	uint8_t ospiclkdiv;/*!< OSPI CLK div */
	SCU_HSC32KCLKSRC_E hsc32k; /*!< HSC 32K CLK*/
	SCU_HSCPUFCLKSRC_E pufclk;/*!< HSC PUF CLK*/
	SCU_PDHSC_DPCLK_CFG_T imageclk;/*!< HSC Image CLK*/
#endif
} SCU_PDHSC_CLK_CFG_T;



/**
 * \struct SCU_PDHSC_DPCLKEN_CFG_T
 * \brief PD_HSC DP CLK Enable configuration Header
 */
typedef struct {
	uint8_t xdma_w1_clk_en;/*!< XDMA W1 hclk & xdma_w1_pclk enable */
	uint8_t xdma_w2_clk_en;/*!< XDMA W2 hclk & xdma_w2_pclk enable */
	uint8_t xdma_w3_clk_en;/*!< XDMA W3 hclk & xdma_w3_pclk enable */
	uint8_t xdma_r_clk_en;/*!< XDMA R hclk & xdma_r_pclk enable */
	uint8_t scclk_clk_en;/*!< PD HSC SC clk enable */
	uint8_t inp_clk_en;/*!< PD HSC inp_cclk, inp_oclk clk enable */
	uint8_t dp_clk_en;/*!< PD HSC dp_pclk clk enable */
	uint8_t dp_2x2_clk_en;/*!< PD HSC dp_2x2_pclk clk enable */
	uint8_t dp_5x5_clk_en;/*!< PD HSC dp_5x5_pclk clk enable */
	uint8_t dp_cdm_clk_en;/*!< PD HSC dp_cdm_pclk clk enable */
	uint8_t dp_jpeg_clk_en;/*!< PD HSC dp_jpeg_pclk clk enable */
	uint8_t dp_tpg_clk_en;/*!< PD HSC dp_tpg_pclk clk enable */
	uint8_t dp_edm_clk_en;/*!< PD HSC dp_edm_pclk & dp_crc_pclk clk enable */
	uint8_t dp_rgb2yuv_pclk_en;/*!< PD HSC dp_rgb2yuv_pclk clk enable */
	uint8_t dp_csc_pclk_en;/*!< PD HSC dp_csc_pclk clk enable */
	uint8_t mipirx_clk_en;/*!< PD HSC mipirx_pclk clk enable */
	uint8_t mipitx_clk_en;/*!< PD HSC mipitx_pclk clk enable */
} SCU_PDHSC_DPCLKEN_CFG_T;

/**
 * \struct SCU_PDHSC_CLKEN_CFG_T
 * \brief PD_HSC CLK Enable configuration Header
 */
typedef struct {
	uint8_t cm55m_clk_en;/*!< CM55M clk enable */
	uint8_t u55_clk_en;/*!< U55 clk enable */
	uint8_t axi_clk_en;/*!< AXI clk enable */
	uint8_t ahb0_clk_en;/*!< AHB0 clk enable */
	uint8_t ahb5_clk_en;/*!< AHB5 clk enable */
	uint8_t ahb1_clk_en;/*!< AHB1 clk enable */
	uint8_t apb2_clk_en;/*!< APB2 clk enable */
	uint8_t rom_clk_en;/*!< ROM clk enable */
	uint8_t sram0_clk_en;/*!< SRAM0 clk enable */
	uint8_t sram1_clk_en;/*!< SRAM1 clk enable */
	uint8_t i3c_hc_clk_en;/*!< I3C HC clk enable */
	uint8_t puf_clk_en;/*!< puf clk enable */
	uint8_t dma0_clk_en;/*!< DMA 0 clk enable */
	uint8_t dma1_clk_en;/*!< DMA 1 clk enable */
	uint8_t sdio_clk_en;/*!< SDIO clk enable */
	uint8_t i2c2ahb_flash_w_clk_en;/*!< i2c2ahb flash write clk enable */
	uint8_t qspi_en;/*!< QSPI clock enable */
	uint8_t ospi_en;/*!< OSPI clock enable */
	uint8_t spi2ahb_en;/*!< SPI2AHB clock enable */
	SCU_PDHSC_DPCLKEN_CFG_T imageclk_en;/*!< image clk enable */
} SCU_PDHSC_CLKEN_CFG_T;

/**
 * \struct SCU_PDLSC_DPCLK_CFG_T
 * \brief PD_LSC DP CLK configuration Header
 */
typedef struct {
	SCU_LSC_SCMCLK_CFG_T scsenmclk;/*!< LSC Sensor MCLK and SC CLK*/
} SCU_PDLSC_DPCLK_CFG_T;

/**
 * \struct SCU_PDLSC_ADCCKIN_CFG_T
 * \brief PD_LSC ADCCKIN CLK configuration Header
 */
typedef struct {
	SCU_LSCADCCKINSRC_E adcckin_src; /*!< LSC ADCCK IN CLK*/
	SCU_LSCADCCKINDIV_E adcckindiv;/*!< LSC ADCCK CLK DIV*/
} SCU_PDLSC_ADCCKIN_CFG_T;

/**
 * \struct SCU_PDLSC_LSCREF_CFG_T
 * \brief PD_LSC LSC Ref CLK configuration Header
 */
typedef struct {
	SCU_LSCREFCLKSRC_E lscref; /*!< LSC Ref CLK*/
	uint8_t i2s_host_sclk_div; /*!< LSC i2s_host_sclk DIV(0~127) CLK*/
	uint8_t pdmclk_div;/*!< LSC PDM DIV(0~127) CLK*/
	uint8_t uart_div;/*!< LSC UART DIV(0~127) CLK*/
	uint8_t i3cslv_div;/*!< LSC I3C Slave DIV(0~127) CLK*/
	uint8_t pwm_div;/*!< LSC PWM DIV(0~127) CLK*/
	uint8_t ro_pd_div;/*!< LSC RO_PD_CK_IN DIV(0~127) CLK*/
	uint8_t i2c_div;/*!< LSC I2C DIV(0~127) CLK*/
} SCU_PDLSC_LSCREF_CFG_T;
/**
 * \struct SCU_PDLSC_CLK_CFG_T
 * \brief PD_LSC CLK configuration Header
 */
typedef struct {
	SCU_LSC_CLK_CFG_T lscclk; /*!< LSC CLK Src */
	SCU_PDLSC_LSCREF_CFG_T lscref_related; /*!< LSC Ref CLK*/
#ifndef BOOTROM
	SCU_LSC32KCLKSRC_E lsc32k; /*!< LSC 32K CLK*/
	SCU_PDLSC_DPCLK_CFG_T image_clk;/*!< LSC CLK Image */
	SCU_LSCVADDCLKSRC_E vad_d_clk;/*!< LSC VAD D CLK SRC*/
	SCU_PDLSC_ADCCKIN_CFG_T adcckin;/*!< LSC ADCCK IN CLK Image */
	SCU_LSCCKMONSRC_E ckmonsrc_clk;/*!< LSC CK MON Source */
	SCU_LSCSSPIMSRC_E sspimsrc_clk;/*!< LSC SSPI M Source */
	uint8_t sspim_div;/*!< LSC SSPI M Div */
#endif
	SCU_LSCSSPISSRC_E sspissrc_clk;/*!< LSC SSPI S Source */
	uint8_t sspis_div;/*!< LSC SSPI S Div */
#ifndef BOOTROM
#if(IC_VERSION >= 30)
	SCU_LSCPDMCLKSRC_E pdmsrc_clk;/*!< LSC PDM Clock Source */
#endif
#endif
} SCU_PDLSC_CLK_CFG_T;


/**
 * \struct SCU_PDLSC_DPCLKEN_CFG_T
 * \brief PD_LSC DP CLK Enable configuration Header
 */
typedef struct {
	uint8_t sc_clk_lsc_en;/*!< sc_clk For PD_LSC clk enable */
} SCU_PDLSC_DPCLKEN_CFG_T;

/**
 * \struct SCU_PDLSC_CLKEN_CFG_T
 * \brief PD_LSC CLK Enable configuration Header
 */
typedef struct {
	uint8_t cm55s_clk_en;/*!< CM55S clk enable */
	uint8_t ahb_m_hclk_en;/*!< AHB_M_HCLK clk enable */
	uint8_t ahb_2_hclk_en;/*!< AHB_2_HCLK clk enable */
	uint8_t ahb_3_hclk_en;/*!< AHB_3_HCLK clk enable */
	uint8_t apb_0_hclk_en;/*!< APB_0_HCLK clk enable */
	uint8_t sram2_clk_en;/*!< SRAM2 clk enable */
	uint8_t dma2_clk_en;/*!< DMA2 clk enable */
	uint8_t dma3_clk_en;/*!< DMA3 clk enable */
	uint8_t i2s_host_sclk_en;/*!< i2s_host_sclk clk enable */
	uint8_t pdm_clk_en;/*!< pdm_clk clk enable */
	uint8_t uart0_clk_en;/*!< uart0_sclk clk enable */
	uint8_t uart1_clk_en;/*!< uart1_sclk clk enable */
	uint8_t uart2_clk_en;/*!< uart2_sclk clk enable */
	uint8_t i3c_slv0_sys_clk_en;/*!< i3c_slv0_sys_clk clk enable */
	uint8_t i3c_slv1_sys_clk_en;/*!< i3c_slv1_sys_clk clk enable */
	uint8_t pwm012_clk_en;/*!< pwm012_clk clk enable */
	uint8_t i2s_slv_sclk_en;/*!< i2s_slv_sclk clk enable */
	uint8_t ro_pd_clk_en;/*!< RO_PD_CK_IN clk enable */
	uint8_t i2c_slv0_ic_clk_en;/*!< i2c_slv0_ic_clk clk enable */
	uint8_t i2c_slv1_ic_clk_en;/*!< i2c_slv1_ic_clk clk enable */
	uint8_t i2c_mst_ic_clk_en;/*!< i2c_mst_ic_clk clk enable */
	uint8_t i2c_mst_sen_ic_clk_en;/*!< i2c_mst_sen_ic_clk clk enable */
	uint8_t sw_clk_en;/*!< sw_clk clk enable */
	uint8_t vad_d_clk_en;/*!< VAD d clk enable */
	uint8_t adcck_en;/*!< adcck clk enable */
	uint8_t gpio_en;/*!< gpio clk enable */
	uint8_t sspim_en;/*!< SSPI Master clk enable */
	uint8_t sspis_en;/*!< SSPI Slave clk enable */
	uint8_t ckmon_en;/*!< clock_monitor clock enable */
	SCU_PDLSC_DPCLKEN_CFG_T imageclk_en;/*!< image clk enable */
} SCU_PDLSC_CLKEN_CFG_T;


/**
 * \struct SCU_PDSB_CLK_CFG_T
 * \brief PD_SB CLK configuration Header
 */
typedef struct {
	SCU_SB_CLK_CFG_T sbclk; /*!< SB CLK Src */
	SCU_SB_32KSRC_E sb32ksrc; /*!< SB 32K Source*/
	SCU_SB_HMXI2CMSRC_E himaxi2cm; /*!< himaxi2cm CLK*/
	SCU_SB_APB1AHB4_CLK_SEL_E sb_psdc_apb1ahb4clk;/*!< PS_DC APB1 and AHB4 CLK*/
	SCU_SB_APB1AHB4_CLK_SEL_E sb_psnondc_apb1ahb4clk;/*!<PS_NonDC AHB4 HCLK*/
} SCU_PDSB_CLK_CFG_T;



/**
 * \struct SCU_PDSB_CLKEN_CFG_T
 * \brief PD_SB CLK Enable configuration Header
 */
typedef struct {
	uint8_t apb1_ahb4_pclk_en;/*!< APB_1_PCLK & AHB_4_HCLKclk enable */
	uint8_t ts_clk_en;/*!< TS CLK clk enable */
	uint8_t adc_lp_hv_clk_en;/*!< ADC_LP_CK_IN_HV clk enable */
	uint8_t I2C2AHB_DBG_en;/*!< I2C2AHB_DBG clk enable */
	uint8_t WDT0_en;/*!< WDT0 clk enable */
	uint8_t WDT1_en;/*!< WDT1 clk enable */
	uint8_t TIMER0_en;/*!< TIMER0 clk enable */
	uint8_t TIMER1_en;/*!< TIMER1 clk enable */
	uint8_t TIMER2_en;/*!< TIMER2 clk enable */
	uint8_t TIMER3_en;/*!< TIMER3 clk enable */
	uint8_t TIMER4_en;/*!< TIMER4 clk enable */
	uint8_t TIMER5_en;/*!< TIMER5 clk enable */
	uint8_t TIMER6_en;/*!< TIMER6 clk enable */
	uint8_t TIMER7_en;/*!< TIMER7 clk enable */
	uint8_t TIMER8_en;/*!< TIMER8 clk enable */
	uint8_t sb_gpio_en;/*!< SB GPIO clk enable */
	uint8_t hmxi2cm_en;/*!< Himax I2C Master clk enable */
} SCU_PDSB_CLKEN_CFG_T;

/**
 * \struct SCU_PDAON_CLK_CFG_T
 * \brief PD_AON CLK configuration Header
 */
typedef struct {
	uint8_t rtc0_clk_en;/*!< RTC0 Clock enable */
	uint8_t rtc1_clk_en;/*!< RTC1 Clock enable */
	uint8_t rtc2_clk_en;/*!< RTC2 Clock enable */
	uint8_t pmu_clk_en;/*!< PMU Clock enable */
	uint8_t aon_gpio_clk_en;/*!< AON GPIO Clock enable */
	uint8_t aon_swreg_clk_en;/*!< AON SW REG Clock enable */
	uint8_t antitamper_clk_en;/*!< ANTI TAMPER Clock enable */
} SCU_PDAON_CLKEN_CFG_T;

typedef struct {
	SCU_AONCLKSRC_E aonclk; /*!< AON CLK Src */
	SCU_ANTITAMP_SRC_E antitamp; /*!< Anti Tamp SRC*/
} SCU_PDAON_CLK_CFG_T;

/**
 * \struct SCU_SIMPLE_CLK_CFG_T
 * \brief Simple CFG CLK configuration Header
 */
typedef struct {
	SCU_SYSCLKSRC_HW_E sysclksrc_sel; /*!< Sys CLK Src Selection*/
	SCU_PLL_FREQ_E pll_freq;/*!< PLL Frequency Selection*/
	SCU_HSCCLKDIV_E cm55m_div;/*!< CM55M div Selection*/
	SCU_LSCCLKDIV_E cm55s_div;/*!< CM55S div Selection*/
	SCU_MCLK_FREQ_E mclk_freq;/*!< MCLK Frequency Selection*/
	uint8_t change_hscdprx;/*!< Change DP RX */
	uint8_t change_hscdp;/*!< Change DP */
	uint8_t change_lscdp;/*!< Change LSC DP */
	uint8_t vad_src_change;/*!< Change vad src*/
	uint8_t hsc32k_src_change;/*!< Change hsc32K src*/
	uint8_t purf_src_change;/*!< Change purf src*/
	uint8_t qspi_change;/*!< Change QSPI*/
	uint8_t ospi_change;/*!< Change OSPI*/
	uint8_t sspim_change;/*!< Change SSPIM*/
	uint8_t sspis_change;/*!< Change SSPIS*/
	uint8_t adcck_change;/*!< Change ADCCK*/
#if(IC_VERSION >= 30)
	uint8_t pdmck_change;/*!< Change PDMCK*/
#endif
	SCU_COREVOLT_CLKTREE_E corev_clktree;/*!< Clk tree CoreVoltage*/
} SCU_SIMPLE_CLK_CFG_T;


/**
 * \struct SCU_PLL_CLK_CFG_T
 * \brief PLL CFG CLK configuration Header
 */
typedef struct {
	uint8_t init_pll_para;/*!< init PLL parameter or not*/
	SCU_SYSCLKSRC_HW_E sysclksrc_sel;/*!< sys clk source selection*/
	SCU_PLLCLKSRC_E clk_src;
	SCU_PLL_FREQ_E freq;/*!< PLL Disable or Enable*/
	uint8_t pllclksrc_div;/*!< PLL Clock source div (0:div1,1:div2...)*/
	uint32_t pll_freq;/*!< PLL Frequency*/
	uint32_t AREG0_3;//AREG0 ~ 3
	uint32_t AREG4_7;//AREG4 ~ 7
	uint32_t AREG8_9_12_13;//AREG8, 9, 12, 13
	uint32_t AREG14_17;//AREG14 ~ 17
	uint32_t AREG18_21;//AREG18 ~ 21
	uint32_t AREG22_100;//AREG22, 100
} SCU_PLL_CLK_CFG_T;
#endif//SUPPORT_CKGEN


/**
 * \struct SCU_AON_SWRESET_T
 * \brief AON SW Reset configuration Header
 */
typedef struct {
	SCU_SWRESET_E aon_gpio;/*!< AON GPIO software reset*/
	SCU_SWRESET_E pmu;/*!< PMU software reset*/
	SCU_SWRESET_E rtc0;/*!< RTC0 software reset*/
	SCU_SWRESET_E rtc1;/*!< RTC1 software reset*/
	SCU_SWRESET_E rtc2;/*!< RTC2 software reset*/
	SCU_SWRESET_E swreg;/*!< SWREG software reset*/
	SCU_SWRESET_E antitamper;/*!< ANTI Tamper software reset*/
} SCU_AON_SWRESET_T;

/**
 * \struct SCU_SB_SWRESET_T
 * \brief SB SW Reset configuration Header
 */
typedef struct {
	SCU_SWRESET_E ts_ctrl;/*!< TS Ctrl software reset*/
	SCU_SWRESET_E himax_i2c_m;/*!< HIMAX_I2C_M software reset*/
	SCU_SWRESET_E sb_gpio;/*!< SB GPIO software reset*/
	SCU_SWRESET_E I2C2AHB_dbg;/*!< I2C2AHB Debug software reset*/
	SCU_SWRESET_E wdt0;/*!< WDT 0 software reset*/
	SCU_SWRESET_E wdt1;/*!< WDT 1 software reset*/
	SCU_SWRESET_E timer0;/*!< Timer 0 software reset*/
	SCU_SWRESET_E timer1;/*!< Timer 1 software reset*/
	SCU_SWRESET_E timer2;/*!< Timer 2 software reset*/
	SCU_SWRESET_E timer3;/*!< Timer 3 software reset*/
	SCU_SWRESET_E timer4;/*!< Timer 4 software reset*/
	SCU_SWRESET_E timer5;/*!< Timer 5 software reset*/
	SCU_SWRESET_E timer6;/*!< Timer 6 software reset*/
	SCU_SWRESET_E timer7;/*!< Timer 7 software reset*/
	SCU_SWRESET_E timer8;/*!< Timer 8 software reset*/
} SCU_SB_SWRESET_T;

/**
 * \struct SCU_LSC_SWRESET_T
 * \brief LSC SW Reset configuration Header
 */
typedef struct {
	SCU_SWRESET_E dma2;/*!< LSC DMA2 software reset*/
	SCU_SWRESET_E dma3;/*!< LSC DMA3 software reset*/
	SCU_SWRESET_E sspim;/*!< sspim software reset*/
	SCU_SWRESET_E sspis;/*!< sspis software reset*/
	SCU_SWRESET_E gpio;/*!< gpio software reset*/
	SCU_SWRESET_E himax_pdm;/*!< himax_pdm software reset*/
	SCU_SWRESET_E adc_ctrl_lv;/*!< ADC_CTRL_LV software reset*/
	SCU_SWRESET_E pwm012;/*!< PWM 0, 1 and 2 software reset*/
	SCU_SWRESET_E sndwire;/*!< Soundwire software reset*/
	SCU_SWRESET_E vad_d;/*!< VAD_D software reset*/

	SCU_SWRESET_E ro_pd; /*!< RO_PD software reset*/
	SCU_SWRESET_E adc; /*!< ADC software reset*/
	SCU_SWRESET_E I2S_m; /*!< I2S Master software reset*/
	SCU_SWRESET_E I2S_s; /*!< I2S Slave software reset*/
	SCU_SWRESET_E uart_0; /*!< UART 0 software reset*/
	SCU_SWRESET_E uart_1; /*!< UART 1 software reset*/
	SCU_SWRESET_E uart_2; /*!< UART 2 software reset*/
	SCU_SWRESET_E i2c_slave_0; /*!< I2C slave 0 software reset*/
	SCU_SWRESET_E i2c_slave_1; /*!< I2C slave 1 software reset*/
	SCU_SWRESET_E i2c_master; /*!< I2C Master software reset*/
	SCU_SWRESET_E i2c_master_sensor; /*!< I2C Master for Sensor software reset software reset*/
	SCU_SWRESET_E i3c_s0; /*!< I3C Slave 0 software reset*/
	SCU_SWRESET_E i3c_s1; /*!< I3C Slave 1 software reset*/
	SCU_SWRESET_E ckmon; /*!< Clock Monitor software reset*/
} SCU_LSC_SWRESET_T;

/**
 * \struct SCU_HSC_SWRESET_T
 * \brief HSC SW Reset configuration Header
 */
typedef struct {
	SCU_SWRESET_E xdma;/*!< XDMA software reset*/
	SCU_SWRESET_E imgpclk;/*!< Image PCLK software reset*/
	SCU_SWRESET_E imgsc;/*!< Image SC software reset*/
	SCU_SWRESET_E mipirx;/*!< MIPI RX software reset*/
	SCU_SWRESET_E mipitx;/*!< MIPI TX software reset*/
	SCU_SWRESET_E dma0;/*!< DMA0 software reset*/
	SCU_SWRESET_E dma1;/*!< DMA1 software reset*/
	SCU_SWRESET_E i3chc;/*!< I3CHC software reset*/
	SCU_SWRESET_E sdio;/*!< SDIO software reset*/
	SCU_SWRESET_E security;/*!< Security software reset*/
	SCU_SWRESET_E i2c2ahbflashw;/*!< I2C2AHBFLASHW software reset*/
	SCU_SWRESET_E qspi;/*!< QSPI software reset*/
	SCU_SWRESET_E ospi;/*!< OSPI software reset*/
	SCU_SWRESET_E spi2ahb;/*!< SPI2AHB software reset*/
} SCU_HSC_SWRESET_T;


/**
 * \struct SCU_DP_SWRESET_T
 * \brief DP SW Reset configuration Header
 */
typedef struct {
	SCU_SWRESET_E LSC_SC;/*!< LSC Image Sensor Control software reset*/
	SCU_SWRESET_E HSC_XDMA;/*!< HSC Image Path XDMA domain software reset*/
	SCU_SWRESET_E HSC_PCLK;/*!< HSC Image Path Pixel clock domain software reset*/
	SCU_SWRESET_E HSC_SCCLK;/*!< HSC Image path sc_clk domain software reset*/
	SCU_SWRESET_E HSC_MIPIRX;/*!< HSC MIPI RX software reset*/
	SCU_SWRESET_E HSC_MIPITX;/*!< HSC MIPI TX software reset*/
} SCU_DP_SWRESET_T;



/**
 * \struct SCU_PINMUX_CFG_T
 * \brief Pin Mux configuration Header
 */
typedef struct {
	SCU_SEN_TRIG_PINMUX_E pin_trigger; /*!< pin trigger*/
	SCU_SEN_FAE_PINMUX_E pin_fae;/*!< pin fae*/
    SCU_SEN_CSW0_PINMUX_E pin_csw0; /*!< pin csw0*/
    SCU_SEN_CSW1_PINMUX_E pin_csw1; /*!< pin csw1*/
    SCU_SEN_INT_PINMUX_E pin_sen_int; /*!< pin sen int*/
    SCU_SEN_GPIO_PINMUX_E pin_sen_gpio; /*!< pin sen gpio*/
    SCU_SEN_I2CM_SDA_PINMUX_E pin_sen_i2cm_sda;  /*!< pin sen i2cm SDA*/
    SCU_SEN_I2CM_SCL_PINMUX_E pin_sen_i2cm_scl;  /*!< pin sen i2cm SCL*/
    SCU_PA0_PINMUX_E  pin_pa0;   /*!< pin PA0*/
    SCU_PA1_PINMUX_E  pin_pa1;   /*!< pin PA1*/
    SCU_PA2_PINMUX_E  pin_pa2;   /*!< pin PA2*/
    SCU_PA3_PINMUX_E  pin_pa3;   /*!< pin PA3*/
    SCU_UART_PINMUX_E  pin_uart;  /*!< pin UART*/
    SCU_PB0_PINMUX_E  pin_pb0;   /*!< pin PB0*/
    SCU_PB1_PINMUX_E  pin_pb1;   /*!< pin PB1*/
    SCU_PB9_PINMUX_E  pin_pb9;   /*!< pin PB9*/
    SCU_PB10_PINMUX_E  pin_pb10;   /*!< pin PB10*/
    SCU_PB11_PINMUX_E  pin_pb11;   /*!< pin PB11*/
    SCU_PC3_PINMUX_E  pin_pc3;   /*!< pin PC3*/
    SCU_PC4_PINMUX_E  pin_pc4;   /*!< pin PC4*/
    SCU_SW_DATA_PINMUX_E pin_sw_data; /*!< pin SW_DATA*/
    SCU_SW_CLK_PINMUX_E pin_sw_clk; /*!< pin SW_CLK*/
    SCU_I2CM_SCLSDA_PINMUX_E pin_i2cm_sdascl; /*!< pin i2cm SDA & SCL*/
    SCU_PB2_PINMUX_E pin_pb2; /*!< pin PB2*/
    SCU_PB3_PINMUX_E pin_pb3; /*!< pin PB3*/
    SCU_PB4_PINMUX_E pin_pb4; /*!< pin PB4*/
    SCU_PB5_PINMUX_E pin_pb5; /*!< pin PB5*/
    SCU_SPIS_DO_PINMUX_E pin_spis_do; /*!< pin SPIS DO*/
    SCU_SPIS_DI_PINMUX_E pin_spis_di; /*!< pin SPIS DI*/
    SCU_SPIS_SCLK_PINMUX_E pin_spis_sclk; /*!< pin SPIS SCLK*/
    SCU_SPIS_CS_PINMUX_E pin_spis_cs; /*!< pin SPIS CS*/
    SCU_SDIO_SDCLK_PINMUX_E pin_sdio_sdclk; /*!< pin SDIO SDCLK*/
    SCU_SDIO_CMD_PINMUX_E pin_sdio_cmd; /*!< pin SDIO CMD*/
    SCU_SDIO_DATA0_PINMUX_E pin_sdio_data0; /*!< pin SDIO data0*/
    SCU_SDIO_DATA1_PINMUX_E pin_sdio_data1; /*!< pin SDIO data1*/
    SCU_SDIO_DATA2_PINMUX_E pin_sdio_data2; /*!< pin SDIO data2*/
    SCU_SDIO_DATA3_PINMUX_E pin_sdio_data3; /*!< pin SDIO data3*/
    SCU_PB6_PINMUX_E pin_pb6; /*!< pin PB6*/
    SCU_PB7_PINMUX_E pin_pb7; /*!< pin PB7*/
    SCU_PB8_PINMUX_E pin_pb8; /*!< pin PB8*/
    SCU_SEN_XSLEEP_PINMUX_E pin_sen_xsleep;   /*!< pin Sen xsleep*/
    SCU_SEN_PCLK_PINMUX_E pin_sen_pclk;   /*!< pin Sen pclk*/
    SCU_SEN_D0_PINMUX_E pin_sen_d0;   /*!< pin Sen D0*/
    SCU_SEN_D1_PINMUX_E pin_sen_d1;   /*!< pin Sen D1*/
    SCU_SEN_D2_PINMUX_E pin_sen_d2;   /*!< pin Sen D2*/
    SCU_SEN_D3_PINMUX_E pin_sen_d3;   /*!< pin Sen D3*/
    SCU_SEN_D4_PINMUX_E pin_sen_d4;   /*!< pin Sen D4*/
    SCU_SEN_MCLK_PINMUX_E pin_sen_mclk;  /*!< pin Sen MCLK*/
    SCU_SEN_D5_PINMUX_E pin_sen_d5;  /*!< pin Sen D5*/
    SCU_SEN_D6_PINMUX_E pin_sen_d6;  /*!< pin Sen D6*/
    SCU_SEN_D7_PINMUX_E pin_sen_d7;  /*!< pin Sen D7*/
    SCU_SEN_FVALID_PINMUX_E pin_sen_fvalid;  /*!< pin Sen FVALID*/
    SCU_SEN_LVALID_PINMUX_E pin_sen_lvalid;  /*!< pin Sen LVALID*/
} SCU_PINMUX_CFG_T;

/**
 * \struct SCU_VMUTE_CFG_T
 * \brief VMUTE configuration Header
 */
typedef struct {
	SCU_VMUTE_CTRL_TIMING_SRC_E timingsrc; /*!< VMUTE CTRL TIMING SRC Selectionr*/
	SCU_VMUTE_CTRL_TXPHY_PWR_E txphypwr;/*!< VMUTE CTRL TXPHY Power Gating*/
    SCU_VMUTE_CTRL_SRC_E ctrlsrc; /*!< VMUTE CTRL source*/
    SCU_VMUTE_CTRL_SW_E  swctrl; /*!< Software set VMUTE*/
} SCU_VMUTE_CFG_T;


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* DRIVERS_INC_HX_DRV_SCU_EXPORT_H_ */
