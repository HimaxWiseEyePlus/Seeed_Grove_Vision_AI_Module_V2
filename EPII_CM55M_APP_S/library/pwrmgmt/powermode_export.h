/*
 * powermode_export.h
 *
 *  Created on: 2021�~12��1��
 *      Author: 902447
 */

#ifndef LIBRARY_PWRMGMT_POWERMODE_EXPORT_H_
#define LIBRARY_PWRMGMT_POWERMODE_EXPORT_H_

#include <stdio.h>
#include <string.h>
#include "WE2_device.h"
#include "hx_drv_pmu_export.h"
#include "hx_drv_scu_export.h"
#include "hx_drv_swreg_aon_export.h"
/**
 * \defgroup	PWRMGMT_LIB	Sensor Control Driver
 * \ingroup		PWRMGMT_LIB
 * \brief	Power Management Library Declaration
 * \details Application can use Power Management API and data structure(PM_CFG_T) to control WE-2 Power Management Unit\n
 */

/**
 * \defgroup	PWRMGMT_LIB_ENUM	Power Manager Enumeration
 * \ingroup	PWRMGMT_LIB
 * \brief	Defines some enumeration of Power Manager Library need.
 * @{
 */
/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/** PMU Errors Type **/
/**
 * \enum PM_ERROR_E
 * \brief PM API ERROR Code
 */
typedef enum PM_ERROR_S
{
	PM_NO_ERROR                     	= 0,	/**< No ERROR */
	PM_ERROR_NOT_INIT              		= 1,	/**< ERROR MSG: not init */
	PM_ERROR_DRIVER						= 2,	/**< ERROR MSG: pmu driver error */
	PM_ERROR_ALREADY_INIT              	= 3,	/**< ERROR MSG: already init */
	PM_ERROR_INVALID_PARAMETERS        	= 4,	/**< ERROR MSG: Invalid parameters */
	PM_ERROR_NULL_POINTER				= 5,	/**< ERROR MSG: NULL pointer */
 	PM_UNKNOWN_ERROR      				= 6,	/**< ERROR MSG: UNKNOWN ERROR*/
 	PM_WARM_NODP_INIT      				= 7,	/**< ERROR MSG: NO DP INIT*/
 	PM_WARM_NO_PMUTOCPU      				= 8,	/**< ERROR MSG: NO DP PMU TO CPU*/
 	PM_WARM_NO_WAITPMUDONE      				= 9,	/**< ERROR MSG: Wait PMU Done*/
 	PM_ERROR_PLL_LOCK_FAIL      				= 10,	/**< ERROR MSG: PLL Lock fail*/
} PM_ERROR_E;

/**
 * \enum PM_CFG_PWR_MODE_E
 * \brief Power Manager Configuration Mode
 */
typedef enum PM_CFG_PWR_MODE_S
{
	PM_MODE_ALL_ON = 0,	/**< All on without any configuration for Power Management**/
	PM_MODE_PS_DPD,						/**< Power State DPD**/
	PM_MODE_PS_NOVID_PREROLLING,		/**< Power State PD No Video Prerolling**/
	PM_MODE_PS_VID_ONLY_PREROLLING,		/**< Power State Video Only Prerolling**/
	PM_MODE_PS_AUD_ONLY_PREROLLING,		/**< Power State Audio Only Prerolling**/
	PM_MODE_PS_VIDAUD_PREROLLING,		/**< Power State Video & Audio Prerolling**/
	PM_MODE_PS_SCL_RC9648_CPU,		/**< Power State SCL RC9648 control by CPU**/
} PM_CFG_PWR_MODE_E;


/**
 * \enum PM_CFG_DCDC_MODE_E
 * \brief Power Manager DCDC Mode
 */
typedef enum PM_CFG_DCDC_MODE_S
{
	PM_CFG_DCDC_MODE_OFF = 0,	/**< DCDC OFF**/
	PM_CFG_DCDC_MODE_AON_GPIO0 = 1,		/**< DCDC Switch output AON_GPIO0(PA0 High Level Power off. Low Level power On)**/
	PM_CFG_DCDC_MODE_VMUTE = 2,		/**< DCDC Switch output VMUTE(PA1 High Level Power off. Low Level power On)**/
	PM_CFG_DCDC_MODE_AON_GPIO0_HIGHPOWON = 3,		/**< DCDC Switch output AON_GPIO0(PA0 High Level Power on. Low Level power Off)**/
	PM_CFG_DCDC_MODE_VMUTE_HIGHPOWON = 4,		/**< DCDC Switch output VMUTE(PA1 High Level Power on. Low Level power Off)**/
} PM_CFG_DCDC_MODE_E;

