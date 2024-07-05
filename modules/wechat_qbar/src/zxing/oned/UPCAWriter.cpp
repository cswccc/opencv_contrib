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

#include <zxing/oned/UPCAWriter.h>

using namespace zxing;
using namespace oned;

ArrayRef<boolean> UPCAWriter::encode(Ref<String> contents)
{
    return subWriter.encode(preencode(contents));
}

/**
 * Transform a UPC-A code into the equivalent EAN-13 code, and add a check digit if it is not
 * already present.
 */
Ref<String> UPCAWriter::preencode(Ref<String> contents) {
    
    int length = contents->length();
    if (length == 11) {
        // No check digit present, calculate it and add it
        int sum = 0;
        for (int i = 0; i < 11; ++i) {
            sum += (contents->charAt(i) - '0') * (i % 2 == 0 ? 3 : 1);
        }
        contents->append((1000 - sum) % 10);
    }
    
    if (length != 13) {
        Ref<String> result(new String("0"));
        
        result->append(contents);
        return result;
    }
    else
    {
        return contents;
    }
}
