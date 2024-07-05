
#include <tscan/comm/tscan_time.h>

#include <string.h>

namespace tscan
{

unsigned int GetNow()
{
#ifdef WIN32
    return clock();
#else
    return time(NULL);
#endif
}

RunTime::RunTime()
{
    has_start_ = false;
    has_end_ = false;
#ifndef WIN32
    memset(&start_, 0, sizeof(timeval));
    memset(&end_, 0, sizeof(timeval));
#endif
}

RunTime::~RunTime()
{}

void RunTime::Start()
{
    if (has_start_) return;
    has_start_ = true;
#ifdef WIN32
    start_ = clock();
#else
    gettimeofday(&start_, NULL);
#endif
}

void RunTime::Stop()
{
    if (has_end_) return;
    has_end_ = true;
#ifdef WIN32
    end_ = clock();
#else
    gettimeofday(&end_, NULL);
#endif
}

long RunTime::GetTimeDiff()
{
    if (false==has_start_ || false == has_end_)
        return 0;
#ifdef WIN32
    return end_ - start_;
#else
    return(end_.tv_usec - start_.tv_usec)/1000 + (1000*(end_.tv_sec-start_.tv_sec));
#endif
}

}  // namespace tscan
