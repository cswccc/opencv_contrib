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

#include <zxing/oned/EAN13Writer.hpp>

using namespace zxing;
using namespace oned;

ArrayRef<boolean> EAN13Writer::encode(Ref<String> contents) {
    
    if (contents->length() != 13) {
        
        if (contents->length() == 12) {
            // Get check sum first for 12 length
            int checkSum = UPCEANReader::getStandardUPCEANChecksum(contents);
            
            contents->append(checkSum);
        }
    }
    
    int firstDigit = Integer::parseInt(contents->substring(0, 1));
    int parities = FIRST_DIGIT_ENCODINGS[firstDigit];
    
    ArrayRef<boolean> result(CODE_WIDTH);
    int pos = 0;
    
    pos += appendPattern(result, pos, START_END_PATTERN_, START_END_PATTERN_LEN, true);
    
    // See {@link #EAN13Reader} for a description of how the first digit & left bars are encoded
    for (int i = 1; i <= 6; i++) {
        int digit = Integer::parseInt(contents->substring(i, i + 1));
        if ((parities >> (6 - i) & 1) == 1) {
            digit += 10;
        }

        pos += appendPattern(result, pos, L_AND_G_PATTERNS_[digit], L_AND_G_PATTERNS_ONE_SIZE, false);
    }
    
    pos += appendPattern(result, pos, MIDDLE_PATTERN_, MIDDLE_PATTERN_LEN, false);
    
    for (int i = 7; i <= 12; i++) {
        int digit = Integer::parseInt(contents->substring(i, i + 1));
        pos += appendPattern(result, pos, L_PATTERNS_[digit], L_AND_G_PATTERNS_ONE_SIZE, true);
    }
    
    appendPattern(result, pos, START_END_PATTERN_, START_END_PATTERN_LEN, true);
    
    return result;
}
