// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Tencent is pleased to support the open source community by making WeChat QRCode available.
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
#include "super_scale.hpp"
#include <iostream>

#define CLIP(x, x1, x2) max(x1, min(x, x2))
namespace cv {
namespace QBarAI {
int SuperScale::Init(const std::string &config_path) {
    
    std::string root = config_path.rfind('/') == std::string::npos ? "./" : config_path.substr(0, config_path.rfind('/'));
    FileStorage fs(config_path, FileStorage::READ);
    if (!fs.isOpened())
    {
        std::cout << "----- Config file not exists" << std::endl;
        return -1;
    }
    std::string srPath = root + "/" + (std::string)fs["MODEL"];
    try
    {
        dnn::Net network = dnn::readNetFromONNX(srPath);
        if(network.empty())
        {
            return -101;
        }
        this->qbar_sr = std::make_shared<dnn::Net>(network);
    }
    catch (const std::exception &e)
    {
        printf(e.what());
        return -3;
    }
    net_loaded_ = true;
    return 0;
}

Mat SuperScale::ProcessImageScale(const Mat &src, float scale, const bool &use_sr,
                                  int sr_max_size) {
    Mat dst = src;
    if (scale == 1.0) {  // src
        return dst;
    }

    int width = src.cols;
    int height = src.rows;
    if (scale == 2.0) {  // upsample
        int SR_TH = sr_max_size;
        if (use_sr && (int)sqrt(width * height * 1.0) < SR_TH && net_loaded_) {
            int ret = SuperResoutionScale(src, dst);
            if (ret == 0) return dst;
        }

        { resize(src, dst, Size(), scale, scale, INTER_CUBIC); }
    } else if (scale < 1.0) {  // downsample
        resize(src, dst, Size(), scale, scale, INTER_AREA);
    }

    return dst;
}

int SuperScale::SuperResoutionScale(const Mat &src, Mat &dst) {
   
    // cv::resize(src, dst, Size(), 2, 2, INTER_CUBIC);
    Mat blob;
    dnn::blobFromImage(src, blob, 1.0, Size(src.cols, src.rows), {0.0f}, false, false);

    qbar_sr->setInput(blob);
    auto prob = qbar_sr->forward();

    dst = Mat(prob.size[2], prob.size[3], CV_32F, prob.ptr<float>());
    dst.convertTo(dst, CV_8UC1);
    return 0;
}
}  // namesapce QBarAI
}  // namesapce cv