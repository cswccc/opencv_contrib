#ifndef QBARDECODER_H
#define QBARDECODER_H

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <memory>
#include <unordered_set>

// zxing
#include <zxing/ZXing.hpp>
#include <zxing/common/Counted.hpp>
#include <zxing/Binarizer.hpp>
#include <zxing/MultiFormatReader.hpp>
#include <zxing/Result.hpp>
#include <zxing/common/GlobalHistogramBinarizer.hpp>
#include <zxing/common/HybridBinarizer.hpp>
#include <zxing/common/FastWindowBinarizer.hpp>
#include <zxing/common/SimpleAdaptiveBinarizer.hpp>
#include <zxing/common/ImageCut.hpp>
#include <zxing/common/UnicomBlock.hpp>
#include <zxing/qrcode/QRCodeReader.hpp>
#include <zxing/BinaryBitmap.hpp>
#include <zxing/DecodeHints.hpp>
#include <zxing/common/StringUtils.hpp>
#include <zxing/common/util/INIReader.hpp>

// qbar
#include <QBarSource.hpp>
#include <BinarizerMgr.hpp>
#include <QBarStruct.hpp>

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
