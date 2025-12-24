/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
#ifndef _DW_I2S_OBJ_H_
#define _DW_I2S_OBJ_H_

#include "WE2_device.h"
#include "dev_common.h"
//#include "dev_i2s.h"

/**
 * \defgroup    I2S_AUD_DATA_RES    I2S Audio Data Resolution
 * \ingroup DEVICE_HAL_I2S_CFG
 * \brief   definitions for I2S audio data resolution
 * @{
 */
typedef enum i2s_aud_data_res {
    I2S_AUD_DATA_NA    = 0, /*!< N/A data resolution */
    I2S_AUD_DATA_16B   = 1, /*!< 16 bits audio data resolution */
    I2S_AUD_DATA_20B   = 2, /*!< 20 bits audio data resolution */
} I2S_AUD_DATA_RES;
/** @} */

/**
 * \defgroup    I2S_AUD_SAMPLE_RATE I2S Audio Sampling Rate
 * \ingroup DEVICE_HAL_I2S_CFG
 * \brief   definitions for I2S audio sampling rate
 * @{
 */
typedef enum i2s_aud_sample_rate {
    I2S_AUD_SR_NA     = 0, /*!< N/A audio sample rate */
    I2S_AUD_SR_16K    = 1, /*!< 16KHz audio sample rate */
    I2S_AUD_SR_32K    = 2, /*!< 32KHz audio sample rate */
    I2S_AUD_SR_48K    = 3, /*!< 48KHz audio sample rate */
    I2S_AUD_SR_96K    = 4, /*!< 96KHz audio sample rate */
} I2S_AUD_SAMPLE_RATE;
/** @} */

/**
 * \enum I2S_CAPTURE_CHANNEL_E
 * \brief I2S Capture Channel Type
 */
typedef enum {
    I2S_CAPTURE_CHANNEL_LEFT_ONLY = 0,  /**< Left channel Only */
    I2S_CAPTURE_CHANNEL_RIGHT_ONLY,     /**< Right channel Only*/
    I2S_CAPTURE_CHANNEL_STEREO,         /**< Stereo */
} I2S_CAPTURE_CHANNEL_E;

typedef struct I2S_CFG_S
{
    uint32_t                reg_addr;                      /* I2S register base address */
    I2S_AUD_SAMPLE_RATE           sr;
    I2S_AUD_DATA_RES         bit_num;
    uint8_t                dma_rx_ch;
} I2S_CFG_T;

/**
 * \enum I2S_PIN_MUX_E
 * \brief I2S Pin Mux
 */
typedef enum I2S_PIN_MUX_S{
    /* I2S Master */
    I2S_MST_SCLK_PIN_MUX_PB0_F14 = 0,
    I2S_MST_SCLK_PIN_MUX_PB4_F14,
    I2S_MST_SCLK_PIN_MUX_PB6_F14,
    I2S_MST_SCLK_PIN_MUX_PB9_F14,
    I2S_MST_WS_PIN_MUX_PB1_F14,
    I2S_MST_WS_PIN_MUX_PB5_F14,
    I2S_MST_WS_PIN_MUX_PB7_F14,
    I2S_MST_WS_PIN_MUX_PB10_F14,
    I2S_MST_SDO_PIN_MUX_PB2_F14,
    I2S_MST_SDO_PIN_MUX_PB8_F14,
    I2S_MST_SDO_PIN_MUX_PC3_F14,
    I2S_MST_SDI_PIN_MUX_PB3_F14,
    I2S_MST_SDI_PIN_MUX_PB11_F14,
    
    /* I2S Slave */
    I2S_SLV_SCLK_PIN_MUX_PB4_F15,
    I2S_SLV_SCLK_PIN_MUX_PB6_F15,
    I2S_SLV_SCLK_PIN_MUX_PB9_F15,
    I2S_SLV_WS_PIN_MUX_PB1_F15,
    I2S_SLV_WS_PIN_MUX_PB5_F15,
    I2S_SLV_WS_PIN_MUX_PB7_F15,
    I2S_SLV_WS_PIN_MUX_PB10_F15,
    I2S_SLV_SDO_PIN_MUX_PB2_F15,
    I2S_SLV_SDO_PIN_MUX_PB8_F15,
    I2S_SLV_SDO_PIN_MUX_PC3_F15,
    I2S_SLV_SDI_PIN_MUX_PB0_F15,
    I2S_SLV_SDI_PIN_MUX_PB3_F15,
    I2S_SLV_SDI_PIN_MUX_PB11_F15,
} I2S_PIN_MUX_E;
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
 * \brief   I2S Driver Init
 *
 * \param[in]	reg_baseaddr : I2S Master/Slave reg base address (BASE_ADDR_I2S_HOST/BASE_ADDR_I2S_SLAVE)
 * \param[in]	data_res : I2S data resolution
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_init(uint32_t reg_baseaddr, I2S_AUD_DATA_RES data_res);

