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

    bool Detect(const Mat& img, OutputArrayOfArrays points);
    std::vector<std::string> Decode(Mat img, InputArrayOfArrays detect_bboxes, OutputArrayOfArrays points);
    std::string GetVersion();
    std::shared_ptr<QBarDecoder> qbarDecode_;
};

static bool checkQRInputImage(InputArray img, Mat& gray) {
    CV_Assert(!img.empty());
    CV_CheckDepthEQ(img.depth(), CV_8U, "");

    int incn = img.channels();
    CV_Check(incn, incn == 1 || incn == 3 || incn == 4, "");
    if (incn == 3 || incn == 4) {
        cvtColor(img, gray, COLOR_BGR2GRAY);
    } else {
        gray = img.getMat();
    }

    return true;
}

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

bool QBar::detect(InputArray img, OutputArrayOfArrays points) {
    Mat input_img;

    if (!checkQRInputImage(img, input_img))
        return false;

    return p->Detect(input_img, points);
}

std::vector<std::string> QBar::decode(InputArray img, InputArrayOfArrays detect_points, OutputArrayOfArrays points) {
    Mat input_img;
    if (!checkQRInputImage(img, input_img))
        return std::vector<std::string>();

    return p->Decode(input_img, detect_points, points);
}

std::vector<std::string> QBar::detectAndDecode(InputArray img, OutputArrayOfArrays points) {
    Mat input_img;

    if (!checkQRInputImage(img, input_img))
        return std::vector<std::string>();

    vector<Mat> detect_points;
    p->Detect(input_img, detect_points);

    return p->Decode(input_img, detect_points, points);
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

bool QBar::Impl::Detect(const Mat& img, OutputArrayOfArrays points) {
    std::vector<DetectInfo> _detect_results;

    qbarDecode_->Detect(img, _detect_results);

    vector<Mat> tmp_points;
    if (points.needed()) {
        for (size_t i = 0; i < _detect_results.size(); i++) {
            auto point_to_save = Mat(4, 2, CV_32FC1);
            point_to_save.at<float>(0, 0) = _detect_results[i].x;
            point_to_save.at<float>(0, 1) = _detect_results[i].y;
            point_to_save.at<float>(1, 0) = _detect_results[i].x + _detect_results[i].width;
            point_to_save.at<float>(1, 1) = _detect_results[i].y;
            point_to_save.at<float>(2, 0) = _detect_results[i].x;
            point_to_save.at<float>(2, 1) = _detect_results[i].y + _detect_results[i].height;
            point_to_save.at<float>(3, 0) = _detect_results[i].x + _detect_results[i].width;
            point_to_save.at<float>(3, 1) = _detect_results[i].y + _detect_results[i].height;

            Mat tmp_point;
            tmp_points.push_back(tmp_point);
            point_to_save.convertTo(((OutputArray)tmp_points[i]), CV_32FC2);
        }

        points.createSameSize(tmp_points, CV_32FC2);
        points.assign(tmp_points);
    }

    return _detect_results.size() != 0;
}

std::vector<std::string> QBar::Impl::Decode(Mat img, InputArrayOfArrays detect_bboxes, OutputArrayOfArrays points)
{
    std::vector<DetectInfo> bboxes;

    std::vector<cv::Mat> points_vec;
    detect_bboxes.getMatVector(points_vec);
    for (size_t i = 0; i < points_vec.size(); i++) {
        DetectInfo bbox;
        cv::Mat point_mat = points_vec[i];

        bbox.x = point_mat.at<cv::Vec2f>(0)[0];
        bbox.y = point_mat.at<cv::Vec2f>(0)[1];
        bbox.width = point_mat.at<cv::Vec2f>(3)[0] - bbox.x;
        bbox.height = point_mat.at<cv::Vec2f>(3)[1] - bbox.y;

        bboxes.push_back(bbox);
    }
    if (bboxes.size() == 0) {
        DetectInfo bbox;
        bbox.x = 0;
        bbox.y = 0;
        bbox.width = img.cols;
        bbox.height = img.rows;

        bboxes.push_back(bbox);
    }

    std::vector<QBAR_RESULT> results;
    results = qbarDecode_->Decode(img, bboxes);

    std::vector<std::string> ret;
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

std::string QBar::Impl::GetVersion()
{
    return QBAR_VERSION;
}
}  // namespace QBarAI
}  // namespace cv