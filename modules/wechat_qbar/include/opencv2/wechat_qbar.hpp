#ifndef OPENCV_WECHAT_QBAR_H
#define OPENCV_WECHAT_QBAR_H

#include <opencv2/core.hpp>

#include <unordered_set>

#ifdef _WIN32
#define __attribute__()
#define __attribute__(x)
#endif

#ifdef _HAS_STD_BYTE
#undef _HAS_STD_BYTE
#endif
#define _HAS_STD_BYTE 0

namespace cv {
namespace QBarAI {
enum DECODER_READER{
    DECODER_ONED_BARCODE = 1,// barcode, which includes UPC_A, UPC_E, EAN_8, EAN_13, CODE_39, CODE_93, CODE_128, ITF, CODABAR
    DECODER_QRCODE = 2,// QRCODE
    DECODER_PDF417 = 3,// PDF417
    DECODER_DATAMATRIX = 4,// DATAMATRIX
};

class CV_EXPORTS_W QBar
{
public:
    CV_WRAP QBar(const std::string& detection_model_path_ = "",
                    const std::string& super_resolution_model_path_ = "",
                    const std::vector<DECODER_READER>& readers = std::vector<DECODER_READER>(),
                    const float detector_iou_thres = 0.6,
                    const float decoder_iou_thres = 0.5,
                    const float score_thres = 0.3,
                    const int reference_size = 480);

    ~QBar() {};
    
    CV_WRAP void setDetectorReferenceSize(int reference_size);
    CV_WRAP void setDetectorScoreThres(float score_thres);
    CV_WRAP void setDetectorIouThres(float iou_thres);
    CV_WRAP void setDecoderIouThres(float iou_thres);

    CV_WRAP bool detect(InputArray img, OutputArrayOfArrays points);
    CV_WRAP std::vector<std::string> decode(InputArray img, InputArrayOfArrays detect_points, OutputArrayOfArrays points);
    CV_WRAP std::vector<std::string> detectAndDecode(InputArray img, OutputArrayOfArrays points);
    
protected:
    class Impl;
    Ptr<Impl> p;
};

typedef std::vector<DECODER_READER> vector_DECODER_READER;
}  // namespace QBarAI
}  // namespace cv
#endif
