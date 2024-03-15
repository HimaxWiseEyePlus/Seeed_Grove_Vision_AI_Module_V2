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
#include "board.h"
#include "xprintf.h"
#include "pdm_single.h"
#include "board.h"
#include "WE2_core.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_pdm_rx.h"

PDM_DEV_INFO pdm_dev_info;

//audio data is a 16-bits data (2bytes)
//for 16Khz of 1 seconds mono audio, takes 2*16*1000 bytes
//create a block with 8000 bytes to save 0.25 second of mono audio data (to meet hx_drv_pdm_dma_lli_transfer size limitation about <8192 bytes)
//create 40 blocks to save 10 second of data
#define quarter_second_mono_bytes   8000
#define blk_num                     40

//allocate a audio buffer with 320000 bytes, this buffer is located in the SRAM which described in the linker script (pdm_single.ld)
int16_t audio_buf[blk_num*quarter_second_mono_bytes/2];

volatile bool rx_cb_flag;
/*******************************************************************************
 * Code
 ******************************************************************************/
void app_pdm_dma_rx_cb()
{
    hx_InvalidateDCache_by_Addr(audio_buf,quarter_second_mono_bytes*blk_num);
    xprintf("\n\n[%s]\n\n", __FUNCTION__);
    rx_cb_flag = true;
}

int app_pdm_setting() {
    memset((void *)&pdm_dev_info, 0x00, sizeof(PDM_DEV_INFO));
    //
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
int app_main(void) {
    rx_cb_flag = false;

    xprintf("PDM example, 16KHz mono, record a single shot\n");

    //select pin mux for PDM, enable PDM CLK(PB9) and PDM DATA0 pin(PB10)  
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_PDM_CLK_13);
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_PDM_DATA0_13);

    app_pdm_setting();

    if(hx_drv_pdm_init(&pdm_dev_info)!= PDM_NO_ERROR)
        return -1;

    //please remind that there will be some noise at first few samples, due to PDM running DC removal function
    //the noise happen at first time record data after PDM init
    hx_drv_pdm_dma_lli_transfer((void *) audio_buf, blk_num, quarter_second_mono_bytes, 0);
    
    while(rx_cb_flag == false)
        ;

    rx_cb_flag = false; 

    xprintf("PDM example, record done\n");


    hx_drv_pdm_deinit();

	return 0;
}