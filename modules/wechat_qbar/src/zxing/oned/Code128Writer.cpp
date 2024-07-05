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

#include <zxing/oned/Code128Writer.h>

#include <map>

using namespace zxing;
using namespace oned;

using namespace zxing::oned::constant::Code128;

ArrayRef<boolean> Code128Writer::encode(Ref<String> contents)
{
    int length = contents->length();
    // Check content
    for (int i = 0; i < length; i++) {
        char c = contents->charAt(i);
        
        if (c < ' ' || c > '~')
        {
            switch (c)
            {
                case ESCAPE_FNC_1:
                case ESCAPE_FNC_2:
                case ESCAPE_FNC_3:
                case ESCAPE_FNC_4:
                    break;
                default:
                    break;
            }
        }
    }
    
    std::vector<const int*> patterns;  // temporary storage for patterns
    int checkSum = 0;
    int checkWeight = 1;
    int codeSet = 0;  // selected code (CODE_CODE_B or CODE_CODE_C)
    int position = 0;  // position in contents
    
    while (position < length) {
        //Select code to use
        int requiredDigitCount = codeSet == CODE_CODE_C ? 2 : 4;
        int newCodeSet;
        if (isDigits(contents, position, requiredDigitCount))
        {
            newCodeSet = CODE_CODE_C;
        }
        else
        {
            newCodeSet = CODE_CODE_B;
        }
        
        // get the pattern index
        int patternIndex;
        if (newCodeSet == codeSet)
        {
            // Encode the current character
            // First handle escapes
            switch (contents->charAt(position)) {
                case ESCAPE_FNC_1:
                    patternIndex = CODE_FNC_1;
                    break;
                case ESCAPE_FNC_2:
                    patternIndex = CODE_FNC_2;
                    break;
                case ESCAPE_FNC_3:
                    patternIndex = CODE_FNC_3;
                    break;
                case ESCAPE_FNC_4:
                    patternIndex = CODE_FNC_4_B;  // FIXME if this ever outputs Code A
                    break;
                default:
                    // Then handle normal characters otherwise
                    if (codeSet == CODE_CODE_B)
                    {
                        patternIndex = contents->charAt(position) - ' ';
                    }
                    else
                    {  // CODE_CODE_C
                        patternIndex = Integer::parseInt(contents->substring(position, position + 2));
                        position++;  // Also incremented below
                    }
            }
            position++;
        }
        else
        {
            // Should we change the current code?
            // Do we have a code set?
            if (codeSet == 0)
            {
                // No, we don't have a code set
                if (newCodeSet == CODE_CODE_B)
                {
                    patternIndex = CODE_START_B;
                }
                else
                {
                    // CODE_CODE_C
                    patternIndex = CODE_START_C;
                }
            }
            else
            {
                // Yes, we have a code set
                patternIndex = newCodeSet;
            }
            codeSet = newCodeSet;
        }
        
        // Get the pattern
        patterns.push_back(CODE_PATTERNS[patternIndex]);
        
        // Compute checksum
        checkSum += patternIndex * checkWeight;
        if (position != 0)
        {
            checkWeight++;
        }
    }
    
    // Compute and append checksum
    checkSum %= 103;
    patterns.push_back(CODE_PATTERNS[checkSum]);
    
    // Append stop code
    patterns.push_back(CODE_PATTERNS[CODE_STOP]);
    
    // Compute result
    ArrayRef<boolean> result(0);
    for (size_t i = 0; i < patterns.size(); i++)
    {
        appendPattern(result, patterns[i], CODE_PATTERNS_ONE_SIZE, true);
    }
    
    int endPattern[1] = {2};
    
    appendPattern(result, endPattern, 1, true);
    
    return result;
}

bool Code128Writer::isDigits(Ref<String> value, int start, int length)
{
    int end = start + length;
    int last = value->length();
    for (int i = start; i < end && i < last; i++) {
        char c = value->charAt(i);
        if (c < '0' || c > '9')
        {
            if (c != ESCAPE_FNC_1)
            {
                return false;
            }
            end++;  // ignore FNC_1
        }
    }
    return end <= last;  // end > last if we've run out of string
}
