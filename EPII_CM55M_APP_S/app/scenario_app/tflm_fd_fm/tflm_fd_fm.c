/*
 * tflm_fd_fm.c
 *
 *  Created on: Feb 15, 2023
 *      Author: bigcat-himax
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "powermode_export.h"

#define WATCH_DOG_TIMEOUT_TH	(500) //ms
#define WE2_CHIP_VERSION_C		0x8538000c

#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
//#include "veneer_table.h"
//
#endif
#endif

#include "WE2_device.h"

#include "spi_master_protocol.h"
#include "hx_drv_spi.h"
#include "spi_eeprom_comm.h"
#include "board.h"
#include "xprintf.h"
#include "tflm_fd_fm.h"
#include "board.h"
#include "WE2_core.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#ifdef IP_sensorctrl
#include "hx_drv_sensorctrl.h"
#endif
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#include "sensor_dp_lib.h"
#endif
#ifdef IP_cdm
#include "hx_drv_cdm.h"
#endif
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#include "hx_drv_pmu_export.h"
#include "hx_drv_pmu.h"
#include "powermode.h"
//#include "dp_task.h"
#include "BITOPS.h"

#include "common_config.h"
#include "cisdp_sensor.h"
#include "event_handler.h"
//#include "cvapp_pl.h"
//#include "cvapp_fd_fl.h"
#include "cvapp_fd_fm.h"

#include "memory_manage.h"
#include "hx_drv_watchdog.h"

#ifdef EPII_FPGA
#define DBG_APP_LOG             (1)
#else
#define DBG_APP_LOG             (0)
#endif
#if DBG_APP_LOG
    #define dbg_app_log(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_app_log(fmt, ...)
#endif

static uint8_t 	g_xdma_abnormal, g_md_detect, g_cdm_fifoerror, g_wdt1_timeout, g_wdt2_timeout,g_wdt3_timeout;
static uint8_t 	g_hxautoi2c_error, g_inp1bitparer_abnormal;
static uint32_t g_dp_event;
static uint8_t 	g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t 	g_time;
static uint8_t g_spi_master_initial_status;
static uint32_t g_use_case;
/*volatile*/ uint32_t jpeg_addr, jpeg_sz;
struct_algoResult algoresult;
struct_hp_algoResult algoresult_pl;
struct_fm_algoResult  algoresult_fm;

void app_start_state(APP_STATE_E state);
void pinmux_init();

/* Init SPI master pin mux */
void spi_m_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb2 = SCU_PB2_PINMUX_SPI_M_DO_1;        /*!< pin PB2*/
	pinmux_cfg->pin_pb3 = SCU_PB3_PINMUX_SPI_M_DI_1;        /*!< pin PB3*/
	pinmux_cfg->pin_pb4 = SCU_PB4_PINMUX_SPI_M_SCLK_1;      /*!< pin PB4*/
	pinmux_cfg->pin_pb11 = SCU_PB11_PINMUX_SPI_M_CS;        /*!< pin PB11*/
}


/*******************************************************************************
 * Code
 ******************************************************************************/
void pinmux_init()
{
	SCU_PINMUX_CFG_T pinmux_cfg;

	hx_drv_scu_get_all_pinmux_cfg(&pinmux_cfg);

	/* Init SPI master pin mux */
	spi_m_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}

static void dp_var_int()
{
	g_xdma_abnormal = 0;
	g_md_detect = 0;
	g_cdm_fifoerror = 0;
	g_wdt1_timeout = 0;
	g_wdt2_timeout = 0;
	g_wdt3_timeout = 0;
	g_inp1bitparer_abnormal = 0;
	g_dp_event = 0;
	g_frame_ready = 0;
	g_time = 0;
	g_cur_jpegenc_frame = 0;
	g_hxautoi2c_error = 0;
	g_spi_master_initial_status = 0;
}

void WDG_Reset_ISR_CB (uint32_t event)
{
	uint32_t read_data;
    read_data = hx_drv_watchdog_value(WATCHDOG_ID_0);
    xprintf ("read_data=%d not reset\n", read_data);
	xprintf("CLI_WDG_Reset_ISR_CB event=%d\n", event);
	//hx_drv_watchdog_stop();
}

