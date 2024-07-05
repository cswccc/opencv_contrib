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

#ifndef QBAR_AI_QBAR_ZXING_ONED_CODE128WRITER_H_
#define QBAR_AI_QBAR_ZXING_ONED_CODE128WRITER_H_

#include <zxing/ZXing.h>
#include <zxing/BarcodeFormat.h>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/oned/OneDimensionalCodeWriter.h>
#include <zxing/common/Integer.h>

#include <map>

namespace zxing {
namespace oned {

/**
 * This object renders a CODE128 code as a {@link BitMatrix}.
 *
 * @author erik.barbara@gmail.com (Erik Barbara)
 */
class Code128Writer : public OneDimensionalCodeWriter {
public:
    ArrayRef<boolean> encode(Ref<String> contents);
    
private:
    static bool isDigits(Ref<String> value, int start, int length);
};

}  // namespace oned
}  // namespace zxing

#endif  // QBAR_AI_QBAR_ZXING_ONED_CODE128WRITER_H_
