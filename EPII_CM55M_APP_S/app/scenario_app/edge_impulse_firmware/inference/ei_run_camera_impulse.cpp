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

#include "model-parameters/model_metadata.h"
#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA)
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "firmware-sdk/ei_camera_interface.h"
#include "firmware-sdk/at_base64_lib.h"
#include "firmware-sdk/jpeg/encode_as_jpg.h"
#include "ei_run_impulse.h"
#include "ei_camera_we2.h"
#include "ei_device_we2.h"

static bool debug_mode = false;
static uint8_t *snapshot_buf = nullptr;

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0)
    {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix + 2];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix += 3;
        pixels_left--;
    }

    // and done!
    return 0;
}

/**
 * This callback is going to be called by camera driver when the frame is ready
 * and it was already rescaled and cropped to the dimensions required by the model
 */
void ei_run_impulse(void)
{
    int res;
    EiCameraWE2 *cam = static_cast<EiCameraWE2 *>(EiCameraWE2::get_camera());

    // tjis is already done in the camera_event_cb
    ei_printf("Taking photo...\n");

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    // print and discard JPEG buffer before inference to free some memory
    if (debug_mode)
    {
        ei_printf("Begin output\n");
        ei_printf("Framebuffer: ");
        encode_rgb888_signal_as_jpg_and_output_base64(&signal, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
        ei_printf("\r\n");
    }

    // run the impulse: DSP, neural network and the Anomaly algorithm
    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, false);
    if (ei_error != EI_IMPULSE_OK)
    {
        ei_printf("ERR: Failed to run impulse (%d)\n", ei_error);
        return;
    }

    display_results(&result);

    if (debug_mode)
    {
        ei_printf("End output\n");
    }
}

bool ei_start_impulse(bool continuous, bool debug, bool use_max_uart_speed)
{
    EiCameraWE2 *cam = static_cast<EiCameraWE2 *>(EiCameraWE2::get_camera());
    auto dev = EiDeviceWE2::get_device();

    debug_mode = debug;
    if(continuous) {
        ei_printf("ERR: Continuous mode not supported for camera\n");
        return false;
    }

    if (cam->is_camera_present() == false) {
        ei_printf("ERR: Failed to start inference, camera is missing!\n");
        return false;
    }

    if (cam->init(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT) == false) {
        return false;
    }

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));
    ei_printf("Starting inferencing in 1 seconds...\n");

    if (use_max_uart_speed && debug_mode) {
        ei_printf("OK\r\n");
        ei_sleep(100);
        dev->set_max_data_output_baudrate();
        ei_sleep(100);
    }

    // start the camera, this is blocking call
    if (cam->start_inference(&snapshot_buf, ei_run_impulse) == false) {
        return false;
    }

    if (use_max_uart_speed && debug_mode) {
        ei_printf("\r\nOK\r\n");
        ei_sleep(100);
        dev->set_default_data_output_baudrate();
        ei_sleep(100);
    }

    return true;
}

// this function for camera will be called only when inference is not running
// when we start the inference, the call is blocking and we will not call this function
// during the inference
bool is_inference_running(void)
{
    return false;
}

void ei_stop_impulse(void)
{
    // do nothing as this function is not used for camera inference
}

#endif /* defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA */
