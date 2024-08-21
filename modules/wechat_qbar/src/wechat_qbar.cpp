#include <opencv2/wechat_qbar.hpp>
#include <QBarStruct.hpp>
#include <QBarDecoder.hpp>
#include <QBarEncoder.hpp>
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

std::vector<std::string> QBar::detectAndDecode(Mat img) {
    CV_Assert(!img.empty());
    CV_CheckDepthEQ(img.depth(), CV_8U, "");

    cout << "img1" << endl;

    cv::Mat grayscale_image;
    cv::cvtColor(img, grayscale_image, cv::COLOR_BGR2GRAY);

    cout << "img" << endl;

    std::vector<std::string> ret;

    cout << "ret" << endl;

    std::vector<QBAR_RESULT> results = p->ScanImage(grayscale_image);

    cout << 666 << endl;

    for (int i = 0; i < results.size(); i++) {
        std::cout << "Type: " << results[i].typeName << " Data: " << results[i].data;
        // std::string info = "Type: " + results[i].typeName + ' ' + "Data: " + results[i].data;
        // ret.push_back(info);
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