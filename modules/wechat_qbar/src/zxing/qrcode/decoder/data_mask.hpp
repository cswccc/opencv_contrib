#ifndef __DATA_MASK_H__
#define __DATA_MASK_H__

/*
 *  DataMask.hpp
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

#include <zxing/common/array.hpp>
#include <zxing/common/counted.hpp>
#include <zxing/common/bit_matrix.hpp>
#include <zxing/error_handler.hpp>

#include <vector>

namespace zxing {
namespace qrcode {

class DataMask : public Counted {
private:
    static std::vector<Ref<DataMask> > DATA_MASKS;
    
public:
    static int buildDataMasks();
    DataMask();
    virtual ~DataMask();
    void unmaskBitMatrix(BitMatrix& matrix, size_t dimension);
    virtual bool isMasked(size_t x, size_t y) = 0;
    static DataMask& forReference(int reference, ErrorHandler & err_handler);
};

}  // namespace qrcode
}  // namespace zxing

#endif  // __DATA_MASK_H__
