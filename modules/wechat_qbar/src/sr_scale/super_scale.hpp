// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Tencent is pleased to support the open source community by making WeChat QRCode available.
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.

#ifndef __SCALE_SUPER_SCALE_HPP_
#define __SCALE_SUPER_SCALE_HPP_

#include <stdio.h>
#include "opencv2/dnn.hpp"
#include "opencv2/imgproc.hpp"
namespace cv {
namespace QBarAI {
// using namespace cv;
using namespace std;

class SuperScale {
public:
    SuperScale(){};
    ~SuperScale(){};
    int Init(const std::string &config_path);
    Mat ProcessImageScale(const Mat &src, float scale, const bool &use_sr, int sr_max_size = 160);

private:
    std::shared_ptr<cv::dnn::Net> qbar_sr;
    bool net_loaded_ = false;
    int SuperResoutionScale(const cv::Mat &src, cv::Mat &dst);
};
}
}
#endif  // __SCALE_SUPER_SCALE_HPP_