#include "opencv2/QBarLib.h"
#include <iostream>

using namespace cv;
using namespace cv::QBarAI;

cv::Mat visualize(const cv::Mat& image, std::vector<QBAR_RESULT>& results, float fps = -1.f) {
    static cv::Scalar text_color{0, 255, 0};

    auto output_image = image.clone();

    if (fps >= 0) {
        cv::putText(output_image, cv::format("FPS: %.2f", fps), cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    for (int i = 0; i < results.size(); i++) {
        int ux = -1, ly = -1, dx = -1, ry = -1;
        for (int j= 0 ; j < results[i].points.size(); j++) {
            cv::Point point(results[i].points[j].x, results[i].points[j].y);
            cv::circle(output_image, point, 5, cv::Scalar(0, 255, 0), -1);

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
    }

    return output_image;
}

int main(int argc, char *argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help  h           |                                   | Print this message}"
        "{input i           |                                   | Set input to a certain image, omit if using camera}"
        "{backend b         | opencv                            | Set DNN backend}"
        "{target t          | cpu                               | Set DNN target}"
        "{save s            | false                             | Whether to save result image or not}"
        "{vis v             | false                             | Whether to visualize result image or not}"
    );
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    std::string input_path = parser.get<std::string>("input");
    std::string backend = parser.get<std::string>("backend");
    std::string target = parser.get<std::string>("target");
    bool save_flag = parser.get<bool>("save");
    bool vis_flag = parser.get<bool>("vis");

    QBar qbar;
    QBAR_MODE mode;
    mode.useAI = true;
    mode.qbar_ml_mode.detection_model_path_ = "models/detector/qbar_det.yml";
    mode.qbar_ml_mode.super_resolution_model_path_ = "models/sr/qbar_sr.yml";
    qbar.SetReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    qbar.Init(mode);
    
    if (!input_path.empty()) {
        auto image = cv::imread(input_path);

        cv::Mat grayscale_image;
        cv::cvtColor(image, grayscale_image, cv::COLOR_BGR2GRAY);
        
        std::vector<QBAR_RESULT> results = qbar.ScanImage(grayscale_image);

        std::cout << cv::format("%d codes detected:\n", results.size());
        for (int i = 0; i < results.size(); i++) {
            std::cout << "Code " << i << ":\n";
            std::cout << "Type: " << results[i].typeName << '\n';
            std::cout << "Data: " << results[i].data << '\n';
        }

        if (save_flag || vis_flag) {
            auto res_frame = visualize(image, results);
            if (save_flag) {
                std::cout << "Results are saved to result.jpg\n";
                cv::imwrite("result.jpg", res_frame);
            }
            if (vis_flag) {
                cv::namedWindow("Wechat QBar", cv::WINDOW_AUTOSIZE);
                cv::imshow("Wechat QBar", res_frame);
                cv::waitKey(0);
            }
        }
    }
    else {
        int device_id = 0;
        auto cap = cv::VideoCapture(device_id);

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
 
    return 0;
}
