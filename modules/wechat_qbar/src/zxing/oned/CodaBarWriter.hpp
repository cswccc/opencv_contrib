/*
 * Copyright 2011 ZXing authors
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

#ifndef __CODABAR_WRITER_H__
#define __CODABAR_WRITER_H__

#include <zxing/oned/OneDimensionalCodeWriter.hpp>
#include <zxing/common/Character.hpp>
#include <zxing/oned/CodaBarReader.hpp>
#include <zxing/common/Str.hpp>

#include <zxing/oned/OneDConstant.hpp>
using namespace zxing::oned::constant::CodaBar;

namespace zxing {
namespace oned {
/**
 * This class renders CodaBar as {@code boolean[]}.
 *
 * @author dsbnatut@gmail.com (Kazuki Nishiura)
 */
class CodaBarWriter  : public OneDimensionalCodeWriter{
public:
    ArrayRef<boolean> encode(Ref<String> contents);
};

}  // namespace oned
}  // namespace zxing

#endif
