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

/**
 * \enum SDIO_DriverStrengthType_E
 * \brief Driver strength type
 */
typedef enum
{
    /** Device driver strength A */
    SDIO_SWITCH_DRIVER_STRENGTH_TYPE_A = 1U,
    /** Device driver strength B */
    SDIO_SWITCH_DRIVER_STRENGTH_TYPE_B = 2U,
    /** Device driver strength C */
    SDIO_SWITCH_DRIVER_STRENGTH_TYPE_C = 3U,
    /** Device driver strength D */
    SDIO_SWITCH_DRIVER_STRENGTH_TYPE_D = 4U
} SDIO_DriverStrengthType_E;

/** 
 * \enum SDCARD_DriverCurrentLimit_E
 * \brief SD card driver current limit 
 */
typedef enum
{
    /** Card driver current limit - 200mA default */
    SDCARD_SWITCH_CURRENT_LIMIT_200 = 0U,
    /** Card driver current limit - 400mA */
    SDCARD_SWITCH_CURRENT_LIMIT_400 = 1U,
    /** Card driver current limit - 600mA */
    SDCARD_SWITCH_CURRENT_LIMIT_600 = 2U,
    /** Card driver current limit - 800mA */
    SDCARD_SWITCH_CURRENT_LIMIT_800 = 3U
} SDCARD_DriverCurrentLimit_E;

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
 * \retval  SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_mem_write(unsigned int sd_address, unsigned int data_size, unsigned char *buffer);

/**
 * \brief 	reads the CCCR register from SD Card using Command CMD52.
 * 
 * \param addr[in]              CCCR register address.
 * \param data[out]             pointer to read data buffer
 * \param sz[in]                size of Data buffer in bytes
 * 
 * \return  SDIO_PASS                Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_cccr_read(unsigned int addr, unsigned char *data, unsigned char sz);

/**
 * \brief 	write the CCCR register from SD Card using Command CMD52.
 * 
 * \param addr                  CCCR register address.
 * \param data                  pointer to write data buffer
 * \param sz                    size of Data buffer in bytes
 * 
 * \return  SDIO_PASS                Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_cccr_write(unsigned int addr, unsigned char *data, unsigned char sz);

/**
 * \brief erases data to SD memory card with given condition.
 * 
 * \param sd_address[in]        SD memory card address in bytes, data value should align to 512 bytes
 * \param data_size[in]         data size in bytes to read, data value should align to 512 bytes
 * 
 * \retval	SDIO_PASS				Operation success
 * \retval	SDIO_ERR_INVALID_PARA	sector size error
 * \retval  SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_mem_erase(unsigned int sd_address, unsigned int data_size);


/**
 * \brief reads SD card status using Command ACMD13
 * 
 * \param buffer[out]           pointer to read data buffer,and it requires at least 64 bytes.
 * 
 * \return  SDIO_PASS               Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_readSDstatus(unsigned char *buffer);

/**
 * \brief reads CSD register of the SD card using Command CMD9.
 * \note The sort of CSD register is little-endian.
 * 
 * \param buffer[out]          pointer to read data buffer,and it requires at least 16 bytes.     
 * 
 * \return  SDIO_PASS               Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_readCSD(unsigned int  *buffer);

/**
 * \brief waits until request finish and returns status of request
 * 
 * \return  SDIO_PASS               Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_WaitInReq(void);

/**
 * \brief sets a SD card max operation current using command CMD6
 * 
 * \param currentLimit[in]          Max current 
 * 
 * \return  SDIO_PASS               Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_SetMaxCurrent(SDCARD_DriverCurrentLimit_E currentLimit);

/**
 * \brief configure driver strength of the SD card
 * 
 * \param driverStrength[in]        new driver strength value
 * 
 * \return  SDIO_PASS               Operation success
 * \retval  SDIO_ERR_FAIL		    Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_SetDriverStrength(SDIO_DriverStrengthType_E driverStrength);

/**
 * \brief	 selects or deselects a card;
 * 		if isSelected parameter is 0, then card will be deselected, 
 * 		otherwise,a card of the RCA value will be selected using command SDIO_CMD7 
 *	
 * \param[in] isSelected[in]		True to set the card into transfer state, false to disselect. 
 *
 * \retval	SDIO_PASS			Operation success
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_SelectCard(unsigned char isSelected);

/**
 * \brief   enable/disable SDCLK clock depending on the Enable parameter
 * 
 * \param enable                    if it is 1 enable clock, if 0 disable clock
 * 
 * \retval	SDIO_PASS			Operation success
 */
extern SDIO_ERROR_E hx_drv_sdio_setSDclk(unsigned char enable);

/**
 * \brief resets SD Host controller
 * 
 * \retval	SDIO_PASS			Operation success
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_HostReset(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INC_HX_DRV_SDIO_H_ */
