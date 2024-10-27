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
    
    int init(QBAR_MODE mode);
    void setReaders(const std::unordered_set<QBAR_READER> &readers);
    void setDetectorReferenceSize(int reference_size);
    void setDetectorScoreThres(float score_thres);
    void setDetectorIouThres(float iou_thres);

    bool detect(const Mat& img, OutputArrayOfArrays points);
    std::vector<std::string> decode(Mat img, InputArrayOfArrays detect_bboxes, OutputArrayOfArrays points);
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
    p->setReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    p->init(mode);
}

void QBar::setDetectorReferenceSize(int reference_size) {
    p->setDetectorReferenceSize(reference_size);
}

void QBar::setDetectorIouThres(float iou_thres) {
    p->setDetectorIouThres(iou_thres);
}

void QBar::setDetectorScoreThres(float score_thres) {
    p->setDetectorScoreThres(score_thres);
}

bool QBar::detect(InputArray img, OutputArrayOfArrays points) {
    Mat input_img;

    if (!checkQRInputImage(img, input_img))
        return false;

    return p->detect(input_img, points);
}

std::vector<std::string> QBar::decode(InputArray img, InputArrayOfArrays detect_points, OutputArrayOfArrays points) {
    Mat input_img;
    if (!checkQRInputImage(img, input_img))
        return std::vector<std::string>();

    return p->decode(input_img, detect_points, points);
}

std::vector<std::string> QBar::detectAndDecode(InputArray img, OutputArrayOfArrays points) {
    Mat input_img;

    if (!checkQRInputImage(img, input_img))
        return std::vector<std::string>();

    vector<Mat> detect_points;
    p->detect(input_img, detect_points);

    return p->decode(input_img, detect_points, points);
}

int QBar::Impl::init(QBAR_MODE mode)
{
    int ret = qbarDecode_->InitAIModel(mode.qbar_ml_mode);
    return ret;
}

void QBar::Impl::setReaders(const std::unordered_set<QBAR_READER> &readers)
{
    qbarDecode_->SetReaders(readers);
}

void QBar::Impl::setDetectorReferenceSize(int reference_size)
{
    qbarDecode_->setDetectorReferenceSize(reference_size);
}

void QBar::Impl::setDetectorScoreThres(float score_thres)
{
    qbarDecode_->setDetectorScoreThres(score_thres);
}

void QBar::Impl::setDetectorIouThres(float iou_thres)
{
    qbarDecode_->setDetectorIouThres(iou_thres);
}

bool QBar::Impl::detect(const Mat& img, OutputArrayOfArrays points) {
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

std::vector<std::string> QBar::Impl::decode(Mat img, InputArrayOfArrays detect_bboxes, OutputArrayOfArrays points)
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
}  // namespace QBarAI
}  // namespace cv