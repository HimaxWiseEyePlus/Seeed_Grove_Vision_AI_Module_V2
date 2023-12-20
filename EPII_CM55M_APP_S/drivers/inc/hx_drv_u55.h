/*
 * hx_drv_u55.h
 *
 *  Created on: 2021�~7��15��
 *      Author: 902449
 */

#ifndef DRIVERS_INC_HX_DRV_U55_H_
#define DRIVERS_INC_HX_DRV_U55_H_

#include "dev_common.h"
#include "WE2_device_addr.h"

#define HX_U55_BASE U55_BASE_REG

/**
 * \defgroup	hx_drv_U55
 * \brief ethos-U55 NPU
 * \details this driver is based on "ARM ethos-u55" drvier and "tensorflow lite for mirco" library
 * 
 * <pre>
 * 		Sample code
 * 		Usage-1 load a model saved in flash.
 * 		#define TFLM_MODEL_SAVED_ADDR (0x3A180000)
 * 		constexpr int tensor_arena_size = 410*1024;
 * 		static uint8_t tensor_arena[tensor_arena_size]  __attribute__((section(".tensor_arena")));
 * 
 * 		///initailize EEPROM library & enable XIP mode.
 * 		hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
 * 		hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
 * 
 * 		///init u55, set to security, privilege.
 * 		hx_drv_u55_init(true, true, U55_BASE_REG);
 * 
 * 		TfLiteTensor* in_ten;
 * 	    TfLiteTensor* out_ten;
 * 		hx_drv_u55_load((void*)tensor_arena, (void *)TFLM_MODEL_SAVED_ADDR, (unsigned int)tensor_arena_size, (void**)&in_ten, (void**)&out_ten);
 * 
 * 		///load input image to the buffer of input tensor
 * 		for (int i = 0; i < in_ten->bytes; ++i) {	
 * 			///in_arr is iuput image, data type:int8_t
 * 			in_ten->data.int8[i] = in_arr[i];
 * 		}    
 * 
 * 		hx_drv_u55_run();
 * 		xprintf("person_score=%d,no_person_score=%d\n",out_ten->data.int8[1], out_ten->data.int8[0]);
 * 
 * </pre>
 */

/**
 * \enum U55_ERROR_E
 * \brief U55 Errors Type
 */
typedef enum {
	DRV_U55_NO_ERROR = 0, /**< No ERROR */
	DRV_U55_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	DRV_U55_NOT_INITIAL, /**< ERROR MSG: U55 not initial yet */
	DRV_U55_MODEL_VERSION_INCORRECT, /**< ERROR MSG: incorrect model version */
	DRV_U55_OUTPUT_DATA_ERROR, /**< ERROR MSG: output data after invoke are incorrect*/
	DRV_U55_ERROR, /**< ERROR MSG: ERROR happen */
} DRV_U55_ERROR_E;

/****************************************************
 * Structure Definition                             *
 ***************************************************/

/****************************************************
 * Function Declaration                             *
 ***************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	initial U55 with given security level and privilege status
 *
 * \param[in] security_enable		security level, set true to enable security
 * \param[in] privilege_enable		privilege status, set true to privilege mode and set false to user mode
 * \param[in] base_addr				The base address of the Ethos U55 controller.
 * \retval	DRV_U55_NO_ERROR		initial success
 * \retval	DRV_U55_ERROR			initial fail
 */
DRV_U55_ERROR_E hx_drv_u55_init(bool security_enable, bool privilege_enable, uint32_t base_addr);

/**
 * \brief	load u55 with given tensor arena and model area address
 * The operators of model given by user must must only be AddEthosU() function.
 * In other words, this function can't support the model with multiple operators.
 * 
 * \note After loading a model, it is not recommended to load another model again.
 * if multiple models are used by user, it is recommended to use the tensorflow lite library to implement 
 *
 * \param[in] tensor_address		tensor arena address pointer given by upper layer, please reserve at least 15KB arena address
 * \param[in] model_address			model arena address pointer given by upper layer
 * \param[in] tensor_arena_size		tensor arena size in bytes
 * \param[in/out] input				input tensor pointer given after load finish
 * \param[out] output				output tensor pointer
 * \retval	DRV_U55_NO_ERROR		run success
 * \retval	DRV_U55_ERROR			run fail
 */
DRV_U55_ERROR_E hx_drv_u55_load( void* tensor_address, void* model_address, unsigned int tensor_arena_size, void** input, void** output);

/**
 * \brief	run u55 with given input tensor, output result will be in the output pointer
 *
 * \retval	DRV_U55_NO_ERROR		run success
 * \retval	DRV_U55_ERROR			run fail
 */
DRV_U55_ERROR_E hx_drv_u55_run();

/**
 * \brief	image scale
 * \retval	DRV_U55_NO_ERROR		run success
 * \retval	DRV_U55_ERROR			run fail
 */
DRV_U55_ERROR_E hx_drv_u55_rescale(void *in_img, void *out_img, unsigned int in_width, unsigned int in_height, unsigned int out_width, unsigned int out_height, unsigned int channel_sz);

void hx_drv_u55_deinit();

#ifdef __cplusplus
}
#endif
#endif /* DRIVERS_INC_HX_DRV_U55_H_ */
