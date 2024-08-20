#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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
#include "console_io.h"

#include "WE2_core.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "kws_pdm_record.h"
#include "cvapp_kws.h"
#include "spi_master_protocol.h"
#include "spi_protocol.h"
#include "BITOPS.h"
#include "common_config.h"

// #include "memory_manage.h"
#include "hx_drv_watchdog.h"

#include "hx_drv_pdm_rx.h" // PDM driver
// #include "spi_fatfs.h"

PDM_DEV_INFO pdm_dev_info;

/*******************************************************************************
 * audio data is a 16-bits data (2bytes)
 * for 16Khz of 1 seconds mono audio, takes 2*16*1000 bytes
 * create a block with 8000 bytes to save 0.25 second of mono audio data (to meet hx_drv_pdm_dma_lli_transfer size limitation about <8192 bytes)
 * create 8 blocks to save 2 second of data
 ******************************************************************************/
#define QUARTER_SECOND_MONO_BYTES   8000    // 0.25 sec
#define BLK_NUM                     2       // 0.5 sec
#define AUDIO_LEN                   16000
#define NUM_BUFF                    8

//allocate 2 audio buffer with 64000 bytes, this buffer is located in the SRAM which described in the linker script (pdm_record.ld)
int16_t audio_buf[NUM_BUFF][BLK_NUM*QUARTER_SECOND_MONO_BYTES/2];

static uint8_t 	g_xdma_abnormal, g_md_detect, g_cdm_fifoerror, g_wdt1_timeout, g_wdt2_timeout,g_wdt3_timeout;
static uint8_t 	g_hxautoi2c_error, g_inp1bitparer_abnormal;
static uint32_t g_dp_event;
static uint8_t 	g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t 	g_time;
static uint8_t g_spi_master_initial_status;
static uint32_t g_use_case;
struct_algoResult algoresult_kws_pdm_record;
static uint32_t g_trans_type;
static uint32_t judge_case_data;
uint32_t audio_addr, audio_sz;
void pinmux_init();


#define GROVE_VISION_AI_II


#ifdef GROVE_VISION_AI_II
/* Init SPI master pin mux (share with SDIO) */
void spi_m_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb2 = SCU_PB2_PINMUX_SPI_M_DO_1;        /*!< pin PB2*/
	pinmux_cfg->pin_pb3 = SCU_PB3_PINMUX_SPI_M_DI_1;        /*!< pin PB3*/
	pinmux_cfg->pin_pb4 = SCU_PB4_PINMUX_SPI_M_SCLK_1;      /*!< pin PB4*/
	pinmux_cfg->pin_pb11 = SCU_PB11_PINMUX_SPI_M_CS;        /*!< pin PB11*/
}
#else
/* Init SPI master pin mux (share with SDIO) */
void spi_m_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg)
{
	pinmux_cfg->pin_pb2 = SCU_PB2_PINMUX_SPI_M_DO_1;        /*!< pin PB2*/
	pinmux_cfg->pin_pb3 = SCU_PB3_PINMUX_SPI_M_DI_1;        /*!< pin PB3*/
	pinmux_cfg->pin_pb4 = SCU_PB4_PINMUX_SPI_M_SCLK_1;      /*!< pin PB4*/
	pinmux_cfg->pin_pb5 = SCU_PB5_PINMUX_SPI_M_CS_1;        /*!< pin PB5*/
}
#endif

void pinmux_init()
{
	SCU_PINMUX_CFG_T pinmux_cfg;

	hx_drv_scu_get_all_pinmux_cfg(&pinmux_cfg);

	/* Init SPI master pin mux (share with SDIO) */
	spi_m_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}


// volatile bool rx_cb_flag;
volatile w_buf_idx = 0;
volatile r_buf_idx = 0;

void app_pdm_dma_rx_cb()
{
    //xprintf("\n[%s]\n", __FUNCTION__);
    // rx_cb_flag = true;

    w_buf_idx++;
    if (w_buf_idx == NUM_BUFF)
    {
        w_buf_idx = 0;
    }
    // xprintf("w_buf_idx %d \n", w_buf_idx);
    hx_drv_pdm_dma_lli_transfer((void *) audio_buf[w_buf_idx], BLK_NUM, QUARTER_SECOND_MONO_BYTES, 0);
    // xprintf("DMA transfer complete\n");

}

