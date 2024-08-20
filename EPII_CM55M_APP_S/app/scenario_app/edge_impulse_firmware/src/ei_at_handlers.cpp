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

/* Include ----------------------------------------------------------------- */
#include <string>
#include <iostream>

#include "ei_at_handlers.h"
#include "ei_camera_we2.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "firmware-sdk/ei_device_info_lib.h"
#include "firmware-sdk/ei_device_lib.h"
#include "firmware-sdk/at-server/ei_at_command_set.h"
#include "firmware-sdk/ei_image_lib.h"
#include "model-parameters/model_metadata.h"
#include "ei_run_impulse.h"

using namespace std;

/******
 *
 * @brief EdgeImpulse AT server functionality
 *
 ******/

/* Private variables ------------------------------------------------------- */
static EiDeviceWE2 *dev;

static inline bool check_args_num(const int &required, const int &received)
{
    if (received < required) {
        ei_printf("Too few arguments! Required: %d\n", required);
        return false;
    }

    return true;
}

bool at_get_device_id(void)
{
    ei_printf("%s\n", dev->get_device_id().c_str());

    return true;
}

bool at_set_device_id(const char **argv, const int argc)
{
    if(argc < 1) {
        ei_printf("Missing argument!\n");
        return true;
    }

    dev->set_device_id(argv[0]);
    ei_printf("OK\n");

    return true;
}

bool at_snapshot_stream(const char **argv, const int argc)
{
    uint32_t width, height;
    bool use_max_baudrate = false;
    EiSnapshotProperties props;
    auto cam = static_cast<EiCameraWE2*>(EiCameraWE2::get_camera());

    if(argc < 2) {
        ei_printf("Width and height arguments missing!\n");
        return true;
    }
    width = atoi(argv[0]);
    height = atoi(argv[1]);

    if(argc >=3 && argv[2][0] == 'y') {
        use_max_baudrate = true;
    }

    ei_printf("Starting snapshot stream...\n");

    if(use_max_baudrate) {
        ei_printf("\r\nOK");
        // make sure to flush data before changing baudrate
        ei_sleep(100);
        dev->set_max_data_output_baudrate();
        ei_sleep(100);
    }

    if(cam->start_stream(width, height) == false) {
        return true;
    }

    if (use_max_baudrate) {
        ei_printf("\nOK\n");
        ei_sleep(100);
        dev->set_default_data_output_baudrate();
    }

    // start stream is blocking, so we can return true and print prompt here
    return true;
}

bool at_get_snapshot(void)
{
    EiSnapshotProperties props;

    props = dev->get_snapshot_list();

    ei_printf("Has snapshot:         %d\n", props.has_snapshot ? 1 : 0);
    if(props.has_snapshot == true) {
        ei_printf("Supports stream:      %d\n", props.support_stream ? 1 : 0);
        ei_printf("Color depth:          RGB\n");
        // These are not native resolutions, but will be provided
        // after image processing (crop/resize) - so camera is not reporting them
        ei_printf("Resolutions:          [ ");
        for(int i = 0; i < props.resolutions_num; i++) {
            ei_printf("%ux%u", props.resolutions[i].width, props.resolutions[i].height);
            if (i != props.resolutions_num - 1) {
                ei_printf(", ");
            }
        }
        ei_printf(" ]\n");
    }

    return true;
}

bool at_take_snapshot(const char **argv, const int argc)
{
    uint32_t width, height;
    bool use_max_baudrate = false;
    auto cam = static_cast<EiCameraWE2*>(EiCameraWE2::get_camera());

    if(argc < 2) {
        ei_printf("Width and height arguments missing!\n");
        return true;
    }
    width = atoi(argv[0]);
    height = atoi(argv[1]);

    if(argc >=3 && argv[2][0] == 'y') {
        use_max_baudrate = true;
    }

    if(use_max_baudrate) {
        ei_printf("\r\nOK");
        // make sure to flush data before changing baudrate
        ei_sleep(100);
        dev->set_max_data_output_baudrate();
        ei_sleep(100);
    }

    cam->take_snapshot(width, height);

    if (use_max_baudrate) {
        ei_printf("\nOK\n");
        ei_sleep(100);
        dev->set_default_data_output_baudrate();
    }

    return true;
}

