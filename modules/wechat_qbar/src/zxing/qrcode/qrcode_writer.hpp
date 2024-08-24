#ifndef __QR_CODE_WRITER_H__
#define __QR_CODE_WRITER_H__

/*
 *  QRCodeWriter.hpp
 *  zxing
 *
 *  Copyright 2013 ZXing authors All rights reserved.
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

#include <zxing/qrcode/version.hpp>
#include <zxing/qrcode/error_correction_level.hpp>
#include <zxing/qrcode/decoder/mode.hpp>
#include <zxing/common/character_set_eci.hpp>
#include <zxing/qrcode/encoder/int_matrix.hpp>
#include <zxing/qrcode/encoder/bit_array_writer.hpp>
#include <zxing/error_handler.hpp>
#include <string>

#include <qbarstruct.hpp>
#include "opencv2/imgproc.hpp"

using namespace cv::QBarAI;

namespace zxing {
namespace qrcode {

class QRCodeWriter {
public:
    static const int NUM_MASK_PATTERNS = 8;
    
    QRCodeWriter();
    static QRCodeWriter encode(const std::string& content,
                               ErrorCorrectionLevel& ecLevel,
                               int forceVersion, bool flag,
                               const std::string& encoding = "ISO-8859-1");
    
#ifdef CALC_CODE_AREA_SCORE
    static QRCodeWriter encode(const std::string& content,
                               ErrorCorrectionLevel& ecLevel,
                               int forceVersion,
                               const std::string& encoding,
                               int maskType, ArrayRef<char> bytes);
#endif
    
    static QRCodeWriter encodeMicro(const std::string& content,
                                    ErrorCorrectionLevel& ecLevel,
                                    bool flag,
                                    const std::string& encoding = "ISO-8859-1");
    
    
    Mode* getMode();
    ErrorCorrectionLevel getECLevel();
    Version* getVersion();
    int getMaskPattern();
    IntMatrix getMatrix();
    void setMode(Mode* value);
    void setECLevel(ErrorCorrectionLevel& value);
    void setVersion(Version* version);
    void setMaskPattern(int value);
    void setMatrix(IntMatrix& value);
    static bool isValidMaskPattern(int maskPattern);
    
    // For visualize
    static void QBarQRCode(const QBAR_IMAGE& code, const QBarDrawParam& param, cv::Mat& img);
    
    // For Personal QRCode
    static void GetDominantColors(cv::Mat& image, std::vector<std::vector<int>>& domi_colors);
    static void DrawPersonalQRCode(cv::Mat& eye_image, cv::Mat& module_image, cv::Mat& logo_image,
                                   const QBAR_IMAGE& code, int box_size, cv::Mat& img_rgba, bool is_white, bool is_group);
    static void DrawRoundQRCode(cv::Mat& eye_image, cv::Mat& module_image, const QBAR_IMAGE& code, int box_size,
                                cv::Mat& img_rgba);
    static void ApplyColorMask(cv::Mat& img_rgba, std::vector<std::vector<int>>& colors);
    
private:
    Mode* mode_;
    ErrorCorrectionLevel ecLevel_;
    Version* version_;
    int maskPattern_;
    IntMatrix matrix_;
};

}  // namespace qrcode
}  // namespace zxing

#endif  // __QR_CODE_READER_H__

