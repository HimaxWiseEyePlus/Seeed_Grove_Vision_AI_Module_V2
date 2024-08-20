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
#ifndef KWS_MICRONET_MFCC_HPP
#define KWS_MICRONET_MFCC_HPP

#include "Mfcc.hpp"
#include <cstdint>
namespace arm {
namespace app {
namespace audio {

    /* Class to provide MicroNet specific MFCC calculation requirements. */
    class MicroNetKwsMFCC : public MFCC {

    public:
        static constexpr uint32_t  ms_defaultSamplingFreq = 16000;
        static constexpr uint32_t  ms_defaultNumFbankBins =    40;
        static constexpr uint32_t  ms_defaultMelLoFreq    =    20;
        static constexpr uint32_t  ms_defaultMelHiFreq    =  7600;
        static constexpr bool      ms_defaultUseHtkMethod =  true;

        explicit MicroNetKwsMFCC(const size_t numFeats, const size_t frameLen)
            :  MFCC(MfccParams(
                        ms_defaultSamplingFreq, ms_defaultNumFbankBins,
                        ms_defaultMelLoFreq, ms_defaultMelHiFreq,
                        numFeats, frameLen, ms_defaultUseHtkMethod))
        {}
        MicroNetKwsMFCC()  = delete;
        ~MicroNetKwsMFCC() = default;
    };

} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* KWS_MICRONET_MFCC_HPP */