bool at_get_config(void)
{
    const ei_device_sensor_t *sensor_list;
    size_t sensor_list_size;

    dev->get_sensor_list((const ei_device_sensor_t **)&sensor_list, &sensor_list_size);

    ei_printf("===== Device info =====\n");
    ei_printf("ID:         %s\n", dev->get_device_id().c_str());
    ei_printf("Type:       %s\n", dev->get_device_type().c_str());
    ei_printf("AT Version: " AT_COMMAND_VERSION "\n");
    ei_printf("Data Transfer Baudrate: %lu\n", dev->get_data_output_baudrate());
    ei_printf("\n");
    ei_printf("===== Sensors ======\n");
    for (size_t ix = 0; ix < sensor_list_size; ix++) {
        ei_printf("Name: %s, Max sample length: %lu s, Frequencies: [", sensor_list[ix].name, sensor_list[ix].max_sample_length_s);
        for (size_t fx = 0; fx < EI_MAX_FREQUENCIES; fx++) {
            if (sensor_list[ix].frequencies[fx] > 0.1f) {
                if (fx != 0) {
                    ei_printf(", ");
                }
                ei_printf_float(sensor_list[ix].frequencies[fx]);
                ei_printf("Hz");
            }
        }
        ei_printf("]\n");
    }

    ei_printf("\n");
    ei_printf("===== Snapshot ======\n");
    at_get_snapshot();
    ei_printf("\n");
    ei_printf("===== Inference ======\n");
    ei_printf("Sensor:           %d\r\n", EI_CLASSIFIER_SENSOR);
#if EI_CLASSIFIER_OBJECT_DETECTION
    #if EI_CLASSIFIER_OBJECT_DETECTION_LAST_LAYER == EI_CLASSIFIER_LAST_LAYER_FOMO
        const char *model_type = "constrained_object_detection";
    #else
        const char *model_type = "object_detection";
    #endif
#else
    const char *model_type = "classification";
#endif
    ei_printf("Model type:       %s\r\n", model_type);
    ei_printf("\n");
    ei_printf("===== WIFI =====\n");
    ei_printf("SSID:      \n");
    ei_printf("Password:  \n");
    ei_printf("Security:  0\n");
    ei_printf("Connected: 0\n");
    ei_printf("Present:   0\n");
    ei_printf("\n");
    ei_printf("===== Sampling parameters =====\n");
    ei_printf("Label:     %s\n", dev->get_sample_label().c_str());
    ei_printf("Interval:  ");
    ei_printf_float(dev->get_sample_interval_ms());
    ei_printf(" ms.\n");
    ei_printf("Length:    %lu ms.\n", dev->get_sample_length_ms());
    ei_printf("HMAC key:  %s\n", dev->get_sample_hmac_key().c_str());
    ei_printf("\n");
    ei_printf("===== Upload settings =====\n");
    ei_printf("Api Key:   %s\n", dev->get_upload_api_key().c_str());
    ei_printf("Host:      %s\n", dev->get_upload_host().c_str());
    ei_printf("Path:      %s\n", dev->get_upload_path().c_str());
    ei_printf("\n");
    ei_printf("===== Remote management =====\n");
    ei_printf("URL:        %s\n", dev->get_management_url().c_str());
    ei_printf("Connected:  0\n");
    ei_printf("Last error: \n");
    ei_printf("\n");
    return true;
}

bool at_clear_config(void)
{
    dev->clear_config();
    dev->init_device_id();

    return true;
}

bool at_sample_start(const char **argv, const int argc)
{
    if(argc < 1) {
        ei_printf("Missing sensor name!\n");
        return true;
    }

    const ei_device_sensor_t *sensor_list;
    size_t sensor_list_size;

    dev->get_sensor_list((const ei_device_sensor_t **)&sensor_list, &sensor_list_size);

    for (size_t ix = 0; ix < sensor_list_size; ix++) {
        if (strcmp(sensor_list[ix].name, argv[0]) == 0) {
            /* Try to start sampling from the requested sensor */
            if (!sensor_list[ix].start_sampling_cb()) {
                ei_printf("ERR: Failed to start sampling\n");
                dev->set_state(eiStateIdle);
            }
            else {
                dev->set_state(eiStateFinished);
            }
            return true;
        }
    }

    return true;
}