static void dp_app_cv_fd_fm_eventhdl_cb(EVT_INDEX_E event)
{
	uint16_t err;
	int32_t read_status;
	uint32_t chipid, version;

	#if DBG_APP_LOG
	dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);
	#endif
	g_dp_event = event;

	switch(event)
	{
	case EVT_INDEX_1BITPARSER_ERR:  /*reg_inpparser_fs_cycle_error*/
		hx_drv_inp1bitparser_get_errstatus(&err);
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_1BITPARSER_ERR err=0x%x\r\n",err);
		hx_drv_inp1bitparser_clear_int();
		hx_drv_inp1bitparser_set_enable(0);
		g_inp1bitparer_abnormal = 1;
		break;
	case EVT_INDEX_EDM_WDT1_TIMEOUT:
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT1_TlenIMEOUT\r\n");
		g_wdt1_timeout = 1;
		break;
	case EVT_INDEX_EDM_WDT2_TIMEOUT:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT2_TIMEOUT\r\n");
		g_wdt2_timeout = 1;
		break;
	case EVT_INDEX_EDM_WDT3_TIMEOUT:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT3_TIMEOUT\r\n");
		g_wdt3_timeout = 1;
		break;

	case EVT_INDEX_CDM_FIFO_ERR:
		/*
		 * error happen need CDM timing & TPG setting
		 * 1. SWRESET Datapath
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_CDM_FIFO_ERR\r\n");
		g_cdm_fifoerror = 1;

		break;

	case EVT_INDEX_XDMA_WDMA1_ABNORMAL:
	case EVT_INDEX_XDMA_WDMA2_ABNORMAL:
	case EVT_INDEX_XDMA_WDMA3_ABNORMAL:
	case EVT_INDEX_XDMA_RDMA_ABNORMAL:
		/*
		 * error happen need
		 * 1. SWRESET Datapath
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_XDMA_WDMA123_ABNORMAL or EVT_INDEX_XDMA_RDMA_ABNORMAL\r\n");
		g_xdma_abnormal = 1;
		break;

	case EVT_INDEX_CDM_MOTION:
		/*
		 * app anything want to do
		 * */
		dbg_printf(DBG_LESS_INFO, "Motion Detect\n");
		g_md_detect = 1;
		break;
	case EVT_INDEX_XDMA_FRAME_READY:
		g_cur_jpegenc_frame++;
    	g_frame_ready = 1;
		dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
		break;

	case EVT_INDEX_SENSOR_RTC_FIRE:
		g_time++;
		break;
	case EVT_INDEX_HXAUTOI2C_ERR:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_HXAUTOI2C_ERR\r\n");
		g_hxautoi2c_error = 1;
		break;
	default:
		dbg_printf(DBG_LESS_INFO,"Other Event %d\n", event);
		break;
	}

	if(g_frame_ready == 1)
	{
		g_frame_ready = 0;
		
		hx_drv_watchdog_update(WATCHDOG_ID_0, WATCH_DOG_TIMEOUT_TH);

		cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

#ifdef CIS_IMX
		hx_drv_scu_get_version(&chipid, &version);
		if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
		{
			cisdp_stream_off();
			set_mipi_csirx_disable();
		}
#endif

#if FRAME_CHECK_DEBUG
			if(g_spi_master_initial_status == 0) {
				if(hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
				{
					dbg_printf(DBG_LESS_INFO, "DEBUG SPI master init fail\r\n");
					sensordplib_retrigger_capture();
					return ;
				}
				g_spi_master_initial_status = 1;
			}
			read_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
			//read_status = hx_drv_spi_mst_protocol_write_sp_wait(jpeg_addr, jpeg_sz, DATA_TYPE_JPG, 0);
			#if DBG_APP_LOG
					dbg_printf(DBG_LESS_INFO, "write frame result %d, data size=%d,addr=0x%x\n",read_status,
					jpeg_sz,jpeg_addr);
			#endif
#endif

		cv_fd_fm_run(&algoresult, &algoresult_fm);

#ifdef CIS_IMX
		hx_drv_scu_get_version(&chipid, &version);
		if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
		{
			set_mipi_csirx_enable();
			cisdp_stream_on();
		}
#endif

#if FRAME_CHECK_DEBUG
		hx_drv_spi_mst_protocol_write_sp((uint32_t)&algoresult_fm, sizeof(struct_fm_algoResult), DATA_TYPE_META_FM_DATA);
		#if DBG_APP_LOG
			dbg_printf(DBG_LESS_INFO, "META write frame result %d, data size=%d,addr=0x%x\n",read_status,
					sizeof(algoresult_fm),&algoresult_fm);
		#endif
#endif
		//clear_alg_rsult
		for (int i = 0; i < MAX_TRACKED_ALGO_RES; ++i) {
			algoresult.ht[i].upper_body_score = 0;
			algoresult.ht[i].upper_body_bbox.x = 0;
			algoresult.ht[i].upper_body_bbox.y = 0;
			algoresult.ht[i].upper_body_bbox.width = 0;
			algoresult.ht[i].upper_body_bbox.height = 0;
		}
		algoresult.num_tracked_human_targets=0;
		//clear_alg_fm_result(struct_fm_algoResult *algoresult_fm)
		for(int i=0;i<MAX_TRACKED_ALGO_RES;i++)
		{
			algoresult_fm.face_bbox[i].height = 0;
			algoresult_fm.face_bbox[i].width = 0;
			algoresult_fm.face_bbox[i].x = 0;
			algoresult_fm.face_bbox[i].y = 0;
			algoresult_fm.face_bbox[i].face_score = 0;
		}
		algoresult_fm.num_tracked_face_targets = 0;
		algoresult_fm.score = 0;
		for(int c=0;c<FACE_MESH_POINT_NUM;c++)
		{
			algoresult_fm.fmr[c].x=0;
			algoresult_fm.fmr[c].y=0;
			// algoresult_fm->fmr[c].z=0;

		}

		for(int i = 0;i<FACE_MESH_IRIS_POINT_NUM;i++)
		{
			algoresult_fm.fmr_iris[i].x=0;
			algoresult_fm.fmr_iris[i].y=0;

		}
		algoresult_fm.left_iris_theta = 0;
		algoresult_fm.left_iris_phi = 0;
		algoresult_fm.right_iris_theta = 0;
		algoresult_fm.right_iris_phi = 0;

		algoresult_fm.face_angle.yaw = 0;
		algoresult_fm.face_angle.pitch = 0;
		algoresult_fm.face_angle.roll = 0;
		algoresult_fm.face_angle.MAR = 0;
		algoresult_fm.face_angle.REAR = 0;
		algoresult_fm.face_angle.LEAR = 0;
	}

	if(g_md_detect == 1)
	{
		g_md_detect = 0;
	}

	if(g_inp1bitparer_abnormal == 1 || g_wdt1_timeout == 1 || g_wdt2_timeout == 1 || g_wdt3_timeout == 1
			|| g_cdm_fifoerror == 1 || g_xdma_abnormal == 1 || g_hxautoi2c_error == 1)
	{
		cisdp_sensor_stop();
	}

}

