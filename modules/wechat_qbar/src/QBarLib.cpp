#include <opencv2/QbarLib.h>
#include <QBarDecoder.h>
#include <QBarEncoder.h>
#include <QBarImportOpenCV.h>
#include "opencv2/opencv.hpp"

namespace cv {
namespace QBarAI {
QBar::QBar()
{
    qbarDecode_ = new QBarDecoder();
}

QBar::~QBar()
{
    delete qbarDecode_;
}

int QBar::Init(QBAR_MODE mode)
{
    int ret = qbarDecode_->InitAIModel(mode.qbar_ml_mode);
    return ret;
}

void QBar::SetReaders(const std::unordered_set<QBAR_READER> &readers)
{
    qbarDecode_->SetReaders(readers);
}

QBAR_RESULT QBar::Decode(uint8_t* imageData, int width, int height)
{
    cv::Mat image(height, width, CV_8UC1, imageData);
    return qbarDecode_->Decode(image);
}

std::vector<QBAR_RESULT> QBar::ScanImage(uint8_t* imageData, int width, int height)
{
    cv::Mat image(height, width, CV_8UC1, imageData);
    return qbarDecode_->ScanImage(image);
}



std::string QBar::GetVersion()
{
    return QBAR_VERSION;
}

QBarGen::QBarGen()
{
}

QBarGen::~QBarGen()
{
}


int QBarGen::Encode(QBAR_IMAGE& code, const std::string & content, const QBAR_ENCODE_CONFIG & config)
{
    return QBarEncoder::Encode(code, content, config);
}

int QBarGen::EncodeQRCodeImage(const std::string &content, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
    QBAR_IMAGE code;
    int ret = QBarEncoder::Encode(code, content, config);
    if (ret != 0) return ret;
    QBarEncoder::MakeQRCodeImage(code, config.draw_param_, buf);
    return 0;
}

int QBarGen::EncodeRoundQRCodeImage(const std::string &content, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
    QBAR_IMAGE code;
    int ret = QBarEncoder::Encode(code, content, config);
    if (ret != 0) return ret;
    ret = MakeRoundQRCode(code, config, buf);
    return ret;
}

int QBarGen::EncodePersonalQRCodeImage(const std::string& content, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
    QBAR_IMAGE code;
    int ret = QBarEncoder::Encode(code, content, config);
    if (ret != 0) return ret;
    if (config.personal_param_.mode_ == MODE_PERSONAL_COLOR)
    {
        ret = MakePersonColorQRCode(code, config, buf);
    }
    else
    {
        ret = MakePersonQRCode(code, config, buf);
    }
    return ret;
}

void QBarGen::MakeQRCodeImage(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
    return QBarEncoder::MakeQRCodeImage(code, config.draw_param_, buf);
}

int QBarGen::MakeRoundQRCode(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
    cv::Mat cvEyeImage, cvModuleImage;
    
    if (config.personal_param_.eye_image_.image_format_ != PIX_FMT_RGBA8888 ||
        config.personal_param_.module_image_.image_format_ != PIX_FMT_RGBA8888)
        return -1;
    
#ifdef USE_IMREAD
    if (config.personal_param_.mode_ == MODE_PERSONAL_WHITE)
    {
        cvEyeImage = cv::imread(config.personal_param_.eye_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvEyeImage.data == nullptr) return -1;
        cvModuleImage = cv::imread(config.personal_param_.module_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvModuleImage.data == nullptr) return -1;
    }
    else if (config.personal_param_.mode_  == MODE_PERSONAL_BLACK)
    {
        cvEyeImage = cv::imread(config.personal_param_.black_eye_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvEyeImage.data == nullptr) return -1;
        cvModuleImage = cv::imread(config.personal_param_.black_module_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvModuleImage.data == nullptr) return -1;
    }
    else return -1;
#else
    if (config.personal_param_.mode_ == MODE_PERSONAL_WHITE)
    {
        cv::Mat cvEyeImage(config.personal_param_.eye_image_.height_,
                           config.personal_param_.eye_image_.width_, CV_8UC4,
                           config.personal_param_.eye_image_.image_data_);
        if (cvEyeImage.data == nullptr) return -1;
        
        cv::Mat cvModuleImage(config.personal_param_.module_image_.height_,
                              config.personal_param_.module_image_.width_, CV_8UC4,
                              config.personal_param_.module_image_.image_data_);
        if (cvModuleImage.data == nullptr) return -1;
    }
    else if (config.personal_param_.mode_  == MODE_PERSONAL_BLACK)
    {
        cv::Mat cvEyeImage(config.personal_param_.black_eye_image_.height_,
                           config.personal_param_.black_eye_image_.width_, CV_8UC4,
                           config.personal_param_.black_eye_image_.image_data_);
        if (cvEyeImage.data == nullptr) return -1;
        cv::Mat cvModuleImage(config.personal_param_.black_module_image_.height_,
                              config.personal_param_.black_module_image_.width_, CV_8UC4,
                              config.personal_param_.black_module_image_.image_data_);
        if (cvModuleImage.data == nullptr) return -1;
    }
    else return -1;
    
#endif
    QBarEncoder::MakeRoundQRCodeImage(code, cvEyeImage, cvModuleImage, config.draw_param_.module_size_, buf);
    
    return 0;
}

int QBarGen::MakePersonQRCode(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
#ifdef USE_IMREAD
    if (config.personal_param_.mode_ == MODE_PERSONAL_WHITE)
    {
        cv::Mat cvEyeImage = cv::imread(config.personal_param_.eye_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvEyeImage.data == nullptr) return -1;
        cv::Mat cvModuleImage = cv::imread(config.personal_param_.module_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvModuleImage.data == nullptr) return -1;
        cv::Mat cvLogoImage = cv::imread(config.personal_param_.logo_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvLogoImage.data == nullptr) return -1;
        
        QBarEncoder::MakePersonalQRCodeImage(code, cvEyeImage, cvModuleImage, cvLogoImage, config.draw_param_.module_size_, buf, true);
    }
    else if (config.personal_param_.mode_  == MODE_PERSONAL_BLACK)
    {
        cv::Mat cvEyeImage = cv::imread(config.personal_param_.black_eye_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvEyeImage.data == nullptr) return -1;
        cv::Mat cvModuleImage = cv::imread(config.personal_param_.black_module_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvModuleImage.data == nullptr) return -1;
        cv::Mat cvLogoImage = cv::imread(config.personal_param_.black_logo_path_, CV_LOAD_IMAGE_UNCHANGED);
        if (cvLogoImage.data == nullptr) return -1;
        
        QBarEncoder::MakePersonalQRCodeImage(code, cvEyeImage, cvModuleImage, cvLogoImage, config.draw_param_.module_size_, buf, false, config.personal_param_.is_group_qrcode_);
    }
    else return -1;
#else
    if (config.personal_param_.mode_ == MODE_PERSONAL_WHITE)
    {
        if (config.personal_param_.eye_image_.image_format_ != PIX_FMT_RGBA8888 ||
            config.personal_param_.module_image_.image_format_ != PIX_FMT_RGBA8888 ||
            config.personal_param_.logo_image_.image_format_ != PIX_FMT_RGBA8888)
            return -1;
        
        cv::Mat cvEyeImage(config.personal_param_.eye_image_.height_,
                           config.personal_param_.eye_image_.width_, CV_8UC4,
                           config.personal_param_.eye_image_.image_data_);
        if (cvEyeImage.data == nullptr) return -1;
        
        cv::Mat cvModuleImage(config.personal_param_.module_image_.height_,
                              config.personal_param_.module_image_.width_, CV_8UC4,
                              config.personal_param_.module_image_.image_data_);
        if (cvModuleImage.data == nullptr) return -1;
        
        cv::Mat cvLogoImage(config.personal_param_.logo_image_.height_,
                            config.personal_param_.logo_image_.width_, CV_8UC4,
                            config.personal_param_.logo_image_.image_data_);
        if (cvLogoImage.data == nullptr) return -1;
        
        QBarEncoder::MakePersonalQRCodeImage(code, cvEyeImage, cvModuleImage, cvLogoImage, config.draw_param_.module_size_, buf, true, config.personal_param_.is_group_qrcode_);
    }
    else if (config.personal_param_.mode_  == MODE_PERSONAL_BLACK)
    {
        if (config.personal_param_.black_eye_image_.image_format_ != PIX_FMT_RGBA8888 ||
            config.personal_param_.black_module_image_.image_format_ != PIX_FMT_RGBA8888 ||
            config.personal_param_.black_logo_image_.image_format_ != PIX_FMT_RGBA8888)
            return -1;
        
        cv::Mat cvEyeImage(config.personal_param_.black_eye_image_.height_,
                           config.personal_param_.black_eye_image_.width_, CV_8UC4,
                           config.personal_param_.black_eye_image_.image_data_);
        if (cvEyeImage.data == nullptr) return -1;
        
        cv::Mat cvModuleImage(config.personal_param_.black_module_image_.height_,
                              config.personal_param_.black_module_image_.width_, CV_8UC4,
                              config.personal_param_.black_module_image_.image_data_);
        if (cvModuleImage.data == nullptr) return -1;
        
        cv::Mat cvLogoImage(config.personal_param_.black_logo_image_.height_,
                            config.personal_param_.black_logo_image_.width_, CV_8UC4,
                            config.personal_param_.black_logo_image_.image_data_);
        if (cvLogoImage.data == nullptr) return -1;
        
        QBarEncoder::MakePersonalQRCodeImage(code, cvEyeImage, cvModuleImage, cvLogoImage, config.draw_param_.module_size_, buf, false, config.personal_param_.is_group_qrcode_);
    }
    else return -1;
#endif
    
    return 0;
}

int QBarGen::MakePersonColorQRCode(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf)
{
#ifdef USE_IMREAD
    cv::Mat cvEyeImage = cv::imread(config.personal_param_.black_eye_path_, CV_LOAD_IMAGE_UNCHANGED);
    if (cvEyeImage.data == nullptr) return -1;
    cv::Mat cvModuleImage = cv::imread(config.personal_param_.black_module_path_, CV_LOAD_IMAGE_UNCHANGED);
    if (cvModuleImage.data == nullptr) return -1;
    cv::Mat cvLogoImage = cv::imread(config.personal_param_.black_logo_path_, CV_LOAD_IMAGE_UNCHANGED);
    if (cvLogoImage.data == nullptr) return -1;
#else
    if (config.personal_param_.black_eye_image_.image_format_ != PIX_FMT_RGBA8888 ||
        config.personal_param_.black_module_image_.image_format_ != PIX_FMT_RGBA8888 ||
        config.personal_param_.black_logo_image_.image_format_ != PIX_FMT_RGBA8888)
        return -1;
    
    cv::Mat cvEyeImage(config.personal_param_.black_eye_image_.height_,
                       config.personal_param_.black_eye_image_.width_, CV_8UC4,
                       config.personal_param_.black_eye_image_.image_data_);
    if (cvEyeImage.data == nullptr) return -1;
    
    cv::Mat cvModuleImage(config.personal_param_.black_module_image_.height_,
                          config.personal_param_.black_module_image_.width_, CV_8UC4,
                          config.personal_param_.black_module_image_.image_data_);
    if (cvModuleImage.data == nullptr) return -1;
    
    cv::Mat cvLogoImage(config.personal_param_.black_logo_image_.height_,
                        config.personal_param_.black_logo_image_.width_, CV_8UC4,
                        config.personal_param_.black_logo_image_.image_data_);
    if (cvLogoImage.data == nullptr) return -1;
#endif
    
    int type = CV_8UC3;
    if (config.personal_param_.head_image_.image_format_ == PIX_FMT_BGRA8888 || config.personal_param_.head_image_.image_format_ == PIX_FMT_RGBA8888) type = CV_8UC4;
    else if (config.personal_param_.head_image_.image_format_ == PIX_FMT_GRAY) type = CV_8UC1;
    
    cv::Mat cvHeadImage(config.personal_param_.head_image_.height_, config.personal_param_.head_image_.width_, type, config.personal_param_.head_image_.image_data_);
    
    if (config.personal_param_.head_image_.image_format_ == PIX_FMT_BGR888) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_BGR2RGB);
    else if (config.personal_param_.head_image_.image_format_ == PIX_FMT_RGBA8888) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_RGBA2RGB);
    else if (config.personal_param_.head_image_.image_format_ == PIX_FMT_BGRA8888) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_BGRA2RGB);
    else if (config.personal_param_.head_image_.image_format_ == PIX_FMT_GRAY) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_GRAY2RGB);
    
    QBarEncoder::MakePersonalColorQRCodeImage(code, cvEyeImage, cvModuleImage, cvLogoImage, cvHeadImage, config.draw_param_.module_size_, buf);
    
    return 0;
}

