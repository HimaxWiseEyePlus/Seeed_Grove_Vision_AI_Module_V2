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

#ifndef EI_DEVICE_WE2_H
#define EI_DEVICE_WE2_H

#include "firmware-sdk/ei_device_info_lib.h"
#include "firmware-sdk/ei_device_memory.h"
#include "ei_camera_we2.h"

#define EI_TRANSFER_BAUDRATE 921600
#define EI_DEFAULT_BAUDRATE  115200

class EiDeviceWE2 : public EiDeviceInfo {
private:
    EiDeviceWE2() = delete;
    static const int standalone_sensor_num = 1;
    ei_device_sensor_t standalone_sensor_list[standalone_sensor_num];
    EiState state;
    bool camera_present;
    EiCameraWE2 *cam;

public:
    EiDeviceWE2(EiDeviceMemory *mem);
    ~EiDeviceWE2();
    void init_device_id(void);
    void clear_config(void);
    uint32_t get_data_output_baudrate(void);
    void set_max_data_output_baudrate(void);
    void set_default_data_output_baudrate(void);
    bool start_sample_thread(void (*sample_read_cb)(void), float sample_interval_ms) override;
    bool stop_sample_thread(void) override;
    void set_state(EiState state) override;
    EiState get_state(void);
    EiSnapshotProperties get_snapshot_list(void);
    bool get_sensor_list(const ei_device_sensor_t **sensor_list, size_t *sensor_list_size);
};

#endif /* EI_DEVICE_WE2_H */
