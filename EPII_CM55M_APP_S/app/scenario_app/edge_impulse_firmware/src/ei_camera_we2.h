/*
 * Copyright (c) 2023 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS
 * IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EI_CAMERA_WE2_H
#define EI_CAMERA_WE2_H
#include "firmware-sdk/ei_camera_interface.h"
#include "firmware-sdk/ei_device_info_lib.h"
extern "C" {
    #include "event_handler_evt.h"
};

typedef enum {
    CAM_STATE_IDLE = 0,
    CAM_STATE_STREAM_ACTIVE,
    CAM_STATE_SNAPSHOT_PREPARE,
    CAM_STATE_SNAPSHOT,
    CAM_STATE_INFERENCING,
} EiCameraState_t;

class EiCameraWE2 : public EiCamera {

private:
    static ei_device_snapshot_resolutions_t resolutions[];
    uint32_t width;
    uint32_t height;
    uint32_t inference_scaled_fb_width;
    uint32_t inference_scaled_fb_height;
    uint8_t *stream_buffer;
    uint32_t stream_buffer_size;
    bool camera_present;
    EiCameraState_t state;
    const uint8_t frames_to_drop = 20;
    void (*run_impulse_callback)(void);

public:
    EiCameraWE2();
    bool init(uint16_t width, uint16_t height) override;
    bool set_resolution(const ei_device_snapshot_resolutions_t res);
    ei_device_snapshot_resolutions_t get_min_resolution(void);
    bool is_camera_present(void);
    void get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num);

    // this is blocking method, it will return then user stop the stream
    bool start_stream(uint32_t width, uint32_t height);
    bool start_inference(uint8_t** frame_buffer, void (*run_impulse_callback)(void));
    void take_snapshot(uint32_t width, uint32_t height);

    // hxevent callback, must be a friend to get access to stream_buffer
    friend void ei_camera_event_cb(EVT_INDEX_E event);
};

#endif /* EI_CAMERA_WE2_H */
