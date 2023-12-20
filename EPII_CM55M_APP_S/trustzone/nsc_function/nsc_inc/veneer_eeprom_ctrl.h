/*
 * eeprom_ctrl.h
 *
 *  Created on: 20220601
 *      Author: 904207
 */
#ifndef TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_EEPROM_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_EEPROM_CTRL_H_
#include "stdbool.h"

/**
 * \enum VENEER_SPI_EEPROM_E
 * \brief SPI Master ID enum
 */
typedef enum 
{
    VENEER_QSPI_EEPROM = 0,
    RESERVE = 1,
    VENEER_OSPI_EEPROM = 2,
} VENEER_SPI_EEPROM_E;

typedef enum
{
    VENEER_FLASH_SINGLE = 1, /*!< Single mode  */
    VENEER_FLASH_DUAL = 2,   /*!< Dual mode  */
    VENEER_FLASH_QUAD = 4,   /*!< Quad mode */
} VENEER_FLASH_ACCESS_MODE_E;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief initialize SPI for eeprom. The default is the highest speed
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \return	.
 */
int32_t veneer_spi_eeprom_open(VENEER_SPI_EEPROM_E spi_id);

/**
 * \brief initialize SPI for eeprom and set spi speed. 
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \return	.
 */
int32_t veneer_spi_eeprom_open_speed(VENEER_SPI_EEPROM_E spi_id, uint32_t clk_hz);

/**
 * \brief read the identity of the SPI eeprom
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   in_info  return EEPORM ID array(3 bytes)
 * \return	.
 */
int32_t veneer_spi_eeprom_read_ID(VENEER_SPI_EEPROM_E spi_id, uint8_t *id_info);

/**
 * \brief single byte read eeprom with address, length information
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   flash_addr epprom address
 * \param[in]   data  return EEPORM data array
 * \param[in]   len data length
 * \return	.
 */
int32_t veneer_spi_eeprom_1read(VENEER_SPI_EEPROM_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);

/**
 * \brief dual bytes read eeprom with address, length information
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   flash_addr epprom address
 * \param[in]   data  return EEPORM data array
 * \param[in]   len data length
 * \return	.
 */
int32_t veneer_spi_eeprom_2read(VENEER_SPI_EEPROM_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);

/**
 * \brief quad bytes read eeprom with address, length information
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   flash_addr epprom address
 * \param[in]   data  return EEPORM data array
 * \param[in]   len data length
 * \return	.
 */
int32_t veneer_spi_eeprom_4read(VENEER_SPI_EEPROM_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);

/**
 * \brief word read eeprom with address, length information
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   flash_addr epprom address
 * \param[in]   data return EEPORM data array(4 bytes)
 * \param[in]   bytes_len data length
 * \return	.
 */
int32_t veneer_spi_eeprom_word_read(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len);

/**
 * \brief word write eeprom with address, length information
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   flash_addr epprom address
 * \param[in]   data  EEPORM data array(4 bytes)
 * \param[in]   bytes_len data length
 * \return	.
 */
int32_t veneer_spi_eeprom_word_write(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len);

/**
 * \brief write eeprom with address, length information. word_switch-when storage data in waord type, change word_switch to true for little endian/big endian switch
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   addr epprom address
 * \param[in]   data  data array
 * \param[in]   len data length
 * \return	.
 */
int32_t veneer_spi_eeprom_write(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint8_t *data, uint32_t bytes_len);

/**
 * \brief send op command to eeprom
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   data  op commaand array
 * \param[in]   len array length
 * \return	.
 */
int32_t veneer_spi_eeprom_Send_Op_code(VENEER_SPI_EEPROM_E spi_id, uint8_t *data, uint32_t len);

#if 0
/**
 * \brief chip erase selected eeprom
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \return	.
 */
int32_t veneer_spi_eeprom_erase_all(VENEER_SPI_EEPROM_E spi_id);
#endif
/**
 * \brief sector erase by given flash address
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   addr  epprom address
 * \return	.
 */
int32_t veneer_spi_eeprom_erase_sector(VENEER_SPI_EEPROM_E spi_id, uint32_t addr);

/**
 * \brief set select eeprom to specific XIP mode. xip_enable- set enable/disable XIP mode. xip_mode- set XIP mode, support dual mode pr quad mode
 *
 * \param[in]	spi_id	QSPI or OSPI
 * \param[in]   xip_enable set enable/disable XIP mode
 * \param[in]   xip_mode set XIP mode
 * \param[in]   xip_cont set enable/disable continuous transfer in XIP mode.
 * \return	.
 */
int32_t veneer_spi_eeprom_enable_XIP(VENEER_SPI_EEPROM_E spi_id, bool xip_enable, VENEER_FLASH_ACCESS_MODE_E xip_mode, bool xip_cont);

#ifdef __cplusplus
}
#endif

#endif /*TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_EEPROM_CTRL_H_*/