/**
 * \brief   I2S Driver Deinit
 *
 */
int32_t hx_drv_i2s_deinit(void);

/**
 * \brief   I2S Regist Callback Function
 *
 * \param[in]	txcb : Pointer for the Call back function called while written audio frame completely
 * \param[in]	rxcb : Pointer for the Call back function called while read an audio frame completely
 * \param[in]	errcb : Pointer for the Call back function called while an error occured during read/write
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_register_cb(void *txcb, void *rxcb, void *errcb);

/**
 * \brief   I2S Driver Polling Read Function
 *
 * \param[in]	data : Buffer pointer for the data receiving from the RX FIFO
 * \param[in]	byte_sz : Buffer length in bytes
 * \return 		> 0 : data of bytes receiving from RX FIFO
 * \return 		else : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_rx_read(void *data, uint32_t byte_sz);

/**
 * \brief   I2S Driver Interrupt Read Function
 *
 * \param[in]	data : Buffer pointer for the data receiving from the RX FIFO
 * \param[in]	byte_sz : Buffer length in bytes
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_int_rx_read(void *data, uint32_t byte_sz);

/**
 * \brief   I2S Driver DMA Channel for Read
 *
 * \param[in]	dma_ch : DMA channel to be use
 */
void hx_drv_i2s_set_dma_rx_ch(uint8_t dma_ch);

/**
 * \brief   I2S Driver DMA Read Function
 *
 * \param[in]	data : Buffer pointer for the data receiving from the RX FIFO
 * \param[in]	byte_sz : Buffer length in bytes
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_dma_rx_read(void *data, uint32_t byte_sz);

/**
 * \brief   I2S Driver Polling Write Function
 *
 * \param[in]	data : Buffer pointer for the data sending to the TX FIFO
 * \param[in]	byte_sz : Buffer length in bytes
 * \return 		> 0 : data of bytes sending to TX FIFO
 * \return 		else : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_tx_write(void *data, uint32_t byte_sz);

/**
 * \brief   I2S Driver Interrupt Write Function
 *
 * \param[in]	data : Buffer pointer for the data sending to the TX FIFO
 * \param[in]	byte_sz : Buffer length in bytes
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_int_tx_write(void *data, uint32_t byte_sz);

/**
 * \brief   I2S Driver DMA Channel for Write
 *
 * \param[in]	dma_ch : DMA channel to be use
 */
void hx_drv_i2s_set_dma_tx_ch(uint8_t dma_ch);

/**
 * \brief   I2S Driver DMA Write Function
 *
 * \param[in]	data : Buffer pointer for the data sending to the RX FIFO
 * \param[in]	byte_sz : Buffer length in bytes
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_dma_tx_write(void *data, uint32_t byte_sz);

/**
 * \brief   I2S Driver Set Sample Rate frequency for I2S Master
 *
 * \param[in]	freq : frequency set to I2S, support I2S_AUD_SR_16K, I2S_AUD_SR_32K, I2S_AUD_SR_48K and I2S_AUD_SR_96K
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_set_pcm_freq(uint32_t freq);

/**
 * \brief   I2S Driver set capture channel
 *
 * \param[in]	cap_ch : capture channel set to I2S, support I2S_CAPTURE_CHANNEL_LEFT_ONLY, I2S_CAPTURE_CHANNEL_RIGHT_ONLY and I2S_CAPTURE_CHANNEL_STEREO
 * \return 		int32_t : an error code of type Main Error Code Definitions
 */
int32_t hx_drv_i2s_capture_channel(I2S_CAPTURE_CHANNEL_E cap_ch);

/**
 * \brief   I2S Driver flush the RX FIFO
 */
void hx_drv_i2s_rx_fifo_flush(void);

/**
 * \brief   I2S Driver flush the TX FIFO
 */
void hx_drv_i2s_tx_fifo_flush(void);
#endif /* _DW_I2S_OBJ_H_ */
