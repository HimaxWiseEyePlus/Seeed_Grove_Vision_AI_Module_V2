#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "powermode_export.h"

#define FRAME_CHECK_DEBUG 1
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
//#include "cvapp.h"
#include "spi_master_protocol.h"
#include "hx_drv_spi.h"
#include "spi_eeprom_comm.h"
#include "board.h"
#include "xprintf.h"
#include "allon_sensor_mipi.h"
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

#include "cisdp_sensor.h"
#include "event_handler.h"
#include "memory_manage.h"

#ifdef EPII_FPGA
#define DBG_APP_LOG             (1)
#else
#define DBG_APP_LOG             (1)
#endif
#if DBG_APP_LOG
    #define dbg_app_log(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_app_log(fmt, ...)
#endif


#define MAX_STRING  100
#define DEBUG_SPIMST_SENDPICS		(0x01) //0x00: off/ 0x01: JPEG/0x02: YUV422/0x03: YUV420/0x04: YUV400/0x05: RGB
#define SPI_SEN_PIC_CLK				(10000000)

#define MEM_FREE_POS		(BOOT2NDLOADER_BASE) //0x340A0000

//flash image start position
//To prevent information losses when M55M sleep w/o retentation, we will add needed information in the MB share data
/*volatile*/ uint32_t g_flash_record_start_pos = 0;
/*volatile*/ uint32_t g_flash_image_cur_pos = 0;
/*volatile*/ uint32_t g_flash_length_cur_pos = 0;
/*volatile*/ uint32_t g_idle_time = 0;
/*volatile*/ uint32_t g_img_data = 0;

static uint8_t 	g_xdma_abnormal, g_md_detect, g_cdm_fifoerror, g_wdt1_timeout, g_wdt2_timeout,g_wdt3_timeout;
static uint8_t 	g_hxautoi2c_error, g_inp1bitparer_abnormal;
static uint32_t g_dp_event;
static uint8_t 	g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t 	g_time;
static uint8_t g_spi_master_initial_status;
/*volatile*/ uint32_t jpeg_addr, jpeg_sz;

void app_start_state(APP_STATE_E state);


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

static void dp_app_cv_eventhdl_cb(EVT_INDEX_E event)
{
	uint16_t err;
	int32_t read_status;
	int score=0;

	dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);
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

		cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

		{
#if FRAME_CHECK_DEBUG
			if(g_spi_master_initial_status == 0) {
				//EPII_set_memory(0x42001110, 0x01010101);
				hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1);
				hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1);
				hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1);
				hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1);
				if(hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
				{
					dbg_printf(DBG_LESS_INFO, "DEBUG SPI master init fail\r\n");
					sensordplib_retrigger_capture();
					return ;
				}
				g_spi_master_initial_status = 1;
			}

			//send jpeg image data
			hx_InvalidateDCache_by_Addr((volatile void *)jpeg_addr, jpeg_sz);

			read_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
					dbg_printf(DBG_LESS_INFO, "write frame result %d, data size=%d,addr=0x%x\n",read_status,
							jpeg_sz,jpeg_addr);
			//send raw image data							
			// uint32_t out_addr = app_get_raw_addr();
			// uint32_t spi_send_datasize;
			// SPI_CMD_DATA_TYPE image_type;

			// image_type = DATA_TYPE_RAW_HEADER_IMG_MONO;
			// spi_send_datasize = DP_HW5X5_OUT_WIDTH * DP_HW5X5_OUT_HEIGHT;

			// uint8_t imagesize_header[4];
			// imagesize_header[0] = DP_HW5X5_OUT_WIDTH & 0xFF;
			// imagesize_header[1] = (DP_HW5X5_OUT_WIDTH>>8) & 0xFF;
			// imagesize_header[2] = DP_HW5X5_OUT_HEIGHT & 0xFF;
			// imagesize_header[3] = (DP_HW5X5_OUT_HEIGHT>>8) & 0xFF;

			// read_status = hx_drv_spi_mst_protocol_write_ex(out_addr, spi_send_datasize,
			// 		image_type, imagesize_header, 4);
#endif

		}

		//recapture image
		sensordplib_retrigger_capture();
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
	if(state == APP_STATE_ALLON) {
        if(cisdp_sensor_init() < 0)
        {
        	xprintf("\r\nCIS Init fail\r\n");
        	APP_BLOCK_FUNC();
        }

        dp_var_int();

        //if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
        if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, dp_app_cv_eventhdl_cb, 4) < 0)
        {
        	xprintf("\r\nDATAPATH Init fail\r\n");
        	APP_BLOCK_FUNC();
        }

        event_handler_init();

        cisdp_sensor_start();

    	event_handler_start();
	}


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

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    xprintf("wakeup_event=0x%x,WakeupEvt1=0x%x\n", wakeup_event, wakeup_event1);

    mm_set_initial(0x34100000, 0x00100000); //memory maximum available size is 2MB, we use upper 1MB for later image capture
    //cvapp_init(true, true);
    app_start_state(APP_STATE_ALLON);

	return 0;
}
