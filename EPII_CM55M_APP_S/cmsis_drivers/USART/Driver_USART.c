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
#include "Driver_USART.h"
#include "dev_common.h"
//#include "dw_uart.h"
//#include "dw_uart_hal.h"
#include "hx_drv_uart.h"

#define RTE_UART0_BD  115200
#define RTE_UART1_BD  115200
#define RTE_UART2_BD  115200

#define ARM_USART_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* driver version */

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { 
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
    0  /* Reserved (must be zero) */
};

typedef struct {
    DEV_UART *dev;         /* UART device structure */
    uint32_t tx_nbr_bytes; /* Number of bytes transfered */
    uint32_t rx_nbr_bytes; /* Number of bytes recevied */
    ARM_USART_STATUS status;
} UARTx_Resources;

static int32_t ARM_USARTx_Initialize(UARTx_Resources *uart, ARM_USART_SignalEvent_t cb_event, uint32_t baudrate)
{
    ARG_UNUSED(cb_event);

    uart->tx_nbr_bytes = 0;
    uart->rx_nbr_bytes = 0;
    uart->status = (ARM_USART_STATUS){0};

    if (uart->dev == NULL)
        return ARM_DRIVER_ERROR;

    uart->dev->uart_open(baudrate);
    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_Uninitialize(UARTx_Resources *uart)
{
    uart->dev->uart_close();
    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_PowerControl(UARTx_Resources *uart, ARM_POWER_STATE state, uint32_t baudrate)
{
    ARG_UNUSED(uart);
    ARG_UNUSED(baudrate);

    switch (state) {
    case ARM_POWER_FULL:
        return ARM_DRIVER_OK;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t  ARM_USARTx_Control(UARTx_Resources *uart, uint32_t control,
                                  uint32_t arg)
{
    if (control & (0xFFFF << ARM_USART_FLOW_CONTROL_Pos))
        return ARM_DRIVER_ERROR_UNSUPPORTED;

    int32_t ret = E_NOSPT;
    
    switch (control & ARM_USART_CONTROL_Msk)
    {
        case ARM_USART_MODE_ASYNCHRONOUS:
            /* USART Baudrate */
            ret = uart->dev->uart_control(UART_CMD_SET_BAUD, (UART_CTRL_PARAM)arg);
            break;
        /* Unsupported command */
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    if (ret != E_OK)
        return ARM_DRIVER_ERROR;

    UART_DPS_FORMAT dps_format = {UART_DATABITS_DEFAULT, UART_PARITY_NONE, UART_STPBITS_ONE};

    /* UART Data bits */
    if (control & ARM_USART_DATA_BITS_Msk) {
        /* Data bit is not configurable */
        return ARM_USART_ERROR_DATA_BITS;
    }

    /* UART Parity */
    switch (control & ARM_USART_PARITY_Msk) {
        case ARM_USART_PARITY_NONE:
            break;
        case ARM_USART_PARITY_EVEN:
            dps_format.parity = UART_PARITY_EVEN;
            break;
        case ARM_USART_PARITY_ODD:
            dps_format.parity = UART_PARITY_ODD;
            break;
        default:
            return ARM_USART_ERROR_PARITY;
    }

    /* USART Stop bits */
    switch (control & ARM_USART_STOP_BITS_Msk) {
        case ARM_USART_PARITY_NONE:
            break;
        case ARM_USART_STOP_BITS_0_5:
            dps_format.stopbits = UART_STPBITS_ONEHALF;
            break;
        case ARM_USART_STOP_BITS_2:
            dps_format.stopbits = UART_STPBITS_TWO;
            break;
        default:
            return ARM_USART_ERROR_STOP_BITS;
    }

    ret = uart->dev->uart_control(UART_CMD_SET_DPS_FORMAT, (UART_CTRL_PARAM)&dps_format);

    if (ret != E_OK)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS ARM_USARTx_GetStatus(UARTx_Resources *uart)
{
    return uart->status;
}

static int32_t ARM_USARTx_Send(UARTx_Resources *uart, const void *data,
                               uint32_t num)
{
    if (uart->dev == NULL)
        return ARM_DRIVER_ERROR;

    const uint8_t *p_data = (const uint8_t *)data;

    if ((data == NULL) || (num == 0U)) {
        /* Invalid parameters */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    uart->tx_nbr_bytes = 0;
    int32_t ret = uart->dev->uart_write(p_data, num);
#if 0
    DW_UART_REG *dw_uart_regbase = ((DW_UART_CTRL_PTR) uart->dev->uart_info.uart_ctrl)->dw_uart_regbase;
    int USR = dw_uart_regbase->USR;
#endif
    uart->status.tx_busy = 0;
#if 0
    if (USR & DW_UART_USR_BUSY)
        uart->status.tx_busy = 1;
#endif
    if (ret < 0)
        return ARM_DRIVER_ERROR;

    uart->tx_nbr_bytes = ret;
    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_Receive(UARTx_Resources *uart,
                                  void *data, uint32_t num)
{
    if (uart->dev == NULL)
        return ARM_DRIVER_ERROR;

    uint8_t *p_data = (uint8_t *)data;

    if ((data == NULL) || (num == 0U)) {
        // Invalid parameters
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    uart->rx_nbr_bytes = 0;
    int32_t ret = uart->dev->uart_read(p_data, num);
#if 0
    DW_UART_REG *dw_uart_regbase = ((DW_UART_CTRL_PTR) uart->dev->uart_info.uart_ctrl)->dw_uart_regbase;
    int LSR = dw_uart_regbase->LSR;
    int USR = dw_uart_regbase->USR;
#endif
    uart->status.rx_framing_error = 0;
    uart->status.rx_parity_error = 0;
    uart->status.rx_break = 0;
    uart->status.rx_overflow = 0;
    uart->status.rx_busy = 0;
#if 0
    if (LSR & DW_UART_LSR_FRAMEERR)
        uart->status.rx_framing_error = 1;
    if (LSR & DW_UART_LSR_PARITYERR)
        uart->status.rx_parity_error = 1;
    if (LSR & DW_UART_LSR_BREAKRCVD)
        uart->status.rx_break = 1;
    if (LSR & DW_UART_LSR_OVERRUN)
        uart->status.rx_overflow = 1;
    if (USR & DW_UART_USR_BUSY)
        uart->status.rx_busy = 1;
#endif
    if (ret < 0)
        return ARM_DRIVER_ERROR;

    uart->rx_nbr_bytes = ret;
    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_Transfer(UARTx_Resources *uart, const void *data_out, void *data_in, uint32_t num)
{
    ARG_UNUSED(uart);
    ARG_UNUSED(data_out);
    ARG_UNUSED(data_in);
    ARG_UNUSED(num);

    // Synchronous mode not supported
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static uint32_t ARM_USARTx_GetTxCount(UARTx_Resources *uart)
{
    return uart->tx_nbr_bytes;
}

static uint32_t ARM_USARTx_GetRxCount(UARTx_Resources *uart)
{
    return uart->rx_nbr_bytes;
}

#if !defined(IP_uart)
#error "IP_uart is not defined!!"
#endif

//
//   Functions
//
#if defined(IP_INST_UART0) || defined(IP_INST_NS_UART0)
static UARTx_Resources USART0_Resources;

static ARM_DRIVER_VERSION ARM_USART0_GetVersion(void)
{
  return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART0_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ARM_USART0_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    USART0_Resources.dev = hx_drv_uart_get_dev(USE_DW_UART_0);
    return ARM_USARTx_Initialize(&USART0_Resources, cb_event, RTE_UART0_BD);
}

static int32_t ARM_USART0_Uninitialize(void)
{
    return ARM_USARTx_Uninitialize(&USART0_Resources);
}

static int32_t ARM_USART0_PowerControl(ARM_POWER_STATE state)
{
    return ARM_USARTx_PowerControl(&USART0_Resources, state, RTE_UART0_BD);
}

static int32_t ARM_USART0_Send(const void *data, uint32_t num)
{
    return ARM_USARTx_Send(&USART0_Resources, data, num);
}

static int32_t ARM_USART0_Receive(void *data, uint32_t num)
{
    return ARM_USARTx_Receive(&USART0_Resources, data, num);
}

static int32_t ARM_USART0_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_USARTx_Transfer(&USART0_Resources, data_out, data_in, num);
}

static uint32_t ARM_USART0_GetTxCount(void)
{
    return ARM_USARTx_GetTxCount(&USART0_Resources);
}

static uint32_t ARM_USART0_GetRxCount(void)
{
    return ARM_USARTx_GetRxCount(&USART0_Resources);
}

static int32_t ARM_USART0_Control(uint32_t control, uint32_t arg)
{
    return ARM_USARTx_Control(&USART0_Resources, control, arg);
}

static ARM_USART_STATUS ARM_USART0_GetStatus(void)
{
    return ARM_USARTx_GetStatus(&USART0_Resources);
}

static int32_t ARM_USART0_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    ARG_UNUSED(control);
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS ARM_USART0_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS modem_status = {0, 0, 0, 0, 0};
    return modem_status;
}

extern ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART Driver_USART0 = {
    ARM_USART0_GetVersion,
    ARM_USART0_GetCapabilities,
    ARM_USART0_Initialize,
    ARM_USART0_Uninitialize,
    ARM_USART0_PowerControl,
    ARM_USART0_Send,
    ARM_USART0_Receive,
    ARM_USART0_Transfer,
    ARM_USART0_GetTxCount,
    ARM_USART0_GetRxCount,
    ARM_USART0_Control,
    ARM_USART0_GetStatus,
    ARM_USART0_SetModemControl,
    ARM_USART0_GetModemStatus
};
#endif //#if defined(IP_INST_UART0) || defined(IP_INST_NS_UART0)

#if defined(IP_INST_UART1) || defined(IP_INST_NS_UART1)
static UARTx_Resources USART1_Resources;

static ARM_DRIVER_VERSION ARM_USART1_GetVersion(void)
{
  return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART1_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ARM_USART1_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    USART1_Resources.dev = hx_drv_uart_get_dev(USE_DW_UART_1);
    return ARM_USARTx_Initialize(&USART1_Resources, cb_event, RTE_UART1_BD);
}

static int32_t ARM_USART1_Uninitialize(void)
{
    return ARM_USARTx_Uninitialize(&USART1_Resources);
}

static int32_t ARM_USART1_PowerControl(ARM_POWER_STATE state)
{
    return ARM_USARTx_PowerControl(&USART1_Resources, state, RTE_UART1_BD);
}

static int32_t ARM_USART1_Send(const void *data, uint32_t num)
{
    return ARM_USARTx_Send(&USART1_Resources, data, num);
}

static int32_t ARM_USART1_Receive(void *data, uint32_t num)
{
    return ARM_USARTx_Receive(&USART1_Resources, data, num);
}

static int32_t ARM_USART1_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_USARTx_Transfer(&USART1_Resources, data_out, data_in, num);
}

static uint32_t ARM_USART1_GetTxCount(void)
{
    return ARM_USARTx_GetTxCount(&USART1_Resources);
}

static uint32_t ARM_USART1_GetRxCount(void)
{
    return ARM_USARTx_GetRxCount(&USART1_Resources);
}

static int32_t ARM_USART1_Control(uint32_t control, uint32_t arg)
{
    return ARM_USARTx_Control(&USART1_Resources, control, arg);
}

static ARM_USART_STATUS ARM_USART1_GetStatus(void)
{
    return ARM_USARTx_GetStatus(&USART1_Resources);
}

static int32_t ARM_USART1_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    ARG_UNUSED(control);
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS ARM_USART1_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS modem_status = {0, 0, 0, 0, 0};
    return modem_status;
}

extern ARM_DRIVER_USART Driver_USART1;
ARM_DRIVER_USART Driver_USART1 = {
    ARM_USART1_GetVersion,
    ARM_USART1_GetCapabilities,
    ARM_USART1_Initialize,
    ARM_USART1_Uninitialize,
    ARM_USART1_PowerControl,
    ARM_USART1_Send,
    ARM_USART1_Receive,
    ARM_USART1_Transfer,
    ARM_USART1_GetTxCount,
    ARM_USART1_GetRxCount,
    ARM_USART1_Control,
    ARM_USART1_GetStatus,
    ARM_USART1_SetModemControl,
    ARM_USART1_GetModemStatus
};
#endif //#if defined(IP_INST_UART1) || defined(IP_INST_NS_UART1)

#if defined(IP_INST_UART2) || defined(IP_INST_NS_UART2)
static UARTx_Resources USART2_Resources;

static ARM_DRIVER_VERSION ARM_USART2_GetVersion(void)
{
  return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART2_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ARM_USART2_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    USART2_Resources.dev = hx_drv_uart_get_dev(USE_DW_UART_2);
    return ARM_USARTx_Initialize(&USART2_Resources, cb_event, RTE_UART2_BD);
}

static int32_t ARM_USART2_Uninitialize(void)
{
    return ARM_USARTx_Uninitialize(&USART2_Resources);
}

static int32_t ARM_USART2_PowerControl(ARM_POWER_STATE state)
{
    return ARM_USARTx_PowerControl(&USART2_Resources, state, RTE_UART2_BD);
}

static int32_t ARM_USART2_Send(const void *data, uint32_t num)
{
    return ARM_USARTx_Send(&USART2_Resources, data, num);
}

static int32_t ARM_USART2_Receive(void *data, uint32_t num)
{
    return ARM_USARTx_Receive(&USART2_Resources, data, num);
}

static int32_t ARM_USART2_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_USARTx_Transfer(&USART2_Resources, data_out, data_in, num);
}

static uint32_t ARM_USART2_GetTxCount(void)
{
    return ARM_USARTx_GetTxCount(&USART2_Resources);
}

static uint32_t ARM_USART2_GetRxCount(void)
{
    return ARM_USARTx_GetRxCount(&USART2_Resources);
}

static int32_t ARM_USART2_Control(uint32_t control, uint32_t arg)
{
    return ARM_USARTx_Control(&USART2_Resources, control, arg);
}

static ARM_USART_STATUS ARM_USART2_GetStatus(void)
{
    return ARM_USARTx_GetStatus(&USART2_Resources);
}

static int32_t ARM_USART2_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    ARG_UNUSED(control);
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS ARM_USART2_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS modem_status = {0, 0, 0, 0, 0};
    return modem_status;
}

extern ARM_DRIVER_USART Driver_USART2;
ARM_DRIVER_USART Driver_USART2 = {
    ARM_USART2_GetVersion,
    ARM_USART2_GetCapabilities,
    ARM_USART2_Initialize,
    ARM_USART2_Uninitialize,
    ARM_USART2_PowerControl,
    ARM_USART2_Send,
    ARM_USART2_Receive,
    ARM_USART2_Transfer,
    ARM_USART2_GetTxCount,
    ARM_USART2_GetRxCount,
    ARM_USART2_Control,
    ARM_USART2_GetStatus,
    ARM_USART2_SetModemControl,
    ARM_USART2_GetModemStatus
};
#endif //#if defined(IP_INST_UART2) || defined(IP_INST_NS_UART2)
