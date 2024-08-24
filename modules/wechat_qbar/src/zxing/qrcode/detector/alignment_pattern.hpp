// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-

#ifndef __ALIGNMENT_PATTERN_H__
#define __ALIGNMENT_PATTERN_H__

/*
 *  AlignmentPattern.hpp
 *  zxing
 *
 *  Copyright 2010 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zxing/result_point.hpp>
#include <zxing/common/bit_matrix.hpp>
#include <cmath>

namespace zxing {
namespace qrcode {

class AlignmentPattern : public ResultPoint {
private:
    float estimatedModuleSize_;

public:
    AlignmentPattern(float posX, float posY, float estimatedModuleSize);
    bool aboutEquals(float moduleSize, float i, float j) const;
    float getModuleSize(){return estimatedModuleSize_;};
    
    Ref<AlignmentPattern> combineEstimate(float i, float j,
                                          float newModuleSize) const;
};

}  // namespace qrcode
}  // namespace zxing

#endif  // __ALIGNMENT_PATTERN_H__
