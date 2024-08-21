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

#include <zxing/oned/OneDimensionalCodeWriter.hpp>

using namespace zxing;
using namespace oned;

/**
 * @param target encode black/white pattern into this array
 * @param pos position to start encoding at in {@code target}
 * @param pattern lengths of black/white runs to encode
 * @param startColor starting color - false for white, true for black
 * @return the number of elements added to target.
 */
int OneDimensionalCodeWriter::appendPattern(ArrayRef<boolean>& target, const int* pattern, int patternSize, bool startColor) {
    bool color = startColor;
    int numAdded = 0;
    
    int size = patternSize;
    for (int i = 0; i < size; i++)
    {
        int len = pattern[i];
        for (int j = 0; j < len; j++)
        {
            target.append(color);
        }
        numAdded += len;
        color = !color;  // flip color after each segment
    }
    return numAdded;
}
int OneDimensionalCodeWriter::appendPattern(ArrayRef<boolean>& target, int pos, const int* pattern, int patternSize, bool startColor) {
    bool color = startColor;
    int numAdded = 0;
    
    int size = patternSize;
    for (int i = 0; i < size; i++)
    {
        int len = pattern[i];
        for (int j = 0; j < len; j++)
        {
            target[pos++] = color;
        }
        numAdded += len;
        color = !color;  // flip color after each segment
    }
    return numAdded;
}
int OneDimensionalCodeWriter::getDefaultMargin() {
    // CodaBar spec requires a side margin to be more than ten times wider than narrow space.
    // This seems like a decent idea for a default for all formats.
    return 10;
}