bool at_set_sample_settings(const char **argv, const int argc)
{
    if(argc < 3) {
        ei_printf("Missing argument! Required: " AT_SAMPLESETTINGS_ARGS "\n");
        return true;
    }

    dev->set_sample_label(argv[0], false);

    string interval_ms_str(argv[1]);
    dev->set_sample_interval_ms(stof(interval_ms_str), false);

    string sample_length_str(argv[2]);
    dev->set_sample_length_ms(stoi(sample_length_str));

    if(argc >= 4) {
        dev->set_sample_hmac_key(argv[3]);
    }

    ei_printf("OK\n");

    return true;
}

bool at_get_sample_settings(void)
{
    ei_printf("Label:     %s\n", dev->get_sample_label().c_str());
    ei_printf("Interval:  ");
    ei_printf_float(dev->get_sample_interval_ms());
    ei_printf(" ms.\n");
    ei_printf("Length:    %lu ms.\n", dev->get_sample_length_ms());
    ei_printf("HMAC key:  %s\n", dev->get_sample_hmac_key().c_str());

    return true;
}

bool at_get_mgmt_url(void)
{
    ei_printf("%s\n", dev->get_management_url().c_str());

    return true;
}

bool at_set_mgmt_url(const char **argv, const int argc)
{
    if(argc < 1) {
        ei_printf("Missing argument!\n");
        return true;
    }

    dev->set_management_url(argv[0]);

    ei_printf("OK\n");

    return true;
}

bool at_read_buffer(const char **argv, const int argc)
{
    if(argc < 2) {
        ei_printf("Missing argument! Required: " AT_READBUFFER_ARGS "\n");
        return true;
    }
    bool success = true;

    size_t start = (size_t)atoi(argv[0]);
    size_t length = (size_t)atoi(argv[1]);

    dev->set_state(eiStateUploading);

    bool use_max_baudrate = false;
    if (argc >= 3 && argv[2][0] == 'y') {
       use_max_baudrate = true;
    }

    if (use_max_baudrate) {
        ei_printf("OK\n");
        dev->set_max_data_output_baudrate();
        ei_sleep(100);
    }

    success = read_encode_send_sample_buffer(start, length);

    if (use_max_baudrate) {
        ei_printf("\nOK\n");
        ei_sleep(100);
        dev->set_default_data_output_baudrate();
    }

    if (!success) {
        ei_printf("ERR: Failed to read from buffer\n");
        dev->set_state(eiStateIdle);
    }
    else {
        ei_printf("\n");
        dev->set_state(eiStateFinished);
    }

    return true;
}

bool at_read_raw(const char **argv, const int argc)
{
    EiDeviceMemory* mem = dev->get_memory();
    if (check_args_num(2, argc) == false) {
        return true;
    }

    size_t start = (size_t)atoi(argv[0]);
    size_t length = (size_t)atoi(argv[1]);

    uint8_t buffer[32];

    int count = 0;
    int n_display_bytes = 16;

    for (; start < length; start += n_display_bytes) {
        mem->read_sample_data(buffer, start, n_display_bytes);

        for (int i = 0; i < n_display_bytes; i++) {
            ei_printf("%02x", buffer[i]);
            if (i % 16 == 15)
                ei_printf("\n");
            else
                ei_printf(" ");
        }
    }
    ei_printf("\n");

    return true;
}

bool at_get_upload_settings(void)
{
    ei_printf("Api Key:   %s\n", dev->get_upload_api_key().c_str());
    ei_printf("Host:      %s\n", dev->get_upload_host().c_str());
    ei_printf("Path:      %s\n", dev->get_upload_path().c_str());

    return true;
}

bool at_set_upload_settings(const char **argv, const int argc)
{
    if(argc < 2) {
        ei_printf("Missing argument! Required: " AT_UPLOADSETTINGS_ARGS "\n");
        return true;
    }

    dev->set_upload_api_key(argv[0], false);
    dev->set_upload_path(argv[1]);

    ei_printf("OK\n");
    return true;
}

bool at_get_upload_host(void)
{
    ei_printf("%s\n", dev->get_upload_host().c_str());

    return true;
}

