/*
 * veneer_psram_ctrl.c
 *
 *  Created on: 2022/06/08
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
#include "veneer_psram_ctrl.h"
#include "spi_psram_comm.h"
// #include "hx_drv_spi.h"

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_psram_open(void)
{
    return hx_lib_spi_psram_open();
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_psram_SetXIP(bool enable)
{
    return hx_lib_spi_psram_SetXIP(enable);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_psram_set_powermode(VENEER_PSRAM_POWER_MDOE_E mode)
{
    return hx_lib_spi_psram_set_powermode((PSRAM_POWER_MDOE_E)mode);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_psram_get_info(uint32_t *data, uint32_t reg, uint32_t id)
{
    return hx_lib_spi_psram_get_info(data, reg, id);
}

__attribute__((cmse_nonsecure_entry)) int32_t veneer_spi_psram_close(void)
{
    return hx_lib_spi_psram_close();
}
#endif //#if !defined(HX_TFM)
