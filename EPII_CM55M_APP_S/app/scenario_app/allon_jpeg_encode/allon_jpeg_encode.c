#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "powermode_export.h"

#define WE2_CHIP_VERSION_C		0x8538000c
#define SEND_IMAGE_SPI			0
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
#include "spi_fatfs.h"
#include "BITOPS.h"

#include "cisdp_sensor.h"
#include "event_handler.h"
#include "common_config.h"
#include "allon_jpeg_encode.h"

#define SPI_SEN_PIC_CLK				(10000000)

static volatile uint32_t g_cur_frame = 0;
static volatile uint32_t g_frame_ready = 0;
static volatile uint32_t g_xdma_abnormal = 0;
static volatile uint32_t g_rs_abnormal = 0;
static volatile uint32_t g_hog_abnormal = 0;
static volatile uint32_t g_rs_frameready = 0;
static volatile uint32_t g_hog_frameready = 0;
static volatile uint32_t g_md_detect = 0;
static volatile uint32_t g_time = 0;
static volatile uint32_t g_prev_time = 0;
static volatile uint32_t g_wdt1_timeout = 0;
static volatile uint32_t g_wdt2_timeout = 0;
static volatile uint32_t g_wdt3_timeout = 0;

void dp_var_init()
{
	g_cur_frame = 0;
	g_frame_ready = 0;
	g_xdma_abnormal = 0;
	g_rs_abnormal = 0;
	g_hog_abnormal = 0;
	g_rs_frameready = 0;
	g_hog_frameready = 0;
	g_md_detect = 0;
	g_time = 0;
	g_prev_time = 0;
	g_wdt1_timeout = 0;
	g_wdt2_timeout = 0;
	g_wdt3_timeout = 0;
}

