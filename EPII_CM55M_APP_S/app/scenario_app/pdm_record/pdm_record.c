#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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
#include "WE2_core.h"
#include "board.h"
#include "xprintf.h"
#include "console_io.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "pdm_record.h"
#include "hx_drv_pdm_rx.h"
#include "spi_fatfs.h"

PDM_DEV_INFO pdm_dev_info;

/*******************************************************************************
 * audio data is a 16-bits data (2bytes)
 * for 16Khz of 1 seconds mono audio, takes 2*16*1000 bytes
 * create a block with 8000 bytes to save 0.25 second of mono audio data (to meet hx_drv_pdm_dma_lli_transfer size limitation about <8192 bytes)
 * create 8 blocks to save 2 second of data
 ******************************************************************************/
#define QUARTER_SECOND_MONO_BYTES   8000    // 0.25 sec
#define BLK_NUM                     8       // 2 sec
#define RCEORD_TIME                 10      // record 20 sec audio and save to 10 .wav files

//allocate 2 audio buffer with 64000 bytes, this buffer is located in the SRAM which described in the linker script (pdm_record.ld)
int16_t audio_buf[2][BLK_NUM*QUARTER_SECOND_MONO_BYTES/2];

volatile bool rx_cb_flag;
/*******************************************************************************
 * Code
 ******************************************************************************/
void app_pdm_dma_rx_cb()
{
    //xprintf("\n[%s]\n", __FUNCTION__);
    rx_cb_flag = true;
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


/*!
 * @brief Main function
 */
int app_main(void)
{
    int32_t buf_idx = 0;
    int32_t save_idx = 0;
    int32_t record_cnt = 0;
    int32_t fs_res = 0;
    char filename[20];
    rx_cb_flag = false;
    int16_t num_chans;
    int32_t srate;

    if ( fatfs_init() != 0 )
    {
        xprintf("SD card fatfs init fail!!\r\n");
        while(1);
    }

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
    wav_header_init(srate, num_chans);

    if ( hx_drv_pdm_init(&pdm_dev_info)!= PDM_NO_ERROR )
        return -1;

	xprintf("Press any key to start audio recording\r\n");
	console_getchar();

    hx_drv_pdm_dma_lli_transfer((void *) audio_buf[buf_idx], BLK_NUM, QUARTER_SECOND_MONO_BYTES, 0);
    xprintf("start audio recording\n");

    do
    {
        while (rx_cb_flag == false);
        rx_cb_flag = false;
        xprintf("\nPDM audio_buf[%d], record done\n", buf_idx);
        save_idx = buf_idx;
        buf_idx = (buf_idx == 0)? 1:0;
        hx_drv_pdm_dma_lli_transfer((void *) audio_buf[buf_idx], BLK_NUM, QUARTER_SECOND_MONO_BYTES, 0);
        xprintf("Start Next PDM audio_buf[%d] recording\n", buf_idx);
        xsprintf(filename, "audio%04d.wav", record_cnt++);
        xprintf("Start to write SD card %s\n", filename);
        //Invalidate DCache before reading PDM data
        SCB_InvalidateDCache_by_Addr(audio_buf[save_idx], BLK_NUM*QUARTER_SECOND_MONO_BYTES);
        fastfs_write_audio((uint32_t)audio_buf[save_idx], BLK_NUM*QUARTER_SECOND_MONO_BYTES, filename);
        xprintf("Write %s done!\n", filename);
    } while ( record_cnt < RCEORD_TIME);

    xprintf("Record PDM data 20 second done!\n");

    hx_drv_pdm_deinit();
    while(1);

	return 0;
}