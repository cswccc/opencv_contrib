cmake_minimum_required(VERSION 3.6.0)
project(Demo)

add_executable(Demo main.cpp)
target_link_libraries(Demo PRIVATE QBar)

if (macOS)
    target_link_libraries(Demo PRIVATE "-framework Accelerate" "-framework Metal" "-framework MetalPerformanceShaders" iconv z)
elseif(Linux)
    target_link_libraries(Demo PRIVATE pthread dl)
endif ()