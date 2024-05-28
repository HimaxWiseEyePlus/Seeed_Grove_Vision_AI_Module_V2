/*
 * hx_drv_sensorctrl.h
 *
 *  Created on: 2018/11/11
 *      Author: 902447
 */

#ifndef INC_HX_DRV_SENSORCTRL_H_
#define INC_HX_DRV_SENSORCTRL_H_

#include "dev_common.h"
#include "hx_drv_gpio.h"
/**
 * \defgroup	SENSORCTRL_DRV	Sensor Control Driver
 * \ingroup		SENSORCTRL_DRV
 * \brief	Sensor Control Driver Declaration
 * \details Application can use Sensor Control driver API to set MCLK control owner, xSleep pin, xShutdown pin, Sensor Share pin\n
 * <pre>
 * For Example
 * Ex1: set SensorContrl MCLK Control by Sensor Control or CPU
 * 		hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_E mclkctrl);
 * Ex2: xSleep pin control by Sensor Control or CPU
 * 		hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_CTRL_E mode);
 * Ex3: set SensorContrl xSleep Pin High or Low if xSleep is controlled by CPU
 * 		hx_drv_sensorctrl_set_xSleep(uint8_t high);
 * Ex4: set Sensor xShutdown selection and high/low
 * 		hx_drv_sensorctrl_set_xShutdown(IOMUX_INDEX_E gpio, uint8_t high);
 * </pre>
 */
/**
 * \defgroup	SENSORCTRL_DRV_ENUM	Sensor Control Driver Enumeration
 * \ingroup	SENSORCTRL_DRV
 * \brief	Defines some enumeration of Sensor Control Driver need.
 * @{
 */
/**
 * \enum SENSORCTRL_ERROR_E
 * \brief Sensor Control Errors Type
 */
typedef enum
{
	SENSORCTRL_NO_ERROR                     	= 0,	/**< No ERROR */
	SENSORCTRL_ERROR_NOT_INIT              	    = 1,	/**< ERROR MSG: not init */
	SENSORCTRL_ERROR_ALREADY_INIT               = 2,	/**< ERROR MSG: already init */
	SENSORCTRL_ERROR_INVALID_PARAMETERS     	= 3,	/**< ERROR MSG: Invalid parameters */
	SENSORCTRL_UNKNOWN_ERROR      				= 4,	/**< ERROR MSG: UNKNOWN ERROR*/
} SENSORCTRL_ERROR_E;

/**
 * \enum SENSORCTRL_OPMODE_E
 * \brief Sensor control operating mode
 */
typedef enum
{
	SENSORCTRL_OPMODE_HM11B1_LOW_POWER	 			= 0,/**<HW Trigger Low power mode - HM11B1*/
	SENSORCTRL_OPMODE_HM01B0_CONTINUOUS 			= 1,/**<HW Trigger Continuous mode - HM01B0*/
	SENSORCTRL_OPMODE_HM0360_CONTINUOUS 			= 2,/**<HW Trigger Continuous mode - HM0360 Oscillator version*/
	SENSORCTRL_OPMODE_HM0360_BURST		 			= 3,/**<HW Trigger Burst mode - HM0360 Oscillator version*/
	SENSORCTRL_OPMODE_HM0360_AUTOWAKEUP_SENACTIVE	= 4,/**<Automatic wake-up mode(sensor active) - HM0360 Oscillator version*/
	SENSORCTRL_OPMODE_HM0360_AUTOWAKEUP_WE1ACTIVE	= 5,/**<Automatic wake-up mode(WE-I ACTIVE) - HM0360 Oscillator/MCLK version*/
	SENSORCTRL_OPMODE_HM0360_CONTINUOUS_WIHMCLK 	= 6,/**<HW Trigger Continuous mode - HM0360 MCLK version*/
	SENSORCTRL_OPMODE_HM0360_BURST_WITHMCLK		 	= 7,/**<HW Trigger Burst mode - HM0360 MCLK version*/
} SENSORCTRL_OPMODE_E;

/**
 * \enum SENSORCTRL_INTSTATUS_E
 * \brief Sensor control Interrupt Status
 */
typedef enum
{
	SENSORCTRL_INTSTATUS_NO		   	  = 0,	/**< NO */
	SENSORCTRL_INTSTATUS_WDT_ERROR    = 1,	/**< Interrupt for FVLD watch dog timer expires.*/
	SENSORCTRL_INTSTATUS_NFRAME_END   = 2,	/**< N Frame End Interrupt*/
} SENSORCTRL_INTSTATUS_E;

/**
 * \enum SENSORCTRL_CLKRSTN_E
 * \brief Sensor control CLK Reset
 */
