#ifndef __REED_SOLOMON_ENCODER_H__
#define __REED_SOLOMON_ENCODER_H__

/*
 *  ReedSolomonEncoder.hpp
 *  zxing
 *
 *  Copyright 2013 ZXing authors All rights reserved.
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
#include <zxing/common/reedsolomon/GenericGFPoly.hpp>
#include <zxing/common/reedsolomon/GenericGF.hpp>
#include <zxing/ErrorHandler.hpp>
#include <vector>

namespace zxing {

class ReedSolomonEncoder {
public:
    ReedSolomonEncoder(Ref<GenericGF> field);
    ~ReedSolomonEncoder();
    void encode(ArrayRef<int> toEncode, int ecBytes);
    
private:
    Ref<GenericGFPoly> _buildGenerator(int degree);
    
    Ref<GenericGF> field_;
    std::vector<Ref<GenericGFPoly> > cachedGenerators_;
};
}  // namespace zxing

#endif  // __REED_SOLOMON_DECODER_H__
