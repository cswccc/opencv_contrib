#include <opencv2/wechat_qbar.hpp>
#include <qbarstruct.hpp>
#include <qbardecoder.hpp>
#include <qbarencoder.hpp>
#include "opencv2/core.hpp"

namespace cv {
namespace QBarAI {
class QBar::Impl {
public:
    Impl() {};
    ~Impl() {};
    
    int Init(QBAR_MODE mode);
    void SetReaders(const std::unordered_set<QBAR_READER> &readers);
    QBAR_RESULT Decode(Mat &image);
    std::vector<QBAR_RESULT> ScanImage(Mat &image);
    std::string GetVersion();
    std::shared_ptr<QBarDecoder> qbarDecode_;
};

QBar::QBar(const std::string& detection_model_path_,
                const std::string& super_resolution_model_path_) {
    p = makePtr<QBar::Impl>();
    p->qbarDecode_ = make_shared<QBarDecoder>();

    QBAR_MODE mode;
    mode.useAI = true;
    mode.qbar_ml_mode.detection_model_path_ = detection_model_path_;
    mode.qbar_ml_mode.super_resolution_model_path_ = super_resolution_model_path_;
    p->SetReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    p->Init(mode);
}

std::vector<std::string> QBar::detectAndDecode(InputArray img, OutputArrayOfArrays points) {
    CV_Assert(!img.empty());
    CV_CheckDepthEQ(img.depth(), CV_8U, "");

    Mat input_img;
    int incn = img.channels();
    CV_Check(incn, incn == 1 || incn == 3 || incn == 4, "");
    if (incn == 3 || incn == 4) {
        cvtColor(img, input_img, COLOR_BGR2GRAY);
    } else {
        input_img = img.getMat();
    }

    std::vector<std::string> ret;

    std::vector<QBAR_RESULT> results = p->ScanImage(input_img);
    
    for (size_t i = 0; i < results.size(); i++) {
        ret.push_back(results[i].data);
    }

    vector<Mat> tmp_points;
    if (points.needed()) {
        for (size_t i = 0; i < results.size(); i++) {
            auto point_to_save = Mat(results[i].points.size(), 2, CV_32FC1);
            for (size_t j = 0; j < results[i].points.size(); j++) {
                point_to_save.at<float>(j, 0) = results[i].points[j].x;
                point_to_save.at<float>(j, 1) = results[i].points[j].y;
            }

            Mat tmp_point;
            tmp_points.push_back(tmp_point);
            point_to_save.convertTo(((OutputArray)tmp_points[i]), CV_32FC2);
        }

        points.createSameSize(tmp_points, CV_32FC2);
        points.assign(tmp_points);
    }

    return ret;
}

std::string QBar::GetVersion() {
    return p->GetVersion();
}

int QBar::Impl::Init(QBAR_MODE mode)
{
    int ret = qbarDecode_->InitAIModel(mode.qbar_ml_mode);
    return ret;
}

void QBar::Impl::SetReaders(const std::unordered_set<QBAR_READER> &readers)
{
    qbarDecode_->SetReaders(readers);
}

QBAR_RESULT QBar::Impl::Decode(Mat &image)
{
    // Mat image(height, width, CV_8UC1, imageData);
    return qbarDecode_->Decode(image);
}

std::vector<QBAR_RESULT> QBar::Impl::ScanImage(Mat &image)
{
    // Mat image(height, width, CV_8UC1, imageData);
    return qbarDecode_->ScanImage(image);
}

std::string QBar::Impl::GetVersion()
{
    return QBAR_VERSION;
}
}  // namespace QBarAI
}  // namespace cv