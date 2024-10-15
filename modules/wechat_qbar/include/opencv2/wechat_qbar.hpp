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
class CV_EXPORTS_W QBar
{
public:
    CV_WRAP QBar(const std::string& detection_model_path_ = "",
                    const std::string& super_resolution_model_path_ = "");

    ~QBar() {};

    CV_WRAP bool detect(InputArray img, OutputArrayOfArrays points);
    CV_WRAP std::vector<std::string> decode(InputArray img, InputArrayOfArrays detect_points, OutputArrayOfArrays points);
    CV_WRAP std::vector<std::string> detectAndDecode(InputArray img, OutputArrayOfArrays points);
    CV_WRAP std::string GetVersion();
    
protected:
    class Impl;
    Ptr<Impl> p;
};
}  // namespace QBarAI
}  // namespace cv
#endif
