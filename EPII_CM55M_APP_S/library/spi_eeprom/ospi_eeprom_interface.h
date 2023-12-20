#ifndef _LIB_OSPI_EEPORM_INTERFACE_H
#define _LIB_OSPI_EEPORM_INTERFACE_H


#define DW_SPI_SPI_EEPROM_WINBOND_CH1 0x60
#define DW_SPI_SPI_EEPROM_WINBOND33V_CH1 0x40

#define DW_SPI_FLASH_WIP_POS 0
#define DW_SPI_FLASH_WRITE_ENABLE_POS 1
#define DW_SPI_MXIC_FLASH_QUAD_ENABLE_MASK 0x40
#define DW_SPI_MXIC_FLASH_QUAD_ENABLE_POS 6
#define DW_SPI_WINBOND_FLASH_QUAD_ENABLE_MASK 0x02
#define DW_SPI_WINBOND_FLASH_QUAD_ENABLE_POS 1

#define DW_SPI_FLASH_CMD_WRITE_STATUS 0x01
#define DW_SPI_FLASH_CMD_WRITE_STATUS2 0x31
#define DW_SPI_FLASH_CMD_VOLATILE_WRITE 0x50
#define DW_SPI_FLASH_CMD_SINGLE_WRITE 0x02
#define DW_SPI_FLASH_CMD_1X_ADDR_QUAD_WRITE 0x32
#define DW_SPI_FLASH_CMD_QUAD_WRITE 0x38
#define DW_SPI_FLASH_CMD_SINGLE_READ 0x03
#define DW_SPI_FLASH_CMD_DUAL_READ_SINGLE_ADDR 0x3B
#define DW_SPI_FLASH_CMD_DUAL_READ 0xBB
#define DW_SPI_FLASH_CMD_QUAD_READ 0xEB
#define DW_SPI_FLASH_CMD_STATUS_CHECK 0x05
#define DW_SPI_FLASH_CMD_STATUS2_CHECK 0x35
#define DW_SPI_FLASH_CMD_WRITE_ENABLE 0x06
#define DW_SPI_FLASH_CMD_ERASE_ALL 0x60
#define DW_SPI_FLASH_CMD_ERASE_SECTOR			0x20
#define DW_SPI_FLASH_CMD_ERASE_32KB				0x52
#define DW_SPI_FLASH_CMD_ERASE_64KB				0xD8
#define DW_SPI_FLASH_CMD_FLASH_INFO 0x9F
#define DW_SPI_FLASH_MXIC_PROTECT_MASK 0xBC
#define DW_SPI_FLASH_WINBOND_PROTECT_MASK 0xFC
#define DW_SPI_FLASH_WINBOND_PROTECT2_MASK 0x79
#define DW_SPI_FLASH_GIGADEV_PROTECT_MASK 0x9C

#ifdef LIB_OSPI_EEPROM
int32_t hx_lib_ospi_eeprom_open();
int32_t hx_lib_ospi_eeprom_open_speed(uint32_t clk_hz);

#ifdef SPI_EEPROM_READ_RELATED
int32_t hx_lib_ospi_eeprom_read_ID(uint8_t *id_info);
int32_t hx_lib_ospi_eeprom_1read(uint32_t flash_addr, uint8_t *data, uint32_t len);
int32_t hx_lib_ospi_eeprom_2read(uint32_t flash_addr, uint8_t *data, uint32_t len);
int32_t hx_lib_ospi_eeprom_4read(uint32_t flash_addr, uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_WORD_W_R_RELATED
int32_t hx_lib_ospi_eeprom_word_read(uint32_t addr, uint32_t *data, uint32_t bytes_len);
int32_t hx_lib_ospi_eeprom_word_write(uint32_t addr, uint32_t *data, uint32_t bytes_len);
#endif 


#ifdef SPI_EEPROM_WRITE_RELATED
int32_t hx_lib_ospi_eeprom_write(uint32_t addr, uint8_t *data, uint32_t len, uint8_t word_switch);
int32_t hx_lib_ospi_eeprom_Send_Op_code(uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_CHIP_ERASE_ERLATED
int32_t hx_lib_ospi_eeprom_erase_all();
#endif

#ifdef SPI_EEPROM_SECTOR_ERASE_RELATED
int32_t hx_lib_ospi_eeprom_erase_sector(uint32_t addr,  FLASH_ERASE_SIZE_E sz);
#endif 

#ifdef SPI_EEPROM_XIP_RELATED
int32_t hx_lib_ospi_eeprom_enable_XIP(bool xip_enable, FLASH_ACCESS_MODE_E xip_mode, bool xip_cont);
#endif      

#endif /*LIB_OSPI_EEPROM*/

#endif /*_LIB_OSPI_EEPORM_INTERFACE_H*/

