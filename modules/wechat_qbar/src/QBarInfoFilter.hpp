//
//  qbarinfofilter.hpp
//  QBar
//
//  Created by merrickmei(梅利健) on 2019/7/15.
//  Copyright © 2019 Valiantliu. All rights reserved.
//

#ifndef QBarInfoFilter_hpp
#define QBarInfoFilter_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "qbarstruct.hpp"

const int MAX_CACHE_FRAME = 15;
namespace cv {
namespace QBarAI {
struct QbarInfoCache{
    QbarInfoCache() = default;
    QbarInfoCache(QBAR_CODE_DETECT_INFO *detect_info_,QBAR_RESULT *result_, int frame_num_){
        frame_num = frame_num_;
        if (detect_info_){
            detect_valid = true;
            detect_info = *detect_info_;
        }
        else{
            detect_valid = false;
        }
        
        if (result_){
            result_valid = true;
            result = *result_;
            result_confidence = 1.0;
        }
        else{
            result_valid = false;
            result_confidence = 0.0;
        }
    };
    bool detect_valid = false;
    QBAR_CODE_DETECT_INFO detect_info;
    bool result_valid = false;
    QBAR_RESULT result;
    float result_confidence = 0;
    int frame_num = 0;
    int info_frame_delay = 0;
};

class QBarInfoFilter
{
public:
    QBarInfoFilter(int window_len):window_len_(window_len < MAX_CACHE_FRAME ? window_len : MAX_CACHE_FRAME){};
    ~QBarInfoFilter() {};
    
    QbarInfoCache filterFrame(QBAR_CODE_DETECT_INFO* detect_info,QBAR_RESULT *result, int frame_num);
    bool predictResultFromeCache(QBAR_CODE_DETECT_INFO* detect_info,QBAR_RESULT &result);
private:
    std::vector<QbarInfoCache> info_cache_;  // save last MAX_LAST_FRAME frames landmarks cache
    
    int window_len_;
};
}  // namespace QBarAI
}  // namespace cv
#endif /* QBarInfoFilter_hpp */
