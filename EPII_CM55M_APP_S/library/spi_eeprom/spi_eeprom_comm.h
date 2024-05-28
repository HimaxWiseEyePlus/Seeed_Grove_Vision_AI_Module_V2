#ifndef LIBRARY_SPI_EEPROM_COMM_H_
#define LIBRARY_SPI_EEPROM_COMM_H_
#include <stdio.h>
#include "dev_common.h"
#include "hx_drv_dmac.h"
#include "hx_drv_spi.h"
#include "WE2_core.h"
#include "cachel1_armv7.h"

#ifdef NSC
#include "veneer_clk_ctrl.h"
#else
#include "hx_drv_scu.h"
#endif
#include "vendor_eeprom_param.h"
/** 
 * \defgroup SPI EEPRM library
 * \note default flash command table is Winbond 128Mb version
 * If Winbond 128Mb isn't selected in spi_eeprom.mk, then default flash command will be NULL
 * Flash command table can be set by using \ref hx_lib_spi_eeprom_set_flash_config or \ref hx_lib_spi_eeprom_read_ID function.
 */
#define SPI_EEPROM_XIP_RELATED
#define SPI_EEPROM_WRITE_RELATED
#define SPI_EEPROM_READ_RELATED
#define SPI_EEPROM_WORD_W_R_RELATED
#define SPI_EEPROM_CHIP_ERASE_ERLATED
#define SPI_EEPROM_SECTOR_ERASE_RELATED

/**
 * \note if this flag is defined, all functions will to set this quad enable bit first
 */
#define LIB_SPI_EEPROM_SET_QUAD_MODE_BIT

/**
 * \note if this flag is defined, erase flash will to clear write protect bit first
 */
#define LIB_SPI_EEPROM_SET_W_PROTECT_BIT

/**
 * \brief if this flag is defined, 
 *      hx_lib_spi_eeprom_set_flash_config is automatically executed when hx_lib_spi_eeprom_read_ID is used.
 *      hx_lib_spi_eeprom_read_ID is automatically executed when the spi eeprom is initialized.
 */
// #define LIB_SPI_EEPROM_AUTO_SET_INST_TABLE

/**
 * \note This flag must be defined, otherwise spi read/write function will be unable to work.
 */
#define LIB_SPI_EEPROM_USE_DMA

/**
 * \brief This flag is used to spi write/word write function
 * if this flag is defined, temp buffer size need by spi write function will be smaller
 * \note only QSPI support
 */
// #define LIB_SPI_EEPORM_USE_DMA_LLI_TRANSFER

#ifdef BOOTROM
#undef SPI_EEPROM_WRITE_RELATED
#undef SPI_EEPROM_READ_RELATED
#undef SPI_EEPROM_WORD_W_R_RELATED
#undef SPI_EEPROM_CHIP_ERASE_ERLATED
#undef SPI_EEPROM_SECTOR_ERASE_RELATED
#undef LIB_SPI_EEPROM_SET_QUAD_MODE_BIT
#undef LIB_SPI_EEPROM_USE_DMA
#undef LIB_SPI_EEPORM_USE_DMA_LLI_TRANSFER
#endif

#ifdef BOOTLOADER
#undef SPI_EEPROM_READ_RELATED
#undef SPI_EEPROM_WORD_W_R_RELATED
#undef SPI_EEPROM_CHIP_ERASE_ERLATED
#undef LIB_SPI_EEPORM_USE_DMA_LLI_TRANSFER
#endif

#ifdef SECONDBOOTLOADER
//#undef SPI_EEPROM_READ_RELATED
#undef SPI_EEPROM_WORD_W_R_RELATED
//#undef LIB_SPI_EEPORM_USE_DMA_LLI_TRANSFER
#define LIB_SPI_EEPROM_AUTO_SET_INST_TABLE
#endif

// only support OSPI QSPI
typedef void (*spi_eeprom_cb_t)(void);

typedef enum CUR_FLASH_TYPE_S
{
    FLASH_TYPE_NONE = 0,
    FLASH_TYPE_MXIC,
    FLASH_TYPE_WINBOND,
    FLASH_TYPE_GIGADEV,
} CUR_FLASH_TYPE_E;

typedef enum CUR_FLASH_SIZE_
{
    FLASH_SIZE_NONE = 0,
    FLASH_SIZE_8Mb = 1,
    FLASH_SIZE_16Mb = 2,
    FLASH_SIZE_32Mb = 3,
    FLASH_SIZE_64Mb = 4,
    FLASH_SIZE_128Mb = 5 ,
} CUR_FLASH_SIZE_E;

#define DAFAULT_FALSH_TYPE FLASH_TYPE_WINBOND

typedef enum {
	FLASH_SECTOR = 0,
	FLASH_32KBLOCK,
	FLASH_64KBLOCK,
}FLASH_ERASE_SIZE_E;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initial QSPI/OSPI master for EEPROM, default speed is max 
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @return int32_t an error code \headerfile "dev_common.h"
 * @note if defined \ref SPI_EEPROM_READ_RELATED,it will read flash ID to switch flash configuration,
 *      otherwise the user needs to set the configuration by \ref hx_lib_spi_eeprom_set_flash_config function.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_open(USE_DW_SPI_MST_E spi_id);

