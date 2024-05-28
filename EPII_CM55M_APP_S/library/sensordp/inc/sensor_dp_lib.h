/**
 ********************************************************************************************
 *  @file      sensor_dp_lib.h
 *  @details   This file contains all sensor datapath related function
 *  @author    himax/902447
 *  @version   V1.0.0
 *  @date      14-May-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/



/**
 * \defgroup	SENSOR_DP_LIB	Sensor Datapath library
 * \ingroup		SENSOR_DP_LIB
 * \brief	Sensor Datapath library Declaration
 * \details Application can use Sensor Datapath library to control almost all cases for datapath\n
 * <pre>
 * For Example
 * Ex1: EP-II Data path memory related address configuration are as follow, you should configure before use datapath
 * 		  a. sensordplib_set_xDMA_baseaddrbyapp(g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
 * 		  b. sensordplib_set_jpegfilesize_addrbyapp(g_jpegautofill_addr);
 * Ex2: Stop Datapath and Software Reset Datapath related API
 * 		  sensordplib_stop_capture();
 * 		  sensordplib_start_swreset();
 * 		  sensordplib_stop_swreset_WoSensorCtrl();
 * Ex3: EP-II RX configuration and change MCLK and xSleep Control owner
 * 		  a. sensordplib_set_sensorctrl_inp(
 * 		  		SENSORDPLIB_SENSOR_E sensor_type,
 * 		  		SENSORDPLIB_STREAM_E type, uint16_t hsize, uint16_t frame_len, INP_SUBSAMPLE_E subsample);
 * 		  b. sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
 * Ex4: Setup Datapath related API
 * 		  a. sensordplib_set_int_hw5x5_jpeg_wdma23(
 *				HW5x5_CFG_T hw5x5_cfg,
 *				JPEG_CFG_T jpeg_cfg,
 *				uint8_t cyclic_buffer_cnt,
 *				sensordplib_CBEvent_t dplib_cb);
 * 		  b. void sensordplib_set_INT1_HWACC(
 *				HW2x2_CFG_T hw2x2_cfg,
 *				CDM_CFG_T cdm_cfg,
 *				HW5x5_CFG_T hw5x5_cfg,
 *				JPEG_CFG_T jpeg_cfg,
 *				uint8_t cyclic_buffer_cnt,
 *				sensordplib_CBEvent_t dplib_cb);
 *	    The datapath related function are non-blocking so you need register callback function in application.
 *	    Then check frame ready event(SENSORDPLIB_STATUS_XDMA_FRAME_READY) if frame ready event is callback you can do CV
 * Ex5: Setup HIMAX AUTO I2C related API (Optional: only for PMU mode)
 * 		a.Set Config: sensordplib_autoi2c_cfg(HXAUTOI2CHC_STATIC_CFG_T scfg,
 * 						HXAUTOI2CHC_INT_CFG_T icfg,
 * 						HXAUTOI2CHC_CMD_CFG_T trig_cfg,
 * 						HXAUTOI2CHC_CMD_CFG_T stop_cfg);
 *
 * 		  scfg.slaveid = CIS_I2C_ID;							//CIS SENSOR I2C ID
 * 		  scfg.clkdiv = HXAUTOI2C_SCL_CLK_DIV_24;				//I2C CLK divider: Source Frequencu is 24M
 * 																//			For I2C CLK 1M, set to HXAUTOI2C_SCL_CLK_DIV_240 (24000000 / 24 = 1000000)
 * 		  scfg.cmdtype = HXAUTOI2CHC_CMD_TRIG_STOP;				// I2C Control CIS SENSOR method:
 * 																// 			Trig means send i2c cmd to stream on CIS SENSOR
 * 																// 			Stop means send i2c cmd to stream off CIS SENSOR
 * 		  scfg.trig_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;		//Trig CMD ctrl setting: set to ctrl by HW
 * 		  scfg.stop_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;     	//Stop CMD ctrl setting: set to ctrl by HW
 *
 *		  //After Trig/Stop CMD, there will be an interrupt to PMU
 * 		  icfg.trig_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;     //Trig CMD interrupt to PMU timing
 * 		  icfg.stop_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;     //Stop CMD interrupt to PMU timing
 * 		  icfg.trig_delay = 0;									//delay time before send Trig CMD interrupt to PMU: 1 unit is 1/24M seconds
 * 		  icfg.stop_delay = 1680;								//delay time before send STOP CMD interrupt to PMU: 1 unit is 1/24M seconds
 *
 * 		  trig_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;				//Trig CMD numbers: 1 means 1 I2C CMD to CIS SENSOR
 * 		  														//                  2 means 2 I2C CMD to CIS SENSOR
 * 		  trig_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Trig CMD 1 I2C CMD length (bytes count)
 * 		  trig_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Trig CMD 2 I2C CMD length (bytes count)
 * 		  trig_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Trig CMD 3 I2C CMD length (bytes count)
 * 		  trig_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Trig CMD 4 I2C CMD length (bytes count)
 * 		  trig_cfg.delay01 = 20000;//40000;						//Trig CMD timing: delay before CMD1 out (1 unit is 1/24M seconds)
 * 		  trig_cfg.delay12 = 0x100;								//Trig CMD timing: delay before CMD2 out (1 unit is 1/24M seconds)
 * 		  trig_cfg.delay23 = 0x100;								//Trig CMD timing: delay before CMD3 out (1 unit is 1/24M seconds)
 * 		  trig_cfg.delay34 = 0x100;								//Trig CMD timing: delay before CMD4 out (1 unit is 1/24M seconds)
 *
 *  	  stop_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;				//Stop CMD numbers: 1 means 1 I2C CMD to CIS SENSOR
 * 		  														//                  2 means 2 I2C CMD to CIS SENSOR
 * 		  stop_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Stop CMD 1 I2C CMD length (bytes count)
 * 		  stop_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Stop CMD 2 I2C CMD length (bytes count)
 * 		  stop_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Stop CMD 3 I2C CMD length (bytes count)
 * 		  stop_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;		//Stop CMD 4 I2C CMD length (bytes count)
 * 		  stop_cfg.delay01 = 0;									//Stop CMD timing: delay before CMD1 out (1 unit is 1/24M seconds)
 * 		  stop_cfg.delay12 = 0x100;								//Stop CMD timing: delay before CMD2 out (1 unit is 1/24M seconds)
 * 		  stop_cfg.delay23 = 0x100;								//Stop CMD timing: delay before CMD3 out (1 unit is 1/24M seconds)
 * 		  stop_cfg.delay34 = 0x100;								//Stop CMD timing: delay before CMD4 out (1 unit is 1/24M seconds)
 *
 *
 * 		b. Set Trig CMD: sensordplib_autoi2c_trigcmd(HXAUTOI2CHC_CMD_T cmd1,
 * 						  	HXAUTOI2CHC_CMD_T cmd2,
 * 							HXAUTOI2CHC_CMD_T cmd3,
 * 							HXAUTOI2CHC_CMD_T cmd4);
 *
 *		   //Trig CMD byte value, if need CMD2, CMD3, CMD4, assign value to trig_cmd2 ,trig_cmd3 ,trig_cmd4
 * 		   trig_cmd1.byte1 = (sensor_default_stream_on[0].RegAddree >> 8 & 0xFF);  	//I2C CMD byte 1 value
 * 		   trig_cmd1.byte2 = (sensor_default_stream_on[0].RegAddree & 0xFF);		//I2C CMD byte 2 value
 * 		   trig_cmd1.byte3 = sensor_default_stream_on[0].Value;					 	//I2C CMD byte 3 value
 * 		   trig_cmd1.byte4 = 0x00;													//I2C CMD byte 4 value
 *
 *
 * 		c. Set Stop CMD: sensordplib_autoi2c_stopcmd(HXAUTOI2CHC_CMD_T cmd1,
 * 						  	HXAUTOI2CHC_CMD_T cmd2,
 * 							HXAUTOI2CHC_CMD_T cmd3,
 * 							HXAUTOI2CHC_CMD_T cmd4);
 *
 *		   //Stop CMD byte value, if need CMD2, CMD3, CMD4, assign value to trig_cmd2 ,trig_cmd3 ,trig_cmd4
 *		   stop_cmd1.byte1 = (sensor_default_stream_off[0].RegAddree >> 8 & 0xFF);	//I2C CMD byte 1 value
 *		   stop_cmd1.byte2 = (sensor_default_stream_off[0].RegAddree & 0xFF);		//I2C CMD byte 2 value
 *		   stop_cmd1.byte3 = sensor_default_stream_off[0].Value;					//I2C CMD byte 3 value
 *		   stop_cmd1.byte4 = 0x00;													//I2C CMD byte 4 value
 *
 *
 *		d. Enable AUTO I2C: sensordplib_autoi2c_enable();
 *		e. Disable AUTO I2C: sensordplib_autoi2c_disable();
 *			After PMU mode, back to ALLON mode, need to disable AUTO I2C
 * Ex6: After all setup are ready, there two method for capturing. One is captured without periodical timer. The Other is captured with periodical timer.
 * 		a. Captured without periodical timer.
 * 		   1. 1st start is start with this API
 * 		      sensordplib_set_sensorctrl_start();
 * 		   2. After 1st frame ready, you can use following API to trigger capture next frame
 * 		      sensordplib_retrigger_capture();
 * 		b. Captured with periodical timer.
 * 		   1. 1st start is start with this API
 * 		      sensordplib_set_sensorctrl_start();
 * 		      sensordplib_set_rtc_start(500);
 * 		   2. After 1st frame ready,
 * 		      a. You want to pause datapath library to capture frame even if timer is fire. You should use the API as follow,
 * 		         sensordplib_set_readyflag(0);
 * 		      b. You want to resume datapath library to capture frame even if timer is fire. You should use the API as follow,
 * 		         sensordplib_set_readyflag(1);
 *
 *
 *
 * </pre>
 */

#ifndef SENSORDPLIB_LIB_H_
#define SENSORDPLIB_LIB_H_

//#include "hx_drv_ckgen.h"
#include "hx_drv_timer.h"
#ifdef IP_timer
#include "timer_interface.h"
#endif