typedef enum
{
	SENSORCTRL_CLKRSTN_RESET	 = 0,	/**< Sensor CLK RSTN Low Reset Mode*/
	SENSORCTRL_CLKRSTN_NORMAL    = 1,	/**< Sensor CLK RSTN High Normal Mode*/
} SENSORCTRL_CLKRSTN_E;

/**
 * \enum SENSORCTRL_RSTSEL_E
 * \brief Sensor control Reset Select
 */
typedef enum
{
    SENSORCTRL_RSTSEL_SW     = 0,   /**< Sensor RST by SW*/
    SENSORCTRL_RSTSEL_PMU    = 1,   /**< Sensor RST by PMU*/
} SENSORCTRL_RSTSEL_E;

/**
 * \enum SENSORCTRL_MCLK_E
 * \brief Datapath CLK Divider for MCLK
 */
typedef enum
{
	SENSORCTRL_MCLK_DIV1	 = 0,	/**< Sensor MCLK DIV1*/
	SENSORCTRL_MCLK_DIV2  	= 1,	/**< Sensor MCLK DIV2*/
	SENSORCTRL_MCLK_DIV3  	= 2,	/**< Sensor MCLK DIV3*/
	SENSORCTRL_MCLK_DIV4  	= 3,	/**< Sensor MCLK DIV4*/
	SENSORCTRL_MCLK_DIV5  	= 4,	/**< Sensor MCLK DIV5*/
	SENSORCTRL_MCLK_DIV6  	= 5,	/**< Sensor MCLK DIV6*/
	SENSORCTRL_MCLK_DIV7  	= 6,	/**< Sensor MCLK DIV7*/
	SENSORCTRL_MCLK_DIV8  	= 7,	/**< Sensor MCLK DIV8*/
	SENSORCTRL_MCLK_DIV9  	= 8,	/**< Sensor MCLK DIV9*/
	SENSORCTRL_MCLK_DIV10  	= 9,	/**< Sensor MCLK DIV10*/
	SENSORCTRL_MCLK_DIV11  	= 10,	/**< Sensor MCLK DIV11*/
	SENSORCTRL_MCLK_DIV12  	= 11,	/**< Sensor MCLK DIV12*/
	SENSORCTRL_MCLK_DIV13  	= 12,	/**< Sensor MCLK DIV13*/
	SENSORCTRL_MCLK_DIV14  	= 13,	/**< Sensor MCLK DIV14*/
	SENSORCTRL_MCLK_DIV15  	= 14,	/**< Sensor MCLK DIV15*/
	SENSORCTRL_MCLK_DIV16  	= 15,	/**< Sensor MCLK DIV16*/
	SENSORCTRL_MCLK_NO  	= 16,	/**< Sensor No MCLK*/
} SENSORCTRL_MCLK_E;

/**
 * \enum SENSORCTRL_STATE_E
 * \brief Sensor Control State
 */
typedef enum
{
	SENSORCTRL_STATE_OFF 		= 0x0,	/**< Sensor Control State sm_off*/
	SENSORCTRL_STATE_PRE 		= 0x08, /**< Sensor Control State sm_pre*/
	SENSORCTRL_STATE_TRIGGER 	= 0x18, /**< Sensor Control State sm_trigger*/
	SENSORCTRL_STATE_CAPTURE	= 0x38, /**< Sensor Control State sm_capture*/
	SENSORCTRL_STATE_POST		= 0x30, /**< Sensor Control State sm_post*/
	SENSORCTRL_STATE_SLEEP		= 0x20,	/**< Sensor Control State sm_sleep*/
}SENSORCTRL_STATE_E;

/**
 * \enum SENSORCTRL_MCLKCTRL_E
 * \brief MCLK control by Sensor Control or CPU
 */
typedef enum
{
	SENSORCTRL_MCLKCTRL_AOS 		= 0x0,/**< 0: For AOS sensor, S_MCLK gated/ungated by sensor_ctrl control.*/
	SENSORCTRL_MCLKCTRL_NONAOS 		= 0x1,/**< 1: For No-AOS sensor, S_MCLK always toggle (in CPU ALLON, PS_ALL state)*/
}SENSORCTRL_MCLKCTRL_E;

/**
 * \enum SENSORCTRL_CXTS_E
 * \brief Context Switch pin 0 and pin 1 value
 */
typedef enum
{
	SENSORCTRL_CXTS_VAL0 	= 0x0,/**< Context Switch 0*/
	SENSORCTRL_CXTS_VAL1 	= 0x1,/**< Context Switch 1*/
}SENSORCTRL_CXTS_E;

