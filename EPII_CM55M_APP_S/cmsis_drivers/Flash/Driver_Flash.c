/*
 * Copyright (c) 2013-2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "WE2_device.h"
#include "Driver_Flash.h"
#include "dev_common.h"
#if !defined(NSC)
#include "spi_eeprom_comm.h"
#else
#include "veneer_eeprom_ctrl.h"
#endif

#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

 enum {
    DATA_WIDTH_8BIT   = 0u,
    DATA_WIDTH_16BIT,
    DATA_WIDTH_32BIT,
    DATA_WIDTH_ENUM_SIZE
};

static const uint32_t data_width_byte[DATA_WIDTH_ENUM_SIZE] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};

/* Sector Information */
#ifdef FLASH_SECTORS
static ARM_FLASH_SECTOR FLASH_SECTOR_INFO[FLASH_SECTOR_COUNT] = {
    FLASH_SECTORS
};
#else
#define FLASH_SECTOR_INFO    NULL
#endif

/* Flash Information */
#define FLASH_TOTAL_SIZE     (0x01000000)  /* 16 MB */
#define FLASH_SECTOR_SIZE    (0x00001000)  /* 4 kB */
#define FLASH_PAGE_SIZE      (256)         /* 256 B */
#define FLASH_PROGRAM_UNIT   (0x01)        /* Minimum write size */
#define FLASH_ERASED_VALUE   (0xFF)

#ifndef HX_CMSIS_FLASH0_ADDR
#define HX_CMSIS_FLASH0_ADDR        0x800000  /* Flash base address */
#endif

#ifndef HX_CMSIS_FLASH0_SIZE
#define HX_CMSIS_FLASH0_SIZE       ((FLASH_TOTAL_SIZE - HX_CMSIS_FLASH0_ADDR) / 4)
#endif

static ARM_FLASH_INFO ARM_FLASH0_DEV_INFO = {
    FLASH_SECTOR_INFO, /* FLASH_SECTOR_INFO  */
    HX_CMSIS_FLASH0_SIZE / FLASH_SECTOR_SIZE, /* FLASH_SECTOR_COUNT */
    FLASH_SECTOR_SIZE, /* FLASH_SECTOR_SIZE  */
    FLASH_PAGE_SIZE, /* FLASH_PAGE_SIZE    */
    FLASH_PROGRAM_UNIT, /* FLASH_PROGRAM_UNIT */
    FLASH_ERASED_VALUE, /* FLASH_ERASED_VALUE */
#if (ARM_FLASH_API_VERSION > 0x201U)
    { 0, 0, 0 }  /* Reserved (must be zero) */
#endif
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus;

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    0, /* erase_chip */
#if (ARM_FLASH_API_VERSION > 0x200U)
    0  /* reserved (must be zero) */
#endif
};

static int32_t is_range_valid(uint32_t offset)
{
    int32_t rc = 0;

    if (offset > (HX_CMSIS_FLASH0_ADDR + HX_CMSIS_FLASH0_SIZE - 1)) {
        rc = -1;
    }

    return rc;
}

static int32_t is_write_aligned(uint32_t param)
{
    int32_t rc = 0;

    if ((param %FLASH_PROGRAM_UNIT) != 0) {
        rc = -1;
    }
    return rc;
}

static int32_t is_sector_aligned(uint32_t offset)
{
    int32_t rc = 0;

    if ((offset % FLASH_SECTOR_SIZE) != 0) {
        rc = -1;
    }
    return rc;
}

//
// Functions
//
#if defined(IP_INST_QSPI_HOST) || defined(NSC)
static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
  return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    static int init_done = 0;

    if (init_done == 0) {
#if !defined(NSC)
        int rc = hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
        //int rc = hx_lib_spi_eeprom_open_speed(USE_DW_SPI_MST_Q, 12000000);
#else
        int rc = veneer_spi_eeprom_open(VENEER_QSPI_EEPROM);
        //int rc = veneer_spi_eeprom_open_speed(VENEER_QSPI_EEPROM, 12000000);
#endif
        if (E_OK != rc)
            return ARM_DRIVER_ERROR;

        init_done = 1;
    }

    if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE) {
        return ARM_DRIVER_ERROR;
    }

    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
    case ARM_POWER_FULL:
        return ARM_DRIVER_OK;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    int32_t rc = 0;
    /* Conversion between data items and bytes */
    cnt *= data_width_byte[DriverCapabilities.data_width];
    addr += HX_CMSIS_FLASH0_ADDR;

    /* Check flash memory boundaries */
    rc = is_range_valid(addr + cnt);

    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

#if !defined(NSC)
    rc = hx_lib_spi_eeprom_2read(USE_DW_SPI_MST_Q, addr, (uint8_t *)data, cnt);
#else
    rc = veneer_spi_eeprom_2read(VENEER_QSPI_EEPROM, addr, (uint8_t *)data, cnt);
#endif

    if (E_OK == rc) {
        cnt /= data_width_byte[DriverCapabilities.data_width];
        return cnt;
    } else {
        return ARM_DRIVER_ERROR;
    }
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    int32_t rc = 0;
    /* Conversion between data items and bytes */
    cnt *= data_width_byte[DriverCapabilities.data_width];
    addr += HX_CMSIS_FLASH0_ADDR;

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid(addr + cnt);
    rc |= is_write_aligned(addr);
    rc |= is_write_aligned(cnt);

    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

#if !defined(NSC)
    rc = hx_lib_spi_eeprom_write(USE_DW_SPI_MST_Q, addr, (uint8_t *)data, cnt, 0);
#else
    rc = veneer_spi_eeprom_write(VENEER_QSPI_EEPROM, addr, (uint8_t *)data, cnt);
#endif

    if (E_OK == rc) {
        cnt /= data_width_byte[DriverCapabilities.data_width];
        return (int32_t)cnt;
    } else {
        return ARM_DRIVER_ERROR;
    }
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    int32_t rc = 0;

    addr += HX_CMSIS_FLASH0_ADDR;
    rc  = is_range_valid(addr);
    rc |= is_sector_aligned(addr);

    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

#if !defined(NSC)
    rc = hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_Q, addr, FLASH_SECTOR);
#else
    rc = veneer_spi_eeprom_erase_sector(VENEER_QSPI_EEPROM, addr);
#endif

    if (E_OK == rc) {
        return ARM_DRIVER_OK;
    } else {
        return ARM_DRIVER_ERROR;
    }
}

static int32_t ARM_Flash_EraseChip(void)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
  return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
  return &ARM_FLASH0_DEV_INFO;
}


// End Flash Interface

extern ARM_DRIVER_FLASH Driver_Flash0;
ARM_DRIVER_FLASH Driver_Flash0 = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};
#endif //#if defined(IP_INST_QSPI_HOST)
