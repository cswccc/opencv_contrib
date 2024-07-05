#include "qbar_detector.hpp"
#define CLIP(x, x1, x2) (std::fmax<float>)(x1, (std::fmin<float>)(x, x2))
namespace cv {
namespace QBarAI
{
    
    int QBarDetector::Init(const std::string &config_path)
    {
        std::string root = config_path.rfind('/') == std::string::npos ? "./" : config_path.substr(0, config_path.rfind('/'));
        cv::FileStorage fs(config_path, cv::FileStorage::READ);
        if (!fs.isOpened())
        {
            std::cout << "----- Config file not exists" << std::endl;
            return -1;
        }
        std::string detPath = root + "/" + (std::string)fs["MODEL"];

        if(!fs["short_side"].empty())
        {
            this->short_side = (int)fs["SHORT_SIDE"];
        } 

        if(!fs["long_side"].empty())
        {
            this->long_side = (int)fs["LONG_SIDE"];
        } 
        try
        {
            cv::dnn::Net network = cv::dnn::readNetFromONNX(detPath);
            if(network.empty())
            {
                return -101;
            }
            this->qbar_detector = std::make_shared<cv::dnn::Net>(network);
        }
        catch (const std::exception &e)
        {
            printf(e.what());
            return -3;
        }
        return 0;
    }

    int QBarDetector::Detect(const cv::Mat &image,std::vector<DetectInfo> &bboxes)
    {
        cv::Mat input_blob;
        int ret = this->pre_process_det(image,input_blob);

        input_blob = cv::dnn::blobFromImage(input_blob);

        std::vector<cv::Mat> outputs;

        //forward

        this->qbar_detector->setInput(input_blob, "input");

        std::vector<std::string> output_names;
        output_names.push_back("cls_pred_stride_8");
        output_names.push_back("dis_pred_stride_8");
        output_names.push_back("cls_pred_stride_16");
        output_names.push_back("dis_pred_stride_16");
        output_names.push_back("cls_pred_stride_32");
        output_names.push_back("dis_pred_stride_32");

        this->qbar_detector->forward(outputs, output_names);

        if(outputs.size()==0)
        {
            return -103;
        }

        
        std::vector<BoxInfo> det_bboxes;
        ret = this->post_process_det(outputs,0.3,0.3,input_blob.size[3],input_blob.size[2],det_bboxes);
        if (!ret)
        {
            for (size_t i = 0; i < det_bboxes.size(); i++)
            {
                DetectInfo object;
                object.class_id = det_bboxes[i].label + 1;
                object.prob = det_bboxes[i].score;
                object.x = det_bboxes[i].x1 * image.cols;
                object.y = det_bboxes[i].y1 * image.rows;
                object.width = det_bboxes[i].x2 * image.cols - object.x;
                object.height = det_bboxes[i].y2 * image.rows - object.y;
                bboxes.push_back(object);
            }
        }
    
    }
    


    int QBarDetector::pre_process_det(const cv::Mat &image,cv::Mat &out_blob)
    {
        
        int minInputSize = this->short_side;
        int maxInputSize = this->long_side;
        int setWidth = minInputSize;
        int setHeight = minInputSize;


        //如果图像的宽和高都小于640, 则长边对齐448
        if (image.cols <= maxInputSize && image.rows <= maxInputSize){
            minInputSize = 448;
            if (image.cols  >= image.rows)
            {
                setWidth = minInputSize;
                setHeight = std::ceil(image.rows * 1.0 * minInputSize / image.cols);
            }
            else
            {
                setHeight = minInputSize;
                setWidth = std::ceil(image.cols * 1.0 * minInputSize / image.rows);
            }
        }
        else //如果图像的宽和高都大于640, 则保证面积不大于minInputSize*minInputSize
        {
            float resizeRatio = sqrt(image.cols * image.rows * 1.0 / (minInputSize * minInputSize));
            setWidth = image.cols / resizeRatio;
            setHeight = image.rows / resizeRatio;
        }
        
        setHeight = static_cast<int>((setHeight + 32 - 1) / 32) * 32;
        setWidth = static_cast<int>((setWidth + 32 - 1) / 32) * 32;

        cv::resize(image,out_blob,cv::Size(setWidth, setHeight));
        out_blob.convertTo(out_blob,CV_32FC1,1.0f/128.0f);
        out_blob = out_blob - cv::Scalar(1.0);
        return 0;
    }


