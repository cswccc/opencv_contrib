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


#include <zxing/oned/EAN8Writer.h>

using namespace zxing;
using namespace oned;

/**
 * @return a byte array of horizontal pixels (false = white, true = black)
 */
ArrayRef<boolean> EAN8Writer::encode(Ref<String> contents) {
    if (contents->length() != 8) {
        if (contents->length() == 7) {
            // Get check sum first for 12 length
            int checkSum = UPCEANReader::getStandardUPCEANChecksum(contents);
            
            contents->append(checkSum);
        }
    }
    
    ArrayRef<boolean> result(CODE_WIDTH);
    int pos = 0;

    pos += appendPattern(result, pos, START_END_PATTERN_, START_END_PATTERN_LEN, true);
    
    for (int i = 0; i <= 3; i++) {
        int digit = Integer::parseInt(contents->substring(i, i+1));
        
        pos += appendPattern(result, pos, L_PATTERNS_[digit], L_AND_G_PATTERNS_ONE_SIZE, false);
    }
    
    pos += appendPattern(result, pos, MIDDLE_PATTERN_, MIDDLE_PATTERN_LEN, false);
    
    for (int i = 4; i <= 7; i++) {
        int digit = Integer::parseInt(contents->substring(i, i+1));
        
        pos += appendPattern(result, pos, L_PATTERNS_[digit], L_AND_G_PATTERNS_ONE_SIZE, true);
    }
    
    appendPattern(result, pos, START_END_PATTERN_, START_END_PATTERN_LEN, true);
    
    return result;
}