/**
 * @brief initial QSPI/OSPI master for EEPROM, speed of spi is seted by user
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param clk_hz the output clock of SPI
 * @note if defined SPI_EEPROM_READ_RELATED,it will read flash ID to switch flash configuration,
 *      otherwise the user needs to set the configuration by \ref hx_lib_spi_eeprom_set_flash_config function. 
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_open_speed(USE_DW_SPI_MST_E spi_id, uint32_t clk_hz);

#ifdef SPI_EEPROM_READ_RELATED
/**
 * @brief read eeprom id information
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param id_info Pointer to the buffer where the received EEPROM Device Identification  will be stored.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_read_ID(USE_DW_SPI_MST_E spi_id, uint8_t *id_info);

/**
 * @brief read eeprom using single mode with address, length information, data frame size= 1 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param flash_addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_1read(USE_DW_SPI_MST_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);
/**
 * @brief read eeprom using dual mode with address, length information, data frame size= 1 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param flash_addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_2read(USE_DW_SPI_MST_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);
/**
 * @brief read eeprom using quad mode with address, length information, data frame size= 1 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param flash_addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_4read(USE_DW_SPI_MST_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);

/**
 * @brief send op command then recvice data from eeprom.
 * 
 * @param spi_id  ID of spi, QSPI
 * @param op Pointer to the data buffer to be written to EEROM device
 * @param op_len The length of the data that will be writed from the EEPROM. max vaild value:256.
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */

extern int32_t hx_lib_spi_eeprom_Send_Op_Read_Data(USE_DW_SPI_MST_E spi_id, uint8_t *op, uint32_t op_len, uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_WORD_W_R_RELATED

/**
 * @brief word read eeprom using quad mode with address, length information, data frame size= 4 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param bytes_len The length of the data that will be received from the EEPROM.  \note bytes_len must be a multiple of 4.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_word_read(USE_DW_SPI_MST_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len);

/**
 * @brief word write eeprom with address, length information
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr Address of EEPROM 
 * @param data Pointer to the data buffer to be written to the EEPROM device
 * @param bytes_len The length of the data that will be written to the EEPROM.  \note bytes_len must be a multiple of 4.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_word_write(USE_DW_SPI_MST_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len);
#endif

#ifdef SPI_EEPROM_WRITE_RELATED
/**
 * @brief  write eeprom with address, length information. word_switch-when storage data in waord type, 
 *      change word_switch to true for little endian/big endian switch
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr Address of EEPROM 
 * @param data Pointer to the data buffer to be written to EEROM device
 * @param len The length of the data that will be writed from the EEPROM.
 * @param word_switch little endian/big endian switch.  
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_write(USE_DW_SPI_MST_E spi_id, uint32_t addr, uint8_t *data, uint32_t len,
                                        uint8_t word_switch);
/**
 * @brief send op command to select eeprom
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param data  Pointer to the data buffer to be written to EEROM device
 *      if OSPI is selected, buffer size is limited  to 256 bytes.
 * @param len The length of the data that will be writed from the EEPROM.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_Send_Op_code(USE_DW_SPI_MST_E spi_id, uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_CHIP_ERASE_ERLATED
/**
 * @brief chip erase select eeprom
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_erase_all(USE_DW_SPI_MST_E spi_id);
#endif
#ifdef SPI_EEPROM_SECTOR_ERASE_RELATED
/**
 * @brief sector erase within a specified block by given flash address
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr Address of EEPROM 
 * @param sz Type of specified block.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_E spi_id, uint32_t addr,  FLASH_ERASE_SIZE_E sz);
#endif

#ifdef SPI_EEPROM_XIP_RELATED
/**
 * @brief set select eeprom to specific XIP mode
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param xip_enable set enable/disable XIP mode
 * @param xip_mode set XIP mode. dual or quad.
 * @param xip_cont set enable/disable continuous transfer in XIP mode
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_E spi_id, bool xip_enable,
                                            FLASH_ACCESS_MODE_E xip_mode, bool xip_cont);
#endif

/**
 * @brief  set select flash config for specific spi master.
 * method 1:
 * when LIB_SPI_EEPROM_USE_SINGLE_FLASH = n and specific vendor and size FLASH must be included in FLASH_SEL
 * user can use this function to switch specific vendor and size FLASH command table
 * eg.   hx_lib_spi_eeprom_set_flash_config(USE_DW_SPI_MST_Q, FLASH_TYPE_GIGADEV, FLASH_SIZE_64Mb, NULL);
 * 
 * method 2:
 * wheh user has the user-defined FLASH command table  .
 * user can use this fuction to set flash conifg by user-defined FLASH command table user.
 * eg.   
 *      SPI_EEPROM_PARAM_S flash_command;
 *      hx_lib_spi_eeprom_set_flash_config(USE_DW_SPI_MST_Q, FLASH_TYPE_NONE, FLASH_SIZE_NONE, &flash_command);
 * 
 * @param spi_id ID of spi, only QSPI support
 * @param flash_type flash vendor 
 * @param flash_size flash size
 * @return int32_t int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_set_flash_config(USE_DW_SPI_MST_E spi_id, 
                            CUR_FLASH_TYPE_E flash_type, CUR_FLASH_SIZE_E flash_size, SPI_EEPROM_PARAM_S* flash_config);

#ifdef __cplusplus
}
#endif

#endif /* LIBRARY_SPI_EEPROM_COMM_H_*/
