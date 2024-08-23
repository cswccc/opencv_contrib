#ifndef __DECODER_RESULT_H__
#define __DECODER_RESULT_H__

/*
 *  DecoderResult.hpp
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

#include <zxing/common/counted.hpp>
#include <zxing/common/array.hpp>
#include <string>
#include <zxing/common/str.hpp>

#include <zxing/qrcode/decoder/qrcode_decoder_meta_data.hpp>

namespace zxing {

class DecoderResult : public Counted {
private:
    ArrayRef<char> rawBytes_;
    Ref<String> text_;
    ArrayRef< ArrayRef<char> > byteSegments_;
    std::string ecLevel_;
    std::string outputCharset_;
    
    int structuredAppendSequenceNumber_;
    int structuredAppendParity_;
    int symbologyModifier_;
    
    int qrcodeVersion_;
    std::string charsetMode_;
    
    Ref<qrcode::QRCodeDecoderMetaData> other_;
    std::string otherClassName;

public:
    DecoderResult(ArrayRef<char> rawBytes,
                  Ref<String> text,
                  ArrayRef< ArrayRef<char> >& byteSegments,
                  std::string const& ecLevel);
    
    DecoderResult(ArrayRef<char> rawBytes,
                  Ref<String> text,
                  ArrayRef< ArrayRef<char> >& byteSegments,
                  std::string const& ecLevel,
                  std::string outputCharset);
    
    DecoderResult(ArrayRef<char> rawBytes,
                  Ref<String> text,
                  ArrayRef< ArrayRef<char> >& byteSegments,
                  std::string const& ecLevel,
                  std::string outputCharset,
                  int qrcodeVersion ,
                  std::string & charsetMode);
    
    DecoderResult(ArrayRef<char> rawBytes,
                  Ref<String> text,
                  ArrayRef< ArrayRef<char> >& byteSegments,
                  std::string const& ecLevel,
                  std::string outputCharset,
                  int saSequence,
                  int saParity,
                  int symbologyModifier,
                  int qrcodeVersion ,
                  std::string & charsetMode);
    
    DecoderResult(ArrayRef<char> rawBytes, Ref<String> text);
    
    DecoderResult(ArrayRef<char> rawBytes, Ref<String> text, std::string outputCharset);
    
    ArrayRef<char> getRawBytes();
    Ref<String> getText();
    std::string getCharset();
    
    void setOther(Ref<qrcode::QRCodeDecoderMetaData> other)
    {
        other_ = other;
        otherClassName = "QRCodeDecoderMetaData";
    };
    
    Ref<qrcode::QRCodeDecoderMetaData> getOther()
    {
        return other_;
    };
    
    std::string getOtherClassName(){return otherClassName;};
    
    // chango add 20160323
    int getQRCodeVersion() const { return qrcodeVersion_; };
    
    void setText(Ref<String> text) { text_ = text; };
    
    std::string getEcLevel() { return ecLevel_; }
    
    std::string getCharsetMode() { return charsetMode_;  }
    
#ifdef CALC_CODE_AREA_SCORE
    bool getMixMode() { return mixMode_; }
    void setMixMode(bool mixMode) { mixMode_ = mixMode; }
#endif
};

}  // namespace zxing

#endif  // __DECODER_RESULT_H__