#include "hx_drv_hw2x2.h"
#include "hx_drv_hw5x5.h"
#include "hx_drv_cdm.h"
#include "hx_drv_jpeg.h"
#include "hx_drv_dp.h"
#include "hx_drv_xdma.h"
#include "hx_drv_tpg.h"
#include "hx_drv_inp.h"
#include "hx_drv_inp1bitparser.h"
#include "hx_drv_inpovparser.h"
#include "hx_drv_sensorctrl.h"
#include "hx_drv_rtc.h"
#include "hx_drv_edm.h"
#include "hx_drv_hxautoi2c_mst.h"
#include "hx_drv_csirx.h"
#include "hx_drv_csitx.h"

#include "sensor_dp_config.h"


/**
 * \defgroup	SENSOR_DP_LIB_ENUM	Sensor Datapath Enumeration
 * \ingroup	SENSOR_DP_LIB
 * \brief	Defines some enumeration of Sensor Datapath Library need.
 * @{
 */
/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \enum SENSORDPLIB_PATH_E
 * \brief Datapath selection this enumeration use in Datapath library internal and you can also use in application layer
 */
typedef enum
{
	SENSORDPLIB_PATH_INP_WDMA2,					/*!< Enum Datapath U1: Sensor --> INP --> WDMA2. */
	SENSORDPLIB_PATH_INP_HW2x2_CDM,   			/*!< Enum Datapath U2: Sensor --> INP --> 2x2 block --> CDM --> WDMA1. */
	SENSORDPLIB_PATH_INP_HW5x5,					/*!< Enum Datapath U3: Sensor --> INP--> 5x5--> WDMA3. */
	SENSORDPLIB_PATH_INP_HW5x5_JPEG,  			/*!< Enum Datapath U4: Sensor --> INP --> 5x5 --> JPEG encoder --> WDMA2. */
	SENSORDPLIB_PATH_JPEGDEC,					/*!< Enum Datapath U8: RDMA --> JPEG decoder --> WDMA3. */
	SENSORDPLIB_PATH_INP_HW2x2,					/*!< Enum Datapath D1: Sensor --> INP--> 2x2 block--> WDMA1. */
	SENSORDPLIB_PATH_INP_CDM,					/*!< Enum Datapath D2: Sensor --> INP --> CDM --> WDMA1. */
	SENSORDPLIB_PATH_INT1,						/*!< Enum Datapath U2 + U3 + U4 integrate*/
	SENSORDPLIB_PATH_INTNOJPEG,       			/*!< Enum Datapath U2 + U3 integrate*/
	SENSORDPLIB_PATH_INT3,						/*!< Enum Datapath U1 + U3 integrate*/
	SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG,		/*!< Enum Datapath U3 + U4 integrate*/
	SENSORDPLIB_PATH_TPG_JPEGENC,				/*!< Enum Datapath RDMA --> TPG --> JPEG Enc --> WDMA2*/
	SENSORDPLIB_PATH_TPG_HW2x2,					/*!< Enum Datapath RDMA --> TPG --> HW2x2 --> WDMA1*/
	SENSORDPLIB_PATH_INT_INP_HW2x2_HW5x5_JPEG,	/*!< Enum Datapath D1 + U4 integrate*/
	SENSORDPLIB_PATH_INP_HXCSC_CDM,   			/*!< Enum Datapath U9: Sensor --> INP --> HXCSC --> CDM --> WDMA1. */
	SENSORDPLIB_PATH_INP_HXCSC,					/*!< Enum Datapath U10: Sensor --> INP--> HXCSC-->WDMA3. */
	SENSORDPLIB_PATH_INP_HXCSC_JPEG,  			/*!< Enum Datapath U11: Sensor --> INP --> HXCSC --> JPEG encoder --> WDMA2. */
	SENSORDPLIB_PATH_INT1_CSC,		    		/*!< Enum Datapath U9 + U10 + U11 integrate*/
	SENSORDPLIB_PATH_INTNOJPEG_CSC,     		/*!< Enum Datapath U9 + U10 integrate*/
	SENSORDPLIB_PATH_INT3_CSC,		    		/*!< Enum Datapath U1 + U10 integrate*/
	SENSORDPLIB_PATH_INT_INP_HXCSC_JPEG,		/*!< Enum Datapath U10 + U11 integrate*/
	SENSORDPLIB_PATH_NO,						/*!< Enum Datapath NO*/
}SENSORDPLIB_PATH_E;


/**
 * \enum SENSORDPLIB_SENSOR_E
 * \brief Sensor and output method selection this enumeration use in Datapath library to set INP related setting
 */
typedef enum
{
	SENSORDPLIB_SENSOR_HM1245				= 0,	/*!< HM1245*/
	SENSORDPLIB_SENSOR_HM2140				,	/*!< HM2140*/
	SENSORDPLIB_SENSOR_HM1055				,	/*!< HM1055*/
	SENSORDPLIB_SENSOR_HM01B0_8BITIO		,	/*!< HM01B0_8BIT IO*/
	SENSORDPLIB_SENSOR_HM01B0_4BITIO_LSB	,	/*!< HM01B0_4BIT IO LSB*/
	SENSORDPLIB_SENSOR_HM01B0_4BITIO_MSB	,	/*!< HM01B0_4BIT IO MSB*/
	SENSORDPLIB_SENSOR_HM01B0_1BITIO_LSB	,	/*!< HM01B0_1BIT IO LSB*/
	SENSORDPLIB_SENSOR_HM01B0_1BITIO_MSB	,	/*!< HM01B0_1BIT IO MSB*/
	SENSORDPLIB_SENSOR_HM0360_MODE1			,	/*!< HM0360 Mode1 8bit IO and 8bit data per pixel*/
	SENSORDPLIB_SENSOR_HM0360_MODE2			,	/*!< HM0360 Mode2 8bit IO and 4bit data per pixel*/
	SENSORDPLIB_SENSOR_HM0360_MODE3			,	/*!< HM0360 Mode3 4bit IO and 8bit data per pixel*/
	SENSORDPLIB_SENSOR_HM0360_MODE5			,	/*!< HM0360 Mode5 1bit IO and 8bit data per pixel*/
	SENSORDPLIB_SENSOR_HM11B1_LSB			,	/*!< HM11B1 LSB*/
	SENSORDPLIB_SENSOR_HM11B1_MSB			,	/*!< HM11B1 MSB*/
	SENSORDPLIB_SENSOR_HM0435			    ,	/*!< HM0435*/
	SENSORDPLIB_SENSOR_HM2130               ,   /*!< HM2130*/
    SENSORDPLIB_SENSOR_HM2170_MIPI          ,   /*!< HM2170 MIPI*/
    SENSORDPLIB_SENSOR_HM2170_SERIAL_LSB    ,   /*!< HM2170 SERIAL LSB*/
    SENSORDPLIB_SENSOR_HM2170_SERIAL_MSB    ,   /*!< HM2170 SERIAL MSB*/
	SENSORDPLIB_SENSOR_HM1246               ,   /*!< HM1246*/
	SENSORDPLIB_SENSOR_HM2056               ,   /*!< HM2056*/
	SENSORDPLIB_SENSOR_OV02C10				,	/*!< OV02C10*/
	SENSORDPLIB_SENSOR_HM5532				,   /*!< HM5532*/
	SENSORDPLIB_SENSOR_SOIK351P				,   /*!< SOIK351P*/
}SENSORDPLIB_SENSOR_E;

/**
 * \enum SENSORDPLIB_STREAM_E
 * \brief Sensor stream method selection this enumeration use in Datapath library to set Sensor control related and INP setting
 */
typedef enum
{
	SENSORDPLIB_STREAM_NONEAOS,				/*!< None-AOS Sensor streaming by I2C-Master*/
	SENSORDPLIB_STREAM_HM01B0_CONT,			/*!< HM01B0 Continuous mode streaming by HW-Trigger pin*/
	SENSORDPLIB_STREAM_HM0360_CONT,			/*!< HM0360 Continuous mode streaming by HW-Trigger pin but WE-1 do not output MCLK*/
	SENSORDPLIB_STREAM_HM0360_BURST,		/*!< HM0360 Burst mode streaming by HW-Trigger pin but WE-1 do not output MCLK*/
	SENSORDPLIB_STREAM_HM0360_SENSOR_ACT,  	/*!< 5.1.4	Senctrl Mode 4: Automatic wake-up mode(sensor active) - HM0360(Motion detect)*/
	SENSORDPLIB_STREAM_HM0360_WE1_ACT,		/*!< 5.1.5	Senctrl Mode 5: Automatic wake-up mode(WE-I ACTIVE) - HM0360(Automatic output frame and sleep)*/
	SENSORDPLIB_STREAM_HM0360_CONT_MCLK,    /*!< HM0360 Continuous mode streaming by HW-Trigger pin and WE-1 output MCLK*/
	SENSORDPLIB_STREAM_HM0360_BURST_MCLK,   /*!< HM0360 Burst mode streaming by HW-Trigger pin and WE-1 output MCLK*/
	SENSORDPLIB_STREAM_HM11B1_LOWPOWER,      /*!< HM11B1 Continuous mode streaming by HW-xSleep pin*/
	SENSORDPLIB_STREAM_HM0360_XSLEEP_ONLY,    /*!< HM0360 streaming by HW-xSleep pin, WE-1 use xSleep pin Chip A not support, Chip C Support*/
	SENSORDPLIB_STREAM_HM0360_XSLEEP_USE_WE1_TRIG,    /*!< HM0360 streaming by HW-xSleep pin, WE-1 use Trigger pin Chip A not support, Chip B Support*/
	SENSORDPLIB_STREAM_NONEAOS_AUTOI2C,		/*!< HM01B0 Continuous mode streaming by HW-Trigger pin but tie HW-Trigger to low*/
	SENSORDPLIB_STREAM_NONEAOS_AUTOI2C_XSLEEP_HIGH,	/*!< HM01B0 Continuous mode streaming by HW-Trigger pin but tie HW-Trigger to low XSLEEP to high*/
	SENSORDPLIB_STREAM_NONEAOS_AUTOI2C_SOI,	 /*!< HM01B0 Continuous mode streaming by HW-Trigger pin but tie HW-Trigger to low XSLEEP keep high*/
}SENSORDPLIB_STREAM_E;

