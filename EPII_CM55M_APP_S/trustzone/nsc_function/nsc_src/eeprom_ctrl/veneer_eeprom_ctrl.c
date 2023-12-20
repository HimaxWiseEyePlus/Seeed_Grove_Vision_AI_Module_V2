/*
 * veneer_eeprom_ctrl.c
 *
 *  Created on: 2022/05/27
 *      Author: 904207
 */
#if !defined(HX_TFM)

#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include <stdio.h>
#include "stdint.h"
#include "arm_cmse.h"
#include "veneer_eeprom_ctrl.h"
#include "spi_eeprom_comm.h"
#include "hx_drv_spi.h"

#define SECURITY_FLASH_LEN (0x800000)

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_open(VENEER_SPI_EEPROM_E spi_id)
{
    return hx_lib_spi_eeprom_open((USE_DW_SPI_MST_E) spi_id);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_open_speed(VENEER_SPI_EEPROM_E spi_id, uint32_t clk_hz)
{
    return hx_lib_spi_eeprom_open_speed((USE_DW_SPI_MST_E) spi_id, clk_hz);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_read_ID(VENEER_SPI_EEPROM_E spi_id, uint8_t *id_info)
{
    return hx_lib_spi_eeprom_read_ID((USE_DW_SPI_MST_E) spi_id, id_info);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_1read(VENEER_SPI_EEPROM_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len)
{
    if(flash_addr < SECURITY_FLASH_LEN)
        return E_MACV;
    return hx_lib_spi_eeprom_1read((USE_DW_SPI_MST_E) spi_id, flash_addr, data, len);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_2read(VENEER_SPI_EEPROM_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len)
{
    if(flash_addr < SECURITY_FLASH_LEN)
        return E_MACV;    
    return hx_lib_spi_eeprom_2read((USE_DW_SPI_MST_E) spi_id, flash_addr, data, len);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_4read(VENEER_SPI_EEPROM_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len)
{
    if(flash_addr < SECURITY_FLASH_LEN)
        return E_MACV;    
    return hx_lib_spi_eeprom_4read((USE_DW_SPI_MST_E) spi_id, flash_addr, data, len);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_word_read(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len)
{
    if(addr < SECURITY_FLASH_LEN)
        return E_MACV;    
    return hx_lib_spi_eeprom_word_read((USE_DW_SPI_MST_E) spi_id, addr, data, bytes_len);
} 

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_word_write(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len)
{
    if(addr < SECURITY_FLASH_LEN)
        return E_MACV;        
    return hx_lib_spi_eeprom_word_write((USE_DW_SPI_MST_E) spi_id, addr, data, bytes_len);
}

// __attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_1write(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint8_t *data_ptr, uint32_t bytes_len, uint8_t word_switch)
// {
//     // return hx_lib_spi_eeprom_write((USE_DW_SPI_MST_E) spi_id, addr, data_ptr, bytes_len, word_switch);
// }

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_write(VENEER_SPI_EEPROM_E spi_id, uint32_t addr, uint8_t *data, uint32_t bytes_len)
{
    if(addr < SECURITY_FLASH_LEN)
        return E_MACV;        
    return hx_lib_spi_eeprom_write((USE_DW_SPI_MST_E) spi_id, addr, data, bytes_len, 0); //TODO  word_switch need to be add
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_Send_Op_code(VENEER_SPI_EEPROM_E spi_id, uint8_t *data, uint32_t len)
{
    return hx_lib_spi_eeprom_Send_Op_code((USE_DW_SPI_MST_E) spi_id, data, len);
}

#if 0
__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_erase_all(VENEER_SPI_EEPROM_E spi_id)
{
    return hx_lib_spi_eeprom_erase_all((USE_DW_SPI_MST_E) spi_id);
} 
#endif

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_erase_sector(VENEER_SPI_EEPROM_E spi_id, uint32_t addr)
{
    if((addr & 0xfffff000) < SECURITY_FLASH_LEN)
        return E_MACV;        
    return hx_lib_spi_eeprom_erase_sector((USE_DW_SPI_MST_E) spi_id, addr, FLASH_SECTOR);
} 

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_eeprom_enable_XIP(VENEER_SPI_EEPROM_E spi_id, bool xip_enable, VENEER_FLASH_ACCESS_MODE_E xip_mode, bool xip_cont)
{
    return hx_lib_spi_eeprom_enable_XIP((USE_DW_SPI_MST_E) spi_id, xip_enable, (FLASH_ACCESS_MODE_E) xip_mode, xip_cont);
}

#endif //#if !defined(HX_TFM)
