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

#ifndef EI_RUN_IMPULSE_H
#define EI_RUN_IMPULSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

bool ei_start_impulse(bool continuous, bool debug, bool use_max_uart_speed);
void ei_run_impulse(void);
void ei_stop_impulse(void);
bool is_inference_running(void);

#ifdef __cplusplus
}
#endif

#endif /* EI_RUN_IMPULSE_H */