/**
 * \enum SENSORDPLIB_CB_FUNTYPE_E
 * \brief Sensor Datapath support 4 types callback function at the same time. Use this enumeration to select which callback function you want to register
 */
typedef enum
{
	SENSORDPLIB_CB_FUNTYPE_DP,      /*!< Datapath interrupt callback Sensor ALL HW-ACC without RS, HOG, JPEG Dec such as frame ready or error event*/
	SENSORDPLIB_CB_FUNTYPE_RS,		/*!< Datapath interrupt callback Resampler such as frame ready or error event*/
	SENSORDPLIB_CB_FUNTYPE_HOG,		/*!< Datapath interrupt callback HOG such as frame ready or error event*/
	SENSORDPLIB_CB_FUNTYPE_JPEGDEC, /*!< Datapath interrupt callback JPEG Decoder such as frame ready or error event*/
}SENSORDPLIB_CB_FUNTYPE_E;

/**
 * \enum SENSORDPLIB_STATUS_E
 * \brief Sensor Datapath interrupt callback event
 */
typedef enum
{
	SENSORDPLIB_STATUS_ERR_FS_HVSIZE        	= -108,  /*!< [8] reg_inpparser_fs_hsize_vsize_error_int_en*/
	SENSORDPLIB_STATUS_ERR_FE_TOGGLE        	= -107,  /*!< [7] reg_inpparser_wait_fe_toggle_error_int_en*/
	SENSORDPLIB_STATUS_ERR_FD_TOGGLE        	= -106,	 /*!< [6] reg_inpparser_wait_fd_toggle_error_int_en*/
	SENSORDPLIB_STATUS_ERR_FS_TOGGLE      		= -105,	 /*!< [5] reg_inpparser_wait_fs_toggle_error_int_en*/
	SENSORDPLIB_STATUS_ERR_BLANK_ERR        	= -104,  /*!< [4] reg_inpparser_blank_toggle_error_int_en*/
	SENSORDPLIB_STATUS_ERR_CRC_ERR        		= -103,  /*!< [3] reg_inpparser_crc_error_int_en*/
	SENSORDPLIB_STATUS_ERR_FE_ERR        		= -102,  /*!< [2] reg_inpparser_fe_cycle_error_int_en*/
	SENSORDPLIB_STATUS_ERR_HSIZE_ERR        	= -101,  /*!< [1] reg_inpparser_hsize_error_int_en*/
	SENSORDPLIB_STATUS_ERR_FS_ERR           	= -100,  /*!< [0] reg_inpparser_fs_cycle_error_int_en*/

	SENSORDPLIB_STATUS_VSYNC					= -95,	/*!< EDM [0]:vsync_active error*/
	SENSORDPLIB_STATUS_HSYNC					= -94,	/*!< EDM [1]:hsync_active error*/
	SENSORDPLIB_STATUS_OVERLAP_VH				= -93,	/*!< EDM [2]:overlap_vh error*/
	SENSORDPLIB_STATUS_OVERLAP_HD				= -92,	/*!< EDM [3]:overlap_hd error*/
	SENSORDPLIB_STATUS_OVERLAP_DF				= -91,	/*!< EDM [4]:overlap_df error*/
	SENSORDPLIB_STATUS_HSYNC_FP					= -90,	/*!< EDM [5]:hsync_front_porch error*/
	SENSORDPLIB_STATUS_HSYNC_BP					= -89,	/*!< EDM [6]:hsync_back_porch error*/
	SENSORDPLIB_STATUS_FE						= -88,	/*!< EDM [7]:frame_end_timing error*/
	SENSORDPLIB_STATUS_CON_VSYNC				= -87,	/*!< EDM [8]:con_vsync_active error*/
	SENSORDPLIB_STATUS_CON_HSYNC				= -86,	/*!< EDM [9]:con_hsync_active error*/
	SENSORDPLIB_STATUS_CH0_DE_LESS				= -85,	/*!< EDM [10]:ch0_de_less error*/
	SENSORDPLIB_STATUS_CH0_DE_MORE				= -84,	/*!< EDM [11]:ch0_de_more error*/
	SENSORDPLIB_STATUS_CH1_DE_LESS				= -83,	/*!< EDM [12]:ch1_de_less error*/
	SENSORDPLIB_STATUS_CH1_DE_MORE				= -82,	/*!< EDM [13]:ch1_de_more error*/
	SENSORDPLIB_STATUS_CH2_DE_LESS				= -81,	/*!< EDM [14]:ch2_de_less error*/
	SENSORDPLIB_STATUS_CH2_DE_MORE				= -80,	/*!< EDM [15]:ch2_de_more error*/
	SENSORDPLIB_STATUS_CONV_DE_LESS				= -79,	/*!< EDM [16]:con_de_less error*/
	SENSORDPLIB_STATUS_CONV_DE_MORE				= -78,	/*!< EDM [17]:con_de_more error*/
	SENSORDPLIB_STATUS_EDM_WDT3_TIMEOUT			= -77,	/*!< EDM WDT3 Timeout*/
	SENSORDPLIB_STATUS_EDM_WDT2_TIMEOUT			= -76,	/*!< EDM WDT2 Timeout*/
	SENSORDPLIB_STATUS_EDM_WDT1_TIMEOUT			= -75,	/*!< EDM WDT1 Timeout*/

	SENSORDPLIB_STATUS_SENSORCTRL_WDT_OUT		= -70,	/*!< Sensor Control Timeout (not used in current code)*/

	SENSORDPLIB_STATUS_CDM_FIFO_OVERFLOW		= -60,	/*!< CDM Abnormal OVERFLOW*/
	SENSORDPLIB_STATUS_CDM_FIFO_UNDERFLOW		= -61,	/*!< CDM Abnormal UnderFLOW */

	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL1		= -50,	/*!< XDMA_WDMA1STATUS_ERR_FE_COUNT_NOT_REACH */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL2		= -51,	/*!< XDMA_WDMA1STATUS_ERR_DIS_BEFORE_FINISH */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL3		= -52,	/*!< XDMA_WDMA1STATUS_ERR_FIFO_CH1_MISMATCH */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL4		= -53,	/*!< XDMA_WDMA1STATUS_ERR_FIFO_CH2_MISMATCH */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL5		= -54,	/*!< XDMA_WDMA1STATUS_ERR_FIFO_CH3_MISMATCH*/
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL6		= -55,	/*!< XDMA_WDMA1STATUS_ERR_FIFO_CH1_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL7		= -56,	/*!< XDMA_WDMA1STATUS_ERR_FIFO_CH2_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL8		= -57,	/*!< XDMA_WDMA1STATUS_ERR_FIFO_CH3_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL9		= -58,	/*!< XDMA_WDMA1STATUS_ERR_BUS */

	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL1		= -40,	/*!< XDMA_WDMA2STATUS_ERR_FE_COUNT_NOT_REACH */
	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL2		= -41,	/*!< XDMA_WDMA2STATUS_ERR_DIS_BEFORE_FINISH */
	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL3		= -42,	/*!< XDMA_WDMA2STATUS_ERR_FIFO_MISMATCH */
	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL4		= -43,	/*!< XDMA_WDMA2STATUS_ERR_FIFO_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL5		= -44,	/*!< XDMA_WDMA2STATUS_ERR_BUS */
	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL6		= -45,	/*!< WDMA2 Abnormal Case6 */
	SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL7		= -46,	/*!< WDMA2 Abnormal Case7 */

	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL1		= -30,	/*!< XDMA_WDMA3STATUS_ERR_FE_COUNT_NOT_REACH */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL2		= -31,	/*!< XDMA_WDMA3STATUS_ERR_DIS_BEFORE_FINISH */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL3		= -32,	/*!< XDMA_WDMA3STATUS_ERR_FIFO_CH1_MISMATCH */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL4		= -33,	/*!< XDMA_WDMA3STATUS_ERR_FIFO_CH2_MISMATCH */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL5		= -34,	/*!< XDMA_WDMA3STATUS_ERR_FIFO_CH3_MISMATCH */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL6		= -35,	/*!< XDMA_WDMA3STATUS_ERR_FIFO_CH1_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL7		= -36,	/*!< XDMA_WDMA3STATUS_ERR_FIFO_CH2_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL8		= -37,	/*!< XDMA_WDMA3STATUS_ERR_FIFO_CH3_OVERFLOW */
	SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL9		= -38,	/*!< XDMA_WDMA3STATUS_ERR_BUS */

	SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL1		= -20,	/*!< XDMA_RDMASTATUS_ERR_DIS_BEFORE_FINISH */
	SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL2		= -21,	/*!< XDMA_RDMASTATUS_ERR_BUS */
	SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL3		= -22,	/*!< RDMA Abnormal Case3 */
	SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL4		= -23,	/*!< RDMA Abnormal Case4 */
	SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL5		= -24,	/*!< RDMA Abnormal Case5 */

	SENSORDPLIB_STATUS_RSDMA_ABNORMAL1			= -10,	/*!< RSDMA_INTSTATUS_RDMA_FIFO_ERROR */
	SENSORDPLIB_STATUS_RSDMA_ABNORMAL2			= -11,	/*!< RSDMA_INTSTATUS_WDMA_FIFO_ERROR */
	SENSORDPLIB_STATUS_RSDMA_ABNORMAL3			= -12,	/*!< RSDMA_INTSTATUS_NOGETFE_FINISH */
	SENSORDPLIB_STATUS_RSDMA_ABNORMAL4			= -13,	/*!< RSDMA_INTSTATUS_NOSENDFE_FINISH */
	SENSORDPLIB_STATUS_RSDMA_ABNORMAL5			= -14,	/*!< RSDMA_INTSTATUS_NOBOTHFE_FINISH */
	SENSORDPLIB_STATUS_RSDMA_ABNORMAL6			= -15,	/*!< RSDMA Abnormal Case6 */
	SENSORDPLIB_STATUS_RSDMA_ABNORMAL7			= -16,	/*!< RSDMA Abnormal Case7 */

	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL1			= -1,	/*!< HOGDMA_INTSTATUS_CH1_RDMA_FIFO_ERR */
	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL2			= -2,	/*!< HOGDMA_INTSTATUS_CH2_RDMA_FIFO_ERR */
	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL3			= -3,	/*!< HOGDMA_INTSTATUS_CH3_RDMA_FIFO_ERR */
	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL4			= -4,	/*!< HOGDMA_INTSTATUS_CH4_WDMA_FIFO_ERR */
	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL5			= -5,	/*!< HOGDMA_INTSTATUS_NOGETFE_FINISH */
	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL6			= -6,	/*!< HOGDMA_INTSTATUS_NOSENDFE_FINISH */
	SENSORDPLIB_STATUS_HOGDMA_ABNORMAL7			= -7,	/*!< HOGDMA_INTSTATUS_NOBOTHFE_FINISH */

	SENSORDPLIB_STATUS_CDM_MOTION_DETECT		= 0,	/*!< CDM Motion Detect */
	SENSORDPLIB_STATUS_XDMA_WDMA1_FINISH		= 1,	/*!< xDMA1 WDMA1 FINISH */
	SENSORDPLIB_STATUS_XDMA_WDMA2_FINISH		= 2,	/*!< xDMA1 WDMA2 FINISH */
	SENSORDPLIB_STATUS_XDMA_WDMA3_FINISH		= 3,	/*!< xDMA1 WDMA3 FINISH */
	SENSORDPLIB_STATUS_XDMA_RDMA_FINISH			= 4,	/*!< xDMA1 RDMA FINISH */

	SENSORDPLIB_STATUS_XDMA_FRAME_READY			= 5,	/*!< xDMA1 Frame Ready by Path*/

	SENSORDPLIB_STATUS_RSDMA_FINISH			    = 6,	/*!< RSDMA Normal finish */
	SENSORDPLIB_STATUS_HOGDMA_FINISH			= 7,	/*!< HOGDMA Normal finish */
	SENSORDPLIB_STATUS_TIMER_FIRE_APP_NOTREADY	= 8,	/*!< Timer Fire but app not ready for frame */
	SENSORDPLIB_STATUS_TIMER_FIRE_APP_READY		= 9,	/*!< Timer Fire and app ready for frame */
} SENSORDPLIB_STATUS_E;