volatile bool kws_processing_complete = true;

// Assume cv_kws_run is modified to accept a callback
void kws_processing_callback(void) {
    kws_processing_complete = true;
}

int app_pdm_setting()
{
    //setup PDM configuration
    pdm_dev_info.pdm_cfg.reg_addr = HIMAX_PDM_BASE_ADDR;
    pdm_dev_info.pdm_cfg.rx_fifo_threshold = 5;
    pdm_dev_info.pdm_cfg.oversampling_ratio = 8; // 6: 32KHz, 8: 16KHz
    pdm_dev_info.pdm_cfg.cic_stages = 0;
    pdm_dev_info.pdm_cfg.cic_delay = 0;
    pdm_dev_info.pdm_cfg.dc_removal = 6;
    pdm_dev_info.pdm_cfg.bit_range_shift = 5;

    //use DATA0 with left channnel selected only(mono)
    pdm_dev_info.pdm_cfg.data_in_0_en = PDM_DATA_IN_ENABLE;
    pdm_dev_info.pdm_cfg.data_in_1_en = PDM_DATA_IN_DISABLE;
    pdm_dev_info.pdm_cfg.data_in_2_en = PDM_DATA_IN_DISABLE;
    pdm_dev_info.pdm_cfg.data_in_3_en = PDM_DATA_IN_DISABLE;
    pdm_dev_info.pdm_cfg.capture_channel = PDM_CAPTURE_CHANNEL_LEFT_ONLY;

    //sampling rate = 16K, use DMA2 channel 0
    pdm_dev_info.pdm_cfg.dma_ch = PDM_USE_DMA2_CH_0;
    pdm_dev_info.pdm_cfg.pdm_clk_src = PDM_CLKSRC_LSCREF;
    pdm_dev_info.pdm_cfg.sample_rate = PDM_PCM_FREQ_16K;
    pdm_dev_info.pdm_rx_cb = app_pdm_dma_rx_cb;
    pdm_dev_info.pdm_err_cb = NULL;
}

// static void dp_app_pdm_kws_eventhdl_cb(EVT_INDEX_E event)
// {
// 	uint16_t err;
// 	int32_t read_status;
	
// 	#if DBG_APP_LOG
// 	dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);
// 	#endif
// 	g_dp_event = event;

// 	switch(event)
// 	{
// 	case EVT_INDEX_1BITPARSER_ERR:  /*reg_inpparser_fs_cycle_error*/
// 		hx_drv_inp1bitparser_get_errstatus(&err);
// 		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_1BITPARSER_ERR err=0x%x\r\n",err);
// 		hx_drv_inp1bitparser_clear_int();
// 		hx_drv_inp1bitparser_set_enable(0);
// 		g_inp1bitparer_abnormal = 1;
// 		break;
// 	case EVT_INDEX_EDM_WDT1_TIMEOUT:
// 		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT1_TlenIMEOUT\r\n");
// 		g_wdt1_timeout = 1;
// 		break;
// 	case EVT_INDEX_EDM_WDT2_TIMEOUT:
// 		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT2_TIMEOUT\r\n");
// 		g_wdt2_timeout = 1;
// 		break;
// 	case EVT_INDEX_EDM_WDT3_TIMEOUT:
// 		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT3_TIMEOUT\r\n");
// 		g_wdt3_timeout = 1;
// 		break;

// 	case EVT_INDEX_CDM_FIFO_ERR:
// 		/*
// 		 * error happen need CDM timing & TPG setting
// 		 * 1. SWRESET Datapath
// 		 * 2. restart streaming flow
// 		 */
// 		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_CDM_FIFO_ERR\r\n");
// 		g_cdm_fifoerror = 1;

// 		break;

