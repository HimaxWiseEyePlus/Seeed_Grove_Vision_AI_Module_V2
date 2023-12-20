#ifndef _LIB_QSPI_EEPORM_INTERFACE_H
#define _LIB_QSPI_EEPORM_INTERFACE_H
#include "spi_eeprom_comm.h"
int32_t hx_lib_qspi_eeprom_open();
int32_t hx_lib_qspi_eeprom_open_speed(uint32_t clk_hz);

#ifdef SPI_EEPROM_READ_RELATED
int32_t hx_lib_qspi_eeprom_read_ID(uint8_t *id_info);
int32_t hx_lib_qspi_eeprom_1read(uint32_t flash_addr, uint8_t *data, uint32_t len);
int32_t hx_lib_qspi_eeprom_2read(uint32_t flash_addr, uint8_t *data, uint32_t len);
int32_t hx_lib_qspi_eeprom_4read(uint32_t flash_addr, uint8_t *data, uint32_t len);
int32_t hx_lib_qspi_eeprom_Send_Op_Read_Data(uint8_t *op, uint32_t op_len, uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_WORD_W_R_RELATED
int32_t hx_lib_qspi_eeprom_word_read(uint32_t addr, uint32_t *data, uint32_t bytes_len);
int32_t hx_lib_qspi_eeprom_word_write(uint32_t addr, uint32_t *data, uint32_t bytes_len);
#endif 

#ifdef SPI_EEPROM_WRITE_RELATED
int32_t hx_lib_qspi_eeprom_write(uint32_t addr, uint8_t *data, uint32_t len, uint8_t word_switch);
int32_t hx_lib_qspi_eeprom_Send_Op_code(uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_CHIP_ERASE_ERLATED
int32_t hx_lib_qspi_eeprom_erase_all();
#endif

#ifdef SPI_EEPROM_SECTOR_ERASE_RELATED
int32_t hx_lib_qspi_eeprom_erase_sector(uint32_t addr,  FLASH_ERASE_SIZE_E sz);
#endif 

#ifdef SPI_EEPROM_XIP_RELATED
int32_t hx_lib_qspi_eeprom_enable_XIP(bool xip_enable, FLASH_ACCESS_MODE_E xip_mode, bool xip_cont);
#endif                                     

int32_t hx_lib_qspi_switch_flash_config(CUR_FLASH_TYPE_E flash_type, CUR_FLASH_SIZE_E flash_size, SPI_EEPROM_PARAM_S* flash_config);
#endif /*_LIB_QSPI_EEPORM_INTERFACE_H*/
