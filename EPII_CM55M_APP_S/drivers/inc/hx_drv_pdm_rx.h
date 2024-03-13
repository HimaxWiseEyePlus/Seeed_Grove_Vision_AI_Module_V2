/*
 * hx_drv_pdm_rx.h
 *
 *  Created on: 2019/03/08
 *      Author: 902453
 */

#ifndef INC_HX_DRV_PDM_RX_H_
#define INC_HX_DRV_PDM_RX_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/**
* \defgroup   PDM
* \brief      PDM (Pulse Density Modulation) interface
* \details    
* <pre>
*
* </pre>
* @{
*/


/**
 * \brief	an enumeration type `PDM_ERROR_E`
 * \details	five possible values representing different error messages related to a PDM (Pulse Density Modulation) system.
 */
typedef enum PDM_ERROR_S
{
    PDM_NO_ERROR                    = 0,        /**< No ERROR */
    PDM_ERROR_NOT_INIT              = 1,        /**< ERROR MSG: not init */
    PDM_ERROR_ALREADY_INIT          = 2,        /**< ERROR MSG: already init */
    PDM_ERROR_INIT_FAIL             = 3,        /**< ERROR MSG: init fail */
    PDM_ERROR_INVALID_PARAMETERS    = 4,        /**< ERROR MSG: Invalid parameters */
    PDM_UNKNOWN_ERROR               = 5,        /**< ERROR MSG: UNKNOWN ERROR*/
} PDM_ERROR_E;


/**
 * \brief	an enumeration type `PDM_PCM_FREQ_E`
 * \details	four possible values representing different sample rates for PCM audio data
 */
typedef enum PDM_PCM_FREQ_S
{
    PDM_PCM_FREQ_8K  = 0,       /**<  8KHz sample rate */
    PDM_PCM_FREQ_16K = 1,       /**< 16KHz sample rate */
    PDM_PCM_FREQ_32K = 2,       /**< 32KHz sample rate */
    PDM_PCM_FREQ_48K = 3,       /**< 48KHz sample rate */
} PDM_PCM_FREQ_E;


/**
 * \brief	an enumeration type `PDM_DATA_OUT_TO_VAD_ENABLE_S`
 * \details	enable or disable the output of PDM data to a HW-VAD (voice activity detection) module.
 */
typedef enum PDM_DATA_OUT_TO_VAD_ENABLE_S
{
    PDM_DATA_OUT_TO_VAD_DISABLE = 0,
    PDM_DATA_OUT_TO_VAD_ENABLE  = 1,
} PDM_DATA_OUT_TO_VAD_ENABLE_E;


/**
 * \brief	an enumeration type `PDM_DATA_PATH_OUT_TO_VAD_E`
 * \details	These values represent the output channels of a PDM audio stream that
 *  are being sent to a Voice Activity Detection (VAD) module.
 */
typedef enum PDM_DATA_PATH_OUT_TO_VAD_S
{
    PDM_DATA_PATH_0_OUT_TO_VAD = 0,
    PDM_DATA_PATH_1_OUT_TO_VAD = 1,
    PDM_DATA_PATH_2_OUT_TO_VAD = 2,
    PDM_DATA_PATH_3_OUT_TO_VAD = 3,
} PDM_DATA_PATH_OUT_TO_VAD_E;


/**
 * \brief	an enumeration type `PDM_DATA_IN_ENABLE_E`
 * \details	This enumeration type can be used to represent the enable/disable state of PDM data input path.
 */
typedef enum PDM_DATA_IN_ENABLE_S
{
    PDM_DATA_IN_DISABLE = 0,
    PDM_DATA_IN_ENABLE  = 1,
} PDM_DATA_IN_ENABLE_E;


/**
 * \brief	an enumeration type `PDM_DATA_IN_CH_E`
 * \details	This enumeration type is used to represent the different data path of PDM (Pulse Density Modulation) input.
 */
typedef enum PDM_DATA_IN_CH_S
{
    PDM_DATA_IN_CH_0 = 0,
    PDM_DATA_IN_CH_1 = 1,
    PDM_DATA_IN_CH_2 = 2,
    PDM_DATA_IN_CH_3 = 3,
} PDM_DATA_IN_CH_E;


/** PDM DMA Channel  **/
typedef enum PDM_DMA_CH_S
{
    PDM_USE_DMA2_CH_0 = 0,
    PDM_USE_DMA2_CH_1 = 1,
    PDM_USE_DMA2_CH_2 = 2,
    PDM_USE_DMA2_CH_3 = 3,
    PDM_USE_DMA2_CH_4 = 4,
    PDM_USE_DMA2_CH_5 = 5,
    PDM_USE_DMA2_CH_6 = 6,
    PDM_USE_DMA2_CH_7 = 7,
} PDM_DMA_CH_E;


/**
 * \enum PDM_PIN_MUX_E
 * \brief PDM PIN Mux
 */
typedef enum PDM_PIN_MUX_S{
    PDM_CLK_PIN_MUX_PB1_F13 = 0,
    PDM_CLK_PIN_MUX_PB4_F13,
    PDM_CLK_PIN_MUX_PB9_F13,
    PDM_CLK_PIN_MUX_PB9_F1,
    PDM_DATA_0_PIN_MUX_PB0_F13,
    PDM_DATA_0_PIN_MUX_PB5_F13,
    PDM_DATA_0_PIN_MUX_PB10_F13,
    PDM_DATA_0_PIN_MUX_PB10_F1,
    PDM_DATA_1_PIN_MUX_PB3_F13,
    PDM_DATA_1_PIN_MUX_PB11_F13,
    PDM_DATA_1_PIN_MUX_PB11_F1,
    PDM_DATA_2_PIN_MUX_PB2_F13,
    PDM_DATA_2_PIN_MUX_PC3_F1,
    PDM_DATA_3_PIN_MUX_PB6_F13,
    PDM_DATA_3_PIN_MUX_PB7_F13,
    PDM_DATA_3_PIN_MUX_PB8_F13,
    PDM_DATA_3_PIN_MUX_PC4_F1,
} PDM_PIN_MUX_E;


/**
 * \enum PDM_CAPTURE_CHANNEL_E
 * \brief PDM Capture Channel Type
 */
typedef enum PDM_CAPTURE_CHANNEL_S{
    PDM_CAPTURE_CHANNEL_LEFT_ONLY = 0,  /**< Left channel Only */
    PDM_CAPTURE_CHANNEL_RIGHT_ONLY,     /**< Right channel Only*/
    PDM_CAPTURE_CHANNEL_STEREO,         /**< Stereo */
} PDM_CAPTURE_CHANNEL_E;


/**
 * \enum PDM_CLKSRC_E
 * \brief PDM Clock source
 */
typedef enum {
	PDM_CLKSRC_LSCREF,  /**< PDM CLK Source: LSC Ref */
	PDM_CLKSRC_EXT,     /**< PDM CLK Source: External PDM Clock */
} PDM_CLKSRC_E;


/**
 * \brief	a structure named "PDM_CFG_S"
 * \details	contains various configuration parameters for a PDM (Pulse Density Modulation) interface.
 *  These parameters include the PDM clock source, sample rate, data input enable flags, capture channel, DMA channel, and receive mode. 
 */
typedef struct PDM_CFG_S
{
    uint32_t reg_addr;                      /* PDM register base address */
    uint16_t rx_fifo_threshold;             /* FIFO depth:8, threshold: 5 (default) */
    uint16_t oversampling_ratio;            /* from 3 to 8 (default) */
    uint16_t cic_stages;                    /* from 0 (default) to 1 */
    uint16_t cic_delay;                     /* from 0 (default) to 1 */
    uint16_t dc_removal;                    /* from 0 to 15, 6 (default) */
    uint16_t bit_range_shift;               /* from 0 to 15, 5 (default) */
    PDM_CLKSRC_E pdm_clk_src;
    PDM_PCM_FREQ_E sample_rate;
    PDM_DATA_IN_ENABLE_E data_in_0_en;
    PDM_DATA_IN_ENABLE_E data_in_1_en;
    PDM_DATA_IN_ENABLE_E data_in_2_en;
    PDM_DATA_IN_ENABLE_E data_in_3_en;
    PDM_CAPTURE_CHANNEL_E capture_channel;
    PDM_DMA_CH_E dma_ch;
} PDM_CFG_T;


/**
 * \brief	defining a function pointer type `PDM_ISR_CB
 * \details	that takes a single parameter of type `uint32_t` and returns `void`. 
 *  This function pointer type is used to define the callback functions for PDM interrupt service routines (ISRs) 
 *  that handle PDM receive, error, and clip events. 
 */
typedef void (*PDM_ISR_CB)(uint32_t int_status);


/**
 * \brief	defining a structure called `pdm_dev_info
 * \details	configuration information for the PDM device. It includes a `PDM_CFG_T` structure for PDM configuration parameters, 
 *  as well as three function pointers (`pdm_rx_cb`, `pdm_err_cb`, and `pdm_clip_cb`) for interrupt service routines (ISRs) 
 *  that handle PDM receive, error, and clip events. The structure is then typedef'd as `PDM_DEV_INFO`, with a pointer version `PDM_DEV_INFO_PTR`.
 */
typedef struct pdm_dev_info
{
    PDM_CFG_T pdm_cfg;
    PDM_ISR_CB pdm_rx_cb;
    PDM_ISR_CB pdm_err_cb;
    PDM_ISR_CB pdm_clip_cb;
} PDM_DEV_INFO, *PDM_DEV_INFO_PTR;


/**
 * This function initializes the PDM device with the given configuration parameters.
 * 
 * @param pdm_dev_info A pointer to a structure containing the PDM device information, including
 * configuration parameters.
 * 
 * @return a PDM_ERROR_E enumeration value, which could be one of the following:
 * PDM_ERROR_ALREADY_INIT, PDM_ERROR_INIT_FAIL, or PDM_NO_ERROR.
 */
PDM_ERROR_E hx_drv_pdm_init(PDM_DEV_INFO *pdm_dev_info);


/**
 * This function deinitializes the PDM device and returns an error if it was not initialized.
 * 
 * @return If `g_pdm_init` is true, `PDM_NO_ERROR` is returned. If `g_pdm_init` is false,
 * `PDM_ERROR_NOT_INIT` is returned.
 */
PDM_ERROR_E hx_drv_pdm_deinit(void);


/**
 * The function initializes PDM transfer and reads data if initialized, otherwise returns an error.
 * Please remind that this API uses interrupt mode with M55 to move the data from PDM to data buffer
 * 
 * @param data A pointer to the buffer where the received PDM data will be stored.
 * @param size The size parameter is a pointer to a variable that will hold the size of the data to be
 * transferred. The function hx_drv_pdm_init_transfer() will read data from the PDM receiver and store
 * it in the buffer pointed to by the data parameter. The size parameter will be updated with the
 * actual
 * 
 * @return If `g_pdm_init` is `true`, the function returns `PDM_NO_ERROR` after reading data from the
 * PDM receiver. If `g_pdm_init` is `false`, the function returns `PDM_ERROR_NOT_INIT` after printing
 * an error message.
 */
PDM_ERROR_E hx_drv_pdm_init_transfer(uint32_t *data, uint32_t *size);


/**
 * This function performs a single transfer of data from the PDM peripheral to memory using DMA.
 * 
 * @param buf Pointer to the buffer where the received data will be stored.
 * @param block_sz The size of the data block to be transferred in bytes. For single DMA transfer size, limitation will be < 8192 Bytes.
 * 
 * @return a PDM_ERROR_E enum value, which could be PDM_ERROR_NOT_INIT, PDM_ERROR_INVALID_PARAMETERS,
 * PDM_UNKNOWN_ERROR, or PDM_NO_ERROR.
 */
PDM_ERROR_E hx_drv_pdm_dma_single_transfer(void *buf, uint32_t block_sz);


/**
 * This function performs a PDM DMA LLI transfer.
 * 
 * @param buf Pointer to the buffer where the received data will be stored.
 * @param block_num The number of blocks to be transferred in the PDM DMA transfer. Maximum block number is 256.
 * @param block_sz The size of each block data to be transferred in bytes. For single DMA transfer size, limitation will be < 8192 Bytes.
 * @param infinite_loop The infinite_loop parameter is a boolean value that determines whether the PDM
 * DMA transfer should loop indefinitely or not. If set to true, the transfer will continue to loop
 * until it is manually stopped. If set to false, the transfer will only occur once.
 * callback function will be called when transfer finished.
 * When set infinite_loop to 0, callback happen when transfer finish.
 * When set infinite_loop to 1, callback will not happen since transfer will looply continue.
 * 
 * @return a PDM_ERROR_E enum value, which could be PDM_ERROR_NOT_INIT, PDM_ERROR_INVALID_PARAMETERS,
 * PDM_UNKNOWN_ERROR, or PDM_NO_ERROR.
 */
PDM_ERROR_E hx_drv_pdm_dma_lli_transfer(void *buf, uint32_t block_num, uint32_t block_sz, uint8_t infinite_loop);

/**
 * The function `hx_drv_pdm_dma_rx` configures and initiates a PDM DMA receive operation.
 * 
 * @param rx_buf The `rx_buf` parameter is a pointer to the buffer where
 * the received data will be stored during the PDM DMA RX operation. It is of type `void*` and points
 * to the memory location where the received data will be written.
 * @param rx_sz The `rx_sz` parameter represents the size of the data to be received in bytes. It is
 * checked in the code to ensure it is aligned to 4 bytes.
 * 
 * @return The function `hx_drv_pdm_dma_rx` will return either `PDM_NO_ERROR` if everything is
 * successful, or `PDM_UNKNOWN_ERROR` if an error occurs during the DMA transfer process.
 */
PDM_ERROR_E hx_drv_pdm_dma_rx(void *rx_buf, uint32_t rx_sz);

/**
 * This function stops the PDM transfer if it was previously initialized, otherwise it returns an
 * error.
 * 
 * @return The function `hx_drv_pdm_stop_transfer` returns a value of type `PDM_ERROR_E`. It returns
 * `PDM_NO_ERROR` if the PDM module has been initialized and the transfer has been stopped
 * successfully. It returns `PDM_ERROR_NOT_INIT` if the PDM module has not been initialized.
 */
PDM_ERROR_E hx_drv_pdm_stop_transfer(void);


/**
 * This function sets the PDM clock frequency based on the desired PCM frequency.
 * 
 * @param freq The desired PCM frequency to be set for the PDM interface. It is of type PDM_PCM_FREQ_E,
 * which is an enumeration of possible PCM frequencies (8KHz, 16KHz, 32KHz, and 48KHz).
 * 
 * @return a PDM_ERROR_E enum value, which could be either PDM_NO_ERROR or
 * PDM_ERROR_INVALID_PARAMETERS.
 */
PDM_ERROR_E hx_drv_pdm_set_pcm_freq(PDM_PCM_FREQ_E freq);


/**
 * The function sets the enable status of the PDM data input channel.
 * 
 * @param ch PDM_DATA_IN_CH_E enum type, which specifies the PDM data input channel (0-3).
 * @param en The enable/disable flag for PDM data input channel. It is of type PDM_DATA_IN_ENABLE_E.
 * 
 * @return a PDM_ERROR_E enum value, which could be PDM_ERROR_NOT_INIT or PDM_NO_ERROR.
 */
PDM_ERROR_E hx_drv_pdm_data_in_set(PDM_DATA_IN_CH_E ch, PDM_DATA_IN_ENABLE_E en);


/**
 * This function sets the PDM capture channel to either left only, right only, or stereo.
 * 
 * @param cap_ch The input parameter "cap_ch" is of type PDM_CAPTURE_CHANNEL_E, which is an enumeration
 * that specifies the PDM capture channel configuration. It can take one of the following values:
 * 
 * @return a PDM_ERROR_E enum value, which could be PDM_ERROR_NOT_INIT or PDM_NO_ERROR depending on the
 * execution of the function.
 */
PDM_ERROR_E hx_drv_pdm_capture_channel(PDM_CAPTURE_CHANNEL_E cap_ch);


/**
 * This function returns the number of PDM channels.
 * 
 * @return The function `hx_drv_pdm_channel_num_get` returns the number of PDM channels (min:1, max:8).
 */
uint8_t hx_drv_pdm_channel_num_get(void);


/**
 * This function sets the PDM VAD output enable and data path source.
 * 
 * @param enable A parameter of type PDM_DATA_OUT_TO_VAD_ENABLE_E, which specifies whether to enable or
 * disable the output of PDM data to the VAD (Voice Activity Detection) module.
 * @param path The channel parameter is used to select the PDM data path output to be sent to the
 * Voice Activity Detection (VAD) module. It is of type PDM_DATA_PATH_OUT_TO_VAD_E.
 * 
 * @return a PDM_ERROR_E enum value, which could be PDM_ERROR_NOT_INIT if the PDM module has not been
 * initialized, or PDM_NO_ERROR if the function executes successfully.
 */
PDM_ERROR_E hx_drv_pdm_vad_output(PDM_DATA_OUT_TO_VAD_ENABLE_E enable, PDM_DATA_PATH_OUT_TO_VAD_E path);


/**
 * The function sets the clock source for the PDM interface.
 * 
 * @param pdm_clk_src An enum type variable that specifies the source of the PDM clock. It can take one
 * of the following values: PDM_CLKSRC_E
 */
void hx_drv_pdm_clk_src_set(PDM_CLKSRC_E pdm_clk_src);

/** @} */ // end of PDM group

#endif /* INC_HX_DRV_PDM_RX_H_ */
