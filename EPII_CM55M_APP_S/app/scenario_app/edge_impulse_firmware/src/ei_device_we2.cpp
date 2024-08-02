/*
 * Copyright (c) 2024 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <string>
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "firmware-sdk/ei_device_info_lib.h"
#include "firmware-sdk/ei_device_memory.h"
#include "ei_device_we2.h"
#include "ei_microphone.h"
#include "flash_memory.h"
#include "cisdp_sensor.h"
#include "hx_drv_uart.h"

EiDeviceWE2::EiDeviceWE2(EiDeviceMemory* mem)
{
    EiDeviceInfo::memory = mem;

    init_device_id();

    load_config();

    device_type = "HIMAX_WE2";

    cam = static_cast<EiCameraWE2*>(EiCameraWE2::get_camera());
    camera_present = cam->is_camera_present();

    standalone_sensor_list[0].name = "Microphone";
    standalone_sensor_list[0].frequencies[0] = 16000.0f;
    standalone_sensor_list[0].start_sampling_cb = &ei_microphone_sample_start;
    standalone_sensor_list[0].max_sample_length_s = mem->get_available_sample_bytes() / (16000 * sizeof(microphone_sample_t));
}

EiDeviceWE2::~EiDeviceWE2()
{

}

EiDeviceInfo* EiDeviceInfo::get_device(void)
{
    /* Initializing EdgeImpulse classes here in order for
     * Flash memory to be initialized before mainloop start
     */
    static EiDeviceRAM<1024,100> memory(sizeof(EiConfig));
    static EiDeviceWE2 dev(static_cast<EiDeviceMemory*>(&memory));

    return &dev;
}

void EiDeviceWE2::init_device_id(void)
{
    //TODO: get uniqe ID of the MCU
    uint64_t id = 0x1122334455667788;
    char temp[18];

    snprintf(temp, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
            (uint8_t)(id>>56),
            (uint8_t)(id>>48),
            (uint8_t)(id>>40),
            (uint8_t)(id>>16),
            (uint8_t)(id>>8),
            (uint8_t)(id));

    device_id = std::string(temp);
}

void EiDeviceWE2::clear_config(void)
{
    EiDeviceInfo::clear_config();

    init_device_id();
    save_config();
}

uint32_t EiDeviceWE2::get_data_output_baudrate(void)
{
    return EI_TRANSFER_BAUDRATE;
}

void EiDeviceWE2::set_max_data_output_baudrate()
{
    DEV_UART *console_uart;
    console_uart = hx_drv_uart_get_dev(USE_DW_UART_0);
    console_uart->uart_control(UART_CMD_SET_BAUD, (UART_CTRL_PARAM)EI_TRANSFER_BAUDRATE);
}

void EiDeviceWE2::set_default_data_output_baudrate()
{
    DEV_UART *console_uart;
    console_uart = hx_drv_uart_get_dev(USE_DW_UART_0);
    console_uart->uart_control(UART_CMD_SET_BAUD, (UART_CTRL_PARAM)EI_DEFAULT_BAUDRATE);
}

bool EiDeviceWE2::start_sample_thread(void (*sample_read_cb)(void), float sample_interval_ms)
{

    return false;
}

bool EiDeviceWE2::stop_sample_thread(void)
{

    this->set_state(eiStateIdle);

    return true;
}

void EiDeviceWE2::set_state(EiState state)
{
    this->state = state;

    switch(state) {
        case eiStateErasingFlash:
        case eiStateSampling:
        case eiStateUploading:
        case eiStateFinished:
        case eiStateIdle:
        default:
            break;
    }
}

EiState EiDeviceWE2::get_state(void)
{
    return this->state;
}

EiSnapshotProperties EiDeviceWE2::get_snapshot_list(void)
{
    ei_device_snapshot_resolutions_t *res = NULL;
    uint8_t res_num = 0;

    EiSnapshotProperties props = {
        .has_snapshot = false,
        .support_stream = false,
        .color_depth = "",
        .resolutions_num = 0,
        .resolutions = res
    };

    if(this->cam->is_camera_present() == true) {
        this->cam->get_resolutions(&res, &res_num);
        props.has_snapshot = true;
        props.support_stream = true;
        props.color_depth = "RGB";
        props.resolutions_num = res_num;
        props.resolutions = res;
    }

    return props;
}

bool EiDeviceWE2::get_sensor_list(const ei_device_sensor_t **sensor_list, size_t *sensor_list_size)
{
    *sensor_list = this->standalone_sensor_list;
    *sensor_list_size = this->standalone_sensor_num;

    return false;
}