    int QBarDetector::post_process_det(std::vector<cv::Mat> outputs,float scoreThres1, float scoreThres2, float inputWidth,float inputHeight,std::vector<BoxInfo>& dets)
    {
        // step 1: extract
        std::vector<std::vector<int>> outShape(6);
        float *outPtr[6];
        for (int i = 0; i < 6; i++)
        {

            outPtr[i] = reinterpret_cast<float *>(outputs[i].data);
            if (outPtr[i] == NULL)
                return -1;
            
            for(int j = 0;j<outputs[i].dims;j++)
            {
                outShape[i].push_back(outputs[i].size[j]);
            }

        }
        
        // step2: decode s8\s16\s32
        std::vector<std::vector<BoxInfo>> results;
        int numClasses = outShape[0][2];
        
        results.resize(numClasses);
        this->decode_infer(outPtr[0], outPtr[1], 8, results, outShape[0], outShape[1], scoreThres1,inputWidth, inputHeight);
        this->decode_infer(outPtr[2], outPtr[3], 16, results, outShape[2], outShape[3], scoreThres1,inputWidth, inputHeight);
        this->decode_infer(outPtr[4], outPtr[5], 32, results, outShape[4], outShape[5], scoreThres1,inputWidth, inputHeight); 
        // step3: nms
        
        std::vector<BoxInfo> rets;
        for (size_t i = 0; i < results.size(); i++)
        {
            this->nms(results[i], 0.6);  // 0.5
            for (auto & box : results[i])
            {
                if (box.score > scoreThres2)
                {
                    rets.push_back(box);
                }
            }
        }
        // step4: multi-class nms to gen BoxMultiInfo results
        this->multiclass_nms(rets, dets, 0.92, inputWidth, inputHeight);
        return 0;
    }
    
    void QBarDetector::multiclass_nms(std::vector<BoxInfo> &input_boxes, std::vector<BoxInfo> &output_boxes, float thr, int inputWidth, int inputHeight)
    {
        if (input_boxes.size() <= 0) return;
        output_boxes.clear();
        
        std::vector<bool> skip(input_boxes.size());
        for (size_t i = 0; i < input_boxes.size(); ++i)
        {
            skip[i] = false;
        }
        
        // merge overlapped results
        for (size_t i = 0; i < input_boxes.size(); ++i)
        {
            int labeli = input_boxes[i].label;
            if (skip[i])
                continue;
            
            for (size_t j = i + 1; j < input_boxes.size(); ++j)
            {
                int labelj = input_boxes[j].label;
                if (skip[j])
                    continue;
                
                //if ((labeli == 0 && labelj == 4) || (labelj == 0 && labeli == 4) || (labeli == 1 && labelj == 5) || (labeli == 5 && labelj == 1))
                if ((labeli == 0 && labelj == 4) || (labelj == 0 && labeli == 4))
                {
                    float area_i = (input_boxes[i].x2 - input_boxes[i].x1 + 1) * (input_boxes[i].y2 - input_boxes[i].y1 + 1);
                    float area_j = (input_boxes[j].x2 - input_boxes[j].x1 + 1) * (input_boxes[j].y2 - input_boxes[j].y1 + 1);
                    float xx1 = (std::max)(input_boxes[i].x1, input_boxes[j].x1);
                    float yy1 = (std::max)(input_boxes[i].y1, input_boxes[j].y1);
                    float xx2 = (std::min)(input_boxes[i].x2, input_boxes[j].x2);
                    float yy2 = (std::min)(input_boxes[i].y2, input_boxes[j].y2);
                    float w = (std::max)(static_cast<float>(0), xx2 - xx1 + 1);
                    float h = (std::max)(static_cast<float>(0), yy2 - yy1 + 1);
                    float inter = w * h;
                    float ovr = inter / (area_i + area_j - inter);
                    float cover = inter / (std::min)(area_i, area_j);
                    
                    if (ovr > thr || cover > 0.96){
                        BoxInfo box;
                        box.x1 = (std::min)(input_boxes[i].x1, input_boxes[j].x1);
                        box.y1 = (std::min)(input_boxes[i].y1, input_boxes[j].y1);
                        box.x2 = (std::max)(input_boxes[i].x2, input_boxes[j].x2);
                        box.y2 = (std::max)(input_boxes[i].y2, input_boxes[j].y2);
                        box.x1 = CLIP(box.x1 / inputWidth, 0, 1);
                        box.y1 = CLIP(box.y1 / inputHeight, 0, 1);
                        box.x2 = CLIP(box.x2 / inputWidth, 0, 1);
                        box.y2 = CLIP(box.y2 / inputHeight, 0, 1);
                        box.score = (std::max)(input_boxes[i].score, input_boxes[j].score);
                        /*if ((labeli == 0 && labelj == 4) || (labelj == 0 && labeli == 4))
                            box.label = 6;
                        else box.label = 7;*/
                        box.label = 5;
                        output_boxes.push_back(box);
                        skip[i] = true;
                        skip[j] = true;
                    }
                }
            }
        }
        // add unmerged results
        for (size_t i = 0; i < input_boxes.size(); ++i)
        {
            if (!skip[i])
            {
                BoxInfo & box = input_boxes[i];
                box.x1 = CLIP(box.x1 / inputWidth, 0, 1);
                box.y1 = CLIP(box.y1 / inputHeight, 0, 1);
                box.x2 = CLIP(box.x2 / inputWidth, 0, 1);
                box.y2 = CLIP(box.y2 / inputHeight, 0, 1);
                output_boxes.push_back(box);
            }
        }
    }

