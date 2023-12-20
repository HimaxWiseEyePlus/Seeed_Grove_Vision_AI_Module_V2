/*
 * hx_drv_sdio.h
 *
 *  Created on: 2023年9月05日
 *      Author: 902449
 */

#ifndef DRIVERS_INC_HX_DRV_SDIO_H_
#define DRIVERS_INC_HX_DRV_SDIO_H_

#include <stdio.h>
#include <stdlib.h>

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \enum SDIO_ERROR_E
 * \brief SDIO Errors Type
 */
typedef enum {
	SDIO_PASS = 0, /**< No ERROR */
	SDIO_ERR_INVALID_PARA, /**< ERROR MSG: Invalid parameters */
	SDIO_ERR_FAIL, /**< ERROR MSG: Operation fail*/
} SDIO_ERROR_E;


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
 * \brief	SDIO card initialization
 *
 * \param[in] clk				clk value in Khz, you can set it to 1000(1000Khz=1Mhz) as default. value range can be 1000~25000 (1Mhz~25Mhz)
 * \param[in] bus_width			bus width of sdio, CSDD_BUS_WIDTH_1 = 1U, CSDD_BUS_WIDTH_4 = 4U, CSDD_BUS_WIDTH_8 = 8U, CSDD_BUS_WIDTH_4_DDR = 5U, CSDD_BUS_WIDTH_8_DDR = 6U
 * 								you can set bus_width to 1 as default.
 * \param[in] sdio_base_addr	sdio base address for further register control, it is need in driver level.
 * 								there is base address in WE2_device_addr.h - SD_REG_BASE for reference.
 *
 * \retval	SDIO_PASS			Operation success
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_init(unsigned int clk, unsigned int bus_width, unsigned int sdio_base_addr);

/**
 * \brief	read data from SD memory card with given condition, read out data will be placed to buffer address
 *
 * \param[in] sd_address		SD memory card address in bytes, data value should align to 512 bytes
 * \param[in] data_size			data size in bytes to read, data value should align to 512 bytes and size smaller than 65536 bytes
 * \param[out] buffer			data array pointer
 *
 * \retval	SDIO_PASS				Operation success
 * \retval	SDIO_ERR_INVALID_PARA	sector size error
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_mem_read(unsigned int sd_address, unsigned int data_size, unsigned char *buffer);

/**
 * \brief	write data to SD memory card with given condition, data in buffer array will be placed into sd memory card
 *
 * \param[in] sd_address		SD memory card address in bytes, data value should align to 512 bytes
 * \param[in] data_size			data size in bytes to read, data value should align to 512 bytes and size smaller than 65536 bytes
 * \param[in] buffer			data array pointer
 *
 * \retval	SDIO_PASS				Operation success
 * \retval	SDIO_ERR_INVALID_PARA	sector size error
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_mem_write(unsigned int sd_address, unsigned int data_size, unsigned char *buffer);

extern SDIO_ERROR_E hx_drv_sdio_cccr_read(unsigned int addr, unsigned char *data, unsigned char sz);

extern SDIO_ERROR_E hx_drv_sdio_cccr_write(unsigned int addr, unsigned char *data, unsigned char sz);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INC_HX_DRV_SDIO_H_ */
