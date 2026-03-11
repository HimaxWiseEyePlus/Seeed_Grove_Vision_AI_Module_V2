/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// clang-format off
#include <torch/library.h>
#include <ATen/Tensor.h>

#include "CustomOpsNativeFunctions.h"

namespace torch {
namespace executor {
namespace function {

namespace {

at::Tensor & wrapper_CPU_out_quantize_per_tensor_out(const at::Tensor & input, double scale, int64_t zero_point, int64_t quant_min, int64_t quant_max, at::ScalarType dtype, at::Tensor & out) {
    // No device check


  // DeviceGuard omitted
  return cortex_m::native::quantize_per_tensor_out(input, scale, zero_point, quant_min, quant_max, dtype, out);
}

} // anonymous namespace

namespace {

at::Tensor & wrapper_CPU_out_dequantize_per_tensor_out(const at::Tensor & input, double scale, int64_t zero_point, int64_t quant_min, int64_t quant_max, at::ScalarType dtype, at::Tensor & out) {
    // No device check


  // DeviceGuard omitted
  return cortex_m::native::dequantize_per_tensor_out(input, scale, zero_point, quant_min, quant_max, dtype, out);
}

} // anonymous namespace

namespace {

at::Tensor wrapper_CPU__quantized_add(const at::Tensor & self, const at::Scalar & self_zero_point, const at::Scalar & self_multiplier, const at::Scalar & self_shift, const at::Tensor & other, const at::Scalar & other_zero_point, const at::Scalar & other_multiplier, const at::Scalar & other_shift, const at::Scalar & output_zero_point, const at::Scalar & output_multiplier, const at::Scalar & output_shift) {
    // No device check


  // DeviceGuard omitted
  return cortex_m::native::quantized_add(self, self_zero_point, self_multiplier, self_shift, other, other_zero_point, other_multiplier, other_shift, output_zero_point, output_multiplier, output_shift);
}

} // anonymous namespace

namespace {

at::Tensor & wrapper_CPU_out_quantized_add_out(const at::Tensor & self, const at::Scalar & self_zero_point, const at::Scalar & self_multiplier, const at::Scalar & self_shift, const at::Tensor & other, const at::Scalar & other_zero_point, const at::Scalar & other_multiplier, const at::Scalar & other_shift, const at::Scalar & output_zero_point, const at::Scalar & output_multiplier, const at::Scalar & output_shift, at::Tensor & out) {
    // No device check


  // DeviceGuard omitted
  return cortex_m::native::quantized_add_out(self, self_zero_point, self_multiplier, self_shift, other, other_zero_point, other_multiplier, other_shift, output_zero_point, output_multiplier, output_shift, out);
}

} // anonymous namespace

// All out variants ops

TORCH_LIBRARY_IMPL(cortex_m, CPU, m) {
m.impl("quantize_per_tensor.out",
TORCH_FN(wrapper_CPU_out_quantize_per_tensor_out));

m.impl("dequantize_per_tensor.out",
TORCH_FN(wrapper_CPU_out_dequantize_per_tensor_out));

m.impl("quantized_add",
TORCH_FN(wrapper_CPU__quantized_add));

m.impl("quantized_add.out",
TORCH_FN(wrapper_CPU_out_quantized_add_out));

}

namespace cpu {



} // namespace cpu

} // namespace function
} // namespace executor
} // namespace torch
