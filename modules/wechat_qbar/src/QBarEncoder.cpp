#include <QBarEncoder.h>

#include <zxing/qrcode/QRCodeWriter.h>
#include <zxing/qrcode/ErrorCorrectionLevel.h>

using namespace zxing::qrcode;

#include <zxing/oned/OneDimensionalCodeWriter.h>
#include <zxing/oned/Code128Writer.h>
#include <zxing/oned/EAN13Writer.h>
#include <zxing/oned/EAN8Writer.h>
#include <zxing/oned/UPCAWriter.h>
#include <zxing/oned/Code39Writer.h>
#include <zxing/oned/ITFWriter.h>
#include <zxing/oned/CodaBarWriter.h>

using namespace zxing::oned;

namespace cv {
namespace QBarAI {
int QBarEncoder::Encode(QBAR_IMAGE& code, const std::string & content, const QBAR_ENCODE_CONFIG & config)
{
    QBAR_CODE_FORMAT format = config.format;
    
    if (format == FMT_QRCODE)
    {
        return EncodeQRCode(code, content, format, config);
    }
    else
    {
        return EncodeOneD(code, content, format, config);
    }
    return -1;
}

int QBarEncoder::EncodeQRCode(QBAR_IMAGE& code, const std::string content, QBAR_CODE_FORMAT format, QBAR_ENCODE_CONFIG config)
{
    if (format!= FMT_QRCODE)
    {
        std::cout<<"Only encoding QR_CODE now!"<<std::endl;
        return -1;
    }
    
    QBAR_QRCODE_ERROR_LEVEL ecLevel = config.ecLevel;
    int forceVersion = config.version;
    const std::string encoding = config.encoding;
    
    zxing::ErrorCorrectionLevel level=zxing::ErrorCorrectionLevel::L;
    
    switch (ecLevel){
        case L:
            level = zxing::ErrorCorrectionLevel::L;
            break;
        case M:
            level = zxing::ErrorCorrectionLevel::M;
            break;
        case H:
            level = zxing::ErrorCorrectionLevel::H;
            break;
        case Q:
            level = zxing::ErrorCorrectionLevel::Q;
            break;
    }
    // try
    {
        QRCodeWriter writer;
        bool flag = true;
        if (config.version == 0)
            writer = QRCodeWriter::encodeMicro(content, level, flag, encoding);
        else
            writer = QRCodeWriter::encode(content, level, forceVersion, flag, encoding);
        
        if (!flag) return -1;
        
        zxing::qrcode::IntMatrix matrix = writer.getMatrix();
        if (matrix.getHeight() < 1 || matrix.getWidth() < 1){
            std::cout<<"Fail to encode a QR_CODE !"<<std::endl;
            return -1;
        }
        int height = matrix.getHeight();
        int width = matrix.getWidth();
        
        code.width = width;
        code.height = height;
        code.data.clear();
        
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                code.data.push_back((unsigned char)matrix.get(j, i));
            }
        }
    }
    
    return 0;
}

int QBarEncoder::EncodeOneD(QBAR_IMAGE& code, const std::string content, QBAR_CODE_FORMAT format, QBAR_ENCODE_CONFIG config)
{
    try
    {
        Ref<OneDimensionalCodeWriter> writer;
        
        switch (format)
        {
            case FMT_CODE128:
                writer = new Code128Writer();
                break;
            case FMT_CODE39:
                writer = new Code39Writer();
                break;
            case FMT_CODABAR:
                writer = new CodaBarWriter();
                break;
            case FMT_EAN8:
                writer = new EAN8Writer();
                break;
            case FMT_EAN13:
                writer = new EAN13Writer();
                break;
            case FMT_ITF:
                writer = new ITFWriter();
                break;
            case FMT_UPCA:
                writer = new UPCAWriter();
                break;
                
            default:
                std::cout<<"Format not supported yet!"<<std::endl;
                return -1;
                break;
        }
        
        ArrayRef<boolean> codeVec = writer->encode(Ref<zxing::String>(new zxing::String(content)));
        
        if (codeVec->size()<1){
            std::cout<<"Fail to encode a barcode !"<<std::endl;
            return -1;
        }
        int height = 1;
        int width = codeVec->size();
        
        code.width = width;
        code.height = height;
        code.data.clear();
        code.data.resize(codeVec->size());
        
        for (int j =0; j < codeVec->size(); j++)
        {
            code.data[j] = (uint8_t)codeVec[j];
        }
    }
    catch(Exception &re){
        std::cout << "Fail to encode a barcode (" << re.what() << ")!" << std::endl;
        return -1;
    }
    return 0;
}