    void QBarDetector::nms(std::vector<BoxInfo>& input_boxes, float NMS_THRESH)
    {
        if (input_boxes.size() <= 1) return;
        std::sort(input_boxes.begin(), input_boxes.end(), [](BoxInfo a, BoxInfo b) { return a.score > b.score; });
        std::vector<float> vArea(input_boxes.size());
        for (size_t i = 0; i < input_boxes.size(); ++i)
        {
            vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
            * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
        }
        for (size_t i = 0; i < input_boxes.size(); ++i)
        {
            for (size_t j = i + 1; j < input_boxes.size();)
            {
                float xx1 = (std::max)(input_boxes[i].x1, input_boxes[j].x1);
                float yy1 = (std::max)(input_boxes[i].y1, input_boxes[j].y1);
                float xx2 = (std::min)(input_boxes[i].x2, input_boxes[j].x2);
                float yy2 = (std::min)(input_boxes[i].y2, input_boxes[j].y2);
                float w = (std::max)(static_cast<float>(0), xx2 - xx1 + 1);
                float h = (std::max)(static_cast<float>(0), yy2 - yy1 + 1);
                float inter = w * h;
                float ovr = inter / (vArea[i] + vArea[j] - inter);
                float cover = inter / (std::min)(vArea[i], vArea[j]);
                
                if (ovr >= NMS_THRESH)
                {
                    input_boxes.erase(input_boxes.begin() + j);
                    vArea.erase(vArea.begin() + j);
                }
                else if (cover >= 0.96)  // qiantao
                {
                    if (vArea[i] > vArea[j])
                    {
                        input_boxes.erase(input_boxes.begin() + j);
                        vArea.erase(vArea.begin() + j);
                    }
                    else
                    {
                        input_boxes[i].x1 = input_boxes[j].x1;
                        input_boxes[i].y1 = input_boxes[j].y1;
                        input_boxes[i].x2 = input_boxes[j].x2;
                        input_boxes[i].y2 = input_boxes[j].y2;
                        input_boxes.erase(input_boxes.begin() + j);
                        vArea.erase(vArea.begin() + j);
                    }
                }
                else
                {
                    j++;
                }
            }
        }
    }

    inline float fast_exp(float x)
    {
        union {
            uint32_t i;
            float f;
        } v{};
        v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
        return v.f;
    }

    template<typename _Tp>
    int activation_function_softmax(const _Tp* src, _Tp* dst, int length)
    {
        const _Tp alpha = *(std::max_element)(src, src + length);
        _Tp denominator{ 0 };
        
        for (int i = 0; i < length; ++i)
        {
            dst[i] = fast_exp(src[i] - alpha);
            denominator += dst[i];
        }
        for (int i = 0; i < length; ++i)
        {
            dst[i] /= denominator;
        }
        
        return 0;
    }
    void QBarDetector::decode_infer(float *clsPred, float *disPred, int stride, std::vector<std::vector<BoxInfo>> &results, const std::vector<int> &outShapeCls, const std::vector<int> &outShapeDis, float scoreThres,float inputWidth,float inputHeight)
    {
        int numClasses = outShapeCls[2];
        int RegMax = (outShapeDis[2] / 4) - 1;
        int lenFeat = outShapeCls[1];
        float * pScore = clsPred;
        std::vector<int> inputShape;
        int featWidth = std::ceil(inputWidth / stride);
        
        for (int idx = 0; idx < lenFeat; idx++)
        {
            for (int label = 0; label < numClasses; label ++)
            {
                float score = pScore[idx * numClasses + label];
                if (score > scoreThres)
                {
                    const float * bboxPred = disPred + idx*outShapeDis[2];  //(RegMax + 1)*4;
                    int row = idx / featWidth;
                    int col = idx % featWidth;
                    // disPred2Bbox
                    float centerX = col * stride;
                    float centerY = row * stride;
                    std::vector<float> disPred;
                    disPred.resize(4);
                    for (int i = 0; i < 4; i++){
                        float dis = 0;
                        float* disAfterSoftmax = new float[RegMax + 1];
                        activation_function_softmax(bboxPred + i*(RegMax + 1), disAfterSoftmax, RegMax+1);
                        for (int j = 0; j < (RegMax+1); j++){
                            dis += j * disAfterSoftmax[j];
                        }
                        dis *= stride;
                        disPred[i] = dis;
                        delete[] disAfterSoftmax;
                    }
                    float xMin = (std::max)(centerX - disPred[0], .0f);
                    float yMin = (std::max)(centerY - disPred[1], .0f);
                    float xMax = (std::min)(centerX + disPred[2], inputWidth);
                    float yMax = (std::min)(centerY + disPred[3], inputHeight);
                    BoxInfo bbox = {xMin, yMin, xMax, yMax, score, label};
                    results[label].push_back(bbox);
                }
            }
        }
    }
}
}