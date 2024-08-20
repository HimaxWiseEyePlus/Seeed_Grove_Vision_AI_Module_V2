/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "TensorFlowLiteMicro.hpp"

void PrintTensorFlowVersion()
{}

arm::app::QuantParams arm::app::GetTensorQuantParams(TfLiteTensor* tensor)
{
    arm::app::QuantParams params;
    if (kTfLiteAffineQuantization == tensor->quantization.type) {
        auto* quantParams = (TfLiteAffineQuantization*) (tensor->quantization.params);
        if (quantParams && 0 == quantParams->quantized_dimension) {
            if (quantParams->scale->size) {
                params.scale = quantParams->scale->data[0];
            }
            if (quantParams->zero_point->size) {
                params.offset = quantParams->zero_point->data[0];
            }
        } else if (tensor->params.scale != 0.0) {
            /* Legacy tensorflow quantisation parameters */
            params.scale = tensor->params.scale;
            params.offset = tensor->params.zero_point;
        }
    }
    return params;
}

