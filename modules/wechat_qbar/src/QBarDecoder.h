#ifndef QBARDECODER_H
#define QBARDECODER_H

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <memory>
#include <unordered_set>

// zxing
#include <zxing/ZXing.h>
#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/common/FastWindowBinarizer.h>
#include <zxing/common/SimpleAdaptiveBinarizer.h>
#include <zxing/common/ImageCut.h>
#include <zxing/common/UnicomBlock.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>
#include <zxing/common/StringUtils.h>
#include <zxing/common/util/INIReader.h>

// qbar
#include <QBarSource.h>
#include <BinarizerMgr.h>
#include <opencv2/QBarStruct.h>

#include "QBarInfoFilter.hpp"

//ai
#include "detector/qbar_detector.hpp"
#include "detector/align.hpp"
#include "sr_scale/super_scale.hpp"

namespace cv {
namespace QBarAI {
class QBarDecoder
{
public:
    QBarDecoder() = default;
    ~QBarDecoder() = default;

    void SetReaders(const std::unordered_set<QBAR_READER> &readers) { readers_ = readers; }
    QBAR_RESULT Decode(Mat& srcImage);

    int InitAIModel(QBAR_ML_MODE &ml_mode);

    std::vector<QBAR_RESULT> ScanImage(Mat& srcImage);

private:
    int Decode(zxing::Ref<zxing::LuminanceSource> source, zxing::Ref<zxing::Result> &result, zxing::DecodeHints& decodeHints);
    QBAR_RESULT ProcessResult(zxing::Result *zx_result);
    void AddFormatsToDecodeHints(zxing::DecodeHints &hints);

    Mat cropObj(const Mat& img, const DetectInfo& bbox, Align& aligner);
    std::vector<float> getScaleList(const int width, const int height);

    zxing::MultiFormatReader reader_;
    BinarizerMgr binarizer_mgr_;
    std::unordered_set<QBAR_READER> readers_;
    std::string output_charset_ = "UTF-8";

    //AI Model
    bool _init_ai_model_ = false;
    std::shared_ptr<QBarDetector> detector_;
    std::shared_ptr<SuperScale> sr_;

};
}  // namespace QBarAI
}  // namespace cv
#endif
