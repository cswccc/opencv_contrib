#include "opencv2/QBarLib.h"
#include <iostream>

using namespace cv;
using namespace cv::QBarAI;


// #define DEFAULT_TEST_IMAGE_PATH "/home/cswccc/Work/opencv_contrib/modules/wechat_qbar/demos/common/images/test.jpg"
#define DEFAULT_TEST_IMAGE_PATH "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/demos/common/images/test_sr.png"

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

cv::Mat visualize(const cv::Mat& image, std::vector<QBAR_RESULT>& results, float fps = -1.f) {
    static cv::Scalar box_color{0, 255, 0};
    static std::vector<cv::Scalar> landmark_color{
        cv::Scalar(255,   0,   0), // right eye
        cv::Scalar(  0,   0, 255), // left eye
        cv::Scalar(  0, 255,   0), // nose tip
        cv::Scalar(255,   0, 255), // right mouth corner
        cv::Scalar(  0, 255, 255)  // left mouth corner
    };
    static cv::Scalar text_color{0, 255, 0};

    auto output_image = image.clone();

    if (fps >= 0) {
        cv::putText(output_image, cv::format("FPS: %.2f", fps), cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    for (int i = 0; i < results.size(); i++) {
        std::cout << "Type: " << results[i].typeName << std::endl;
        std::cout << "Data: " << results[i].data << std::endl;
        std::string info = results[i].data;
        cout << "points number: " << results[i].points.size() << endl;

        int ux = -1, ly = -1, dx = -1, ry = -1;
        for (int j= 0 ; j < results[i].points.size(); j++) {
            cv::Point point(results[i].points[j].x, results[i].points[j].y);
            cv::circle(output_image, point, 5, landmark_color[i % landmark_color.size()], -1);
            cout << results[i].points[j].x << ' ' << results[i].points[j].y << endl;

            if (ly == -1 || ly > results[i].points[j].y) {
                ly = results[i].points[j].y;
            }
            if (ry == -1 || ry < results[i].points[j].y) {
                ry = results[i].points[j].y;
            }
            if (dx == -1 || dx > results[i].points[j].x) {
                dx = results[i].points[j].x;
            }
            if (ux == -1 || ux < results[i].points[j].x) {
                ux = results[i].points[j].x;
            }
        }
        cv::Size textSize = cv::getTextSize(results[i].data.c_str(), cv::FONT_HERSHEY_DUPLEX, 0.5, 1, 0);

        int text_x = (ux + dx) / 2 - textSize.width / 2;
        int text_y = (ly + ry) / 2 + textSize.height / 2;

        cv::putText(output_image, cv::format("%s", results[i].data.c_str()), cv::Point(text_x, text_y), cv::FONT_HERSHEY_DUPLEX, 0.5, text_color, 1);
        cout << "----" << endl;
    }

    return output_image;
}

void imageInput(QBar &qbar, cv::Mat &image) {
    cv::Mat grayscale_image;
    cv::cvtColor(image, grayscale_image, cv::COLOR_BGR2GRAY);
    
    std::vector<QBAR_RESULT> results = qbar.ScanImage(grayscale_image);

    auto res_frame = visualize(image, results);

    cv::namedWindow("Wechat QBar", cv::WINDOW_AUTOSIZE);
    cv::imshow("Wechat QBar", res_frame);
    cv::waitKey(0);
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
        cv::Mat grayscale_image;
        cv::cvtColor(frame, grayscale_image, cv::COLOR_BGR2GRAY);

        tick_meter.start();
        std::vector<QBAR_RESULT> results = qbar.ScanImage(grayscale_image);
        tick_meter.stop();

        auto res_frame = visualize(frame, results, (float)tick_meter.getFPS());

        cv::imshow("Wechat QBar", res_frame);

        tick_meter.reset();
    }
}

int main(int argc, char *argv[])
{
    std::string image_path = GetImagePathFromArgs(argc, argv);
    cv::Mat image = cv::imread(image_path);

    QBar qbar;
    QBAR_MODE mode;
    mode.useAI = true;
    mode.qbar_ml_mode.detection_model_path_ = "models/detector/qbar_det.yml";
    mode.qbar_ml_mode.super_resolution_model_path_ = "models/sr/qbar_sr.yml";
    qbar.SetReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    qbar.Init(mode);
    
    // imageInput(qbar, image);
    cameraInput(qbar);
 
    return 0;
}
