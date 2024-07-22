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

#include "ei_camera_we2.h"
#include "ei_device_we2.h"
#include "model-parameters/model_metadata.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "firmware-sdk/at_base64_lib.h"
#include <cmath>
#include "cisdp_sensor.h"
#include "console_io.h"
#include "img_proc_helium.h"
extern "C" {
    #include "event_handler.h"
    #include <hx_drv_uart.h>
};

ei_device_snapshot_resolutions_t EiCameraWE2::resolutions[] = {
        { .width = 640, .height = 480 },
    };

void ei_camera_event_cb(EVT_INDEX_E event)
{
    char c;
    size_t uart_read_count;
    uint16_t err;
    bool stop_sensor = false;
    bool frame_ready = false;
    DEV_UART* console_uart;
    uint32_t jpeg_addr, jpeg_sz;
    auto cam = static_cast<EiCameraWE2*>(EiCameraWE2::get_camera());
    static uint8_t frames_to_drop = cam->frames_to_drop;

    console_uart = hx_drv_uart_get_dev(USE_DW_UART_0);
    console_uart->uart_open(UART_BAUDRATE_921600);

    // ei_printf("EVT event = %d\n", event);

	switch(event) {
        case EVT_INDEX_1BITPARSER_ERR:  /*reg_inpparser_fs_cycle_error*/
            hx_drv_inp1bitparser_get_errstatus(&err);
            ei_printf("EVT_INDEX_1BITPARSER_ERR err=0x%x\r\n",err);
            hx_drv_inp1bitparser_clear_int();
            hx_drv_inp1bitparser_set_enable(0);
            stop_sensor = true;
            break;
        case EVT_INDEX_EDM_WDT1_TIMEOUT:
            ei_printf("EVT_INDEX_EDM_WDT1_TlenIMEOUT\r\n");
            stop_sensor = true;
            break;
        case EVT_INDEX_EDM_WDT2_TIMEOUT:
            ei_printf("EVT_INDEX_EDM_WDT2_TIMEOUT\r\n");
            stop_sensor = true;
            break;
        case EVT_INDEX_EDM_WDT3_TIMEOUT:
            ei_printf("EVT_INDEX_EDM_WDT3_TIMEOUT\r\n");
            stop_sensor = true;
            break;
        case EVT_INDEX_CDM_FIFO_ERR:
            ei_printf("EVT_INDEX_CDM_FIFO_ERR\r\n");
            stop_sensor = true;
            break;
        case EVT_INDEX_XDMA_WDMA1_ABNORMAL:
        case EVT_INDEX_XDMA_WDMA2_ABNORMAL:
        case EVT_INDEX_XDMA_WDMA3_ABNORMAL:
        case EVT_INDEX_XDMA_RDMA_ABNORMAL:
            ei_printf("EVT_INDEX_XDMA_WDMA123_ABNORMAL or EVT_INDEX_XDMA_RDMA_ABNORMAL\r\n");
            stop_sensor = true;
            break;
        case EVT_INDEX_CDM_MOTION:
            ei_printf("Motion Detect\n");
            break;
        case EVT_INDEX_XDMA_FRAME_READY:
            frame_ready = true;
            // ei_printf("SENSORDPLIB_STATUS_XDMA_FRAME_READY\n");
            break;
        case EVT_INDEX_SENSOR_RTC_FIRE:
            ei_printf("EVT_INDEX_SENSOR_RTC_FIRE\n");
            break;
        case EVT_INDEX_HXAUTOI2C_ERR:
            ei_printf("EVT_INDEX_HXAUTOI2C_ERR\n");
            stop_sensor = true;
            break;
        default:
            ei_printf("Other Event %d\n", event);
            break;
    }

    if (frame_ready)
    {
        frame_ready = false;
        cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

#ifdef CIS_IMX
        hx_drv_scu_get_version(&chipid, &version);
        if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
        {
            cisdp_stream_off();
            set_mipi_csirx_disable();
        }
#endif
        if(cam->state == CAM_STATE_STREAM_ACTIVE) {
            SCB_InvalidateDCache_by_Addr((void *)jpeg_addr, jpeg_sz);
            base64_encode((char*)jpeg_addr, jpeg_sz, xputc);
            ei_printf("\r\n");
        }
        else if(cam->state == CAM_STATE_SNAPSHOT_PREPARE) {
            // it looks like first ~20 frames are pretty dark due to sensor auto exposure
            // so we drop them to get better quality
            if(!--frames_to_drop) {
                cam->state = CAM_STATE_SNAPSHOT;
                frames_to_drop = cam->frames_to_drop;
            }
        }
        else if(cam->state == CAM_STATE_SNAPSHOT) {
            SCB_InvalidateDCache_by_Addr((void *)jpeg_addr, jpeg_sz);
            base64_encode((char*)jpeg_addr, jpeg_sz, xputc);
            stop_sensor = true;
        }
        else if(cam->state == CAM_STATE_INFERENCING) {
            uint32_t img_w = app_get_raw_width();
            uint32_t img_h = app_get_raw_height();
            uint32_t ch = app_get_raw_channels();
            uint32_t raw_addr = app_get_raw_addr();
            // we are rescaling keeping the aspect ratio, so just one scale factor
            float scale = (float)(img_h - 1) / (EI_CLASSIFIER_INPUT_HEIGHT - 1);
            hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr,
                                                        cam->stream_buffer,
                                                        img_w, img_h,
                                                        ch,
                                                        cam->inference_scaled_fb_width, cam->inference_scaled_fb_height,
                                                        scale, scale);
            const uint32_t start_x = (cam->inference_scaled_fb_width - EI_CLASSIFIER_INPUT_WIDTH) / 2;
            ei::image::processing::crop_image_rgb888_packed(cam->stream_buffer,
                                                            cam->inference_scaled_fb_width, cam->inference_scaled_fb_height,
                                                            start_x, 0, /* startY is zero because height of source is the same as height of destination */
                                                            cam->stream_buffer,
                                                            EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
            cam->run_impulse_callback();
        }
        else {
            ei_printf("Unknown camera state %d\n", cam->state);
            stop_sensor = true;
        }

#ifdef CIS_IMX
        hx_drv_scu_get_version(&chipid, &version);
        if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
        {
            set_mipi_csirx_enable();
            cisdp_stream_on();
        }
#endif

        // recapture image
        if (!stop_sensor) {
            sensordplib_retrigger_capture();
        }
    }

    uart_read_count = console_uart->uart_read_nonblock(&c, 1);
    if(uart_read_count > 0 && c == 'b') {
        stop_sensor = true;
    }

    if(stop_sensor) {
        cam->state = CAM_STATE_IDLE;
        cisdp_sensor_stop();
        event_handler_stop();
    }
}

