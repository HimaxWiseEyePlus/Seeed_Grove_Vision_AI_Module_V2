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
#include "Driver_SPI.h"
#include "dev_common.h"
#include "hx_drv_spi.h"

#if defined(IP_INST_SSPI_HOST) || defined(IP_INST_NS_SSPI_HOST)

typedef void (*FnPtr_GPIO_Output_Level) (bool setLevelHigh);
typedef void (*FnPtr_GPIO_Dir) (bool setDirOut);
typedef void (*FnPtr_GPIO_Pinmux) (bool setGpioFn);

static FnPtr_GPIO_Dir sspi_cs_gpio_dir;
static FnPtr_GPIO_Output_Level sspi_cs_gpio_output_level;
static FnPtr_GPIO_Pinmux sspi_cs_gpio_pinmux;

// Driver_SPI0 getFnPtr_CMSIS_Driver default return NULL that CS GPIO is not implemented
// User can ovrride getFnPtr_CMSIS_Driver to implement CMSIS Driver CS GPIO
__WEAK FnPtr_GPIO_Output_Level getFnPtr_CMSIS_Driver_SPI0_SSPI_CS_GPIO_Output_Level(void) {
    return NULL;
}

__WEAK FnPtr_GPIO_Dir getFnPtr_CMSIS_Driver_SPI0_SSPI_CS_GPIO_Dir(void) {
    return NULL;
}

__WEAK FnPtr_GPIO_Pinmux getFnPtr_CMSIS_Driver_SPI0_SSPI_CS_GPIO_Pinmux(void) {
    return NULL;
}

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

#define ARM_SPI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_SPI_API_VERSION,
    ARM_SPI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
    0, /* Reserved (must be zero) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface */
    0, /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
#if (defined(ARM_SPI_API_VERSION) && (ARM_SPI_API_VERSION >= 0x202U))
    0  /* Reserved (must be zero) */
#endif
};

//
//  Functions
//

static ARM_DRIVER_VERSION ARM_SPI_GetVersion(void)
{
  return DriverVersion;
}

static ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void)
{
  return DriverCapabilities;
}

typedef struct {
    DEV_SPI *dev;    /* SPI device structure */
    uint32_t xfer_num;
    uint32_t max_freq;
    ARM_SPI_STATUS status;
    ARM_SPI_SignalEvent_t cb_event;
} SPIx_Resources;

static volatile SPIx_Resources SPI0_Resources;
static volatile bool spi0_done;

static void callback_spi0_tx(void)
{
    spi0_done = 1;
}

static void callback_spi0_rx(void)
{
    spi0_done = 1;
}

static void callback_spi0_xfer(void)
{
    spi0_done = 1;
}

static void callback_spi0_error(void)
{
    SPI0_Resources.status.data_lost = 1;
}

static int32_t ARM_SPIx_Initialize(SPIx_Resources *spi, ARM_SPI_SignalEvent_t cb_event)
{
    spi->xfer_num = 0;
    spi->status = (ARM_SPI_STATUS){0};
    spi->cb_event = cb_event;
    sspi_cs_gpio_output_level = getFnPtr_CMSIS_Driver_SPI0_SSPI_CS_GPIO_Output_Level();
    sspi_cs_gpio_dir = getFnPtr_CMSIS_Driver_SPI0_SSPI_CS_GPIO_Dir();
    sspi_cs_gpio_pinmux = getFnPtr_CMSIS_Driver_SPI0_SSPI_CS_GPIO_Pinmux();

    if (spi->dev == NULL)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

static int32_t ARM_SPIx_Uninitialize(SPIx_Resources* spi)
{
    if (spi->dev == NULL)
        return ARM_DRIVER_ERROR;

    spi->dev->spi_close();
    spi->status = (ARM_SPI_STATUS){0};
    return ARM_DRIVER_OK;
}

static int32_t ARM_SPIx_PowerControl(SPIx_Resources* spi, ARM_POWER_STATE state)
{
    switch (state) {
        case ARM_POWER_OFF:
            spi->dev->spi_control(SPI_CMD_DIS_DEV, NULL);
            spi->status = (ARM_SPI_STATUS){0};
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Nothing to be done. It's already full power*/
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static ARM_SPI_STATUS ARM_SPIx_GetStatus(SPIx_Resources* spi)
{
    uint32_t busy_status = 1;

    if (spi0_done)
        spi->dev->spi_control(SPI_CMD_GET_BUSY_STATUS, (SPI_CTRL_PARAM)&busy_status);

    spi->status.busy = busy_status;
    //SPI0_Resources.status.busy = busy_status;
    return spi->status;
}

static uint32_t ARM_SPIx_GetDataCount(const SPIx_Resources* spi)
{
    if (spi->status.busy || spi->status.data_lost)
        return 0;

    return spi->xfer_num;
}

static int32_t ARM_SPIx_Control(SPIx_Resources *spi, uint32_t control, uint32_t arg)
{
    int32_t ret = E_OK;
    uint32_t spi_freq;
    DW_SPI_M_RXSD_S rx_delay_config;

    switch(control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_MASTER:
            spi->dev->spi_control(SPI_CMD_MST_UPDATE_SYSCLK, (SPI_CTRL_PARAM)&spi->max_freq);

            if (arg < spi->max_freq)
                spi_freq = arg;

            ret = spi->dev->spi_open(DEV_MASTER_MODE, spi_freq);
            DEV_SPI_INFO_PTR info;
            info = &(spi->dev->spi_info);
            info->spi_cbs.tx_cb = (DEV_CALLBACK)callback_spi0_tx; 
            info->spi_cbs.rx_cb = (DEV_CALLBACK)callback_spi0_rx; 
            info->spi_cbs.xfer_cb = (DEV_CALLBACK)callback_spi0_xfer; 
            info->spi_cbs.err_cb = (DEV_CALLBACK)callback_spi0_error;

            rx_delay_config.RSD = 0;
            rx_delay_config.SE = 0;
            ret |= spi->dev->spi_control(SPI_CMD_MST_SET_RXSD, (SPI_CTRL_PARAM)&rx_delay_config);

            break;
        case ARM_SPI_SET_BUS_SPEED:
            spi->dev->spi_control(SPI_CMD_MST_UPDATE_SYSCLK, (SPI_CTRL_PARAM)&spi->max_freq);

            if (arg < spi->max_freq)
                spi_freq = arg;

            ret = spi->dev->spi_control(SPI_CMD_MST_SET_FREQ, (SPI_CTRL_PARAM)spi_freq);
            break;
        case ARM_SPI_GET_BUS_SPEED:
            spi->dev->spi_control(SPI_CMD_MST_GET_CURRENT_FREQ, (SPI_CTRL_PARAM)&spi_freq);
            return spi_freq;
        case ARM_SPI_MODE_INACTIVE:
            ret = spi->dev->spi_control(SPI_CMD_DIS_DEV, NULL);
            break;
        case ARM_SPI_CONTROL_SS:
            if (sspi_cs_gpio_output_level && sspi_cs_gpio_dir && sspi_cs_gpio_pinmux) {
                if (arg == ARM_SPI_SS_INACTIVE)
                    sspi_cs_gpio_output_level(true);
                else
                    sspi_cs_gpio_output_level(false);
            }
            else {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            break;
        case ARM_SPI_SET_DEFAULT_TX_VALUE:
            ret = spi->dev->spi_control(SPI_CMD_SET_DUMMY_DATA, (void *)arg);
            break;
        case ARM_SPI_MODE_SLAVE:
        case ARM_SPI_MODE_MASTER_SIMPLEX:
        case ARM_SPI_MODE_SLAVE_SIMPLEX:
        case ARM_SPI_ABORT_TRANSFER:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        default:
            break;
    }

    uint32_t options = control & ARM_SPI_FRAME_FORMAT_Msk;

    if (options == ARM_SPI_CPOL0_CPHA0)
        spi->dev->spi_control(SPI_CMD_SET_CLK_MODE, (SPI_CTRL_PARAM)SPI_CPOL_0_CPHA_0);
    if (options == ARM_SPI_CPOL0_CPHA1)
        spi->dev->spi_control(SPI_CMD_SET_CLK_MODE, (SPI_CTRL_PARAM)SPI_CPOL_0_CPHA_1);
    if (options == ARM_SPI_CPOL1_CPHA0)
        spi->dev->spi_control(SPI_CMD_SET_CLK_MODE, (SPI_CTRL_PARAM)SPI_CPOL_1_CPHA_0);
    if (options == ARM_SPI_CPOL1_CPHA1)
        spi->dev->spi_control(SPI_CMD_SET_CLK_MODE, (SPI_CTRL_PARAM)SPI_CPOL_1_CPHA_1);

    if (control & ARM_SPI_BIT_ORDER_Msk)
        return ARM_SPI_ERROR_BIT_ORDER;

    options = control & ARM_SPI_SS_MASTER_MODE_Msk;

    if (options == ARM_SPI_SS_MASTER_HW_INPUT)
        return ARM_SPI_ERROR_SS_MODE;

    if (options == ARM_SPI_SS_MASTER_HW_OUTPUT) {
        if (sspi_cs_gpio_pinmux)
            sspi_cs_gpio_pinmux(false);      //Pinmux Off for Hardware Control CS
    }
    else if (options == ARM_SPI_SS_MASTER_SW) {
        if (sspi_cs_gpio_output_level && sspi_cs_gpio_dir && sspi_cs_gpio_pinmux) {
            sspi_cs_gpio_output_level(true); //GPIO high for ARM_SPI_SS_INACTIVE
            sspi_cs_gpio_dir(true);          //GPIO dirction output
            sspi_cs_gpio_pinmux(true);       //Pinmux On for Software Control CS
        }
        else {
            return ARM_SPI_ERROR_SS_MODE;
        }
    }

    if (ret != E_OK)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

//static void ARM_SPIx_SignalEvent(const SPIx_Resources* spi, uint32_t event)
//{
//    if (spi->cb_event)
//        spi->cb_event(event);
//
//    return;
//}

static int32_t ARM_SPIx_Send(SPIx_Resources* spi, const void* data, uint32_t num)
{
    if(data == NULL || num == 0)
        return ARM_DRIVER_ERROR_PARAMETER;

    if (spi->status.busy)
        return ARM_DRIVER_ERROR_BUSY;

    spi0_done = 0;
    spi->status = (ARM_SPI_STATUS){0};
    spi->xfer_num = num;

    int32_t ret = spi->dev->spi_write(data, num);

    if (ret != E_OK)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

static int32_t ARM_SPIx_Receive(SPIx_Resources* spi, void* data, uint32_t num)
{
    if(data == NULL || num == 0)
        return ARM_DRIVER_ERROR_PARAMETER;

    if (spi->status.busy)
        return ARM_DRIVER_ERROR_BUSY;

    spi0_done = 0;
    spi->status = (ARM_SPI_STATUS){0};
    spi->xfer_num = num;

    int32_t ret = spi->dev->spi_read(data, num);

    if (ret != E_OK)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

static int32_t ARM_SPIx_Transfer(SPIx_Resources* spi, const void *data_out, void *data_in, uint32_t num)
{
    if(data_out == NULL || data_out == data_in || num == 0)
        return ARM_DRIVER_ERROR_PARAMETER;

    if (spi->status.busy)
        return ARM_DRIVER_ERROR_BUSY;

    DEV_BUFFER xfer_tx_buff, xfer_rx_buff;

    spi0_done = 0;
    spi->status = (ARM_SPI_STATUS){0};
    spi->xfer_num = num;

    xfer_tx_buff.buf = (void *)data_out;
    xfer_tx_buff.len = num;
    xfer_tx_buff.ofs = 0;
    xfer_rx_buff.buf = (void *)data_in;
    xfer_rx_buff.len = num;
    xfer_rx_buff.ofs = 0;

    spi->dev->spi_control(SPI_CMD_SET_TXINT_BUF, (SPI_CTRL_PARAM)&xfer_tx_buff);
    spi->dev->spi_control(SPI_CMD_SET_RXINT_BUF, (SPI_CTRL_PARAM)&xfer_rx_buff);
    spi->dev->spi_control(SPI_CMD_MST_SET_RX_COUNT, (SPI_CTRL_PARAM)num);

    int32_t ret = spi->dev->spi_control(SPI_CMD_TRANSFER_INT, (SPI_CTRL_PARAM)&(spi->dev->spi_info.xfer));

    if (ret != E_OK)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

static int32_t ARM_SPI_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI0_Resources.dev = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_S);
    return ARM_SPIx_Initialize(&SPI0_Resources, cb_event);
}

static int32_t ARM_SPI_Uninitialize(void)
{
    return ARM_SPIx_Uninitialize(&SPI0_Resources);
}

static int32_t ARM_SPI_PowerControl(ARM_POWER_STATE state)
{
    return (ARM_SPIx_PowerControl(&SPI0_Resources, state));
}

static int32_t ARM_SPI_Send(const void *data, uint32_t num)
{
    return ARM_SPIx_Send(&SPI0_Resources, data, num);
}

static int32_t ARM_SPI_Receive(void *data, uint32_t num)
{
    return ARM_SPIx_Receive(&SPI0_Resources, data, num);
}

static int32_t ARM_SPI_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_SPIx_Transfer(&SPI0_Resources, data_out, data_in, num);
}

static uint32_t ARM_SPI_GetDataCount(void)
{
    return (ARM_SPIx_GetDataCount(&SPI0_Resources));
}

static int32_t ARM_SPI_Control(uint32_t control, uint32_t arg)
{
    return (ARM_SPIx_Control(&SPI0_Resources, control, arg));
}

static ARM_SPI_STATUS ARM_SPI_GetStatus(void)
{
    return (ARM_SPIx_GetStatus(&SPI0_Resources));
}

// End SPI Interface

extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI Driver_SPI0 = {
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    ARM_SPI_Initialize,
    ARM_SPI_Uninitialize,
    ARM_SPI_PowerControl,
    ARM_SPI_Send,
    ARM_SPI_Receive,
    ARM_SPI_Transfer,
    ARM_SPI_GetDataCount,
    ARM_SPI_Control,
    ARM_SPI_GetStatus
};
#endif