/**
 * \enum SENSORDPLIB_POWERSWITCH_CTRL_E
 * \brief Power State in DC or control by PMU or CPU
 */
typedef enum
{
	SENSORDPLIB_POWERSWITCH_CTRL_DC_PMU, /*!< Power State at DC or Power Switch control by PMU */
	SENSORDPLIB_POWERSWITCH_CTRL_CPU		/*!< Power State control by CPU */
}SENSORDPLIB_POWERSWITCH_CTRL_E;
/** @} */


/****************************************************
 * Structure Definition                             *
 ***************************************************/
/**
 * \defgroup	SENSOR_DP_LIB_REGSTRUCT		Sensor Datapath Library Structure
 * \ingroup	SENSOR_DP_LIB
 * \brief	contains definitions of Sensor Datapath library structure.
 * @{
 */
/**
 * \struct SENSORDPLIB_HOGDMA_CFG_T
 * \brief HOG DMA configuration by sensor datapath library
 */
typedef struct
{
	uint32_t wdma_startaddr;		/*!< HOG WDMA Destination memory start address */
	uint32_t rdma_ch1_startaddr;    /*!< HOG RDMA Y Channel Source memory start address */
	uint32_t rdma_ch2_startaddr;	/*!< HOG RDMA U Channel Source memory start address */
	uint32_t rdma_ch3_startaddr;	/*!< HOG RDMA V Channel Source memory start address */
} SENSORDPLIB_HOGDMA_CFG_T;

/**
 * \struct SENSORDPLIB_HM11B1_HEADER_T
 * \brief HM11B1 for 1bitparser INP information
 */
typedef struct
{
	INP_1BITPARSER_FSM_E fsm;		/*!< 1bit INP parser fsm state for debug purpose */
	uint16_t hw_hsize;				/*!< 1bit INP parser HW calculation HSIZE */
	uint16_t hw_vsize;				/*!< 1bit INP parser HW calculation VSIZE */
	uint16_t sensor_hsize;			/*!< 1bit INP parser Decode Sensor Frame Header HSIZE */
	uint16_t sensor_vsize;			/*!< 1bit INP parser Decode Sensor Frame Header VSIZE */
	uint16_t frame_len;				/*!< 1bit INP parser Decode Sensor Frame Header FrameLen */
	uint16_t line_len;				/*!< 1bit INP parser Decode Sensor Frame Header LineLen */
	uint8_t again;					/*!< 1bit INP parser Decode Sensor Frame Header Again */
	uint16_t dgain;					/*!< 1bit INP parser Decode Sensor Frame Header dgain */
	uint16_t intg;					/*!< 1bit INP parser Decode Sensor Frame Header Coarse Integration time */
	uint8_t intsrc;					/*!< 1bit INP parser Decode Sensor Frame Header INT SRC */
	uint16_t fstus;					/*!< 1bit INP parser Decode Sensor Frame Header FSTUS */
	uint16_t fc;					/*!< 1bit INP parser Decode Sensor Frame Header Frame Counter */
	uint16_t sensor_crc;			/*!< 1bit INP parser Decode Sensor Frame Header Frame CRC */
	uint16_t hw_crc;				/*!< 1bit INP parser HW calculation Frame CRC */
	uint16_t err_status;			/*!< 1bit INP parser HW error status */
	uint16_t fs_cycle;				/*!< 1bit INP parser HW detect FS cycle count (it should be 128 cycle)*/
	uint16_t fe_cycle;				/*!< 1bit INP parser HW detect FE cycle count (it should be 16 cycle)*/
	uint16_t fs_cycle_err_cnt;		/*!< 1bit INP parser HW detect FS cycle count error times for debug purpose */
} SENSORDPLIB_HM11B1_HEADER_T;

/**
 * \struct SENSORDPLIB_RDMA_CFG_T
 * \brief Sensor DPLib TPG path with RDMA configuration for memory address
 */
typedef struct
{
	uint32_t rdma_ch1_startaddr;	/*!< TPG RDMA Source channel 1 memory start address */
	uint32_t rdma_ch2_startaddr;	/*!< TPG RDMA Source channel 2 memory start address */
	uint32_t rdma_ch3_startaddr;	/*!< TPG RDMA Source channel 3 memory start address */
} SENSORDPLIB_RDMA_CFG_T;
/** @} */


/**
 * \defgroup	SENSOR_DP_LIB_FUNCDLR	Sensor Datapath library Function Declaration
 * \ingroup	SENSOR_DP_LIB
 * \brief	Contains declarations of Sensor Datapath library functions.
 * @{
 */
/**
 * \brief Sensor DP library callback function type definition
 *
 * \param[in] event dp library callback event
 * \retval void.
 */
typedef void (*sensordplib_CBEvent_t) (SENSORDPLIB_STATUS_E event);
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief	set INP & Sensor control related setting
 *
 * set INP & Sensor control related setting and edm configuration
 * \param[in]	sensor_type	Support Sensor ID
 * \param[in]	type	sensor control streaming type
 * \param[in]	hsize	sensor Hsize for INP
 * \param[in] 	frame_len	sensor frame len for INP
 * \param[in] 	subsample   inp subsample
 * \retval	0	success
 * \retval	-1	fail
 */
int sensordplib_set_sensorctrl_inp(
		SENSORDPLIB_SENSOR_E sensor_type,
		SENSORDPLIB_STREAM_E type, uint16_t hsize, uint16_t frame_len, INP_SUBSAMPLE_E subsample);

/**
 * \brief   set INP & Sensor control related setting
 *
 * set INP & Sensor control related setting, edm configuration and enable CROP
 * \param[in]   sensor_type Support Sensor ID
 * \param[in]   type    sensor control streaming type
 * \param[in]   hsize   sensor Hsize for INP
 * \param[in]   frame_len   sensor frame len for INP
 * \param[in]   subsample   inp subsample
 * \param[in]   crop        inp crop setting
 * \retval  0   success
 * \retval  -1  fail
 */
int sensordplib_set_sensorctrl_inp_wi_crop(
        SENSORDPLIB_SENSOR_E sensor_type,
        SENSORDPLIB_STREAM_E type, uint16_t hsize, uint16_t frame_len, INP_SUBSAMPLE_E subsample, INP_CROP_T crop);

#if(IC_VERSION >= 30)
/**
 * \brief   set INP & Sensor control related setting
 *
 * set INP & Sensor control related setting, edm configuration and CROP and BINNING
 * \param[in]   sensor_type Support Sensor ID
 * \param[in]   type    sensor control streaming type
 * \param[in]   hsize   sensor Hsize for INP
 * \param[in]   frame_len   sensor frame len for INP
 * \param[in]   subsample   inp subsample
 * \param[in]   crop        inp crop setting
 * \param[in]   binmode     inp binning setting
 * \retval  0   success
 * \retval  -1  fail
 */
int sensordplib_set_sensorctrl_inp_wi_crop_bin(
		SENSORDPLIB_SENSOR_E sensor_type, SENSORDPLIB_STREAM_E type,
		uint16_t hsize, uint16_t frame_len, INP_SUBSAMPLE_E subsample, INP_CROP_T crop, INP_BINNING_E binmode);
#endif

/**
 * \brief   set INP & Sensor control related setting
 *
 * set INP & Sensor control related setting, edm configuration and enable CROP
 * \param[in]   sensor_type Support Sensor ID
 * \param[in]   type    sensor control streaming type
 * \param[in]   hsize   sensor Hsize for INP
 * \param[in]   frame_len   sensor frame len for INP
 * \param[in]   subsample   inp subsample
 * \param[in]   h_fporch    H FPORCH for INP
 * \retval  0   success
 * \retval  -1  fail
 */
int sensordplib_set_sensorctrl_inp_serial_pack2l(
        SENSORDPLIB_SENSOR_E sensor_type,
        SENSORDPLIB_STREAM_E type, uint16_t hsize, uint16_t frame_len, INP_SUBSAMPLE_E subsample, uint16_t h_fporch);