void app_start_state(APP_STATE_E state)
{
	if(cisdp_sensor_init() < 0)
	{
		xprintf("\r\nCIS Init fail\r\n");
		APP_BLOCK_FUNC();
	}

	dp_var_int();
	
	if (state == APP_STATE_ALLON_FD_FM)
	{
		if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, dp_app_cv_fd_fm_eventhdl_cb, 4, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0)// YUV 640X480
        {
        	xprintf("\r\nDATAPATH Init fail\r\n");
        	APP_BLOCK_FUNC();
        }
	}
	else
	{
        xprintf("\r\nDATAPATH Init fail\r\n");
        APP_BLOCK_FUNC();
    }

	event_handler_init();
    cisdp_sensor_start();
   	event_handler_start();
}


/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int app_main(void) {

	uint32_t wakeup_event;
	uint32_t wakeup_event1;
	uint32_t freq=0;

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

    hx_drv_swreg_aon_get_pllfreq(&freq);
    xprintf("wakeup_event=0x%x,WakeupEvt1=0x%x, freq=%d\n", wakeup_event, wakeup_event1, freq);

    pinmux_init();

	hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);

	hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);

#ifdef WATCHDOG_VERSION
	//watch dog start
	WATCHDOG_CFG_T wdg_cfg;
	wdg_cfg.period = WATCH_DOG_TIMEOUT_TH;
	wdg_cfg.ctrl = WATCHDOG_CTRL_CPU;
	wdg_cfg.state = WATCHDOG_STATE_DC;
	wdg_cfg.type = WATCHDOG_RESET;//wewweWATCHDOG_INT;
	hx_drv_watchdog_start(WATCHDOG_ID_0, &wdg_cfg , WDG_Reset_ISR_CB);
	xprintf("hx_drv_watchdog_start\n");
#endif

	mm_set_initial(BOOT2NDLOADER_BASE, 0x00200000-(BOOT2NDLOADER_BASE-0x34000000));

	//Face Detection Face mesh
	xprintf("Face Detection Face mesh\n");
	cv_fd_fm_init(true, true, FACE_DECTECT_FLASH_ADDR, FACE_MESH_FLASH_ADDR, IRIS_LANDMARKS_FLASH_ADDR);
	app_start_state(APP_STATE_ALLON_FD_FM);
	
	return 0;
}
