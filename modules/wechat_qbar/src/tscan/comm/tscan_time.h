
#pragma once

#ifdef WIN32
#include <time.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

namespace tscan
{

unsigned int GetNow();

class RunTime
{
public:
    RunTime();
    ~RunTime();
    
    void Start();
    void Stop();
    long GetTimeDiff();
    
private:
    bool has_start_;
    bool has_end_;
    
#ifdef WIN32
    clock_t start_;
    clock_t end_;
#else
    timeval start_;
    timeval end_;
#endif
    
};

}  // namespace tscan