/**
 * \brief	set Sensor control start capture that streaming mode is not SENSORDPLIB_STREAM_HM0360_SENSOR_ACT and SENSORDPLIB_STREAM_HM0360_WE1_ACT and start EDM Watch dog Timer
 *
 * \retval	0	success
 * \retval	-1	fail
 */
int sensordplib_set_sensorctrl_start();

/**
 * \brief	set RTC start that streaming mode is not SENSORDPLIB_STREAM_HM0360_SENSOR_ACT and SENSORDPLIB_STREAM_HM0360_WE1_ACT
 *
 * \param[in] time_interval: RTC Timer interval (unit:ms)
 * \retval	0	success
 * \retval	-1	fail
 */
int sensordplib_set_rtc_start(uint32_t time_interval);

/**
 * \brief	sensor --> INP --> WDMA2 setup
 *
 * \param[in]	width	HW-ACC input width
 * \param[in]	height	HW-ACC input height
 * \param[in]	dplib_cb	callback function for datapath library callback
 * \retval	void.
 */
void sensordplib_set_raw_wdma2(
		uint16_t width, uint16_t height
		, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief	sensor --> INP --> WDMA2 setup
 *
 * \param[in]	width	HW-ACC input width
 * \param[in]	height	HW-ACC input height
 * \param[in]	cyclic_buffer_cnt: RAW cyclic buffer total slots
 * \param[in]	cyclic_buffer_start_no: Specific the cyclic buffer index to start putting images.
 *										Use hx_drv_xdma_get_WDMA2NextFrameIdx to get the start index
 *										before re-starting the datapath.
 * \param[in]	dplib_cb	callback function for datapath library callback
 * \retval	void.
 */
void sensordplib_set_raw_wdma2_start_no(
		uint16_t width, uint16_t height
		, uint8_t cyclic_buffer_cnt
		, uint8_t cyclic_buffer_start_no
		, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> INP --> HW5x5 --> WDMA3 setup
 *
 * \param 	hw5x5_cfg	HW5x5 HW-ACC configuration
 * \param 	dplib_cb	callback function for datapath library callback
 * \retval	void.
 */
void sensordplib_set_hw5x5_wdma3(
		HW5x5_CFG_T hw5x5_cfg
		, sensordplib_CBEvent_t dplib_cb);


/**
 * \brief sensor --> INP --> HXCSC(+HXRGB2YUV) --> WDMA3 setup
 *
 * \param[in]	width	HW-ACC input width
 * \param[in]	height	HW-ACC input height
 * \param 	hXCSC_cfg	HXCSC HW-ACC configuration
 * \param 	dplib_cb	callback function for datapath library callback
 * \retval	void.
 */
void sensordplib_set_hxcscYUV_wdma3(
		uint16_t width, uint16_t height,
		HXCSC_CFG_T hxcsc_cfg, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> INP --> HXCSC --> WDMA3 setup
 *
 * \param[in]	width	HW-ACC input width
 * \param[in]	height	HW-ACC input height
 * \param 	hXCSC_cfg	HXCSC HW-ACC configuration
 * \param 	dplib_cb	callback function for datapath library callback
 * \retval	void.
 */
void sensordplib_set_hxcsc_wdma3(
		uint16_t width, uint16_t height,
		HXCSC_CFG_T hxcsc_cfg, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> INP --> HW2x2 --> WDMA1
 *
 * \param[in] hw2x2_cfg: HW2x2 HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_HW2x2_wdma1(
		HW2x2_CFG_T hw2x2_cfg
		, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> INP --> CDM--> WDMA1
 *
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_CDM(
		CDM_CFG_T cdm_cfg
		, sensordplib_CBEvent_t dplib_cb);


/**
 * \brief sensor --> INP --> HW2x2  --> CDM--> WDMA1
 *
 * \param[in] hw2x2_cfg: HW2x2 HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_HW2x2_CDM(
		HW2x2_CFG_T hw2x2_cfg, CDM_CFG_T cdm_cfg
		, sensordplib_CBEvent_t dplib_cb);


/**
 * \brief sensor --> INP --> HXCSC --> CDM--> WDMA1
 *
 * \param[in] hxcsc_cfg: HXCSC HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_HXCSC_CDM(
		HXCSC_CFG_T hxcsc_cfg, CDM_CFG_T cdm_cfg
		, sensordplib_CBEvent_t dplib_cb);


/**
 * \brief sensor --> INP --> HW5x5 --> JPEGENC --> WDMA2
 *
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_hw5x5_jpeg_wdma2(
		HW5x5_CFG_T hw5x5_cfg
		, JPEG_CFG_T jpeg_cfg
		, uint8_t cyclic_buffer_cnt
		,sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> INP --> HW5x5 --> JPEGENC --> WDMA2
 *
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \param[in] cyclic_buffer_start_no: Specific the cyclic buffer index to start putting images.
 *									  Use hx_drv_xdma_get_WDMA2NextFrameIdx to get the start index
 *									  before re-starting the datapath.
 * \retval void.
 */
void sensordplib_set_hw5x5_jpeg_wdma2_start_no(
		HW5x5_CFG_T hw5x5_cfg
		, JPEG_CFG_T jpeg_cfg
		, uint8_t cyclic_buffer_cnt
		, uint8_t cyclic_buffer_start_no
		,sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> INP --> HXCSC(+RGB2YUV) --> JPEGENC --> WDMA2
 *
 * \param[in] hxcsc_cfg: HHXCSC HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_hxcsc_jpeg_wdma2(
		HXCSC_CFG_T hxcsc_cfg
		, JPEG_CFG_T jpeg_cfg
		, uint8_t cyclic_buffer_cnt
		,sensordplib_CBEvent_t dplib_cb);


/**
 * \brief RDMA--> TPG --> JPEG --> WDMA3
 *
 * \param[in] jpegdec_cfg: JPEG Decoder HW-ACC configuration
 * \param[in] in_width: JPEG bitstream full frame input width
 * \param[in] in_height: JPEG bitstream full frame input height
 * \param[in] frame_no: JPEG cyclic buffer slot number (from 0). If target is cyclic 10 frame, frame no is 0 ~ 9
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_jpegdec(JPEG_CFG_T jpegdec_cfg,
		uint16_t in_width, uint16_t in_height,
		uint32_t frame_no,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief RDMA--> TPG --> JPEG --> WDMA3
 *
 * \param[in] jpegdec_cfg: JPEG Decoder HW-ACC configuration
 * \param[in] in_width: JPEG bitstream full frame input width
 * \param[in] in_height: JPEG bitstream full frame input height
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_jpegdec_1stframe(JPEG_CFG_T jpegdec_cfg,
		uint16_t in_width, uint16_t in_height,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief RS RDMA --> RS --> RS WDMA
 *
 * \param[in] in_width: Resampler input width
 * \param[in] in_height: Resampler input height
 * \param[in] st_x: Resampler ROI start x position
 * \param[in] st_y: Resampler ROI start y position
 * \param[in] crop_width: Resampler ROI width
 * \param[in] crop_height: Resampler ROI height
 * \param[in] mode: Resampler method
 * \param[in] ratio: Resampler ratio
 * \param[in] rs_round_mode: Resampler downscale rounding method
 * \param[in] rs_rdma_startaddr: Resampler input source pattern start memory address
 * \param[in] rs_wdma_startaddr: Resampler output source pattern start memory address
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
//void sensordplib_set_RS(uint16_t in_width, uint16_t in_height,
//		uint16_t st_x, uint16_t st_y,
//		uint16_t crop_width, uint16_t crop_height
//		,RS_SCALEMODE_E mode, RS_SCALERATIO_E ratio
//		,RS_ROUNDMODE_E rs_round_mode
//		,uint32_t rs_rdma_startaddr
//		,uint32_t rs_wdma_startaddr
//		,sensordplib_CBEvent_t dplib_cb);

/**
 * \brief HOG RDMA --> HOG(ROI) --> HOG WDMA
 *
 * HOG ROI do in HOG HW-ACC
 * Timing constraint st_x should be multiple by 2
 * Timing constraint st_y should be multiple by 2
 * Timing constraint roi_width should be multiple by 2
 * Timing constraint roi_height should be multiple by 2
 *
 * \param[in] hog_cfg: HOG HW-ACC related configuration
 * \param[in] dma_cfg: HOG DMA related configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval int. 0: success
 * \retval int. -1: fail
 */
//int sensordplib_set_HOG_HWACC_Crop(HOG_CFG_T hog_cfg, SENSORDPLIB_HOGDMA_CFG_T dma_cfg, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief HOG RDMA(ROI)--> HOG --> HOG WDMA but do not send extra boundary data to HOG
 *
 * HOG ROI do in HOG DMAC but do not send extra boundary data to HOG
 * Timing constraint st_x should be multiple by 8 in YUV420 type.
 * Timing constraint st_x should be multiple by 4 in Mono type.
 * Timing constraint st_y should be multiple by 2
 * Timing constraint roi_width should be multiple by 8 in YUV420 type
 * Timing constraint roi_width should be multiple by 4 in Mono TYpe
 * Timing constraint roi_height should be multiple by 2
 *
 * \param[in] hog_cfg: HOG HW-ACC related configuration
 * \param[in] dma_cfg: HOG DMA related configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval int. 0: success
 * \retval int. -1: fail
 */
//int sensordplib_set_HOG_DMACropNoExtraBND(HOG_CFG_T hog_cfg, SENSORDPLIB_HOGDMA_CFG_T dma_cfg, sensordplib_CBEvent_t dplib_cb);

/**
 * \brief HOG RDMA(ROI)--> HOG --> HOG WDMA and send extra boundary data to HOG
 *
 *  HOG ROI do in HOG DMAC and send extra boundary data to HOG
 * Timing constraint st_x should be multiple by 8 in YUV420 type.
 * Timing constraint st_x should be multiple by 4 in Mono type.
 * Timing constraint st_y should be multiple by 2
 * Timing constraint roi_width should be multiple by 8 in YUV420 type
 * Timing constraint roi_width should be multiple by 4 in Mono TYpe
 * Timing constraint roi_height should be multiple by 2
 *
 * \param[in] hog_cfg: HOG HW-ACC related configuration
 * \param[in] dma_cfg: HOG DMA related configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval int. 0: success
 * \retval int. -1: fail
 */
//int sensordplib_set_HOG_DMACropExtraBND(HOG_CFG_T hog_cfg, SENSORDPLIB_HOGDMA_CFG_T dma_cfg, sensordplib_CBEvent_t dplib_cb);


/**
 * \brief sensor --> datapath INT_NoJPEG setup
 *
 * Sensor --> INP --> HW2x2 --> CDM --> WDMA1
 * Sensor --> INP --> HW5x5 --> WDMA3
 *
 * \param[in] hw2x2_cfg: HW2x2 HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_INTNoJPEG_HWACC(
		HW2x2_CFG_T hw2x2_cfg,
		CDM_CFG_T cdm_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INTCSC_NoJPEG setup
 *
 * Sensor --> INP --> HXCSC --> CDM --> WDMA1
 * Sensor --> INP --> HXCSC --> WDMA3
 *
 * \param[in] hxcsc_cfg: HXCSC HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_INTCSCNoJPEG_HWACC(
		HXCSC_CFG_T hxcsc_cfg,
		CDM_CFG_T cdm_cfg,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT1 setup
 *
 * Sensor --> INP --> HW2x2 --> CDM --> WDMA1
 * Sensor --> INP --> HW5x5(YUV) --> JPEG Enc --> WDMA2
 * Sensor --> INP --> HW5x5(YUV) --> WDMA3
 *
 * \param[in] hw2x2_cfg: HW2x2 HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Encoder HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_INT1_HWACC(
		HW2x2_CFG_T hw2x2_cfg,
		CDM_CFG_T cdm_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT1 setup
 *
 * Sensor --> INP --> HW2x2 --> CDM --> WDMA1
 * Sensor --> INP --> HW5x5 --> JPEG Enc --> WDMA2
 * Sensor --> INP --> HW5x5 --> WDMA3
 *
 * \param[in] hw2x2_cfg: HW2x2 HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Encoder HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] cyclic_buffer_start_no: Specific the cyclic buffer index to start putting images.
 *									  Use hx_drv_xdma_get_WDMA2NextFrameIdx to get the start index
 *									  before re-starting the datapath.
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_INT1_HWACC_start_no(
		HW2x2_CFG_T hw2x2_cfg,
		CDM_CFG_T cdm_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		uint8_t cyclic_buffer_start_no,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT1 setup
 *
 * Sensor --> INP --> HW2x2 --> CDM --> WDMA1
 * Sensor --> INP --> HW5x5(RGB) --> JPEG Enc --> WDMA2
 * Sensor --> INP --> HW5x5(RGB) --> WDMA3
 *
 * \param[in] hw2x2_cfg: HW2x2 HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Encoder HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_INT1RGB_HWACC(
        HW2x2_CFG_T hw2x2_cfg,
        CDM_CFG_T cdm_cfg,
        HW5x5_CFG_T hw5x5_cfg,
        JPEG_CFG_T jpeg_cfg,
        uint8_t cyclic_buffer_cnt,
        sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT1CSC setup
 *
 * Sensor --> INP --> HXCSC --> CDM --> WDMA1
 * Sensor --> INP --> HXCSC --> JPEG Enc --> WDMA2
 * Sensor --> INP --> HXCSC --> WDMA3
 *
 * \param[in] hxcsc_cfg: HXCSC HW-ACC configuration
 * \param[in] cdm_cfg: CDM HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Encoder HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_INT1CSC_HWACC(
		HXCSC_CFG_T hxcsc_cfg,
		CDM_CFG_T cdm_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT3_Txx setup
 *
 * Sensor --> INP --> WDMA2
 * Sensor --> INP --> HW5x5 --> WDMA3
 *
 * \param[in] width: input width for WDMA2
 * \param[in] height: input height for WDMA2
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_int_raw_hw5x5_wdma23(
		uint16_t width, uint16_t height,
		HW5x5_CFG_T hw5x5_cfg,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT3_Txx setup
 *
 * Sensor --> INP --> WDMA2
 * Sensor --> INP --> HXCSC --> WDMA3
 *
 * \param[in] width: input width for WDMA2
 * \param[in] height: input height for WDMA2
 * \param[in] hxcsc_cfg: HXCSC HW-ACC configuration
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_int_raw_hxcsc_wdma23(
		uint16_t width, uint16_t height,
		HXCSC_CFG_T hxcsc_cfg,
		sensordplib_CBEvent_t dplib_cb);


/**
 * \brief sensor --> datapath INT4_Txx setup
 *
 * Sensor --> INP --> HW5x5(YUV) --> JPEG Enc
 * Sensor --> INP --> HW5x5(YUV) --> WDMA3
 *
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Enc HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_int_hw5x5_jpeg_wdma23(
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT4_Txx setup
 *
 * Sensor --> INP --> HW5x5 --> JPEG Enc
 * Sensor --> INP --> HW5x5 --> WDMA3
 *
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Enc HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] cyclic_buffer_start_no: Specific the cyclic buffer index to start putting images.
 *									  Use hx_drv_xdma_get_WDMA2NextFrameIdx to get the start index
 *									  before re-starting the datapath.
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_int_hw5x5_jpeg_wdma23_start_no(
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		uint8_t cyclic_buffer_start_no,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT4_Txx setup
 *
 * Sensor --> INP --> HW5x5(RGB) --> RGB2YUV --> JPEG Enc
 * Sensor --> INP --> HW5x5(RGB) --> WDMA3
 *
 * \param[in] hw5x5_cfg: HW5x5 HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Enc HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_int_hw5x5rgb_jpeg_wdma23(
        HW5x5_CFG_T hw5x5_cfg,
        JPEG_CFG_T jpeg_cfg,
        uint8_t cyclic_buffer_cnt,
        sensordplib_CBEvent_t dplib_cb);

/*
 * export API datapath INT4_Txx all setup
 * Sensor --> INP --> HW2x2 --> WDMA1
 * Sensor --> INP --> HW5x5 --> JPEG Enc --> WDMA2
 */
void sensordplib_set_int_hw2x2_hw5x5_jpeg_wdma12(
		HW2x2_CFG_T hw2x2_cfg,
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		sensordplib_CBEvent_t dplib_cb);

/**
 * \brief sensor --> datapath INT4_Txx setup
 *
 * Sensor --> INP --> HXCSC(YUV422) --> JPEG Enc
 * Sensor --> INP --> HXCSC(YUV422orRGB888) --> WDMA3
 *
 * \param[in] hxcsc_cfg: HXCSC HW-ACC configuration
 * \param[in] jpeg_cfg: JPEG Enc HW-ACC configuration
 * \param[in] cyclic_buffer_cnt: JPEG cyclic buffer total slots
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_set_int_hxcsc_jpeg_wdma23(
		HXCSC_CFG_T hxcsc_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		sensordplib_CBEvent_t dplib_cb);


/**
 * \brief Stop Capture frame to xDMA
 * 1. Disable xDMA
 * 2. Disable DP Mux
 * 3. Disable CDM
 * 4. Disable Periodic Timer
 * 5. Set Sensor Control to SWRESET
 * 6. STOP EDM WDT
 *
 * \retval void.
 */
void sensordplib_stop_capture();


/**
 * \brief start sw reset for Datapath xDMA, Sensor Control, INP, HW-ACC
 *
 * Bit 0: INP software reset
 * Bit 1: Sensor_ctrl software reset
 * Bit 2: HWC 2x2 block software reset
 * Bit 3: HWC 5x5 block software reset
 * Bit 4: HWC CDM block software reset
 * Bit 5: HWC JPEG block software reset
 * Bit 6: TPG software reset
 * Bit 7: EDM software reset
 * Bit 8: Datapath software reset
 * Bit 9: DMA write channel 1 software reset
 * Bit 10: DMA write channel 2 software reset
 * Bit 11: DMA write channel 3 software reset
 * Bit 12: DMA read channel software reset
 *
 * \retval void.
 */
void sensordplib_start_swreset();


/**
 * \brief set Normal Mode for Datapath xDMA, , INP, HW-ACC but SensorCtrl Keep SWRESET
 *
 * Bit 0: INP Normal Mode
 * Bit 1: Sensor_ctrl software reset
 * Bit 2: HWC 2x2 block Normal Mode
 * Bit 3: HWC 5x5 block Normal Mode
 * Bit 4: HWC CDM block Normal Mode
 * Bit 5: HWC JPEG block Normal Mode
 * Bit 6: TPG Normal Mode
 * Bit 7: EDM Normal Mode
 * Bit 8: Datapath Normal Mode
 * Bit 9: DMA write channel 1 Normal Mode
 * Bit 10: DMA write channel 2 Normal Mode
 * Bit 11: DMA write channel 3 Normal Mode
 * Bit 12: DMA read channel Normal Mode
 *
 * \retval void.
 */
void sensordplib_stop_swreset_WoSensorCtrl();

/**
 * \brief re-trigger capture frame
 *
 * For not enable RTC case, after setup datapath and capture 1st frame, you can use this API to capture new frame
 *
 * \retval void.
 */
void sensordplib_retrigger_capture();

/**
 * \brief get datapath library xdma flag status (debug purpose)
 *
 * \param[out] wdma1_fin: 0: wdma1 not finish, 1: wdma1 finish
 * \param[out] wdma2_fin: 0: wdma2 not finish, 1: wdma2 finish
 * \param[out] wdma3_fin: 0: wdma3 not finish, 1: wdma3 finish
 * \param[out] rdma_fin: 0: rdma not finish, 1: rdma finish
 * \retval void.
 */
void sensordplib_get_xdma_fin(uint8_t *wdma1_fin,uint8_t *wdma2_fin,uint8_t *wdma3_fin,uint8_t *rdma_fin);

/**
 * \brief get datapath library version (debug purpose)
 *
 * \param[out] version: datapath library version number
 * \retval void.
 */
void sensordplib_get_version(uint32_t *version);

/**
 * \brief set EDM timing configuration if we enable EDM timing detect interrupt (default compiler is disable) (debug purpose)
 *
 * default compiler flag for EDM timing detect interrupt is disable so set this function is no effect.
 * \param[in] width: input timing width for EDM detect
 * \param[in] height: input timing height for EDM detect
 * \retval void.
 */
void sensordplib_edm_timing_config(uint16_t width, uint16_t height);

/**
 * \brief set EDM WDT detect enable (default it enable in datapath setup)
 *
 * WDT period define in macro WDT_TIMEOUT_PERIOD
 * \param[in] wdt1_en: WDMA1 Watchdog enable
 * \param[in] wdt2_en: WDMA2 Watchdog enable
 * \param[in] wdt3_en: WDMA3 Watchdog enable
 * \retval void.
 */
void sensordplib_edm_wdt_config(uint8_t wdt1_en, uint8_t wdt2_en, uint8_t wdt3_en);

/**
 * \brief set EDM timing detect by datapath or not(default it enable in datapath setup)
 *
 * Default datapath EDM timing detect path monitor after INP only
 * For some test purpose, we may configure EDM timing detect by app.
 * You can use this API to disable datapath library set EDM timing detect
 *
 * \param[in] enable: enable datapath set timing detect configuration or not
 * \retval void.
 */
void sensordplib_set_edm_timing_inlib(uint8_t enable);

/**
 * \brief set xDMA base start address by app
 *
 * Default xDMA base address is as follow if you don't set by application
 * WDMA1_addr: HW2x2 and CDM use memory (0x20050000)
 * WDMA2_addr: JPEG use memory (0x2009B000)
 * WDMA3_addr: HW5x5 use memory (0x0x200E6000)
 *
 * \param[in] wdma1_addr: set WDMA1 base address
 * \param[in] wdma2_addr: set WDMA2 base address
 * \param[in] wdma3_addr: set WDMA3 base address
 * \retval void.
 */
void sensordplib_set_xDMA_baseaddrbyapp(uint32_t wdma1_addr, uint32_t wdma2_addr, uint32_t wdma3_addr);

/**
 * \brief get current xDMA base start address setting in datapath library
 *
 * Default xDMA base address is as follow if you don't set by application
 * WDMA1_addr: HW2x2 and CDM use memory (0x20050000)
 * WDMA2_addr: JPEG use memory (0x2009B000)
 * WDMA3_addr: HW5x5 use memory (0x0x200E6000)
 *
 * \param[out] wdma1_addr: get WDMA1 base address
 * \param[out] wdma2_addr: get WDMA2 base address
 * \param[out] wdma3_addr: get WDMA3 base address
 * \retval void.
 */
void sensordplib_get_xDMA_baseaddr(uint32_t *wdma1_addr, uint32_t *wdma2_addr, uint32_t *wdma3_addr);

/**
 * \brief set jpeg auto fill file size base address by app
 *
 * Default HW auto fill jpeg file size base address is as follow if you don't set by application
 * jpeg_enc file size auto fill address: 0x2015FE70
 *
 * \param[in] jpegfilesize_autoaddr: set jpeg auto fill file size base address
 * \retval void.
 */
void sensordplib_set_jpegfilesize_addrbyapp(uint32_t jpegfilesize_autoaddr);

/**
 * \brief get jpeg auto fill file size base address
 *
 * \details Default HW auto fill jpeg file size base address is as follow if you don't set by application
 * jpeg_enc file size auto fill address: 0x2015FE70
 *
 * \param[out] jpegfilesize_autoaddr: get jpeg auto fill file size base address
 * \retval void.
 */
void sensordplib_get_jpegfilesize_addrbyapp(uint32_t *jpegfilesize_autoaddr);

/**
 * \brief snesor library initialize
 *
 * \retval void.
 */
void sensordplib_init();


/**
 * \brief enable/disable hm11b1 header information store in datapath library
 *
 * default it is enable
 * \param[in] enable: enable/disable hm11b1 header information store in datapath library
 * \retval void.
 */
void sensordplib_set_hm11b1headerget_en(uint8_t enable);

/**
 * \brief get hm11b1 header information store in datapath library
 * \param[out] info: get hm11b1 header information store in datapath library
 * \retval void.
 */
void sensordplib_get_hm11b1header(SENSORDPLIB_HM11B1_HEADER_T *info);

/**
 * \brief set ready flag status in datapath library
 *
 * set ready flag 1 when timer is fire, it will capture one frame
 * set ready flag 0 when timer is fire, it pause capture frame
 *
 * \param[out] ready_flag: 1 when timer is fire, it will capture one frame. 0 when timer is fire, it pause capture frame
 * \retval void.
 */
void sensordplib_set_readyflag(uint8_t ready_flag);

/**
 * \brief get ready flag status in datapath library(debug purpose)
 *
 * get ready flag 1 when timer is fire, it will capture one frame
 * get ready flag 0 when timer is fire, it pause capture frame
 *
 * \param[out] ready_flag: 1 when timer is fire, it will capture one frame. 0 when timer is fire, it pause capture frame
 * \retval void.
 */
void sensordplib_get_readyflag(uint8_t *ready_flag);

/**
 * \brief get rtc timer period in datapath library(debug purpose)
 *
 * \param[out] rtc_interval: rtc timer period (unit:ms) (0 not enable RTC)
 * \retval void.
 */
void sensordplib_get_rtctimer(uint32_t *rtc_interval);

/**
 * \brief get status from dplib(debug purpose)
 *
 *
 * \param[out] ready_flag: app set ready to capture status in dp  library
 * \param[out] nframe_end: sensor control NFrame End Status
 * \param[out] xdmadone: xDMA finish flag status
 * \retval void.
 */
void sensordplib_get_status(uint8_t *ready_flag,uint8_t *nframe_end,uint8_t *xdmadone);


/**
 * \brief set MCLK control by CPU or Sensor Control
 *
 * \details SENSORDPLIB_STREAM_NONEAOS        ==> MCLK Control and xSleep control by CPU
 * SENSORDPLIB_STREAM_HM01B0_CONT    ==> MCLK Control and xSleep control by Sensor Control
 * SENSORDPLIB_STREAM_HM0360_CONT    ==> No Need MCLK and xSleep control by Sensor Control
 * SENSORDPLIB_STREAM_HM0360_BURST   ==> No Need MCLK and xSleep control by Sensor Control
 * SENSORDPLIB_STREAM_HM0360_SENSOR_ACT  ==> MCLK Control by CPU and xSleep control by CPU
 * SENSORDPLIB_STREAM_HM0360_WE1_ACT     ==> MCLK Control by CPU and xSleep control by CPU
 * SENSORDPLIB_STREAM_HM0360_CONT_MCLK   ==> MCLK Control by Sensor Control and xSleep control by Sensor Control
 * SENSORDPLIB_STREAM_HM0360_BURST_MCLK  ==> MCLK Control by Sensor Control and xSleep control by Sensor Control
 * SENSORDPLIB_STREAM_HM11B1_LOWPOWER    ==> MCLK Control by Sensor Control and xSleep control by Sensor Control
 *
 * \retval void.
 */
void sensordplib_set_mclkctrl_xsleepctrl_bySCMode();

/**
 * \brief set source memory address --> RDMA --> TPG --> JPEG Enc --> WDMA --> Destination memory address Snapshot
 *
 *
 * \param[in] jpeg_cfg: app set ready to capture status in dp  library
 * \param[in] wdma2_startaddr: TPG path with WDMA2 configuration for memory address
 * \param[in] jpegfilesize_fillen: JPEG file size fill to store in another memory space enable flag
 * \param[in] jpegfilesize_filladdr: JPEG file size fill to store in another memory address
 * \param[in] dplib_rdma_cfg: TPG path with RDMA configuration for memory address
 * \param[in] dplib_cb: callback function for datapath library callback
 * \retval void.
 */
void sensordplib_tpg_jpegenc_wdma2(
		JPEG_CFG_T jpeg_cfg
		,uint32_t wdma2_startaddr
		,uint8_t jpegfilesize_fillen
		,uint32_t jpegfilesize_filladdr
		,SENSORDPLIB_RDMA_CFG_T dplib_rdma_cfg
		,sensordplib_CBEvent_t dplib_cb);

/**
 * \brief set source memory address --> RDMA --> TPG --> JPEG Enc --> WDMA --> Destination memory address Snapshot
 *
 *
 * \param[in] input_width: TPG path input width
 * \param[in] input_height: TPG path input height
 * \param[in] hw2x2_cfg: HW2x2 configuration
 * \param[in] wdma1_ch1_startaddr: TPG path with WDMA1 configuration for memory address
 * \param[in] rdma_ch1_startaddr: TPG path with RDMA configuration for memory address
 * \retval void.
 */
void sensordplib_tpg_HW2x2_wdma1(
		uint16_t input_width,
		uint16_t input_height,
		HW2x2_CFG_T hw2x2_cfg,
		uint32_t wdma1_ch1_startaddr,
		uint32_t rdma_ch1_startaddr,
		sensordplib_CBEvent_t dplib_cb);

/*
 * @brief register DP lib DataPath Status callback
 *
 * register DP lib DataPath Status callback
 * you can use different callback for DP, HOG and RS
 * @param cb_event dp library callback event function
 * @param type register library callback function type (Sensor DP, HOG, RS, JPEG_DEC)
 * @return void.
 * */
void hx_dplib_register_cb(sensordplib_CBEvent_t  cb_event, SENSORDPLIB_CB_FUNTYPE_E type);

/**
 * \brief get xdma and Sensor control finish flag status in datapath library
 *
 * get xdma Finish flag 1, xDMA is ready
 * get xdma Finish flag 0, xDMA is not ready, It cannot re-capture
 *
 * \param[out] xdma_fin_flag: 1 xDMA finish, 0: xDMA not finish
 * \param[out] sc_fin_flag: 1 Sensor Control finish, 0: Sensor Control  not finish
 * \retval void.
 */
void sensordplib_get_xdma_sc_finflag(uint8_t *xdma_fin_flag, uint8_t *sc_fin_flag);


/**
 * \brief ungated dp clock by data path case
 *
 *
 * \param[in] dplib_case: ungated dp clock by data path case
 * \retval void.
 */
void sensordplib_ungated_dp_clk_bycase(SENSORDPLIB_PATH_E dplib_case);

/**
 * \brief gated dp clock by data path case
 *
 *
 * \param[in] dplib_case: gated dp clock by data path case
 * \retval void.
 */
void sensordplib_gated_dp_clk_bycase(SENSORDPLIB_PATH_E dplib_case);

/**
 * \brief get current data path case
 *
 *
 * \param[out] dplib_case: get current data path case
 * \retval void.
 */
void sensordplib_get_cur_dp_path(SENSORDPLIB_PATH_E *dplib_case);

/**
 * \brief get current configuration sensor
 *
 *
 * \param[out] cursensorId: get current configuration sensor
 * \retval void.
 */
void sensorlib_get_cur_sensortype(SENSORDPLIB_SENSOR_E *cursensorId);

#ifdef DUT
/**
 * \brief trigger wdma2,3 mannually
 */
void trigger_wdma23_mannually();
#endif

/**
 * \brief Sensor DP library PMU to CPU control
 *
 * \retval void.
 */
void sensordplib_pmutocpuctrl(void);

/**
 * \brief Sensor DP library Wait DMA Done
 *
 * \retval void.
 */
void sensordplib_waitpmudmadone(void);

/**
 * \brief Sensor DP library Init
 *
 * \retval void.
 */
void sensordplib_pmudpinit(void);

/**
 * \brief Sensor DP library AUTO I2C cfg
 *
 * \param[in] scfg	 		auto i2c static config
 * \param[in] icfg	 		auto i2c interrupt config
 * \param[in] trig_cfg	 	auto i2c trig command config
 * \param[in] stop_cfg	 	auto i2c stop command config
 * \retval void.
 */
void sensordplib_autoi2c_cfg(HXAUTOI2CHC_STATIC_CFG_T scfg, HXAUTOI2CHC_INT_CFG_T icfg, HXAUTOI2CHC_CMD_CFG_T trig_cfg, HXAUTOI2CHC_CMD_CFG_T stop_cfg);

/**
 * \brief Sensor DP library AUTO I2C TRIG CMD
 *
 * \param[in] cmd1	 		auto i2c trig command1
 * \param[in] cmd2	 		auto i2c trig command2
 * \param[in] cmd3	 		auto i2c trig command3
 * \param[in] cmd4	 		auto i2c trig command4
 * \retval void.
 */
void sensordplib_autoi2c_trigcmd(HXAUTOI2CHC_CMD_T cmd1, HXAUTOI2CHC_CMD_T cmd2, HXAUTOI2CHC_CMD_T cmd3, HXAUTOI2CHC_CMD_T cmd4);

/**
 * \brief Sensor DP library AUTO I2C STOP CMD
 *
 * \param[in] cmd1	 		auto i2c stop command1
 * \param[in] cmd2	 		auto i2c stop command2
 * \param[in] cmd3	 		auto i2c stop command3
 * \param[in] cmd4	 		auto i2c stop command4
 * \retval void.
 */
void sensordplib_autoi2c_stopcmd(HXAUTOI2CHC_CMD_T cmd1, HXAUTOI2CHC_CMD_T cmd2, HXAUTOI2CHC_CMD_T cmd3, HXAUTOI2CHC_CMD_T cmd4);

/**
 * \brief Sensor DP library AUTO I2C ENABLE
 *
 * \retval void.
 */
void sensordplib_autoi2c_enable(void);

/**
 * \brief Sensor DP library AUTO I2C DISABLE
 *
 * \retval void.
 */
void sensordplib_autoi2c_disable(void);

/**
 * \brief Sensor DP library CSIRX(MIPIRX) ENABLE
 *
 * \param[in] lane_nb	 	MIPI CSIX LANE number
 * \retval void.
 */
void sensordplib_csirx_enable(uint8_t lane_nb);

/**
 * \brief Sensor DP library CSIRX(MIPIRX) ENABLE PATH THROUGH ONLY
 *
 * \param[in] lane_nb	 	MIPI CSIX LANE number
 * \retval void.
 */
void sensordplib_csirx_enable_path_through_only(uint8_t lane_nb);

/**
 * \brief Sensor DP library CSIRX(MIPIRX) DISABLE
 *
 * \retval void.
 */
void sensordplib_csirx_disable(void);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Set Virtual Channel Number
 *
 * \param[in] vc_num	 	MIPI CSIRX virtual channel number
 * \retval void.
 */
void sensordplib_csirx_set_vcnum(uint8_t vc_num);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Get Virtual Channel Number
 *
 * \param[out] vc_num	 	MIPI CSIRX virtual channel number
 * \retval void.
 */
void sensordplib_csirx_get_vcnum(uint8_t *vc_num);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Set HS-Count Config
 *
 * \param[in] hs_cnt	 	MIPI CSIRX HS-Count config
 * \retval void.
 */
void sensordplib_csirx_set_hscnt(MIPIRX_DPHYHSCNT_CFG_T hs_cnt);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Get HS-Count Config
 *
 * \param[out] hs_cnt	 	MIPI CSIRX HS-Count config
 * \retval void.
 */
void sensordplib_csirx_get_hscnt(MIPIRX_DPHYHSCNT_CFG_T *hs_cnt);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Set DESKEW
 *
 * \param[in] enable	 	DESKEW enble
 * \retval void.
 */
void sensordplib_csirx_set_deskew(uint8_t enable);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Get DESKEW
 *
 * \param[out] enable	 	DESKEW enble
 * \retval void.
 */
void sensordplib_csirx_get_deskew(uint8_t *enable);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Set pixel dpeth
 *
 * \param[in] depth	 	pixel depth
 * \retval void.
 */
void sensordplib_csirx_set_pixel_depth(uint8_t depth);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Get pixel depth
 *
 * \param[out] depth	 	pixel depth
 * \retval void.
 */
void sensordplib_csirx_get_pixel_depth(uint8_t *depth);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Set FIFO FILL
 *
 * \param[in] fifo_fill	 	FIFO FILL value
 * \retval void.
 */
void sensordplib_csirx_set_fifo_fill(uint16_t fifo_fill);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Get FIFO FILL
 *
 * \param[out] fifo_fill	 	FIFO FILL value
 * \retval void.
 */
void sensordplib_csirx_get_fifo_fill(uint16_t *fifo_fill);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Set LANE SWAP Enable
 *
 * \param[in] enable	 	LANE SWAP enable
 * \retval void.
 */
void sensordplib_csirx_set_lnswap_enable(uint8_t enable);

/**
 * \brief Sensor DP library CSIRX(MIPI RX) Get LANE SWAP Enable
 *
 * \param[out] enable	 	LANE SWAP enable
 * \retval void.
 */
void sensordplib_csirx_get_lnswap_enable(uint8_t *enable);

/**
 * \brief Sensor DP library CSITX(MIPI TX) ENABLE
 *
 * \param[in] lane_nb	 	CSITX lane number
 * \param[in] bit_rate	 	CSITX bit rate
 * \param[in] line_len	 	CSITX line length
 * \param[in] frame_len	 	CSITX frame length
 * \retval void.
 */
void sensordplib_csitx_enable(uint8_t lane_nb, uint16_t bit_rate, uint16_t line_len, uint16_t frame_len);

/**
 * \brief Sensor DP library CSITX(MIPI TX) with XTAL24M ENABLE
 *
 * \param[in] lane_nb	 	CSITX lane number
 * \param[in] bit_rate	 	CSITX bit rate
 * \param[in] line_len	 	CSITX line length
 * \param[in] frame_len	 	CSITX frame length
 * \retval void.
 */
void sensordplib_csitx_wi_xtal24_enable(uint8_t lane_nb, uint16_t bit_rate, uint16_t line_len, uint16_t frame_len);

/**
 * \brief Sensor DP library CSIRX(MIPI TX) DISABLE
 *
 * \retval void.
 */
void sensordplib_csitx_disable(void);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Set FIFO FILL
 *
 * \param[in] fifo_fill	 	FIFO FILL value
 * \retval void.
 */
void sensordplib_csitx_set_fifo_fill(uint16_t fifo_fill);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Get FIFO FILL
 *
 * \param[out] fifo_fill	 	FIFO FILL value
 * \retval void.
 */
void sensordplib_csitx_get_fifo_fill(uint16_t *fifo_fill);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Set CLK MODE
 *
 * \param[in] clkmode	 	TX CLK MODE
 * \retval void.
 */
void sensordplib_csitx_set_dphy_clkmode(CSITX_DPHYCLKMODE_E clkmode);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Get CLK MODE
 *
 * \param[out] clkmode	 	TX CLK MODE
 * \retval void.
 */
void sensordplib_csitx_get_dphy_clkmode(CSITX_DPHYCLKMODE_E *clkmode);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Set DESKEW
 *
 * \param[in] enable	 	TX DESKEW enable
 * \retval void.
 */
void sensordplib_csitx_set_deskew(uint8_t enable);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Get DESKEW
 *
 * \param[out] enable	 	TX DESKEW enable
 * \retval void.
 */
void sensordplib_csitx_get_deskew(uint8_t *enable);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Set pixel depth
 *
 * \param[in] depth	 	pixel depth
 * \retval void.
 */
void sensordplib_csitx_set_pixel_depth(uint8_t depth);

/**
 * \brief Sensor DP library CSITX(MIPI TX) Get pixel depth
 *
 * \param[out] depth	 	pixel depth
 * \retval void.
 */
void sensordplib_csitx_get_pixel_depth(uint8_t *depth);

/**
 * \brief Sensor DP library INP set crop area
 *
 * \param[in] crop	 	crop area start and last point
 * \retval void.
 */
void sensordplib_inp_set_crop_area(INP_CROP_T crop);
/**
 * \brief Sensor DP library Power State control by PMU or CPU
 *
 * \param[in] ctrl	 	Power State control by PMU or CPU
 * \retval void.
 */
void sensordplib_set_power_switch(SENSORDPLIB_POWERSWITCH_CTRL_E ctrl);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* INTEGRATE_CLI_HM1245_DATAPATH_SENSORDPLIB_LIB_H_ */