static void app_dplib_cb(SENSORDPLIB_STATUS_E event)
{
	uint32_t de0_count, conv_count;
	dbg_printf(DBG_MORE_INFO, "event = %d\n", event);

	if(event != SENSORDPLIB_STATUS_TIMER_FIRE_APP_NOTREADY)
	{
		hx_drv_edm_get_de_count(0, &de0_count);
		dbg_printf(DBG_MORE_INFO, "de0_count = 0x%x\n", de0_count);
		hx_drv_edm_get_conv_de_count(&conv_count);
		dbg_printf(DBG_MORE_INFO, "conv_count = 0x%x\n", conv_count);
	}
	switch(event)
	{
	case SENSORDPLIB_STATUS_EDM_WDT1_TIMEOUT:
		dbg_printf(DBG_MORE_INFO, "CB WDT1 Timeout\n");
		g_wdt1_timeout = 1;
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		break;

	case SENSORDPLIB_STATUS_EDM_WDT2_TIMEOUT:
		dbg_printf(DBG_MORE_INFO, "CB WDT2 Timeout\n");
		g_wdt2_timeout = 1;
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		break;

	case SENSORDPLIB_STATUS_EDM_WDT3_TIMEOUT:
		dbg_printf(DBG_MORE_INFO, "CB WDT3 Timeout\n");
		g_wdt3_timeout = 1;
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		break;
		
	case SENSORDPLIB_STATUS_SENSORCTRL_WDT_OUT:
		/*
		 * TODO error happen need check sensor
		 * 1. SWRESET Sensor Control & DP
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_MORE_INFO, "WDT OUT %d\n", event);
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		break;

	case SENSORDPLIB_STATUS_CDM_FIFO_OVERFLOW:
	case SENSORDPLIB_STATUS_CDM_FIFO_UNDERFLOW:
		dbg_printf(DBG_MORE_INFO, "CDM_FIFO_ERROR %d\n", event);
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		g_xdma_abnormal = 1;
		break;

	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL1:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL2:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL3:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL4:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL5:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL6:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL7:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL8:
	case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL9:

	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL1:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL2:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL3:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL4:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL5:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL6:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL7:

	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL1:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL2:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL3:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL4:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL5:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL6:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL7:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL8:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL9:

		dbg_printf(DBG_MORE_INFO, "WDMA123 abnormal %d\n", event);
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		g_xdma_abnormal = 1;
		break;

	case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL1:
	case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL2:
	case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL3:
	case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL4:
	case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL5:
		dbg_printf(DBG_MORE_INFO, "RDMA abnormal %d\n", event);
		g_xdma_abnormal = 1;
    	hx_drv_swreg_aon_set_sensorinit(SWREG_AON_SENSOR_INIT_NO);
		break;

	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL1:
	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL2:
	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL3:
	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL4:
	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL5:
	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL6:
	case SENSORDPLIB_STATUS_RSDMA_ABNORMAL7:
		/*
		 *  error happen need
		 * 1. SWRESET RS & RS DMA
		 * 2. Re-run flow again
		 */
		dbg_printf(DBG_MORE_INFO, "RSDMA abnormal %d\n", event);
	    g_rs_abnormal = 1;
		break;

	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL1:
	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL2:
	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL3:
	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL4:
	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL5:
	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL6:
	case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL7:
		/*
		 *  error happen need
		 * 1. SWRESET HOG & HOG DMA
		 * 2. Re-run flow again
		 */
		dbg_printf(DBG_MORE_INFO, "HOGDMA abnormal %d\n", event);
		g_hog_abnormal = 1;
		break;

	case SENSORDPLIB_STATUS_CDM_MOTION_DETECT:
		/*
		 * app anything want to do
		 * */
		g_md_detect = 1;
		dbg_printf(DBG_MORE_INFO, "Motion Detect\n");
		break;
	case SENSORDPLIB_STATUS_XDMA_FRAME_READY:
    	g_cur_frame++;
        // already capture user wanted frame number
    	g_frame_ready = 1;
		dbg_printf(DBG_LESS_INFO, "XDMA_FRAME_READY : %d \n", g_cur_frame);
		break;
	case SENSORDPLIB_STATUS_XDMA_WDMA1_FINISH:
	case SENSORDPLIB_STATUS_XDMA_WDMA2_FINISH:
	case SENSORDPLIB_STATUS_XDMA_WDMA3_FINISH:
	case SENSORDPLIB_STATUS_XDMA_RDMA_FINISH:
		break;

	case SENSORDPLIB_STATUS_RSDMA_FINISH:
		/*
		 * RS Frame result ready
		 */
		g_rs_frameready = 1;
		break;
	case SENSORDPLIB_STATUS_HOGDMA_FINISH:
		/*
		 * HOG Frame result ready
		 */
		g_hog_frameready = 1;
		break;
	case SENSORDPLIB_STATUS_TIMER_FIRE_APP_NOTREADY:
		g_time++;
		break;
	case SENSORDPLIB_STATUS_TIMER_FIRE_APP_READY:
		g_time++;
		break;
	default:
		break;
	}
}

void app_dump_single_jpeginfo(uint32_t *jpeg_enc_filesize, uint32_t *jpeg_enc_addr)
{
    uint8_t frame_no;
    uint8_t buffer_no = 0;
    hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
    hx_drv_xdma_get_WDMA2NextFrameIdx(&frame_no);
    uint32_t reg_val=0, mem_val=0;

	dbg_printf(DBG_MORE_INFO, "app_dump_single_jpeginfo:buffer_no=%d, frame_no=%d\n",buffer_no, frame_no);
    if(frame_no == 0)
    {
        frame_no = buffer_no - 1;
    }else{
        frame_no = frame_no - 1;
    }

    hx_drv_jpeg_get_EncOutRealMEMSize(&reg_val);
	*jpeg_enc_filesize = reg_val;
	*jpeg_enc_addr = app_get_jpeg_addr();

    dbg_printf(DBG_MORE_INFO, "current frame_no=%d, jpeg_size=0x%x,addr=0x%x\n",frame_no,*jpeg_enc_filesize,*jpeg_enc_addr);
}

