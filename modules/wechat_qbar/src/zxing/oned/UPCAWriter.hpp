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

#ifndef __UPCA_WRITER_H__
#define __UPCA_WRITER_H__

#include <zxing/BarcodeFormat.hpp>
#include <zxing/Exception.hpp>
#include <zxing/common/BitMatrix.hpp>

#include <zxing/oned/OneDimensionalCodeWriter.hpp>

#include <zxing/oned/EAN13Writer.hpp>

namespace zxing {
namespace oned {
/**
 * This object renders a UPC-A code as a {@link BitMatrix}.
 *
 * @author qwandor@google.com (Andrew Walbran)
 */
class UPCAWriter : public OneDimensionalCodeWriter {
private:
    EAN13Writer subWriter;
    
public:
    ArrayRef<boolean> encode(Ref<String> contents);
    
private:
    static Ref<String> preencode(Ref<String> contents);
};

}  // namespace oned
}  // namespace zxing

#endif
