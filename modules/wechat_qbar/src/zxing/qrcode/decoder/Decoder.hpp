#ifndef __DECODER_H__
#define __DECODER_H__

/*
 *  Decoder.hpp
 *  zxing
 *
 *  Copyright 2010 ZXing authors All rights reserved.
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

#include <zxing/common/reedsolomon/ReedSolomonDecoder.hpp>
#include <zxing/common/Counted.hpp>
#include <zxing/common/Array.hpp>
#include <zxing/common/DecoderResult.hpp>
#include <zxing/common/BitMatrix.hpp>
#include <zxing/qrcode/decoder/BitMatrixParser.hpp>
#include <zxing/common/DetectorResult.hpp>
#include <zxing/qrcode/Version.hpp>
#include <zxing/ErrorHandler.hpp>

namespace zxing {
namespace qrcode {

class Decoder {
public:
    enum  DecoderState{ 
        NOTSTART = 19,
        START = 20, 
        READVERSION = 21,
        READERRORCORRECTIONLEVEL = 22,
        READCODEWORDSORRECTIONLEVEL = 23,
        FINISH = 24
    };

private:
    DecoderState decoderState_;
    float possibleFix_;
    ReedSolomonDecoder rsDecoder_;
    void correctErrors(ArrayRef<char> bytes, int numDataCodewords, ErrorHandler & err_handler);
    
public:
    Decoder();
    Ref<DecoderResult> decode(Ref<BitMatrix> bits, ErrorHandler & err_handler);
    
private:
    Ref<DecoderResult> decode(Ref<BitMatrix> bits, bool isMirror, ErrorHandler & err_handler);
    
    float estimateFixedPattern(Ref<BitMatrix> bits, Version * version, ErrorHandler & err_handler);
    
private:
    unsigned int possibleVersion_;

public:
    unsigned int getPossibleVersion();
    DecoderState getState(){return decoderState_;}
    float getPossibleFix() { return possibleFix_;  }
};

}  // namespace qrcode
}  // namespace zxing

#endif  // __DECODER_H__
