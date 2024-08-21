#include <opencv2/opencv.hpp>
#include <opencv2/wechat_qbar.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat visualize(const Mat image, vector<Mat> points, vector<pair<string, string>> res, float fps=-1.f) {
    static cv::Scalar text_color{0, 255, 0};

    auto output_image = image.clone();

    if (fps >= 0)
    {
        cv::putText(output_image, cv::format("FPS: %.2f", fps), cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    for (size_t i = 0; i < points.size(); i++) {
        float center_x = 0, center_y = 0;

        for (size_t r = 0; r < points[i].rows; r++) {
            cv::Point point(points[i].at<float>(r, 0), points[i].at<float>(r, 1));
            cv::circle(output_image, point, 5, cv::Scalar(0, 255, 0), -1);

            center_x += points[i].at<float>(r, 0);
            center_y += points[i].at<float>(r, 1);
        }

        center_x /= points[i].rows;
        center_y /= points[i].rows;

        cv::Size textSize = cv::getTextSize(res[i].second.c_str(), cv::FONT_HERSHEY_DUPLEX, 0.5, 1, 0);

        int text_x = center_x - textSize.width / 2;
        int text_y = center_y + textSize.height / 2;

        cv::putText(output_image, cv::format("%s", res[i].second.c_str()), cv::Point(text_x, text_y), cv::FONT_HERSHEY_DUPLEX, 0.5, text_color, 1);
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

    QBarAI::QBar qbar("models/detector/qbar_det.yml", "models/sr/qbar_sr.yml");

    if (!input_path.empty()) {
        auto img = cv::imread(input_path);

        vector<Mat> points;
        auto res = qbar.detectAndDecode(img, points);

        std::cout << cv::format("%zu codes detected:\n", res.size());
        for (size_t i = 0; i < res.size(); i++) {
            std::cout << "Code " << i << ":\n";
            std::cout << "Type: " << res[i].first << '\n';
            std::cout << "Data: " << res[i].second << '\n';
        }

        if (save_flag || vis_flag) {
            auto res_frame = visualize(img, points, res);
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
            vector<Mat> points;

            tick_meter.start();
            auto res = qbar.detectAndDecode(frame, points);
            tick_meter.stop();

            auto res_frame = visualize(frame, points, res, (float)tick_meter.getFPS());

            cv::imshow("Wechat QBar", res_frame);

            tick_meter.reset();
        }
    }

    return 0;
}