// 	case EVT_INDEX_XDMA_WDMA1_ABNORMAL:
// 	case EVT_INDEX_XDMA_WDMA2_ABNORMAL:
// 	case EVT_INDEX_XDMA_WDMA3_ABNORMAL:
// 	case EVT_INDEX_XDMA_RDMA_ABNORMAL:
// 		/*
// 		 * error happen need
// 		 * 1. SWRESET Datapath
// 		 * 2. restart streaming flow
// 		 */
// 		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_XDMA_WDMA123_ABNORMAL or EVT_INDEX_XDMA_RDMA_ABNORMAL\r\n");
// 		g_xdma_abnormal = 1;
// 		break;

// 	case EVT_INDEX_CDM_MOTION:
// 		/*
// 		 * app anything want to do
// 		 * */
// 		dbg_printf(DBG_LESS_INFO, "Motion Detect\n");
// 		g_md_detect = 1;
// 		break;
// 	case EVT_INDEX_XDMA_FRAME_READY:
// 		g_cur_jpegenc_frame++;
//     	g_frame_ready = 1;
// 		dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
// 		break;

// 	case EVT_INDEX_SENSOR_RTC_FIRE:
// 		g_time++;
// 		break;
// 	case EVT_INDEX_HXAUTOI2C_ERR:
// 		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_HXAUTOI2C_ERR\r\n");
// 		g_hxautoi2c_error = 1;
// 		break;
// 	default:
// 		dbg_printf(DBG_LESS_INFO,"Other Event %d\n", event);
// 		break;
// 	}

// 	if(g_frame_ready == 1)
// 	{
// 		g_frame_ready = 0;

// 		hx_drv_swreg_aon_get_appused1(&judge_case_data);

// #ifdef CPU_24MHZ_VERSION
// 			SetPSPDNoVid_24M();//
// #else
// 			SetPSPDNoVid();
// #endif

// #if FRAME_CHECK_DEBUG
// 			if(g_spi_master_initial_status == 0) {
// 				if(hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
// 				{
// 					dbg_printf(DBG_LESS_INFO, "DEBUG SPI master init fail\r\n");
// 					sensordplib_retrigger_capture();
// 					return ;
// 				}
// 				g_spi_master_initial_status = 1;
//             }
//             #ifdef UART_SEND_ALOGO_RESEULT

// 				hx_drv_swreg_aon_get_appused1(&judge_case_data);
// 				g_trans_type = (judge_case_data>>16);
// 				if( g_trans_type == 0 )// transfer type is (UART) 
// 				{

// 				}
// 				else if( g_trans_type == 1 || g_trans_type == 2)// transfer type is (SPI) or (UART & SPI) 
// 				{
// 					read_status = hx_drv_spi_mst_protocol_write_sp(audio_addr, audio_sz, DATA_TYPE_PDM); // this needs to be looked into. what's the audio equivalent to this?
// 				}
//             #else
//                 read_status = hx_drv_spi_mst_protocol_write_sp(audio_addr, audio_sz, DATA_TYPE_PDM);
// 			#endif
// 			#if DBG_APP_LOG
// 					dbg_printf(DBG_LESS_INFO, "write frame result %d, data size=%d,addr=0x%x\n",read_status,
// 							audio_sz,audio_addr);
// 			#endif

// #endif
// #ifdef EN_ALGO
// #ifdef UART_SEND_ALOGO_RESEULT

//     hx_drv_swreg_aon_get_appused1(&judge_case_data);
// 	g_trans_type = (judge_case_data>>16);
// 	if( g_trans_type == 0 )// transfer type is (UART) 
// 	{
// 		cv_kws_run(&algoresult_kws_pdm_record);
// 	}
//     else if( g_trans_type == 1 || g_trans_type == 2)// transfer type is (SPI) or (UART & SPI) 
// 	{
// 		#if TOTAL_STEP_TICK
// 				uint32_t systick_1, systick_2;
// 				uint32_t loop_cnt_1, loop_cnt_2;
// 				SystemGetTick(&systick_1, &loop_cnt_1);
// 		#endif
//             cv_kws_run(&algoresult_kws_pdm_record);

