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

#include <zxing/oned/code39writer.hpp>

using namespace zxing;

using namespace oned;

ArrayRef<boolean> Code39Writer::encode(Ref<String> contents) {
    int length = contents->length();

    int widths[9];
    int codeWidth = 24 + 1 + length;
    for (int i = 0; i < length; i++) {
        int indexInString = StrUtil::indexOf(ALPHABET_STRING, contents->charAt(i));
        toIntArray(CHARACTER_ENCODINGS[indexInString], widths);
        for (int i = 0; i < 9; i++) {
            int width = widths[i];
            codeWidth += width;
        }
    }
    ArrayRef<boolean> result(codeWidth);
    toIntArray(CHARACTER_ENCODINGS[39], widths);
    int pos = appendPattern(result, 0, widths, 9, true);

    int narrowWhite[1] = {1};
    pos += appendPattern(result, pos, narrowWhite, 1, false);
    
    // append next character to byte matrix
    for (int i = 0; i < length; i++) {
        int indexInString = StrUtil::indexOf(ALPHABET_STRING, contents->charAt(i));
        toIntArray(CHARACTER_ENCODINGS[indexInString], widths);

        pos += appendPattern(result, pos, widths, 9, true);
        pos += appendPattern(result, pos, narrowWhite, 1, false);
    }
    toIntArray(CHARACTER_ENCODINGS[39], widths);
    
    appendPattern(result, pos, widths, 9, true);
    return result;
}


void Code39Writer::toIntArray(int a, int* toReturn) {
    for (int i = 0; i < 9; i++) {
        int temp = a & (1 << (8 - i));
        toReturn[i] = temp == 0 ? 1 : 2;
    }
}
