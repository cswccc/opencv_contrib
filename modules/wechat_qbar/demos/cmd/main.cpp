#include "opencv2/QbarLib.h"
#include <iostream>

using namespace cv;
using namespace cv::QBarAI;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

uint8_t *LoadGrayscaleImage(const std::string &image_path, int *image_width, int *image_height)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    uint8_t *image = stbi_load(image_path.c_str(), &width, &height, &channels, 0);
    if (!image)
    {
        return nullptr;
    }
    uint8_t *grayscale_image = new uint8_t[width * height];
    if (channels >= 3)
    {
        uint8_t *p = image;
        uint8_t *pg = grayscale_image;
        for (int k = 0; k < width * height; k++)
        {
            *pg = static_cast<uint8_t>(0.2989 * p[0] + 0.587 * p[1] + 0.114 * p[2]);
            pg++;
            p += channels;
        }
    }
    else
    {
        memcpy(grayscale_image, image, width * height);
    }
    stbi_image_free(image);
    if (image_width)
    {
        *image_width = width;
    }
    if (image_height)
    {
        *image_height = height;
    }
    return grayscale_image;
}

int main(int argc, char *argv[])
{
    int width = 0;
    int height = 0;
    std::string image_path = GetImagePathFromArgs(argc, argv);
    uint8_t *grayscale_image = LoadGrayscaleImage(image_path, &width, &height);
    if (!grayscale_image)
    {
        std::cout << "Load image failed: " << image_path << std::endl;
        return -1;
    }

    QBar qbar;
    QBAR_MODE mode;
    mode.useAI = true;
    mode.qbar_ml_mode.detection_model_path_ = "models/detector/qbar_det.yml";
    mode.qbar_ml_mode.super_resolution_model_path_ = "models/sr/qbar_sr.yml";
    qbar.SetReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    qbar.Init(mode);
    //QBAR_RESULT result = qbar.Decode(grayscale_image, width, height);
    
    std::vector<QBAR_RESULT> results = qbar.ScanImage(grayscale_image, width, height);
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

    if (grayscale_image)
    {
        delete grayscale_image;
    }

    return 0;
}