//         #if TOTAL_STEP_TICK						
// 				SystemGetTick(&systick_2, &loop_cnt_2);
// 			#if TOTAL_STEP_TICK_DBG_LOG
// 				xprintf("Tick for TOTAL KWS_PDM_RECORD:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));					
// 			#endif	

// 			if(g_trans_type == 1)//USE SPI
// 			{
// 				algoresult_kws_pdm_record.algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2);		
// 			} 		
// 		#endif

//         #if FRAME_CHECK_DEBUG
// 			hx_drv_spi_mst_protocol_write_sp((uint32_t)&algoresult_kws_pdm_record, sizeof(struct_kws_algoResult), DATA_TYPE_META_KWS_PDM_RECORD_DATA);
// 		#endif
//     }
// #else 
// 	#if TOTAL_STEP_TICK
// 			uint32_t systick_1, systick_2;
// 			uint32_t loop_cnt_1, loop_cnt_2;
// 			SystemGetTick(&systick_1, &loop_cnt_1);
// 	#endif

//             cv_kws_run(&algoresult_kws_pdm_record);
//     #if TOTAL_STEP_TICK						
// 			SystemGetTick(&systick_2, &loop_cnt_2);
// 		#if TOTAL_STEP_TICK_DBG_LOG
// 			xprintf("Tick for TOTAL KWS_PDM_RESULT:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));				
// 		#endif	
// 		algoresult_kws_pdm_record.algo_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2);				
// 	#endif

//     #if FRAME_CHECK_DEBUG
// 			hx_drv_spi_mst_protocol_write_sp((uint32_t)&algoresult_kws_pdm_record, sizeof(struct_kws_algoResult), DATA_TYPE_META_KWS_PDM_RECORD_DATA);
// 	#endif

// #endif

//         //Clear algo result here

// #endif

// 	}

// 	if(g_md_detect == 1)
// 	{
// 		g_md_detect = 0;
// 	}

// 	if(g_inp1bitparer_abnormal == 1 || g_wdt1_timeout == 1 || g_wdt2_timeout == 1 || g_wdt3_timeout == 1
// 			|| g_cdm_fifoerror == 1 || g_xdma_abnormal == 1 || g_hxautoi2c_error == 1)
// 	{
// 		cisdp_sensor_stop();
// 	}
// }

// }//end of dp_app_pdm_kws_eventhdl_cb

int kws_pdm_record_app(void)
{

    uint32_t wakeup_event;
	uint32_t wakeup_event1;
	uint32_t freq=0;
	int32_t buf_idx = 0;
    int32_t save_idx = 0;
    int32_t record_cnt = 0;
    int32_t fs_res = 0;
    int16_t num_chans;
    int32_t srate;

	//select pin mux for PDM, enable PDM CLK(PB9) and PDM DATA0 pin(PB10)  
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_PDM_CLK_13, 1);
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_PDM_DATA0_13, 1);

	app_pdm_setting();

    //setup wav header info
    switch( pdm_dev_info.pdm_cfg.sample_rate )
    {
        case PDM_PCM_FREQ_8K:
            srate = 8000;
            break;
        case PDM_PCM_FREQ_16K:
            srate = 16000;
            break;
        case PDM_PCM_FREQ_32K:
            srate = 32000;
            break;
        case PDM_PCM_FREQ_48K:
            srate = 48000;
            break;
        default:
            srate = 16000;
            break;
    }
	num_chans = 1;
	// wav_header_init(srate, num_chans);

	if ( hx_drv_pdm_init(&pdm_dev_info)!= PDM_NO_ERROR )
        return -1;

    hx_drv_swreg_aon_get_pllfreq(&freq);

    pinmux_init();

    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);

    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);


    hx_drv_swreg_aon_get_appused1(&judge_case_data);
	//transfer type
	g_trans_type = (judge_case_data>>16);
	//model case 
	g_use_case = (judge_case_data&0xff);