// ================= QRCODE VISUALIZE ========== //
void QBarEncoder::MakeQRCodeImage(const QBAR_IMAGE& code, const QBarDrawParam& param, QBarImageInfo& buf)
{
    cv::Mat img;
    QRCodeWriter::QBarQRCode(code, param, img);
    if (param.image_mode_ == IMAGE_COLOR)
    {
        // cv::imencode(".jpg", img, buf);
        buf.image_format_ = PIX_FMT_RGB888;
        buf.width_ = img.cols;
        buf.height_ = img.rows;
        buf.image_data_ = new uint8_t[buf.width_ * buf.height_ * 3];
        memcpy(buf.image_data_, img.data, buf.width_ * buf.height_ * 3);
    }
    else
    {
        // cv::imencode(".png", img, buf);
        buf.image_format_ = PIX_FMT_RGBA8888;
        buf.width_ = img.cols;
        buf.height_ = img.rows;
        buf.image_data_ = new uint8_t[buf.width_ * buf.height_ * 4];
        memcpy(buf.image_data_, img.data, buf.width_ * buf.height_ * 4);
    }
}

void QBarEncoder::GetDominantColors(cv::Mat& image,
                                   std::vector<std::vector<int>>& domi_colors)
{
    QRCodeWriter::GetDominantColors(image, domi_colors);
}

int QBarEncoder::MakeRoundQRCodeImage(const QBAR_IMAGE& code,
                                     cv::Mat& eye_image,
                                     cv::Mat& module_image,
                                     int box_size,
                                     QBarImageInfo& buf)
{
    if (eye_image.data == nullptr || module_image.data == nullptr)
        return -1;
    
    int width = code.width;
    int height = code.height;
    
    int image_width = width * box_size;
    int image_height = height * box_size;
    
    cv::Mat img_rgba(image_height, image_width, CV_8UC4, cv::Scalar(255, 255, 255, 0));
    
    QRCodeWriter::DrawRoundQRCode(eye_image, module_image, code, box_size, img_rgba);
    
    buf.image_format_ = PIX_FMT_RGBA8888;
    buf.width_ = img_rgba.cols;
    buf.height_ = img_rgba.rows;
    buf.image_data_ = new uint8_t[buf.width_ * buf.height_ * 4];
    memcpy(buf.image_data_, img_rgba.data, buf.width_ * buf.height_ * 4);
    // cv::imencode(".png", img_rgba, buf);
    
    return 0;
}

int QBarEncoder::MakePersonalQRCodeImage(const QBAR_IMAGE& code,
                                        cv::Mat& eye_image,
                                        cv::Mat& module_image,
                                        cv::Mat& logo_image,
                                        int box_size,
                                        QBarImageInfo& buf,
                                        bool is_white, bool is_group)
{
    if (eye_image.data == nullptr || module_image.data == nullptr || logo_image.data == nullptr)
        return -1;
    
    int width = code.width;
    int height = code.height;
    
    int image_width = width * box_size;
    int image_height = height * box_size;
    
    cv::Mat img_rgba(image_height, image_width, CV_8UC4, cv::Scalar(255, 255, 255, 0));
    
    if (is_white)
    {
        QRCodeWriter::DrawPersonalQRCode(eye_image, module_image, logo_image, code, box_size, img_rgba, true, is_group);
    }
    else
    {
        QRCodeWriter::DrawPersonalQRCode(eye_image, module_image, logo_image, code, box_size, img_rgba, false, is_group);
    }
    
    buf.image_format_ = PIX_FMT_RGBA8888;
    buf.width_ = img_rgba.cols;
    buf.height_ = img_rgba.rows;
    buf.image_data_ = new uint8_t[buf.width_ * buf.height_ * 4];
    memcpy(buf.image_data_, img_rgba.data, buf.width_ * buf.height_ * 4);
    // cv::imencode(".png", img_rgba, buf);
    
    return 0;
}

int QBarEncoder::MakePersonalColorQRCodeImage(const QBAR_IMAGE& code,
                                             cv::Mat& eye_image,
                                             cv::Mat& module_image,
                                             cv::Mat& logo_image,
                                             cv::Mat& head_image,
                                             int box_size,
                                             QBarImageInfo& buf)
{
    if (eye_image.data == nullptr || module_image.data == nullptr || logo_image.data == nullptr || head_image.data == nullptr)
        return -1;
    
    int width = code.width;
    int height = code.height;
    
    int image_width = width * box_size;
    int image_height = height * box_size;
    
    cv::Mat img_rgba(image_height, image_width, CV_8UC4, cv::Scalar(255, 255, 255, 0));
    
    QRCodeWriter::DrawPersonalQRCode(eye_image, module_image, logo_image, code, box_size, img_rgba, false, false);
    
    std::vector<std::vector<int>> colors;
    QRCodeWriter::GetDominantColors(head_image, colors);
    QRCodeWriter::ApplyColorMask(img_rgba, colors);
    
    buf.image_format_ = PIX_FMT_RGBA8888;
    buf.width_ = img_rgba.cols;
    buf.height_ = img_rgba.rows;
    buf.image_data_ = new uint8_t[buf.width_ * buf.height_ * 4];
    memcpy(buf.image_data_, img_rgba.data, buf.width_ * buf.height_ * 4);
    // cv::imencode(".png", img_rgba, buf);
    
    return 0;
}
}
}