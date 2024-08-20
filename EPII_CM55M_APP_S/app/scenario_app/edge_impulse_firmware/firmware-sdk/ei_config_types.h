/*
 * Copyright (c) 2022 EdgeImpulse Inc.
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

#ifndef EI_CONFIG_TYPES_H
#define EI_CONFIG_TYPES_H

#include <cstdint>

/** ei sampler callback function, call with sample data */
typedef bool (*sampler_callback)(const void *sample_buf, uint32_t byteLenght);
typedef bool (*starter_callback)(sampler_callback callback, float sample_interval_ms);

// return types
typedef enum {
    EI_CONFIG_OK = 0,
    EI_CONFIG_BOUNDS_ERROR = -1,
    EI_CONFIG_CONTEXT_ERROR = -2,
    EI_CONFIG_NO_CONTEXT = -8,
    EI_CONFIG_WIFI_CONN_FAILED = -9,
    EI_CONFIG_NOT_IMPLEMENTED = -10,
} EI_CONFIG_ERROR;

// same as https://github.com/ARMmbed/mbed-os/blob/d4122b0b3a091d3f1130f3bb0d91c96f9183e2de/features/netsocket/nsapi_types.h#L115
typedef enum {
    EI_SECURITY_NONE         = 0x0,      /*!< open access point */
    EI_SECURITY_WEP          = 0x1,      /*!< phrase conforms to WEP */
    EI_SECURITY_WPA          = 0x2,      /*!< phrase conforms to WPA */
    EI_SECURITY_WPA2         = 0x3,      /*!< phrase conforms to WPA2 */
    EI_SECURITY_WPA_WPA2     = 0x4,      /*!< phrase conforms to WPA/WPA2 */
    EI_SECURITY_PAP          = 0x5,      /*!< phrase conforms to PPP authentication context */
    EI_SECURITY_CHAP         = 0x6,      /*!< phrase conforms to PPP authentication context */
    EI_SECURITY_EAP_TLS      = 0x7,      /*!< phrase conforms to EAP-TLS */
    EI_SECURITY_PEAP         = 0x8,      /*!< phrase conforms to PEAP */
    EI_SECURITY_UNKNOWN      = 0xFF,     /*!< unknown/unsupported security in scan results */
} ei_config_security_t;


// All the possible configuration options we can set
typedef struct {
    char wifi_ssid[128];
    char wifi_password[128];
    ei_config_security_t wifi_security;
    float sample_interval_ms;
    uint32_t sample_length_ms;
    char sample_label[128];
    char sample_hmac_key[33];
    char upload_host[128];
    char upload_path[128];
    char upload_api_key[128];
    char mgmt_url[128];
    uint32_t magic;
} ei_config_t;

#endif /* EI_CONFIG_TYPES_H */

