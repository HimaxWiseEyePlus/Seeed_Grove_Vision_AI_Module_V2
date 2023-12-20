#ifndef LIBRARY_SPI_PSRAM_COMM_H_
#define LIBRARY_SPI_PSRAM_COMM_H_
#include "dev_common.h"
#include "hx_drv_dmac.h"
#include "hx_drv_spi.h"
#include "WE2_core.h"

#ifdef NSC
#include "veneer_clk_ctrl.h"
#else
#include "hx_drv_scu.h"
#endif

/** 
 * \defgroup SPI PSRAM library for OSPI
 * \brief This library is based on Winbond W956D8MBYA datasheet
 */

typedef void (*spi_psram_cb_t)(void);

/**
 * \enum PSRAM_POWER_MDOE_E
 * \brief psram power mode enum
 */
typedef enum psram_power_mode_e
{
    PSRAM_WAKE_UP = 0,
    PSRAM_HYPER_SLEEP = 1,     /** data in Memory Space and Register Space is retained */
    PSRAM_DEEP_POWER_DOWN = 2, /** The data in Memory Space is lost, during DPD state */
} PSRAM_POWER_MDOE_E;

typedef enum DRV_PSRAM_REGISTER_S
{
    DEV_PSRAM_ID = 0,
    DEV_PSRAM_CONFIG = 1,
} DRV_PSRAM_REGISTER_E;

/**
 * @brief initialize OSPI, and default spi speed is max.(Divider = 2)
 * 
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_psram_open(void);

/**
 * @brief initialize OSPI, and the output clock of SPI is setted by user.
 * 
 * @param clk_hz the output clock of SPI
 * @return int32_t 
 */
extern int32_t hx_lib_spi_psram_open_speed(uint32_t clk_hz);

/**
 * @brief set select PSRAM to specific XIP mode
 * 
 * @param enable set enable/disable XIP mode
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_psram_SetXIP(bool enable);

/**
 * @brief set PSRAM to specific power mode
 * 
 * @param mode power mode
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_psram_set_powermode(PSRAM_POWER_MDOE_E mode);

/**
 * @brief read PSRAM register information
 * 
 * @param data Pointer of the buffer where the received data from PSRAM will be stored
 * @param reg The type of register. \ref DRV_PSRAM_REGISTER_E
 * @param id The ID of register. 0 or 1
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_psram_get_info(uint32_t *data, uint32_t reg, uint32_t id);

/**
 * @brief close library of ospi
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_psram_close(void);

#endif /* LIBRARY_SPI_PSRAM_COMM_H_ */
