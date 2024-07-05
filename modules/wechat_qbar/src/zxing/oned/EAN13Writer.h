/*
 * Copyright 2009 ZXing authors
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

#ifndef __EAN13_WRITER_H__
#define __EAN13_WRITER_H__

#include <zxing/BarcodeFormat.h>
#include <zxing/FormatException.h>
#include <zxing/Exception.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/oned/UPCEANWriter.h>
#include <zxing/oned/UPCEANReader.h>
#include <zxing/oned/OneDConstant.h>
#include <zxing/common/Integer.h>
#include <map>

using zxing::oned::UPCEANReader;

using namespace zxing::oned::constant::UPCEAN;
using namespace zxing::oned::constant::EAN13;

namespace zxing {
namespace oned {

/**
 * This object renders an EAN13 code as a {@link BitMatrix}.
 *
 * @author aripollak@gmail.com (Ari Pollak)
 */
class EAN13Writer : public UPCEANWriter {
    static const int CODE_WIDTH = 3 +  // start guard
    (7 * 6) +  // left bars
    5 +  // middle guard
    (7 * 6) +  // right bars
    3;  // end guard
    
public:
    ArrayRef<boolean> encode(Ref<String> contents);
};

}  // namespace oned
}  // namespace zxing

#endif
