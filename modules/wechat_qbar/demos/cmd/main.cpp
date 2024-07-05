#include "opencv2/QbarLib.h"
#include <iostream>

using namespace cv;
using namespace cv::QBarAI;


#define DEFAULT_TEST_IMAGE_PATH "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/demos/common/images/test.jpg"

std::string GetImagePathFromArgs(int argc, char *argv[])
{
    if (argc >= 2)
    {
        return argv[1];
    }
    std::cout << "CMD usage: " << argv[0] << " <image-path>" << std::endl;
    std::cout << "There's is no input image, use default test image: " << DEFAULT_TEST_IMAGE_PATH << std::endl;
    return DEFAULT_TEST_IMAGE_PATH;
}

int main(int argc, char *argv[])
{
    std::string image_path = GetImagePathFromArgs(argc, argv);
    cv::Mat grayscale_image = cv::imread(image_path, cv::IMREAD_GRAYSCALE);

    QBar qbar;
    QBAR_MODE mode;
    mode.useAI = true;
    mode.qbar_ml_mode.detection_model_path_ = "models/detector/qbar_det.yml";
    mode.qbar_ml_mode.super_resolution_model_path_ = "models/sr/qbar_sr.yml";
    qbar.SetReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    qbar.Init(mode);
    //QBAR_RESULT result = qbar.Decode(grayscale_image, width, height);
    
    std::vector<QBAR_RESULT> results = qbar.ScanImage(grayscale_image);
    for(int i = 0; i < results.size(); i++)
    {
        if(results[i].typeID!=0)
        {
            std::cout << "Decode success!" << std::endl;
            std::cout << "Type: " << results[i].typeName << std::endl;
            std::cout << "Data: " << results[i].data << std::endl;
        }
         else
        {
            std::cout << "Decode failed!" << std::endl;
        }
    }

    return 0;
}