#ifndef CPU_24MHZ_VERSION
	xprintf("ori_clk src info, 0x56100030=%x\n",EPII_get_memory(0x56100030));
	xprintf("ori_clk src info, 0x56100034=%x\n",EPII_get_memory(0x56100034));
	xprintf("ori_clk src info, 0x56100038=%x\n",EPII_get_memory(0x56100038));

	EPII_set_memory(0x56100030,0x4037);
	EPII_set_memory(0x56100034,0x0);
	EPII_set_memory(0x56100038,0xc1b8);

	xprintf("clk src info, 0x56100030=%x\n",EPII_get_memory(0x56100030));
	xprintf("clk src info, 0x56100034=%x\n",EPII_get_memory(0x56100034));
	xprintf("clk src info, 0x56100038=%x\n",EPII_get_memory(0x56100038));
#endif

//  #ifdef __GNU__
// 	xprintf("__GNUC \n");
// 	extern char __mm_start_addr__;
// 	xprintf("__mm_start_addr__ address: %x\r\n",&__mm_start_addr__);
// 	mm_set_initial((int)(&__mm_start_addr__), 0x00200000-((int)(&__mm_start_addr__)-0x34000000));
// #else
// 	static uint8_t mm_start_addr __attribute__((section(".bss.mm_start_addr")));
// 	xprintf("mm_start_addr address: %x \r\n",&mm_start_addr);
// 	mm_set_initial((int)(&mm_start_addr), 0x00200000-((int)(&mm_start_addr)-0x34000000));   
// #endif

	hx_drv_pdm_dma_lli_transfer((void *) audio_buf[w_buf_idx], BLK_NUM, QUARTER_SECOND_MONO_BYTES, 0);
    xprintf("start KWS exmample now recording\n");

    cv_kws_init(true, true, KWS_FLASH_ADDR); 

    int16_t temp_buf[AUDIO_LEN];

    volatile a = 0;
    volatile miss_inf = 0;

	do {
        int32_t current_buf = w_buf_idx;
        int32_t prev_buf = (current_buf - 1 + NUM_BUFF) % NUM_BUFF;
        int32_t a = a % NUM_BUFF;

        if(a != prev_buf)
        {   
            if(r_buf_idx > 0)
            {
                miss_inf++;
            }
            xprintf("We miss 1 inference where ");
            xprintf("prev buf: %d and ", prev_buf);
            xprintf("a: %d\n", a);
        }

        while (!kws_processing_complete);

        // dma transfer should be complete
        while (w_buf_idx == current_buf);

        // invalud the cache here
        SCB_InvalidateDCache_by_Addr((uint32_t*)audio_buf[prev_buf], QUARTER_SECOND_MONO_BYTES);
        SCB_InvalidateDCache_by_Addr((uint32_t*)audio_buf[current_buf], QUARTER_SECOND_MONO_BYTES);

        // memcpy(temp_buf, audio_buf[prev_buf], 8000 * sizeof(int));
        // memcpy(temp_buf + 8000, audio_buf[current_buf], 8000 * sizeof(int));

        memcpy(temp_buf, audio_buf[prev_buf], BLK_NUM*QUARTER_SECOND_MONO_BYTES/2 * sizeof(int));
        memcpy(temp_buf + BLK_NUM*QUARTER_SECOND_MONO_BYTES/2, audio_buf[current_buf], BLK_NUM*QUARTER_SECOND_MONO_BYTES/2 * sizeof(int));

        if (r_buf_idx > 0) {
            cv_kws_run(&algoresult_kws_pdm_record, 
                        temp_buf, 
                        AUDIO_LEN,
                        kws_processing_callback);
            
        } else {
            // Skip processing for the first iteration
            xprintf("Skipping first buffer processing\n");
        }
        if (r_buf_idx > 0)
        {
            a++;
        }
        // a++;
        r_buf_idx++;

        if (r_buf_idx%20 == 0)
        {
            xprintf("Missed inferences %d out of Total inferences %d\n", miss_inf, r_buf_idx);
        }

    } while(1);
	
    xprintf("KWS example should be finish now\n");

	hx_drv_pdm_deinit();
    cv_kws_deinit();
    while(1);

	return 0;


}

