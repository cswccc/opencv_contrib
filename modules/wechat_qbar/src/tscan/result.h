
#pragma once

#include <stdint.h>

#include <string>

namespace tscan
{
class ReaderTimeConsuming
{
public:
    ReaderTimeConsuming(
                        std::string filename = "",
                        const uint32_t time_reader = 0,
                        const uint32_t time_detect = 0,
                        const uint32_t time_decode = 0) :
    filename_(filename),
    time_reader_(time_reader),
    time_detect_(time_detect),
    time_decode_(time_decode) {}
    ~ReaderTimeConsuming() {}
    
    void Reset()
    {
        filename_ = "";
        time_reader_ = 0;
        time_detect_ = 0;
        time_decode_ = 0;
    }
    
public:
    std::string filename_;
    uint32_t time_reader_;
    uint32_t time_detect_;
    uint32_t time_decode_;
    
};

class QBarTimeConsuming
{
public:
    QBarTimeConsuming(const uint32_t time_create_pyramid = 0) :
    time_create_pyramid_(time_create_pyramid),time_detect_(0),
    time_super_resolution_(0),time_decode_(0),
    time_pre_detect(0), time_after_detect(0), time_seg(0), time_after_seg(0),
    time_decode_all_(0), time_all_(0),
    detect_succ_(false), has_super_resolution_(false), has_bar_segment_(false), has_decode_(false),
    debug_img_buff_(NULL), debug_img_width_(0), debug_img_height_(0)
    {}
    
    ~QBarTimeConsuming() {
    }
    
    void Reset()
    {
        time_create_pyramid_ = 0;
        time_detect_ = 0;
        time_super_resolution_ = 0;
        time_decode_ = 0;
        time_all_ = 0;

        time_pre_detect = 0;
        time_detect_pre_infer_ = 0;
        time_detect_infer_ = 0;
        time_detect_after_infer_ = 0;
        time_after_detect = 0;
        time_seg = 0;
        time_after_seg = 0;
        time_decode_pre_ = 0;
        time_decode_all_ = 0;
        
        detect_succ_ = false;
        has_super_resolution_ = false;
        has_decode_ = false;
        debug_img_buff_ = NULL;
        debug_img_width_ = 0;
        debug_img_height_ = 0;
        
        has_bar_segment_ = false;
    }
    
public:
    uint32_t time_create_pyramid_;
    uint32_t time_detect_;
    uint32_t time_super_resolution_;
    uint32_t time_decode_;
    
    uint32_t time_pre_detect;
    uint32_t time_detect_infer_;
    uint32_t time_detect_pre_infer_;
    uint32_t time_detect_after_infer_;
    uint32_t time_after_detect;
    uint32_t time_seg;
    uint32_t time_after_seg;
    uint32_t time_decode_pre_;
    uint32_t time_decode_all_;
    uint32_t time_all_;
    
    bool detect_succ_;
    bool has_super_resolution_;
    bool has_bar_segment_;
    bool has_decode_;
    uint8_t * debug_img_buff_;
    uint32_t debug_img_width_;
    uint32_t debug_img_height_;
};

}  // tscan
