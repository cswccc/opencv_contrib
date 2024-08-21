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

#ifndef __ONED_WRITER_H__
#define __ONED_WRITER_H__

#include <zxing/common/Counted.hpp>
#include <zxing/BarcodeFormat.hpp>
#include <zxing/Exception.hpp>
#include <zxing/common/BitMatrix.hpp>
#include <zxing/common/Str.hpp>

#include <zxing/oned/OneDConstant.hpp>

#include <map>

namespace zxing
{
namespace oned
{

/**
 * <p>Encapsulates functionality and implementation that is common to one-dimensional barcodes.</p>
 *
 * @author dsbnatut@gmail.com (Kazuki Nishiura)
 */
class OneDimensionalCodeWriter : public Counted{
public:
    /**
     * @param target encode black/white pattern into this array
     * @param pos position to start encoding at in {@code target}
     * @param pattern lengths of black/white runs to encode
     * @param startColor starting color - false for white, true for black
     * @return the number of elements added to target.
     */

protected:
    static int appendPattern(ArrayRef<boolean>& target, const int* pattern, int patternSize, bool startColor);
    
    static int appendPattern(ArrayRef<boolean>& target, int pos, const int* pattern, int patternSize, bool startColor);
    
public:
    virtual int getDefaultMargin();
    
    /**
     * Encode the contents to bool array expression of one-dimensional barcode.
     * Start code and end code should be included in result, and side margins should not be included.
     *
     * @param contents barcode contents to encode
     * @return a {@code boolean[]} of horizontal pixels (false = white, true = black)
     */
    virtual ArrayRef<boolean> encode(Ref<String> contents) = 0;
};

}  // namespace oned
}  // namespace zxing

#endif
