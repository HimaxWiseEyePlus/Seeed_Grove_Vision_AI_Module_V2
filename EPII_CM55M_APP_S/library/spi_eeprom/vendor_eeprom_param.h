#ifndef _LIB_SPI_EEPROM_VENDOR_PARAM_H
#define _LIB_SPI_EEPROM_VENDOR_PARAM_H
#include <stdio.h>
#include "WE2_device.h"

// #define SPI_EEPROM_USE_WB_25Q128JW_INST_
// #define SPI_EEPROM_USE_WB_25Q64JW_INST_
// #define SPI_EEPROM_USE_WB_25Q32JW_INST_
// #define SPI_EEPROM_USE_WB_25Q16JW_INST_
// #define SPI_EEPROM_USE_MX_25U12843_INST_
// #define SPI_EEPROM_USE_MX_25U6432_INST_
// #define SPI_EEPROM_USE_MX_25U3232_INST_
// #define SPI_EEPROM_USE_MX_25U1632_INST_
// #define SPI_EEPROM_USE_GD_25LQ128_INST_
// #define SPI_EEPROM_USE_GD_25LQ64_INST_
// #define SPI_EEPROM_USE_GD_25LQ32_INST_
// #define SPI_EEPROM_USE_GD_25LQ16_INST_

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
#define __SPI_EEPROM_NAME(__VENDOR, __SIZE, __NAME) __VENDOR ## _ ## __SIZE ## _ ## __NAME
#define __SPI_EEPROM_CMD(__VENDOR, __SIZE, __CMD) __VENDOR ## _ ## __SIZE ## _CMD_ ## __CMD
#define __SPI_EEPROM_CONFIG(__VENDOR, __SIZE, __CMD, __PARAM) __VENDOR ## _ ## __SIZE ## _ ## __CMD ## _## __PARAM

typedef struct spi_eeprom_cmd_config_s
{
    uint8_t spi_frf;
    uint8_t inst_len;
    uint8_t addr_len;
    uint8_t trans_type;
    uint8_t wait_cycle;
} SPI_EEPROM_CMD_CONFIG_S;

typedef struct spi_eeprom_param_s
{
    uint8_t flash_vendor;
    uint8_t flash_size;
    
    uint8_t flash_info_cmd;

    uint8_t flash_1read_cmd;

    uint8_t flash_2read_cmd;
    SPI_EEPROM_CMD_CONFIG_S dual_read_config;

    uint8_t flash_4read_cmd;
    SPI_EEPROM_CMD_CONFIG_S quad_read_config;

    uint8_t flash_word_read_cmd;
    SPI_EEPROM_CMD_CONFIG_S word_read_config;

    uint8_t flash_word_write_cmd;
    SPI_EEPROM_CMD_CONFIG_S word_write_config;

    uint8_t flash_write_cmd;

    uint8_t flash_erase_all_cmd;

    uint8_t flash_erase_sector_4k;
    uint8_t flash_erase_sector_32k;
    uint8_t flash_erase_sector_64k;

    uint8_t flash_xip_raed_dual_cmd;
    SPI_EEPROM_CMD_CONFIG_S xip_read_dual_config;

    uint8_t flash_xip_raed_quad_cmd;
    SPI_EEPROM_CMD_CONFIG_S xip_read_quad_config;

    uint8_t status1_check_cmd;
    uint8_t status2_check_cmd;
    uint8_t status3_check_cmd;

    uint8_t status1_write_cmd;
    uint8_t status2_write_cmd;
    uint8_t status3_write_cmd;
    uint8_t status1_write_counts; /*< if counts == 2, it means that status1_writes_command + status1 reg value + status2 reg value */

    uint8_t write_enable_cmd;

    uint8_t wel_status_read_cmd;
    uint8_t wel_pos;

    uint8_t wip_status_read_cmd;
    uint8_t wip_pos;

    uint8_t protect_en_mask; /*< enable bit by bit*/
    uint8_t protect1_mask;
    uint8_t protect2_mask; /*< If not required, set to 0 0*/

    uint8_t QE_status_write_cmd;
    uint8_t QE_status_read_cmd;
    /** if QE_status_write_count == 1, 
     *      it means that QE_status_write_cmd + status1 reg. value ,and QE bit in status1 register 
     *  if QE_status_write_count == 2, 
     *      it means that QE_status_write_cmd + status1 reg. value + status2 reg. value ,and QE bit in status2 register 
    */
    uint8_t QE_status_write_count; 
    uint8_t QE_pos;

} SPI_EEPROM_PARAM_S;

#ifdef SPI_EEPROM_USE_WB_25Q128JW_INST_
extern SPI_EEPROM_PARAM_S WB_25Q128JW_inst_config;
#endif
#ifdef SPI_EEPROM_USE_WB_25Q64JW_INST_
extern SPI_EEPROM_PARAM_S WB_25Q64JW_inst_config;
#endif
#ifdef SPI_EEPROM_USE_WB_25Q32JW_INST_
extern SPI_EEPROM_PARAM_S WB_25Q32JW_inst_config;
#endif
#ifdef SPI_EEPROM_USE_WB_25Q16JW_INST_
extern SPI_EEPROM_PARAM_S WB_25Q16JW_inst_config;
#endif

#ifdef SPI_EEPROM_USE_MX_25U12843_INST_
extern SPI_EEPROM_PARAM_S MX_25U12843_inst_config;
#endif
#ifdef SPI_EEPROM_USE_MX_25U6432_INST_
extern SPI_EEPROM_PARAM_S MX_25U6432_inst_config;
#endif
#ifdef SPI_EEPROM_USE_MX_25U3232_INST_
extern SPI_EEPROM_PARAM_S MX_25U3232_inst_config;
#endif
#ifdef SPI_EEPROM_USE_MX_25U1632_INST_
extern SPI_EEPROM_PARAM_S MX_25U1632_inst_config;
#endif

#ifdef SPI_EEPROM_USE_GD_25LQ128_INST_
extern SPI_EEPROM_PARAM_S GD_25LQ128_inst_config;
#endif
#ifdef SPI_EEPROM_USE_GD_25LQ64_INST_
extern SPI_EEPROM_PARAM_S GD_25LQ64_inst_config;
#endif
#ifdef SPI_EEPROM_USE_GD_25LQ32_INST_
extern SPI_EEPROM_PARAM_S GD_25LQ32_inst_config;
#endif 
#ifdef SPI_EEPROM_USE_GD_25LQ16_INST_
extern SPI_EEPROM_PARAM_S GD_25LQ16_inst_config;
#endif 
#endif /*_LIB_SPI_EEPROM_VENDOR_PARAM_H*/
