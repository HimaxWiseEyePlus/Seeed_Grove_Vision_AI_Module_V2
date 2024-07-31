/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "PlatformMath.hpp"
#include "log_macros.h"
#include <algorithm>

namespace arm {
namespace app {
namespace math {

    float MathUtils::CosineF32(float radians)
    {
#if ARM_MATH_DSP
        return arm_cos_f32(radians);
#else /* ARM_MATH_DSP */
        return cosf(radians);
#endif /* ARM_MATH_DSP */
    }

    float MathUtils::SineF32(float radians)
    {
#if ARM_MATH_DSP
        return arm_sin_f32(radians);
#else /* ARM_MATH_DSP */
        return sinf(radians);
#endif /* ARM_MATH_DSP */
    }

    float MathUtils::SqrtF32(float input)
    {
#if ARM_MATH_DSP
        float output = 0.f;
        arm_sqrt_f32(input, &output);
        return output;
#else /* ARM_MATH_DSP */
        return sqrtf(input);
#endif /* ARM_MATH_DSP */
    }

    float MathUtils::MeanF32(float* ptrSrc, const uint32_t srcLen)
    {
        if (!srcLen) {
            return 0.f;
        }

#if ARM_MATH_DSP
        float result = 0.f;
        arm_mean_f32(ptrSrc, srcLen, &result);
        return result;
#else /* ARM_MATH_DSP */
        float acc = std::accumulate(ptrSrc, ptrSrc + srcLen, 0.0);
        return acc/srcLen;
#endif /* ARM_MATH_DSP */
    }

    float MathUtils::StdDevF32(float* ptrSrc, const uint32_t srcLen,
                               const float mean)
    {
        if (!srcLen) {
            return 0.f;
        }
#if ARM_MATH_DSP
        /**
         * Note Standard deviation calculation can be off
         * by > 0.01 but less than < 0.1, according to
         * preliminary findings.
         **/
        UNUSED(mean);
        float stdDev = 0;
        arm_std_f32(ptrSrc, srcLen, &stdDev);
        return stdDev;
#else /* ARM_MATH_DSP */
        auto VarianceFunction = [=](float acc, const float value) {
            return acc + (((value - mean) * (value - mean))/ srcLen);
        };

        float acc = std::accumulate(ptrSrc, ptrSrc + srcLen, 0.0,
                                    VarianceFunction);

        return sqrtf(acc);
#endif /* ARM_MATH_DSP */
    }

    void MathUtils::FftInitF32(const uint16_t fftLen,
                               FftInstance& fftInstance,
                               const FftType type)
    {
        fftInstance.m_fftLen = fftLen;
        fftInstance.m_initialised = false;
        fftInstance.m_optimisedOptionAvailable = false;
        fftInstance.m_type = type;

#if ARM_MATH_DSP
        arm_status status = ARM_MATH_ARGUMENT_ERROR;
        switch (fftInstance.m_type) {
        case FftType::real:
            status = arm_rfft_fast_init_f32(&fftInstance.m_instanceReal, fftLen);
            break;

        case FftType::complex:
            status = arm_cfft_init_f32(&fftInstance.m_instanceComplex, fftLen);
            break;

        default:
            printf_err("Invalid FFT type\n");
            return;
        }

        if (ARM_MATH_SUCCESS != status) {
            printf_err("Failed to initialise FFT for len %d\n", fftLen);
        } else {
            fftInstance.m_optimisedOptionAvailable = true;
        }
#endif /* ARM_MATH_DSP */

        debug("Optimised FFT will be used: %s.\n", fftInstance.m_optimisedOptionAvailable? "yes": "no");

        fftInstance.m_initialised = true;
    }

    static void FftRealF32(std::vector<float>& input,
                           std::vector<float>& fftOutput)
    {
        const size_t inputLength = input.size();
        const size_t halfLength = input.size() / 2;

        fftOutput[0] = 0;
        fftOutput[1] = 0;
        for (size_t t = 0; t < inputLength; t++) {
            fftOutput[0] += input[t];
            fftOutput[1] += input[t] *
                MathUtils::CosineF32(2 * M_PI * halfLength * t / inputLength);
        }

        for (size_t k = 1, j = 2; k < halfLength; ++k, j += 2) {
            float sumReal = 0;
            float sumImag = 0;

            const auto theta = static_cast<float>(2 * M_PI * k / inputLength);

            for (size_t t = 0; t < inputLength; t++) {
                const auto angle = static_cast<float>(t * theta);
                sumReal += input[t] * MathUtils::CosineF32(angle);
                sumImag += -input[t]* MathUtils::SineF32(angle);
            }

            /* Arrange output to [real0, realN/2, real1, im1, real2, im2, ...] */
            fftOutput[j] = sumReal;
            fftOutput[j + 1] = sumImag;
        }
    }

