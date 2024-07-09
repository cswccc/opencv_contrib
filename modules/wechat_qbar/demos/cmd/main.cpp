#include "opencv2/QBarLib.h"
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

void imageInput(QBar &qbar, cv::Mat &grayscale_image) {
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
}

void cameraInput(QBar &qbar) {
    int device_id = 0;
    auto cap = cv::VideoCapture(device_id);
    int w = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    auto tick_meter = cv::TickMeter();
    cv::Mat frame;
    while (cv::waitKey(1) < 0) {
        bool has_frame = cap.read(frame);
        if (!has_frame) {
            std:: cout << "No frames grabbed!\n";
            break;
        }

        tick_meter.start();
        std::vector<QBAR_RESULT> results = qbar.ScanImage(grayscale_image);
        tick_meter.stop();

        cv::imshow("Wechat QBar", frame);

        tick_meter.reset();
    }
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
    
    imageInput(qbar, grayscale_image);

    return 0;
}
