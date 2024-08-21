// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __BIT_ARRAY_WRITER_H__
#define __BIT_ARRAY_WRITER_H__

/*
 *  Copyright 2010 ZXing authors. All rights reserved.
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

#include <zxing/ZXing.hpp>
#include <zxing/common/Counted.hpp>
#include <zxing/common/IllegalArgumentException.hpp>
#include <zxing/common/Array.hpp>
#include <vector>
#include <limits>

namespace zxing {

class BitArrayWriter : public Counted {
public:
    static const int bitsPerWord = std::numeric_limits<unsigned int>::digits;
    
private:
    int size;
    ArrayRef<int> bits;
    static const int logBits = ZX_LOG_DIGITS(bitsPerWord);
    static const int bitsMask = (1 << logBits) - 1;
    
public:
    BitArrayWriter(int size);
    ~BitArrayWriter();
    int getSize() const;
    
    bool get(int i) const {
        return (bits[i >> logBits] & (1 << (i & bitsMask))) != 0;
    }
    
    void set(int i) {
        bits[i >> logBits] |= 1 << (i & bitsMask);
    }
    
    int getNextSet(int from);
    int getNextUnset(int from);
    
    void setBulk(int i, int newBits);
    void setRange(int start, int end);
    void clear();
    bool isRange(int start, int end, bool value);
    std::vector<int>& getBitArrayWriter();
    
    void reverse();
    
    class Reverse {
    private:
        Ref<BitArrayWriter> array;
    public:
        Reverse(Ref<BitArrayWriter> array);
        ~Reverse();
    };
    
    void appendBit(bool value);
    int getSizeInBytes() const;
    int appendBits(int value, int numberOfBits);
    void appendBitArray(const BitArrayWriter& array);
    void toBytes(int bitOffset, ArrayRef<int>& array, int offset, int numBytes);
    int bitXor(const BitArrayWriter& other);
    
private:
    static int makeArraySize(int size);
};

std::ostream& operator << (std::ostream&, BitArrayWriter const&);

}  // namespace zxing

#endif  // __BIT_ARRAY_H__