/**
 * \enum PM_CFG_PD_IORET_E
 * \brief Power Manager IO Retention Mode
 */
typedef enum PM_CFG_PD_IORET_S
{
	PM_CFG_PD_IORET_ON = 0,	/**< IO Retention ON**/
	PM_CFG_PD_IORET_OFF = 1,	/**< IO Retention OFF**/
} PM_CFG_PD_IORET_E;

/**
 * \enum PM_RTC_INT_MASK_E
 * \brief Power Manager RTC IP INT Mask
 */
typedef enum PM_RTC_INT_MASK_S
{
	PM_RTC_INT_MASK_ALLUNMASK = 0x0,	/**< ALL RTC INT Un-Mask**/
	PM_RTC_INT_MASK_RTC0 = 0x1,	/**< RTC0 INT Mask**/
	PM_RTC_INT_MASK_RTC1 = 0x2,	/**< RTC1 INT Mask**/
	PM_RTC_INT_MASK_RTC2 = 0x4,	/**< RTC2 INT Mask**/
	PM_RTC_INT_MASK_ALLMASK = 0x7,	/**< ALL RTC INT Mask**/
	PM_RTC_INT_MASK_RESERVED = 0xFF,	/**< RESERVED**/
} PM_RTC_INT_MASK_E;

/**
 * \enum PM_IP_INT_MASK_E
 * \brief Power Manager IP INT Mask
 */
typedef enum PM_IP_INT_MASK_S
{
	PM_IP_INT_MASK_ALL_UNMASK = 0x0,	/**< ALL INT UN-Mask**/
	PM_IP_INT_MASK = 0x1,	/**< IP INT Mask**/
	PM_IP_INT_RESERVED = 0xFF,	/**< Reserved**/
} PM_IP_INT_MASK_E;

/**
 * \enum PM_TIMER_INT_MASK_E
 * \brief Power Manager TIMER INT Mask
 */
typedef enum PM_TIMER_INT_MASK_S
{
	PM_TIMER_INT_MASK_TIMER_ALLUNMASK = 0x0,	/**< TIMER ALL INT Un-Mask**/
	PM_TIMER_INT_MASK_TIMER0 = 0x1,	/**< TIMER0 INT Mask**/
	PM_TIMER_INT_MASK_TIMER1 = 0x2,	/**< TIMER1 INT Mask**/
	PM_TIMER_INT_MASK_TIMER2 = 0x4,	/**< TIMER2 INT Mask**/
	PM_TIMER_INT_MASK_TIMER3 = 0x8,	/**< TIMER3 INT Mask**/
	PM_TIMER_INT_MASK_TIMER4 = 0x10,	/**< TIMER4 INT Mask**/
	PM_TIMER_INT_MASK_TIMER5 = 0x20,	/**< TIMER5 INT Mask**/
	PM_TIMER_INT_MASK_TIMER6 = 0x40,	/**< TIMER6 INT Mask**/
	PM_TIMER_INT_MASK_TIMER7 = 0x80,	/**< TIMER7 INT Mask**/
	PM_TIMER_INT_MASK_TIMER8 = 0x100,	/**< TIMER8 INT Mask**/
	PM_TIMER_INT_MASK_TIMER_ALLMASK = 0x1FF,	/**< TIMER ALL INT Mask**/
	PM_TIMER_INT_MASK_TIMER_RESERVED = 0xFFFF,	/**< Reserved**/
} PM_TIMER_INT_MASK_E;

/**
 * \enum PM_MEM_RET_E
 * \brief Power Manager Memory Retention
 */
typedef enum PM_MEM_RET_S
{
	PM_MEM_RET_NO = 0x0,	/**< No Memory Retention**/
	PM_MEM_RET_YES = 0x1,	/**< Memory Retention**/
	PM_MEM_RET_RESV = 0xFF,	/**< Reserved**/
} PM_MEM_RET_E;

/**
 * \enum PM_BOOTWITHCAP_E
 * \brief Power Manager BOOT with capture
 */
