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

#include <zxing/oned/CodaBarWriter.h>

using namespace zxing;
using namespace oned;

ArrayRef<boolean> CodaBarWriter::encode(Ref<String> contents) {
    
    if (contents->length() < 2)
    {
        // Can't have a start/end guard, so tentatively add default guards
        
        contents = StrUtil::COMBINE_STRING(DEFAULT_GUARD, contents, DEFAULT_GUARD);
    }
    else
    {
        // Verify input and calculate decoded length.
        char firstChar = Character::toUpperCase(contents->charAt(0));
        bool startsNormal = CodaBarReader::arrayContains(START_END_CHARS, firstChar);
        bool startsAlt = CodaBarReader::arrayContains(ALT_START_END_CHARS, firstChar);
        if (!startsNormal && !startsAlt)
            contents = StrUtil::COMBINE_STRING(DEFAULT_GUARD, contents, DEFAULT_GUARD);
    }
    
    // The start character and the end character are decoded to 10 length each.
    int resultLength = 20;
    for (int i = 1; i < contents->length() - 1; i++) {
        if (Character::isDigit(contents->charAt(i)) || contents->charAt(i) == '-' || contents->charAt(i) == '$')
        {
            resultLength += 9;
        }
        else if (CodaBarReader::arrayContains(CHARS_WHICH_ARE_TEN_LENGTH_EACH_AFTER_DECODED, contents->charAt(i)))
        {
            resultLength += 10;
        }
    }
    // A blank is placed between each character.
    resultLength += contents->length() - 1;
    
    ArrayRef<boolean> result(resultLength);
    int position = 0;
    for (int index = 0; index < contents->length(); index++) {
        char c = Character::toUpperCase(contents->charAt(index));
        if (index == 0 || index == contents->length() - 1)
        {
            // The start/end chars are not in the CodaBarReader.ALPHABET.
            switch (c) {
                case 'T':
                    c = 'A';
                    break;
                case 'N':
                    c = 'B';
                    break;
                case '*':
                    c = 'C';
                    break;
                case 'E':
                    c = 'D';
                    break;
            }
        }
        int code = 0;
        for (int i = 0; i < ALPHABET_LENGTH; i++) {
            // Found any, because I checked above.
            if (c == ALPHABET[i])
            {
                code = CHARACTER_ENCODINGS[i];
                break;
            }
        }
        bool color = true;
        int counter = 0;
        int bit = 0;
        while (bit < 7) {  // A character consists of 7 digit.
            result[position] = color;
            position++;
            if (((code >> (6 - bit)) & 1) == 0 || counter == 1)
            {
                color = !color;  // Flip the color.
                bit++;
                counter = 0;
            }
            else
            {
                counter++;
            }
        }
        if (index < contents->length() - 1)
        {
            result[position] = false;
            position++;
        }
    }
    return result;
}
