
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <string>

namespace tscan{
namespace debug{

#define TSLOG(_fmt, ...)  \
{tscan::debug::TsDebugLog("%s:%s:%d: "_fmt ,__FILE__, __func__, __LINE__, __VA_ARGS__);}

static void TsDebugLog(const char * pc_fmt , ...)
{
#ifndef LINUX_DEBUG
    return;
#endif
    
    if ( NULL == pc_fmt)
        return;
    
    va_list va;
    va_start(va , pc_fmt);
    size_t val_size = vsnprintf(NULL , 0 , pc_fmt , va);
    va_end(va);
    if (val_size > 0)
    {
        std::string buf;
        buf.resize(val_size + 1);
        
        char * tmp = (char *)(buf.data());
        
        va_list va2;
        va_start(va2 , pc_fmt);
        vsnprintf(tmp , val_size + 1 , pc_fmt , va2);
        va_end(va2);
        
        printf("%s\n", buf.c_str());
    }
    return;
}

}  // namespace debug
}  // namespace tscan