typedef enum PM_BOOTWITHCAP_S
{
	PM_BOOTWITHCAP_NO = 0x0,	/**< No Boot with capture**/
	PM_BOOTWITHCAP_YES = 0x1,	/**< Boot with capture**/
	PM_BOOTWITHCAP_RESV = 0xFF,	/**< Reserved**/
} PM_BOOTWITHCAP_E;

/**
 * \enum PM_NONAOS_PREROLL_CAP_E
 * \brief Power Manager Non_AOS Prerolling Capture supprot
 */
typedef enum PM_NONAOS_PREROLL_CAP_S
{
	PM_NONAOS_PREROLL_CAP_NO = 0x0,	/**< No Support Non-AOS PREROLLING Cap**/
	PM_NONAOS_PREROLL_CAP_YES = 0x1,	/**< Support Non-AOS PREROLLING Cap**/
	PM_NONAOS_PREROLL_CAP_RESV = 0xFF,	/**< Reserved**/
} PM_NONAOS_PREROLL_CAP_E;

/**
 * \enum PM_SENSOR_TIMING_E
 * \brief Sensor Timing
 */
typedef enum PM_SENSOR_TIMING_S{
	PM_SENSOR_TIMING_FVLDLVLD_CON = 0x0,	/**< Sesnor Timing FVLD and LVLD Con-current**/
	PM_SENSOR_TIMING_FVLDLVLD_SHIFT = 0x1,	/**< Sesnor Timing FVLD and LVLD Shift 2 lines**/
}PM_SENSOR_TIMING_E;

/**
 * \enum PM_SENSOR_TIMING_E
 * \brief 0: Keep Power on   1: Turn off ULPSIMO at PS_PD (Can't set to 1 @PS_ACT_DC to PS_DPD)
 */
typedef enum PM_SIMO_PD_ONOFF_S{
	PM_SIMO_PD_ONOFF_ON = 0x0,	/**< PD SIMO PD ON**/
	PM_SIMO_PD_ONOFF_OFF = 0x1,	/**< PD SIMO PD OFF**/
}PM_SIMO_PD_ONOFF_E;

/**
 * \enum PM_EXT_INT_STATUS_TYPE_E
 * \brief Get PMU INT Status for PA01 GPIO IP INT(not for DPD) or PA23 GPIO IP INT (not for DPD)or PA0 Level in DPD or PA1 Level in DPD
 */
typedef enum PM_EXT_INT_STATUS_TYPE_S{
	PMU_EXT_INT_TYPE_PA01_GPIO = 0x0,	/**< PA01 GPIO INT**/
	PMU_EXT_INT_TYPE_PA23_GPIO = 0x1,	/**< PA23 GPIO INT**/
	PMU_EXT_INT_TYPE_PA0_GPIO_DPD = 0x2,	/**< PA0 GPIO INT in DPD**/
	PMU_EXT_INT_TYPE_PA1_GPIO_DPD = 0x3,	/**< PA1 GPIO INT in DPD**/
}PM_EXT_INT_STATUS_TYPE_E;

/**
 * \enum PM_CLK_PARA_CTRL_E
 * \brief PMU clock control by PMU lib or APP
 */
typedef enum PM_CLK_PARA_CTRL_S{
	PM_CLK_PARA_CTRL_BYPMLIB = 0x0,	/**< PMU Clock PARA control by PMU lib**/
	PM_CLK_PARA_CTRL_BYAPP = 0x1,	/**< PMU Clock PARA control by APP**/
}PM_CLK_PARA_CTRL_E;

/**
 * \enum PM_MCLK_CTRL_E
 * \brief PMU MCLK control by PMU or CPU
 */
typedef enum PM_MCLK_CTRL_S{
	PM_MCLK_CTRL_BYPMU = 0x0,	/**< PMU MCLK control by PMU**/
	PM_MCLK_CTRL_BYCPU = 0x1,	/**< PMU MCLK control by CPU**/
}PM_MCLK_CTRL_E;
/** @} */

/****************************************************
 * Structure Definition                             *
 ***************************************************/
/**
 * \defgroup	PWRMGMT_LIB_REGSTRUCT		Power Manager Library Structure
 * \ingroup	PWRMGMT_LIB
 * \brief	contains definitions of Power Manager library structure.
 * @{
 */

