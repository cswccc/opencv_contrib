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

#include <zxing/oned/ITFWriter.h>

using namespace zxing;
using namespace oned;

ArrayRef<boolean> ITFWriter::encode(Ref<String> contents) {
    int length = contents->length();
    
    ArrayRef<boolean> result(9 + 9 * length);
    
    int pos = appendPattern(result, 0, START_PATTERN, START_PATTERN_LEN, true);
    for (int i = 0; i < length; i += 2) {
        int one = Character::digit(contents->charAt(i), 10);
        int two = Character::digit(contents->charAt(i+1), 10);
        int encoding[18];
        memset(encoding, 0, 18 * sizeof(int));
        for (int j = 0; j < 5; j++) {
            encoding[2 * j] = PATTERNS[one][j];
            encoding[2 * j + 1] = PATTERNS[two][j];
        }
       
        pos += appendPattern(result, pos, encoding, 18, true);
    }
    
    appendPattern(result, pos, END_PATTERN, END_PATTERN_LEN, true);
    
    return result;
}
