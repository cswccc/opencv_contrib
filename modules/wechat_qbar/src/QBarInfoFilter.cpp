//
//  QBarInfoFilter.cpp
//  QBar
//
//  Created by merrickmei(梅利健) on 2019/7/15.
//  Copyright © 2019 Valiantliu. All rights reserved.
//

#include "QBarInfoFilter.hpp"

namespace cv {
namespace QBarAI {
QbarInfoCache QBarInfoFilter::filterFrame(QBAR_CODE_DETECT_INFO* detect_info, QBAR_RESULT *result, int frame_num){
    QbarInfoCache  cur_info = QbarInfoCache(detect_info, result, frame_num);
    if (static_cast<int>(info_cache_.size()) >= window_len_)
    {
        info_cache_.pop_back();
    }
    info_cache_.insert(info_cache_.begin(), cur_info);
    
    size_t cache_size = info_cache_.size();
    float score = 0;
    float total = 0;
    int latest_valid_index = -1;
    for (size_t i = 0; i < cache_size; i++)
    {
        QbarInfoCache& info = info_cache_[i];
        if (info.result_valid)
        {
            if (latest_valid_index < 0)
            {
                latest_valid_index = i;
                score += 1.0;
            }
            else if (info_cache_[latest_valid_index].result.data == info.result.data)
            {
                score += 1.0;
            }
            else
            {  // 解码结果不一致, ignore
            }
        }
        total += 1.0;
    }
    
    if (latest_valid_index >= 0 && total > 0)
    {
        cur_info.result_valid = true;
        cur_info.result = info_cache_[latest_valid_index].result;
    
        cur_info.detect_info = info_cache_[latest_valid_index].detect_info;
        cur_info.result_confidence = score / total;
        cur_info.info_frame_delay = cur_info.frame_num - info_cache_[latest_valid_index].frame_num;
    }
    
    return cur_info;
}


bool QBarInfoFilter::predictResultFromeCache(QBAR_CODE_DETECT_INFO* detect_info, QBAR_RESULT &result){
    size_t cache_size = info_cache_.size();
    int latest_valid_index = -1;
    for (size_t i = 0; i < cache_size; i++)
    {
        QbarInfoCache& info = info_cache_[i];
        if (info.result_valid)
        {
            latest_valid_index = i;
            break;
        }
    }
    
    if (latest_valid_index >= 0)
    {
        result = info_cache_[latest_valid_index].result;
        printf("predictResultFromeCache sucess, from pre frame_num:%d\n", info_cache_[latest_valid_index].frame_num);
        return true;
    }
    
    return false;
}
}
}