/**
 * \struct PM_WARMBOOT_SPEED_T
 * \brief Power manager configuration for warm boot speed
 */
typedef struct PM_WARMBOOT_SPEED_S{
	SCU_PLL_FREQ_E 	bootromclkfreq;       /**< bootrom clk frequency **/
	uint32_t pll_freq;					/**< bootrom PLL frequency **/
	SCU_HSCCLKDIV_E	cm55m_div;			/**< bootrom CM55M Frequency Division **/
	SCU_LSCCLKDIV_E cm55s_div;			/**< bootrom CM55S Frequency Division **/
}PM_WARMBOOT_SPEED_T;



/**
 * \struct PM_DPD_CFG_T
 * \brief Power manager configuration for DPD
 */
typedef struct PM_DPD_CFG_S{
	PM_WARMBOOT_SPEED_T 	bootromspeed;       /**< bootrom Speed **/
	SWREG_AON_PMUWAKE_CM55S_RERESET_E cm55s_reset;	/**< CM55Reset when warm boot **/
	PM_RTC_INT_MASK_E		pmu_rtc_mask;	/**< PMU RTC Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_anti_mask;	/**< PMU ANTI Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa0_mask;	/**< PMU PAD PA0 Interrupt Mask DPD Use**/
	PM_IP_INT_MASK_E		pmu_pad_pa1_mask;	/**< PMU PAD PA1 Interrupt Mask DPD Use**/
	PM_CFG_DCDC_MODE_E		pmu_dcdc_outpin;	/**< PMU DCDC output pin **/
	PMU_DPD_PA01_GPIO_POL_WAKEUP_E gpio_wakeup_pol;/**< PMU GPIO Wakeup Polarity **/
	uint8_t 	support_debugdump;			/**< Support debug register dump**/
}PM_DPD_CFG_T;

/**
 * \struct PM_PD_NOVIDPRE_CFG_T
 * \brief Power manager configuration for no video prerolling in PD State
 */
typedef struct PM_PD_NOVIDPRE_CFG_S{
	PM_WARMBOOT_SPEED_T 	bootromspeed;       /**< bootrom Speed **/
	PMU_NFRAMEEND_CTRL_E	nframeend_ctrl;	/**< NFrame Control **/
	SWREG_AON_PMUWAKE_CM55S_RERESET_E cm55s_reset;	/**< CM55Reset when warm boot **/
	PM_RTC_INT_MASK_E		pmu_rtc_mask;			/**< PMU RTC Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa01_mask;			/**< PMU PA0,PA1 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa23_mask;			/**< PMU PA2,PA3 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_i2cw_mask;			/**< PMU I2C Wakeup Interrupt Mask **/
	PM_TIMER_INT_MASK_E	pmu_timer_mask;			/**< PMU Timer0~8 Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_cmp_mask;			/**< PMU CMP Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_ts_mask;			/**< PMU TS Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_anti_mask;	/**< PMU ANTI Interrupt Mask **/
	PM_MEM_RET_E 	tcm_retention;			/**< CM55M TCM Retention**/
	PM_MEM_RET_E 	hscsram_retention[4];	/**< HSC SRAM Retention**/
	PM_MEM_RET_E 	lscsram_retention;		/**< LSC SRAM Retention**/
	SWREG_AON_RETENTION_CFG_T 	skip_bootflow;			/**< Skip Boot Flow**/
	PM_BOOTWITHCAP_E 	support_bootwithcap;		/**< Support capture when boot up**/
	PM_CFG_DCDC_MODE_E		pmu_dcdc_outpin;	/**< PMU DCDC output pin **/
	PMU_MIPI_LANE_E mipi_lane_en;		/**< PMU MIPI Lane Enable **/
	PM_CFG_PD_IORET_E ioret;/**< PMU PD IO Retention **/
	PM_SENSOR_TIMING_E sensor_type;/**< Sensor Timing Type **/
	PM_SIMO_PD_ONOFF_E simo_pd_onoff;/**< SIMP PD OnOFF**/
	uint8_t 	support_debugdump;			/**< Support debug register dump**/
}PM_PD_NOVIDPRE_CFG_T;


/**
 * \struct PM_PD_VIDPRE_CFG_T
 * \brief Power manager configuration for video pre-rolling
 */