/**
 * \enum SENSORCTRL_XSLEEP_CTRL_E
 * \brief xSleep control by Sensor Control or CPU
 */
typedef enum
{
	SENSORCTRL_XSLEEP_BY_SC 	= 0x0,/**< 0: SensorCtrl control the S_XSLEEP*/
	SENSORCTRL_XSLEEP_BY_CPU 	= 0x1,/**< 1: CPU control the S_XSLEEP (from REG_PMU_S_XSLEEP)*/
}SENSORCTRL_XSLEEP_CTRL_E;

/**
 * \enum SENSORCTRL_CXTS_CTRL_E
 * \brief context switch by Sensor Control or CPU
 */
typedef enum
{
	SENSORCTRL_CXTS_BY_SC 		= 0x0,/**< 0: SensorCtrl control the S_CXTSW*/
	SENSORCTRL_CXTS_BY_CPU 		= 0x1,/**< 1: CPU control the S_CXTSW (from REG_PMU_S_CXTSW)*/
}SENSORCTRL_CXTS_CTRL_E;

/**
 * \enum SENSORCTRL_TRIGGER_CTRL_E
 * \brief context switch by Sensor Control or CPU
 */
typedef enum
{
	SENSORCTRL_TRIGGER_BY_SC 		= 0x0,/**< 0: SensorCtrl control the S_TRIGGER*/
	SENSORCTRL_TRIGGER_BY_CPU 		= 0x1,/**< 1: CPU control the S_TRIGGER (from REG_PMU_S_TRIGGER)*/
}SENSORCTRL_TRIGGER_CTRL_E;

/**
 * \enum SENSORCTRL_WLCSP_SHAREPIN_E
 * \brief WLCSP38 share pin for HM11B1, HM01B0, HM0360_MCLK(VerC Chip), HM0360_OSC(VerC Chip)
 */
typedef enum
{
	SENSORCTRL_WLCSP_SHAREPIN_HM11B1 		= 0x0,/**< 0: WLCSP38 Share pin for HM11B1*/
	SENSORCTRL_WLCSP_SHAREPIN_HM01B0 		= 0x1,/**< 1: WLCSP38 Share pin for HM01B0*/
	SENSORCTRL_WLCSP_SHAREPIN_HM0360_MCLK 	= 0x2,/**< 2: WLCSP38 Share pin for HM0360 MCLK VerC Chip Support (xSleep, HSync, VSync, MCLK)*/
	SENSORCTRL_WLCSP_SHAREPIN_HM0360_OSC 	= 0x3,/**< 3: WLCSP38 Share pin for HM0360 OSC VerC Chip Support (xSleep, HSync, VSync, Trigger)*/
}SENSORCTRL_WLCSP_SHAREPIN_E;

/**
 * \enum SENSORCTRL_WLCSP_SHAREPIN_E
 * \brief WLCSP38 share pin for HM11B1, HM01B0, HM0360_MCLK(VerC Chip), HM0360_OSC(VerC Chip)
 */
typedef enum
{
	SENSORCTRL_EXT_XSLEEP_MCLK_DISABLE 		= 0x0,/**< 0: DISABLE extend XSLEEP & MCLK*/
	SENSORCTRL_EXT_XSLEEP_MCLK_ENABLE 		= 0x1,/**< 1: ENABLE extend XSLEEP & MCLK*/
}SENSORCTRL_EXTEND_XSLEEP_MCLK_E;

/** @} */

/**
 * \defgroup	SENSORCTRL_DRV_REGSTRUCT	Sensor Control Driver Structure
 * \ingroup	SENSORCTRL_DRV
 * \brief	contains definitions of Sensor Control Driver structure.
 * @{
 */
/**
 * \struct SENSORCTRL_CFG_T
 * \brief Sensor Control configuration by Sensor Control Driver
 */
typedef struct
{
	SENSORCTRL_OPMODE_E opmode;/**< Sensor control operating mode*/
	uint16_t fsm_pretiming;/**< Sensor control FSM pre-timing*/
	uint16_t fsm_posttiming;/**< Sensor control FSM post-timing*/
	uint8_t num_frame;/**< Sensor control capture Frame per time*/
	uint16_t triggerlowlen;/**< frame length calculation trigger from high to low */
	uint16_t wdt;/**< Sensor control Watch Dog time*/
	uint16_t input_hsize; /**< Sensor control NON_CONTI_DE_HSIZE_SUB_1*/
	SENSORCTRL_EXTEND_XSLEEP_MCLK_E extend_xsleep_mclken;/**< Sensor control entend xsleep mclken*/
} SENSORCTRL_CFG_T;
/** @} */