EiCameraWE2::EiCameraWE2()
{
    // hardcoded
    // TODO: get camera detect form sensor_init
    this->camera_present = true;
    this->state = CAM_STATE_IDLE;
}

bool EiCameraWE2::init(uint16_t width, uint16_t height)
{
    if(cisdp_sensor_init() < 0) {
        ei_printf("CIS Init fail\r\n");
        return false;
    }

    // 0 is jpeg_ratio, not used (except hm0360)
    if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, ei_camera_event_cb, 0, APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X) < 0)
    {
        ei_printf("\r\nDATAPATH Init fail\r\n");
    }

    event_handler_init();

    return true;
}

bool EiCameraWE2::set_resolution(const ei_device_snapshot_resolutions_t res)
{
    width = res.width;
    height = res.height;

    return true;
}

ei_device_snapshot_resolutions_t EiCameraWE2::get_min_resolution(void)
{
    return resolutions[0];
}

EiCamera* EiCamera::get_camera(void)
{
    static EiCameraWE2 cam;

    return &cam;
}

bool EiCameraWE2::is_camera_present(void)
{
    return camera_present;
}

void EiCameraWE2::get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num)
{
    *res = &EiCameraWE2::resolutions[0];
    *res_num = sizeof(EiCameraWE2::resolutions) / sizeof(ei_device_snapshot_resolutions_t);
}

