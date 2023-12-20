/*
 * pmu_ctrl.h
 *
 *  Created on: 20220601
 *      Author: 904207
 */
#ifndef TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_PSRAM_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_PSRAM_CTRL_H_
#include "stdbool.h"

/**
 * \enum PSRAM_POWER_MDOE_E
 * \brief psram power mode enum
 */
typedef enum veneer_psram_power_mode_e
{
    VENEER_PSRAM_WAKE_UP = 0,
    VENEER_PSRAM_HYPER_SLEEP = 1,     /** data in Memory Space and Register Space is retained */
    VENEER_PSRAM_DEEP_POWER_DOWN = 2, /** The data in Memory Space is lost, during DPD state */
} VENEER_PSRAM_POWER_MDOE_E;

typedef enum veneer_psram_register_e
{
    VENEER_PSRAM_ID = 0,
    VENEER_PSRAM_CONFIG = 1,
} VENEER_PSRAM_REGISTER_E;

/**
 * \brief initialize OSPI for psram. The default is the highest speed
 *
 * \return	.
 */
int32_t veneer_spi_psram_open(void);

/**
 * \brief set select psram to XIP mode. 
 *
 * \param[in]   enable set enable/disable XIP mode
 * \return	.
 */
int32_t veneer_spi_psram_SetXIP(bool enable);

int32_t veneer_spi_psram_set_powermode(VENEER_PSRAM_POWER_MDOE_E mode);

int32_t veneer_spi_psram_get_info(uint32_t *data, uint32_t reg, uint32_t id);

int32_t veneer_spi_psram_close(void);
#endif /*TRUSTZONE_NSC_FUNCTION_NSC_INC_VENEER_PSRAM_CTRL_H_*/