typedef struct PM_PD_VIDPRE_CFG_S{
	PM_WARMBOOT_SPEED_T 	bootromspeed;       /**< bootrom Speed **/
	uint32_t sensor_timer;	 /**< Sensor Timer **/
	uint32_t wdt_timer;	 /**< WDT Timer **/
	PMU_NFRAMEEND_CTRL_E	nframeend_ctrl;	/**< NFrame Control **/
	SWREG_AON_PMUWAKE_CM55S_RERESET_E cm55s_reset;	/**< CM55Reset when warm boot **/
	PM_RTC_INT_MASK_E		pmu_rtc_mask;			/**< PMU RTC Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa01_mask;			/**< PMU PA0,PA1 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa23_mask;			/**< PMU PA2,PA3 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_i2cw_mask;			/**< PMU I2C Wakeup Interrupt Mask **/
	PM_TIMER_INT_MASK_E	pmu_timer_mask;			/**< PMU Timer0~8 Wakeup Interrupt Mask (Timer0 do not support wake up in this case)   **/
	PM_IP_INT_MASK_E		pmu_cmp_mask;			/**< PMU CMP Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_ts_mask;			/**< PMU TS Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_senint_mask;		/**< PMU SENSOR Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_mipii2c_noack_mask;		/**< PMU MIPI I2C No ACK Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_anti_mask;	/**< PMU ANTI Interrupt Mask **/
	PM_MEM_RET_E 	tcm_retention;			/**< CM55M TCM Retention**/
	PM_MEM_RET_E 	hscsram_retention[4];	/**< HSC SRAM Retention**/
	PM_MEM_RET_E 	lscsram_retention;		/**< LSC SRAM Retention**/
	SWREG_AON_RETENTION_CFG_T 	skip_bootflow;			/**< Skip Boot Flow**/
	PM_NONAOS_PREROLL_CAP_E 	fast_vpr;		/**< Non-AOS Prerolling Capture Support**/
	PM_CFG_DCDC_MODE_E		pmu_dcdc_outpin;	/**< PMU DCDC output pin **/
	PMU_MIPI_LANE_E mipi_lane_en;		/**< PMU MIPI Lane Enable **/
	PM_SENSOR_TIMING_E sensor_type;/**< Sensor Timing Type **/
	uint8_t 	support_debugdump;			/**< Support debug register dump**/
}PM_PD_VIDPRE_CFG_T;

/**
 * \struct PM_PD_AUDPRE_CFG_T
 * \brief Power manager configuration for audio pre-rolling
 */
typedef struct PM_PD_AUDPRE_CFG_S{
	PM_WARMBOOT_SPEED_T 	bootromspeed;       /**< bootrom Speed **/
	SWREG_AON_PMUWAKE_CM55S_RERESET_E cm55s_reset;	/**< CM55Reset when warmboot **/
	PM_RTC_INT_MASK_E		pmu_rtc_mask;			/**< PMU RTC Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa01_mask;			/**< PMU PA0,PA1 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa23_mask;			/**< PMU PA2,PA3 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_i2cw_mask;			/**< PMU I2C Wakeup Interrupt Mask **/
	PM_TIMER_INT_MASK_E	pmu_timer_mask;		    /**< PMU Timer0~8 Wakeup Interrupt Mask (Timer0 do not support wake up in this case) **/
	PM_IP_INT_MASK_E		pmu_cmp_mask;			/**< PMU CMP Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_ts_mask;			/**< PMU TS Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_senint_mask;		/**< PMU SENSOR Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_anti_mask;	/**< PMU ANTI Interrupt Mask **/
	PM_MEM_RET_E 	tcm_retention;			/**< CM55M TCM Retention**/
	PM_MEM_RET_E 	hscsram_retention[4];	/**< HSC SRAM Retention**/
	PM_MEM_RET_E 	lscsram_retention;		/**< LSC SRAM Retention**/
	SWREG_AON_RETENTION_CFG_T 	skip_bootflow;			/**< Skip Boot Flow**/
	PM_CFG_DCDC_MODE_E		pmu_dcdc_outpin;	/**< PMU DCDC output pin **/
	uint8_t 	support_debugdump;			/**< Support debug register dump**/
}PM_PD_AUDPRE_CFG_T;


/**
 * \struct PM_PD_VIDAUDPRE_CFG_T
 * \brief Power manager configuration for video and audio pre-rolling
 */
