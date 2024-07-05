// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
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

#include <zxing/qrcode/encoder/BitArrayWriter.h>
#include <zxing/common/Array.h>

using zxing::Array;
using zxing::ArrayRef;
using std::vector;
using zxing::BitArrayWriter;

// VC++
using zxing::Ref;

int BitArrayWriter::makeArraySize(int size) {
    return (size + bitsPerWord - 1) >> logBits;
}

BitArrayWriter::BitArrayWriter(int size_)
: size(size_), bits(makeArraySize(size)) {}

BitArrayWriter::~BitArrayWriter() {
}

int BitArrayWriter::getSize() const {
    return size;
}

void BitArrayWriter::setBulk(int i, int newBits) {
    bits[i >> logBits] = newBits;
}

void BitArrayWriter::clear() {
    int max = bits->size();
    for (int i = 0; i < max; i++) {
        bits[i] = 0;
    }
}

bool BitArrayWriter::isRange(int start, int end, bool value) {
    if (end < start)
    {
        //  throw IllegalArgumentException();
        return false;
    }
    if (end == start)
    {
        return true;  // empty range matches
    }
    end--;  // will be easier to treat this as the last actually set bit -- inclusive
    int firstInt = start >> logBits;
    int lastInt = end >> logBits;
    for (int i = firstInt; i <= lastInt; i++) {
        int firstBit = i > firstInt ? 0 : start & bitsMask;
        int lastBit = i < lastInt ? (bitsPerWord-1) : end & bitsMask;
        int mask;
        if (firstBit == 0 && lastBit == (bitsPerWord-1))
        {
            mask = -1;
        }
        else
        {
            mask = 0;
            for (int j = firstBit; j <= lastBit; j++) {
                mask |= 1 << j;
            }
        }
        
        // Return false if we're looking for 1s and the masked bits[i] isn't all 1s (that is,
        // equals the mask, or we're looking for 0s and the masked portion is not all 0s
        if ((bits[i] & mask) != (value ? mask : 0))
        {
            return false;
        }
    }
    return true;
}

vector<int>& BitArrayWriter::getBitArrayWriter() {
    return bits->values();
}

void BitArrayWriter::reverse() {
    ArrayRef<int> newBits(bits->size());
    int size = this->size;
    for (int i = 0; i < size; i++) {
        if (get(size - i - 1))
        {
            newBits[i >> logBits] |= 1 << (i & bitsMask);
        }
    }
    bits = newBits;
}

BitArrayWriter::Reverse::Reverse(Ref<BitArrayWriter> array_) : array(array_) {
    array->reverse();
}

BitArrayWriter::Reverse::~Reverse() {
    array->reverse();
}

namespace {
// N.B.: This only works for 32 bit ints ...
int numberOfTrailingZeros(int i) {
    // HD, Figure 5-14
    int y;
    if (i == 0) return 32;
    int n = 31;
    y = i <<16; if (y != 0) { n = n -16; i = y; }
    y = i << 8; if (y != 0) { n = n - 8; i = y; }
    y = i << 4; if (y != 0) { n = n - 4; i = y; }
    y = i << 2; if (y != 0) { n = n - 2; i = y; }
    return n - (((unsigned int)(i << 1)) >> 31);
}
}  // namespace

int BitArrayWriter::getNextSet(int from) {
    if (from >= size)
    {
        return size;
    }
    int bitsOffset = from >> logBits;
    int currentBits = bits[bitsOffset];
    // mask off lesser bits first
    currentBits &= ~((1 << (from & bitsMask)) - 1);
    while (currentBits == 0) {
        if (++bitsOffset == static_cast<int>(bits->size()))
        {
            return size;
        }
        currentBits = bits[bitsOffset];
    }
    int result = (bitsOffset << logBits) + numberOfTrailingZeros(currentBits);
    return result > size ? size : result;
}

int BitArrayWriter::getNextUnset(int from) {
    if (from >= size)
    {
        return size;
    }
    int bitsOffset = from >> logBits;
    int currentBits = ~bits[bitsOffset];
    // mask off lesser bits first
    currentBits &= ~((1 << (from & bitsMask)) - 1);
    while (currentBits == 0) {
        if (++bitsOffset == static_cast<int>(bits->size()))
        {
            return size;
        }
        currentBits = ~bits[bitsOffset];
    }
    int result = (bitsOffset << logBits) + numberOfTrailingZeros(currentBits);
    return result > size ? size : result;
}

void BitArrayWriter::appendBit(bool value) {
    ArrayRef<int> newBits(makeArraySize(size + 1));
    for (int i = 0; i < bits.array_->size(); ++i) {
        newBits[i] = bits[i];
    }
    bits = newBits;
    if (value)
    {
        set(size);
    }
    ++size;
}

int BitArrayWriter::getSizeInBytes() const {
    return (size + 7) >> 3;
}

// Appends the least-significant bits, from value, in order from
// most-significant to least-significant. For example, appending 6 bits
// from 0x000001E will append the bits 0, 1, 1, 1, 1, 0 in that order.
int BitArrayWriter::appendBits(int value, int numBits) {
    if (numBits < 0 || numBits > 32)
    {
        //  throw IllegalArgumentException("Number of bits must be between 0 and 32");
        return -1;
    }
    ArrayRef<int> newBits(makeArraySize(size + numBits));
    for (int i = 0; i < bits.array_->size(); ++i) {
        newBits[i] = bits[i];
    }
    bits = newBits;
    for (int numBitsLeft = numBits; numBitsLeft > 0; numBitsLeft--) {
        if (((value >> (numBitsLeft - 1)) & 0x01) == 1)
        {
            set(size);
        }
        ++size;
    }
    
    return 0;
}

void BitArrayWriter::appendBitArray(const BitArrayWriter& array) {
    ArrayRef<int> newBits(makeArraySize(size + array.getSize()));
    for (int i = 0; i < bits.array_->size(); ++i) {
        newBits[i] = bits[i];
    }
    bits = newBits;
    for (int i = 0; i < array.getSize(); ++i) {
        if (array.get(i)){
            set(size);
        }
        ++size;
    }
}

void BitArrayWriter::toBytes(int bitOffset, ArrayRef<int>& array, int offset, int numBytes) {
    for (int i = 0; i < numBytes; ++i) {
        int theByte = 0;
        for (int j = 0; j < 8; ++j) {
            if (get(bitOffset))
            {
                theByte |= 1 << (7 - j);
            }
            bitOffset++;
        }
        array[offset + i] = theByte;
    }
}

int BitArrayWriter::bitXor(const BitArrayWriter& other) {
    if (size != other.size)
    {
        return -1;
    }
    for (int i = 0; i < bits.array_->size(); ++i) {
        // The last byte could be incomplete (i.e. not have 8 bits in it) but
        // there is no problem since 0 XOR 0 == 0.
        bits[i] ^= other.bits[i];
    }
    return 0;
}