void app_setup_dplib_4_yuv_then_jpg() {
	uint32_t jpeg_enc_filesize;
	uint32_t jpeg_enc_addr;
	uint32_t read_status;
	char filename[20];

	//Case 1: plus WE-2 center crop 1280x960 -> JPEG, 4x compression ratio
	dbg_printf(DBG_LESS_INFO, "app_setup_dplib_4jpg start\n");

	if (cisdp_sensor_init() < 0)
	{
		xprintf("\r\nCIS nonAOS Init fail - cisdp_sensor_init\r\n");
		APP_BLOCK_FUNC();
	}

	dp_var_init();
	
	if (cisdp_dp_init(true, CISDP_INIT_TYPE_INP_CROP_1280x960_RAW, SENSORDPLIB_PATH_INP_WDMA2, app_dplib_cb, 4) < 0)
	{
		dbg_printf(DBG_MORE_INFO, "\r\nDATAPATH nonAOS Init fail- cisdp_dp_init\r\n");
		APP_BLOCK_FUNC();
	}
	
	cisdp_sensor_start();

	while( 1 )
	{
		if  (g_frame_ready == 1 ) {
			g_frame_ready = 0;
			//4 times pic
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					uint32_t w = cisdp_get_raw_width(), h = cisdp_get_raw_height();
					uint32_t half_w = w/2, half_h = h/2;
					copy_mem_to_mem(cisdp_get_raw_addr(), cisdp_get_quater_raw_addr(), w, h, 0+half_w*i, 0+half_h*j, half_w, half_h);

					app_mem_to_hw5x5(cisdp_get_quater_raw_addr(), cisdp_get_yuv_addr(), half_w, half_h); 

					while( cisdp_get_hw5x5_flag() == 0 );

					app_mem_to_jpg(cisdp_get_yuv_addr(), app_get_jpeg_addr(), half_w, half_h);

					while( cisdp_get_jpg_flag() == 0 );

					app_dump_single_jpeginfo(&jpeg_enc_filesize, &jpeg_enc_addr);
					hx_drv_jpeg_get_EncOutRealMEMSize(&jpeg_enc_filesize);

					#if SEND_IMAGE_SPI
					read_status = hx_drv_spi_mst_protocol_write_sp(app_get_jpeg_addr(), jpeg_enc_filesize, DATA_TYPE_JPG);
					dbg_printf(DBG_MORE_INFO, "write frame result %d, data size=%d, addr=0x%x\n", read_status, jpeg_enc_filesize, jpeg_enc_addr);
					#else
					xsprintf(filename, "image%04d_%d.jpg", g_cur_frame, i*2+j);
					dbg_printf(DBG_MORE_INFO, "write frame to %s, data size=%d,addr=0x%x\n", filename, jpeg_enc_filesize, jpeg_enc_addr);
					read_status = fastfs_write_image(jpeg_enc_addr, jpeg_enc_filesize, filename);
					#endif
				}
			}
			
			if (cisdp_dp_init(false, CISDP_INIT_TYPE_INP_CROP_1280x960_RAW, SENSORDPLIB_PATH_INP_WDMA2, app_dplib_cb, 4) < 0)
			{
				dbg_printf(DBG_MORE_INFO, "\r\nDATAPATH nonAOS Init fail- cisdp_dp_init\r\n");
				APP_BLOCK_FUNC();
			}

			sensordplib_retrigger_capture();	
		}
	}
}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int app_main(void) {

#if SEND_IMAGE_SPI
	hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
	hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
	hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_SPI_M_CS, 1);
	if (hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
	{
		dbg_printf(DBG_MORE_INFO, "DEBUG SPI master init fail\r\n");
		sensordplib_retrigger_capture();
		return ;
	}
#else
	fatfs_init();
#endif

	app_setup_dplib_4_yuv_then_jpg();

	return 0;
}
