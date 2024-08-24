#ifndef QBARENCODER_H
#define QBARENCODER_H

#include <string>

#include <qbarstruct.hpp>

#include "opencv2/core.hpp"

namespace cv {
namespace QBarAI {
class QBarEncoder
{
public:
    static int Encode(QBAR_IMAGE& code, const std::string & content, const QBAR_ENCODE_CONFIG & config);
    
    static void MakeQRCodeImage(const QBAR_IMAGE& code, const QBarDrawParam& param, QBarImageInfo& buf);
    
    static int MakeRoundQRCodeImage(const QBAR_IMAGE& code,
                                    Mat& eye_image,
                                    Mat& module_image,
                                    int box_size,
                                    QBarImageInfo& buf);
    
    static int MakePersonalQRCodeImage(const QBAR_IMAGE& code,
                                       Mat& eye_image,
                                       Mat& module_image,
                                       Mat& logo_image,
                                       int box_size,
                                       QBarImageInfo& buf,
                                       bool is_white, bool is_group);
    static int MakePersonalColorQRCodeImage(const QBAR_IMAGE& code,
                                            Mat& eye_image,
                                            Mat& module_image,
                                            Mat& logo_image,
                                            Mat& head_image_,
                                            int box_size,
                                            QBarImageInfo& buf);
    
    static void GetDominantColors(Mat& image,
                                  std::vector<std::vector<int>>& domi_colors);
    
private:
    static int EncodeQRCode(QBAR_IMAGE& code, const std::string content, QBAR_CODE_FORMAT format, QBAR_ENCODE_CONFIG config);
    static int EncodeOneD(QBAR_IMAGE& code, const std::string content, QBAR_CODE_FORMAT format, QBAR_ENCODE_CONFIG config);
};
}  // namesapce QBarAI
}  // namesapce cv
#endif  // QBARENCODER_H
