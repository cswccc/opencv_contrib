/*
 * Copyright 2010 ZXing authors
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

#ifndef __CODE_39_WRITER_H__
#define __CODE_39_WRITER_H__

// package com.google.zxing.oned;

#include <zxing/BarcodeFormat.h>
// import com.google.zxing.EncodeHintType;
#include <zxing/Exception.h>
#include <zxing/common/BitMatrix.h>

#include <map>

#include <zxing/oned/OneDimensionalCodeWriter.h>
#include <zxing/oned/OneDConstant.h>
#include <zxing/oned/Code39Reader.h>
#include <zxing/common/Str.h>

using namespace zxing::oned::constant::Code39;

namespace zxing {
namespace oned {
/**
 * This object renders a CODE39 code as a {@link BitMatrix}.
 *
 * @author erik.barbara@gmail.com (Erik Barbara)
 */
class Code39Writer  : public OneDimensionalCodeWriter{
public:
    ArrayRef<boolean> encode(Ref<String> contents);
    
private:
    static void toIntArray(int a, int* toReturn);
};

}  // namespace oned
}  // namespace zxing

#endif