    static void FftComplexF32(std::vector<float>& input,
                           std::vector<float>& fftOutput)
    {
        const size_t fftLen = input.size() / 2;
        for (size_t k = 0; k < fftLen; k++) {
            float sumReal = 0;
            float sumImag = 0;
            const auto theta = static_cast<float>(2 * M_PI * k / fftLen);
            for (size_t t = 0; t < fftLen; t++) {
                const auto angle = theta * t;
                const auto cosine = MathUtils::CosineF32(angle);
                const auto sine = MathUtils::SineF32(angle);
                sumReal += input[t*2] * cosine + input[t*2 + 1] * sine;
                sumImag += -input[t*2] * sine + input[t*2 + 1] * cosine;
            }
            fftOutput[k*2] = sumReal;
            fftOutput[k*2 + 1] = sumImag;
        }
    }

    void MathUtils::FftF32(std::vector<float>& input,
                           std::vector<float>& fftOutput,
                           arm::app::math::FftInstance& fftInstance)
    {
        if (!fftInstance.m_initialised) {
            printf_err("FFT uninitialised\n");
            return;
        } else if (input.size() < fftInstance.m_fftLen) {
            printf_err("FFT len: %" PRIu16 "; input len: %zu\n",
                fftInstance.m_fftLen, input.size());
            return;
        } else if (fftOutput.size() < input.size()) {
            printf_err("Output vector len insufficient to hold FFTs\n");
            return;
        }

        switch (fftInstance.m_type) {
        case FftType::real:

#if ARM_MATH_DSP
            if (fftInstance.m_optimisedOptionAvailable) {
                arm_rfft_fast_f32(&fftInstance.m_instanceReal, input.data(), fftOutput.data(), 0);
                return;
            }
#endif /* ARM_MATH_DSP */
            FftRealF32(input, fftOutput);
            return;

        case FftType::complex:
            if (input.size() < fftInstance.m_fftLen * 2) {
                printf_err("Complex FFT instance should have input size >= (FFT len x 2)");
                return;
            }
#if ARM_MATH_DSP
            if (fftInstance.m_optimisedOptionAvailable) {
                fftOutput = input; /* Complex function works in-place */
                arm_cfft_f32(&fftInstance.m_instanceComplex, fftOutput.data(), 0, 1);
                return;
            }
#endif /* ARM_MATH_DSP */
            FftComplexF32(input, fftOutput);
            return;

        default:
            printf_err("Invalid FFT type\n");
            return;
        }
    }

    void MathUtils::VecLogarithmF32(std::vector <float>& input,
                                    std::vector <float>& output)
    {
#if ARM_MATH_DSP
        arm_vlog_f32(input.data(), output.data(),
                     output.size());
#else /* ARM_MATH_DSP */
        for (auto in = input.begin(), out = output.begin();
             in != input.end() && out != output.end(); ++in, ++out) {
            *out = logf(*in);
        }
#endif /* ARM_MATH_DSP */
    }

    float MathUtils::DotProductF32(float* srcPtrA, float* srcPtrB,
                                   const uint32_t srcLen)
    {
        float output = 0.f;

#if ARM_MATH_DSP
        arm_dot_prod_f32(srcPtrA, srcPtrB, srcLen, &output);
#else /* ARM_MATH_DSP */
        for (uint32_t i = 0; i < srcLen; ++i) {
            output += *srcPtrA++ * *srcPtrB++;
        }
#endif /* ARM_MATH_DSP */

        return output;
    }

    bool MathUtils::ComplexMagnitudeSquaredF32(float* ptrSrc,
                                               const uint32_t srcLen,
                                               float* ptrDst,
                                               const uint32_t dstLen)
    {
        if (dstLen < srcLen/2) {
            printf_err("dstLen must be greater than srcLen/2");
            return false;
        }

#if ARM_MATH_DSP
        arm_cmplx_mag_squared_f32(ptrSrc, ptrDst, srcLen/2);
#else /* ARM_MATH_DSP */
        for (uint32_t j = 0; j < srcLen/2; ++j) {
            const float real = *ptrSrc++;
            const float im = *ptrSrc++;
            *ptrDst++ = real*real + im*im;
        }
#endif /* ARM_MATH_DSP */
        return true;
    }

    void MathUtils::SoftmaxF32(std::vector<float>& vec)
    {
        /* Fix for numerical stability and apply exp. */
        auto start = vec.begin();
        auto end = vec.end();

        float maxValue = *std::max_element(start, end);
        for (auto it = start; it != end; ++it) {
            *it = std::exp((*it) - maxValue);
        }

        float sumExp = std::accumulate(start, end, 0.0f);

        for (auto it = start; it != end; ++it) {
            *it = (*it)/sumExp;
        }
    }

    float MathUtils::SigmoidF32(float x)
    {
        return 1.f/(1.f + std::exp(-x));
    }

} /* namespace math */
} /* namespace app */
} /* namespace arm */