bool at_set_upload_host(const char **argv, const int argc)
{
    if(argc < 1) {
        ei_printf("Missing argument!\n");
        return true;
    }

    dev->set_upload_host(argv[0]);

    ei_printf("OK\n");

    return true;
}

bool at_unlink_file(const char **argv, const int argc)
{
    ei_printf("\n");

    return true;
}

bool at_run_impulse(void)
{
    return !ei_start_impulse(false, false, false);;
}

/**
 * @brief Handler for RUNIMPULSECONT
 *
 * @return
 */
static bool at_run_impulse_cont(void)
{
    return !ei_start_impulse(true, false, false);
}

bool at_run_impulse_debug(const char **argv, const int argc)
{
    bool use_max_uart_speed = false;
    if(argc > 0 && argv[0][0] == 'y') {
        use_max_uart_speed = true;
    }

    return !ei_start_impulse(false, true, use_max_uart_speed);
}

ATServer *ei_at_init(EiDeviceWE2 *device)
{
    ATServer *at;
    dev = device;

    at = ATServer::get_instance();

    at->register_command(AT_DEVICEID, AT_DEVICEID_HELP_TEXT, nullptr, at_get_device_id, at_set_device_id, AT_DEVICEID_ARGS);
    at->register_command(AT_CONFIG, AT_CONFIG_HELP_TEXT, nullptr, at_get_config, nullptr, nullptr);
    at->register_command(AT_CLEARCONFIG, AT_CLEARCONFIG_HELP_TEXT, at_clear_config, nullptr, nullptr, nullptr);
    at->register_command(AT_SAMPLESETTINGS, AT_SAMPLESETTINGS_HELP_TEXT, nullptr, at_get_sample_settings, at_set_sample_settings, AT_SAMPLESETTINGS_ARGS);
    at->register_command(AT_SAMPLESTART, AT_SAMPLESTART_HELP_TEXT, nullptr, nullptr, at_sample_start, AT_SAMPLESTART_ARGS);
    at->register_command(AT_MGMTSETTINGS, AT_MGMTSETTINGS_HELP_TEXT, nullptr, at_get_mgmt_url, at_set_mgmt_url, AT_MGMTSETTINGS_ARGS);
    at->register_command(AT_READBUFFER, AT_READBUFFER_HELP_TEXT, nullptr, nullptr, at_read_buffer, AT_READBUFFER_ARGS);
    at->register_command(AT_UPLOADSETTINGS, AT_UPLOADSETTINGS_HELP_TEXT, nullptr, at_get_upload_settings, at_set_upload_settings, AT_UPLOADSETTINGS_ARGS);
    at->register_command(AT_UPLOADHOST, AT_UPLOADHOST_HELP_TEXT, nullptr, at_get_upload_host, at_set_upload_host, AT_UPLOADHOST_ARGS);
    at->register_command(AT_UNLINKFILE, AT_UNLINKFILE_HELP_TEXT, nullptr, nullptr, at_unlink_file, AT_UNLINKFILE_ARGS);
    at->register_command(AT_READRAW, AT_READRAW_HELP_TEXT, nullptr, nullptr, at_read_raw, AT_READRAW_ARS);
    at->register_command(AT_RUNIMPULSE, AT_RUNIMPULSE_HELP_TEXT, at_run_impulse, nullptr, nullptr, nullptr);
    at->register_command(AT_RUNIMPULSECONT, AT_RUNIMPULSE_HELP_TEXT, at_run_impulse_cont, nullptr, nullptr, nullptr);
    at->register_command(AT_RUNIMPULSEDEBUG, AT_RUNIMPULSEDEBUG_HELP_TEXT, nullptr, nullptr, at_run_impulse_debug, AT_RUNIMPULSEDEBUG_ARGS);
    at->register_command(AT_SNAPSHOT, AT_SNAPSHOT_HELP_TEXT, nullptr, at_get_snapshot, at_take_snapshot, AT_SNAPSHOT_ARGS);
    at->register_command(AT_SNAPSHOTSTREAM, AT_SNAPSHOTSTREAM_HELP_TEXT, nullptr, nullptr, at_snapshot_stream, AT_SNAPSHOTSTREAM_ARGS);

    return at;
}