/**
 * \defgroup	SENSORCTRL_DRV_FUNCDLR	Sensor Control Driver Function Declaration
 * \ingroup	SENSORCTRL_DRV
 * \brief	Contains declarations of Sensor Control Driver functions.
 * @{
 */
/**
 * \brief	Sensor Control interrupt callback function
 *
  * \param[in]	event	 Sensor Control interrupt callback event
 * \return	void.
 */
typedef void (*SensorCtrl_ISREvent_t) (SENSORCTRL_INTSTATUS_E event);
/**
 * \brief	Sensor interrupt callback function
 *
  * \param[in]	event	 Sensor interrupt callback event
 * \return	void.
 */
typedef void (*Sensor_ISREvent_t) (uint8_t event);

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief	init SensorContrl Driver
 *
 * \param[in]	reg_baseaddr	 	Sensor Control reg base address
 * \param[in]	reg_lsc_baseaddr	 Sensor Control LSC reg base address
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_init(uint32_t reg_baseaddr, uint32_t reg_lsc_baseaddr);

/**
 * \brief	set all SensorContrl related Setting
 *
 * \param[in]	sensorctrl_cfg	 Sensor Control Configuration
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_allCfg(SENSORCTRL_CFG_T sensorctrl_cfg);

/**
 * \brief	get default SensorContrl related Setting
 *
 * \param[out]	sensorctrl_cfg	 Sensor Control Configuration
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_defCfg(SENSORCTRL_CFG_T *sensorctrl_cfg);

/**
 * \brief	set SensorContrl Operating Mode
 *
 * \param[in]	mode	 Sensor control operating mode
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_op(SENSORCTRL_OPMODE_E mode);

/**
 * \brief	get SensorContrl Operating Mode
 *
 * \param[out]	mode	 Sensor control operating mode
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_op(SENSORCTRL_OPMODE_E *mode);

/**
 * \brief	set SensorContrl FSM Timing
 *
 * \param[in]	fsm_pretiming	 Sensor control FSM pre-timing
 * \param[in]	fsm_posttiming	 Sensor control FSM post-timing
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_FSMTiming(uint16_t fsm_pretiming, uint16_t fsm_posttiming);

/**
 * \brief	get SensorContrl FSM Timing
 *
 * \param[out]	fsm_pretiming	 Sensor control FSM pre-timing
 * \param[out]	fsm_posttiming	 Sensor control FSM post-timing
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_FSMTiming(uint16_t *fsm_pretiming, uint16_t *fsm_posttiming);

/**
 * \brief	set SensorContrl Number of Frame
 *
 * \param[in]	nFrame	 Sensor control capture N Frame per time
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_nFrame(uint8_t nFrame);
/**
 * \brief	get SensorContrl Number of Frame
 *
 * \param[out]	nFrame	 Sensor control capture N Frame per time
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_nFrame(uint8_t *nFrame);

/**
 * \brief	set SensorContrl WDT
 *
 * \param[in]	wdt_time	 Sensor control Watchdog time
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_WDT(uint16_t wdt_time);

/**
 * \brief	get SensorContrl WDT
 *
 * \param[out]	wdt_time	 Sensor control Watchdog time
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_WDT(uint16_t *wdt_time);

/**
 * \brief	set SensorContrl Trigger Low Length
 *
 * \param[in]	triggerlow_len	 Sensor control trigger from high to low time by frame length
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_triggerlowlen(uint16_t triggerlow_len);

/**
 * \brief	get SensorContrl Trigger Low Length
 *
 * \param[out]	triggerlow_len	 Sensor control trigger from high to low time by frame length
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_triggerlowlen(uint16_t *triggerlow_len);

/**
 * \brief   set SensorContrl noncount_de_hszie
 *
 * \param[in]   hsize   Sensor control sensor input hsize
 * \return  SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_noncount_de_hszie(uint16_t hsize);

/**
 * \brief   get SensorContrl noncount_de_hszie
 *
 * \param[out]  hsize   Sensor control sensor input hsize
 * \return  SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_noncount_de_hszie(uint16_t *hsize);

/**
 * \brief	set SensorContrl CLK RSTN reset or normal
 *
 * \param[in]	clk_rstn	 Sensor control CLK Reset
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_CLKRSTN(SENSORCTRL_CLKRSTN_E clk_rstn);

/**
 * \brief	get SensorContrl CLK reset or normal
 *
 * \param[out]	clk_rstn	 Sensor control CLK Reset
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_CLKRSTN(SENSORCTRL_CLKRSTN_E *clk_rstn);

/**
 * \brief   set SensorContrl RST by SW or PMU
 *
 * \param[in]   sel     Sensor control RST selection
 * \return  SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_RSTSEL(SENSORCTRL_RSTSEL_E sel);

/**
 * \brief   get SensorContrl RST by SW or PMU
 *
 * \param[out]  sel    Sensor control RST selection
 * \return  SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_RSTSEL(SENSORCTRL_RSTSEL_E *sel);

/**
 * \brief	set SensorContrl MCLK enable and MCLK selection, to change mclk divider, use API hx_drv_scu_set_pdlsc_dpclk_cfg() instead
 * 			The API currently use for enable mclk output enable
 *
 * \param[in]	enable	 enable MCLK out
 * \param[in]	mclk	 Datapath CLK Divider for MCLK (deprecated)
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_MCLK(uint8_t enable, SENSORCTRL_MCLK_E mclk);

/**
 * \brief	get SensorContrl MCLK enable and MCLK selection
 *
 * \param[out]	enable	 enable MCLK out
 * \param[out]	mclk	 Datapath CLK Divider for MCLK
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_MCLK(uint8_t *enable, SENSORCTRL_MCLK_E *mclk);

/**
 * \brief	set Sensor xShutdown selection and high/low
 *
 * \param[in]	high	 xShutdown high or low
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_xShutdown(GPIO_INDEX_E gpio, uint8_t high);


/**
 * \brief	get SensorContrl FSM state
 *
 * \param[out]	state	 Sensor Control State
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_state(SENSORCTRL_STATE_E *state);

/**
 * \brief	register sensor control interrupt callback
 *
 * \param[in]	cb_event	 Interrupt callback function
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_register_cb(SensorCtrl_ISREvent_t  cb_event);

/**
 * \brief	register sensor interrupt callback
 *
 * \param[in]	cb_event	 Interrupt callback function
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensor_register_cb(Sensor_ISREvent_t  cb_event);


/**
 * \brief	set SensorContrl FAE selection
 *
 * \param[in]	high	 FAE pin high/low
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_FAE(uint8_t high);

/**
 * \brief	xSleep control by Sensor Control or CPU
 *
 * \param[in]	mode	 xSleep control by Sensor Control or CPU
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_CTRL_E mode);

/**
 * \brief	set SensorContrl xSleep Pin High or Low  if xSleep is controlled by CPU
 *
 * \param[in]	high	 xSleep pin high or low
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_xSleep(uint8_t high);

/**
 * \brief	set SensorContrl CXTSW Control selection
 *
 * \param[in]	mode	 context switch by Sensor Control or CPU
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_CXTSWCtrl(SENSORCTRL_CXTS_CTRL_E mode);

/**
 * \brief	set SensorContrl CXTSW pin status
 *
 * \param[in]	mode	 Context Switch pin 0 and pin 1 value
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_CXTSW(SENSORCTRL_CXTS_E mode);

/**
 * \brief	set SensorContrl MCLK Control by Sensor Control or CPU
 *
 * \param[in]	mclkctrl	 MCLK control by Sensor Control or CPU
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_E mclkctrl);

/**
 * \brief	get SensorContrl MCLK Ctrl selection
 *
 * \param[in]	mclkctrl	 MCLK control by Sensor Control or CPU
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_MCLKCtrl(SENSORCTRL_MCLKCTRL_E *mclkctrl);

/**
 * \brief	set Sensor Trigger ctrl 1: cpu, 0:sensor control
 *
 * \param[in]	ctrl	 context switch by Sensor Control or CPU
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_trigger_ctrl(SENSORCTRL_TRIGGER_CTRL_E ctrl);

/**
 * \brief	set Sensor Trigger pin status
 *
 * \param[in]	trigger_val	 trigger pin by cpu
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_trigger_val(uint8_t trigger_val);

/**
 * \brief	set SensorContrl Extend XSLEEP MCLK
 *
 * \param[in]	extxsleepmclk	 Extend XSleep MLCK
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_set_EXTXSLEEPMCLK(SENSORCTRL_EXTEND_XSLEEP_MCLK_E extxsleepmclk);

/**
 * \brief	get SensorContrl Extend XSLEEP MCLK
 *
 * \param[in]	extxsleepmclk	 Extend XSleep MLCK
 * \return	SENSORCTRL_ERROR_E.
 */
SENSORCTRL_ERROR_E hx_drv_sensorctrl_get_EXTXSLEEPMCLK(SENSORCTRL_EXTEND_XSLEEP_MCLK_E *extxsleepmclk);


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* INC_HX_DRV_SENSORCTRL_H_ */
