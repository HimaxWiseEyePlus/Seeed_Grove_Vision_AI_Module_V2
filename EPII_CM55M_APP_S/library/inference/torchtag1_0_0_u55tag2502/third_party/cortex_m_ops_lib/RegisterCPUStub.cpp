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



namespace torch {
namespace executor {
namespace function {


at::Tensor & wrapper_CPU_out_quantize_per_tensor_out(const at::Tensor & input, double scale, int64_t zero_point, int64_t quant_min, int64_t quant_max, at::ScalarType dtype, at::Tensor & out) {
    return out;
}


at::Tensor & wrapper_CPU_out_dequantize_per_tensor_out(const at::Tensor & input, double scale, int64_t zero_point, int64_t quant_min, int64_t quant_max, at::ScalarType dtype, at::Tensor & out) {
    return out;
}


at::Tensor wrapper_CPU__quantized_add(const at::Tensor & self, const at::Scalar & self_zero_point, const at::Scalar & self_multiplier, const at::Scalar & self_shift, const at::Tensor & other, const at::Scalar & other_zero_point, const at::Scalar & other_multiplier, const at::Scalar & other_shift, const at::Scalar & output_zero_point, const at::Scalar & output_multiplier, const at::Scalar & output_shift) {
    return self;
}


at::Tensor & wrapper_CPU_out_quantized_add_out(const at::Tensor & self, const at::Scalar & self_zero_point, const at::Scalar & self_multiplier, const at::Scalar & self_shift, const at::Tensor & other, const at::Scalar & other_zero_point, const at::Scalar & other_multiplier, const at::Scalar & other_shift, const at::Scalar & output_zero_point, const at::Scalar & output_multiplier, const at::Scalar & output_shift, at::Tensor & out) {
    return out;
}

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