bool EiCameraWE2::start_stream(uint32_t width, uint32_t height)
{
    // try to set required resolution, returned is what has been set
    ei_device_snapshot_resolutions_t sensor_res = {width, height};
    auto dev = EiDeviceWE2::get_device();

    this->init(width, height);

    if(set_resolution(sensor_res) == 0) {
        ei_printf("ERR: Failed to set camera resolution!\n");
        return false;
    }

    // this is estimated JPEG size (1/4th of the raw data)
    this->stream_buffer_size = this->width * this->height * 3 / 4;
    this->stream_buffer = (uint8_t*)ei_malloc(stream_buffer_size);
    if(this->stream_buffer == nullptr) {
        ei_printf("ERR: Failed to allocate stream buffer! (%u B)\n", stream_buffer_size);
        return false;
    }

    this->state = CAM_STATE_STREAM_ACTIVE;

    cisdp_sensor_start();

    // this is blocking call, the rest of the logic is happening in the ei_camera_event_cb
    event_handler_start();

    this->state = CAM_STATE_IDLE;
    ei_printf("Snapshot streaming stopped by user\r\n");
    ei_printf("OK\r\n");
    ei_free(this->stream_buffer);

    return true;
}

bool EiCameraWE2::start_inference(uint8_t** frame_buffer, void (*run_impulse_callback)(void))
{
    this->state = CAM_STATE_INFERENCING;
    this->run_impulse_callback = run_impulse_callback;

    // try to set required resolution, returned is what has been set
    ei_device_snapshot_resolutions_t sensor_res = {width, height};

    this->init(width, height);

    if(set_resolution(sensor_res) == 0) {
        ei_printf("ERR: Failed to set camera resolution!\n");
        return false;
    }

    // we are going to convert YUV to RGB and rescale the image using img_proc library
    // to avoid squeezing the image, we are rescaling both width and height by the same factor
    // however, infernce is usually done on square images, so we are going to crop the image
    // using EI image processing library.
    // So, we have to allocate frame buffer for the rescaled image but not cropped image
    // First, calculate the rescale factor (on the height only)
    const uint32_t scale_factor = SENCTRL_SENSOR_HEIGHT / EI_CLASSIFIER_INPUT_HEIGHT;
    this->inference_scaled_fb_width = SENCTRL_SENSOR_WIDTH / scale_factor;
    this->inference_scaled_fb_height = EI_CLASSIFIER_INPUT_HEIGHT;
    // Allocate the frame buffer for the rescaled image
    size_t frame_buffer_size = this->inference_scaled_fb_width * EI_CLASSIFIER_INPUT_HEIGHT * 3;
    *frame_buffer = (uint8_t*)ei_malloc(frame_buffer_size);
    if(*frame_buffer == nullptr) {
        ei_printf("ERR: Failed to allocate frame buffer! (%u B)\n", frame_buffer_size);
        return false;
    }
    this->stream_buffer = *frame_buffer;
    this->stream_buffer_size = frame_buffer_size;
    cisdp_sensor_start();

    // this is blocking call, the rest of the logic is happening in the ei_camera_event_cb
    event_handler_start();

    ei_free(*frame_buffer);

    this->state = CAM_STATE_IDLE;
    return true;
}

void EiCameraWE2::take_snapshot(uint32_t width, uint32_t height)
{
    // try to set required resolution, returned is what has been set
    ei_device_snapshot_resolutions_t sensor_res = {width, height};
    auto dev = EiDeviceWE2::get_device();

    this->init(width, height);

    if(set_resolution(sensor_res) == 0) {
        ei_printf("ERR: Failed to set camera resolution!\n");
        return;
    }

    // this is estimated JPEG size (1/4th of the raw data)
    this->stream_buffer_size = this->width * this->height * 3 / 4;
    this->stream_buffer = (uint8_t*)ei_malloc(stream_buffer_size);
    if(this->stream_buffer == nullptr) {
        ei_printf("ERR: Failed to allocate stream buffer! (%u B)\n", stream_buffer_size);
        return;
    }

    this->state = CAM_STATE_SNAPSHOT_PREPARE;

    cisdp_sensor_start();

    // this is blocking call, the rest of the logic is happening in the ei_camera_event_cb
    event_handler_start();

    this->state = CAM_STATE_IDLE;
    ei_printf("\r\nOK\r\n");
    ei_free(this->stream_buffer);
}