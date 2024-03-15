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
// #include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
// #include "veneer_table.h"
//
#endif
#endif

#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "board.h"
#include "hx_drv_scu.h"
#include "hx_drv_spi.h"
#include "hx_drv_gpio.h"
#include "WE2_core.h"
#include "WE2_debug.h"
#include "mmc_we2.h"
#include "hx_drv_swreg_aon.h"
#include "hx_drv_pdm_rx.h"


PDM_DEV_INFO pdm_dev_info;

// audio data is a 16-bits data (2bytes)
// for 16Khz of 1 seconds mono audio, takes 2*16*1000 bytes
// create a block with 8000 bytes to save 0.25 second of mono audio data (to meet hx_drv_pdm_dma_lli_transfer size limitation about <8192 bytes)
// create 40 blocks to save 10 second of data
#define quarter_second_mono_bytes 8000
#define blk_num 20

// allocate a audio buffer with 320000 bytes, this buffer is located in the SRAM which described in the linker script (pdm_single.ld)
int16_t audio_buf[blk_num * quarter_second_mono_bytes / 2];

volatile bool rx_cb_flag;
/*******************************************************************************
 * Code
 ******************************************************************************/
void app_pdm_dma_rx_cb()
{
	hx_InvalidateDCache_by_Addr(audio_buf, quarter_second_mono_bytes * blk_num);
	xprintf("\n\n[%s]\n\n", __FUNCTION__);
	rx_cb_flag = true;
}

int app_pdm_setting()
{
	memset((void *)&pdm_dev_info, 0x00, sizeof(PDM_DEV_INFO));
	//
	pdm_dev_info.pdm_cfg.reg_addr = HIMAX_PDM_BASE_ADDR;
	pdm_dev_info.pdm_cfg.rx_fifo_threshold = 5;
	pdm_dev_info.pdm_cfg.oversampling_ratio = 8; // 6: 32KHz, 8: 16KHz
	pdm_dev_info.pdm_cfg.cic_stages = 0;
	pdm_dev_info.pdm_cfg.cic_delay = 0;
	pdm_dev_info.pdm_cfg.dc_removal = 6;
	pdm_dev_info.pdm_cfg.bit_range_shift = 5;

	// use DATA0 with left channnel selected only(mono)
	pdm_dev_info.pdm_cfg.data_in_0_en = PDM_DATA_IN_ENABLE;
	pdm_dev_info.pdm_cfg.data_in_1_en = PDM_DATA_IN_DISABLE;
	pdm_dev_info.pdm_cfg.data_in_2_en = PDM_DATA_IN_DISABLE;
	pdm_dev_info.pdm_cfg.data_in_3_en = PDM_DATA_IN_DISABLE;
	pdm_dev_info.pdm_cfg.capture_channel = PDM_CAPTURE_CHANNEL_LEFT_ONLY;

	// sampling rate = 16K, use DMA2 channel 0
	pdm_dev_info.pdm_cfg.dma_ch = PDM_USE_DMA2_CH_0;
	pdm_dev_info.pdm_cfg.pdm_clk_src = PDM_CLKSRC_LSCREF;
	pdm_dev_info.pdm_cfg.sample_rate = PDM_PCM_FREQ_16K;
	pdm_dev_info.pdm_rx_cb = app_pdm_dma_rx_cb;
	pdm_dev_info.pdm_err_cb = NULL;
}

/*!
@brief Main function
 */
int app_main(void)
{

	hx_drv_scu_set_SEN_D0_pinmux(SCU_SEN_D0_PINMUX_GPIO18); // button
	hx_drv_scu_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_GPIO20); // light
	hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_GPIO0_1, 0);		// 7
	hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_GPIO1_1, 0);		// 8
	hx_drv_scu_set_PB8_pinmux(SCU_PB8_PINMUX_GPIO2_1, 0);		// 3
	hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_GPIO2, 0);		// 2
	hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_2, 0);	// 1

	hx_drv_gpio_set_input(GPIO18);
	hx_drv_gpio_set_output(GPIO20, 0);

	hx_drv_gpio_set_output(GPIO0, 0);
	hx_drv_gpio_set_output(GPIO1, 0);

	hx_drv_gpio_set_output(GPIO2, 0);

	hx_drv_gpio_set_output(AON_GPIO0, 0);

	rx_cb_flag = false;

	// select pin mux for PDM, enable PDM CLK(PB9) and PDM DATA0 pin(PB10)
	hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_PDM_CLK_13, 0);
	hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_PDM_DATA0_13, 0);

	app_pdm_setting();

	if (hx_drv_pdm_init(&pdm_dev_info) != PDM_NO_ERROR)
		return -1;

	// please remind that there will be some noise at first few samples, due to PDM running DC removal function
	// the noise happen at first time record data after PDM init

	FATFS fs;
	TCHAR drv[3] = {_T('0'), _T(':'), _T('0')};
	FRESULT res = f_mount(&fs, drv, 1);
	bool mount = false;
	if (res != FR_OK)
	{
		mount = false;
		xprintf("Mount Failed: %d\n", res);
	}
	else
	{
		mount = true;
		xprintf("Mount Success\n");
	}
	// file read/write test
	uint32_t count = 0;
	char buffer[512];
	int value = 0;
	if (mount)
	{
		FIL file;

		res = f_open(&file, _T("/test.txt"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

		if (res != FR_OK)
		{
			xprintf("Open Failed: %d\n", res);
		}
		else
		{

			memset(buffer, 0, 512);
			sprintf(buffer, "Hello World %d", count++);
			res = f_write(&file, buffer, strlen(buffer), NULL);

			if (res != FR_OK)
			{
				xprintf("Write Failed: %d\n", res);
				f_close(&file);
			}
			else
			{

				f_close(&file);

				xprintf("Write: %s sucessful\n", buffer);

				res = f_open(&file, _T("/test.txt"), FA_READ);
				memset(buffer, 0, 512);
				res = f_read(&file, buffer, 512, NULL);
				if (res != FR_OK)
				{
					xprintf("Read Failed: %d\n", res);
					f_close(&file);
				}
				else
				{

					xprintf("Read: %s sucessful\n", buffer);

					f_close(&file);
				}
			}
		}
	}

	xprintf("PDM example, 16KHz mono, record a single shot\n");
	hx_drv_pdm_dma_lli_transfer((void *)audio_buf, blk_num, quarter_second_mono_bytes, 0);

	while (1)
	{

		value = 1 - value;

		hx_drv_gpio_set_out_value(GPIO0, value);
		hx_drv_gpio_set_out_value(GPIO1, value);
		hx_drv_gpio_set_out_value(GPIO2, value);
		hx_drv_gpio_set_out_value(AON_GPIO0, value);

		uint8_t read_status = 0;
		hx_drv_gpio_get_in_value(GPIO18, &read_status);
		if (read_status == 1)
		{
			hx_drv_gpio_set_out_value(GPIO20, 1);
		}
		else
		{
			hx_drv_gpio_set_out_value(GPIO20, 0);
		}

		if (rx_cb_flag)
		{
			rx_cb_flag = false;

			xprintf("PDM example, record done\n");

			for (int i = 0; i < blk_num * quarter_second_mono_bytes / 2; i++)
			{
				xprintf("%04x\n", audio_buf[i] & 0xffff);
				uint8_t read_status = 0;
				hx_drv_gpio_get_in_value(GPIO18, &read_status);
				if (read_status == 1)
				{
					hx_drv_gpio_set_out_value(GPIO20, 1);
				}
				else
				{
					hx_drv_gpio_set_out_value(GPIO20, 0);
				}
			}

			xprintf("PDM example, print done\n");

			hx_drv_pdm_deinit();
		}
		board_delay_ms(50);
	}

	return 0;
}