void QBarGen::GetDominantColors(QBarImageInfo& head_image,
                                std::vector<std::vector<int>>& domi_colors)
{
    if (head_image.image_data_ == nullptr || head_image.width_ <= 0 || head_image.height_ <= 0)
    {
        domi_colors.clear();
        domi_colors.resize(2);
        domi_colors[0] = {100, 100, 100};
        domi_colors[1] = {23, 23, 23};
        
        return;
    }
    int type = CV_8UC3;
    if (head_image.image_format_ == PIX_FMT_BGRA8888 || head_image.image_format_ == PIX_FMT_RGBA8888) type = CV_8UC4;
    else if (head_image.image_format_ == PIX_FMT_GRAY) type = CV_8UC1;
    cv::Mat cvHeadImage(head_image.height_, head_image.width_, type, head_image.image_data_);
    if (head_image.image_format_ == PIX_FMT_BGR888) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_BGR2RGB);
    else if (head_image.image_format_ == PIX_FMT_RGBA8888) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_RGBA2RGB);
    else if (head_image.image_format_ == PIX_FMT_BGRA8888) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_BGRA2RGB);
    else if (head_image.image_format_ == PIX_FMT_GRAY) cv::cvtColor(cvHeadImage, cvHeadImage, _CV_GRAY2RGB);
    
    QBarEncoder::GetDominantColors(cvHeadImage, domi_colors);
}
}
}