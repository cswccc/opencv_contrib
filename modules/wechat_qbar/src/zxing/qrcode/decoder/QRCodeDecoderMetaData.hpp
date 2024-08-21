#ifndef __DECODER_META_DATA_H__
#define __DECODER_META_DATA_H__

/*
 * Copyright 2013 ZXing authors
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

#include <zxing/common/Counted.hpp>
// #include <zxing/common/DecoderResult.hpp>
// #include <zxing/qrcode/decoder/Decoder.hpp>
// #include <zxing/qrcode/decoder/BitMatrixParser.hpp>
// #include <zxing/qrcode/ErrorCorrectionLevel.hpp>
// #include <zxing/qrcode/Version.hpp>
// #include <zxing/qrcode/decoder/DataBlock.hpp>
// #include <zxing/qrcode/decoder/DecodedBitStreamParser.hpp>
// #include <zxing/ReaderException.hpp>
// #include <zxing/ChecksumException.hpp>
// #include <zxing/common/reedsolomon/ReedSolomonException.hpp>
// #include <iostream>
#include <zxing/ResultPoint.hpp>

// using zxing::qrcode::Decoder;
// using zxing::DecoderResult;
// using zxing::Ref;
// using std::cout;
// using std::endl;

// VC++
// The main class which implements QR Code decoding -- as opposed to locating and extracting
// the QR Code from an image.
// using zxing::ArrayRef;
// using zxing::BitMatrix;

namespace zxing {
	namespace qrcode {

/**
 * Meta-data container for QR Code decoding. Instances of this class may be used to convey information back to the
 * decoding caller. Callers are expected to process this.
 * 
 * @see com.google.zxing.common.DecoderResult#getOther()
 */
class QRCodeDecoderMetaData : public Counted {
private:
	bool mirrored;
  
public:
  QRCodeDecoderMetaData(bool mirrored) {
		this->mirrored = mirrored;
  }

public:
  /** 
   * @return true if the QR Code was mirrored. 
   */
  bool isMirrored() {
    return mirrored;
  };

  /**
   * Apply the result points' order correction due to mirroring.
   * 
   * @param points Array of points to apply mirror correction to.
   */
  void applyMirroredCorrection(ArrayRef< Ref<ResultPoint> >& points) {
    if (!mirrored || points->size() < 3) {
      return;
    }
    Ref<ResultPoint> bottomLeft = points[0];
    points[0] = points[2];
    points[2] = bottomLeft;
    // No need to 'fix' top-left and alignment pattern.
  };

};

	}
}

#endif  // __DECODER_META_DATA_H__