typedef struct PM_PD_VIDAUDPRE_CFG_S{
	PM_WARMBOOT_SPEED_T 	bootromspeed;       /**< bootrom Speed **/
	uint32_t sensor_timer;		 /**< Sensor Timer **/
	uint32_t wdt_timer;	 /**< WDT Timer **/
	PMU_NFRAMEEND_CTRL_E	nframeend_ctrl;	/**< NFrame Control **/
	SWREG_AON_PMUWAKE_CM55S_RERESET_E cm55s_reset;	/**< CM55Reset when warm boot **/
	PM_RTC_INT_MASK_E		pmu_rtc_mask;			/**< PMU RTC Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa01_mask;			/**< PMU PA0,PA1 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_pad_pa23_mask;			/**< PMU PA2,PA3 Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_i2cw_mask;			/**< PMU I2C Wakeup Interrupt Mask **/
	PM_TIMER_INT_MASK_E	pmu_timer_mask;		    /**< PMU Timer0~5 Wakeup Interrupt Mask (Timer0 do not support wake up in this case)   **/
	PM_IP_INT_MASK_E		pmu_cmp_mask;			/**< PMU CMP Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_ts_mask;			/**< PMU TS Wakeup Interrupt Mask  **/
	PM_IP_INT_MASK_E		pmu_senint_mask;		/**< PMU SENSOR Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_mipii2c_noack_mask;		/**< PMU MIPI I2C No ACK Interrupt Mask **/
	PM_IP_INT_MASK_E		pmu_anti_mask;	/**< PMU ANTI Interrupt Mask **/
	PM_MEM_RET_E 	tcm_retention;			/**< CM55M TCM Retention**/
	PM_MEM_RET_E 	hscsram_retention[4];	/**< HSC SRAM Retention**/
	PM_MEM_RET_E 	lscsram_retention;		/**< LSC SRAM Retention**/
	SWREG_AON_RETENTION_CFG_T 	skip_bootflow;			/**< Skip Boot Flow**/
	PM_NONAOS_PREROLL_CAP_E 	fast_vpr;		/**< Non-AOS Prerolling Capture Support**/
	PM_CFG_DCDC_MODE_E		pmu_dcdc_outpin;	/**< PMU DCDC output pin **/
	PMU_MIPI_LANE_E mipi_lane_en;		/**< PMU MIPI Lane Enable **/
	PM_SENSOR_TIMING_E sensor_type;/**< Sensor Timing Type **/
	uint8_t 	support_debugdump;			/**< Support debug register dump**/
	PM_MCLK_CTRL_E mclk_ctrl; /**< MCLK control**/
}PM_PD_VIDAUDPRE_CFG_T;

/**
 * \struct PM_VIDPRE_CM55S_CFG_T
 * \brief Power manager configuration for video pre-rolling by CM55S
 */
typedef struct PM_VIDPRE_CM55S_CFG_S{
	PM_WARMBOOT_SPEED_T 	bootromspeed;       /**< bootrom Speed **/
	SWREG_AON_PMUWAKE_CM55S_RERESET_E cm55s_reset;	/**< CM55Reset when warmboot **/
	PM_MEM_RET_E 	tcm_retention;			/**< CM55M TCM Retention**/
	PM_MEM_RET_E 	hscsram_retention[4];	/**< HSC SRAM Retention**/
	PM_MEM_RET_E 	lscsram_retention;		/**< LSC SRAM Retention**/
	SWREG_AON_RETENTION_CFG_T 	skip_bootflow;			/**< Skip Boot Flow**/
	PM_CFG_DCDC_MODE_E		pmu_dcdc_outpin;	/**< PMU DCDC output pin **/
	uint8_t 	support_debugdump;			/**< Support debug register dump**/
}PM_VIDPRE_CM55S_CFG_T;

/**
 * \brief Sensor DP library PMU to CPU control
 *
 * \retval void.
 */
typedef void (*sensordplib_pmutocpuctrl_t) (void);

/**
 * \brief Sensor DP library Wait DMA Done
 *
 * \retval void.
 */
typedef void (*sensordplib_waitpmudmadone_t) (void);

/**
 * \brief Sensor DP library Init
 *
 * \retval void.
 */
typedef void (*sensordplib_pmudpinit_t) (void);
/** @} */



#endif /* LIBRARY_PWRMGMT_POWERMODE_EXPORT_H_ */
