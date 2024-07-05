#[=======================================================================[.rst:

【使用样例】
    include(FindOpenCV.cmake)
    find_opencv()
    target_link_libraries(xxx_target PUBLIC ${OpenCV_LIBS})

【输出变量】
    OpenCV_INCLUDE_DIRS  - OpenCV头文件目录列表
    OpenCV_LIBS          - OpenCV库列表
    OpenCV_FRAMEWORK     - OpenCV的framework（仅iOS存在）
    OpenCV_VERSION_MAJOR - OpenCV版本号major
    OpenCV_VERSION_MINOR - OpenCV版本号minor
    OpenCV_VERSION_PATCH - OpenCV版本号patch

#]=======================================================================]

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.24)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

macro(find_opencv)
    if(IOS)
        set(OpenCV_LIBS opencv)
        set(OpenCV_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/_deps/opencv-src)
        set(OpenCV_FRAMEWORK ${CMAKE_CURRENT_BINARY_DIR}/_deps/opencv-src/opencv2.framework)
        message(STATUS "Fetching OpenCV from GitHub ...")
        FetchContent_Declare(opencv 
            URL "https://github.com/opencv/opencv/releases/download/4.9.0/opencv-4.9.0-ios-framework.zip"
            SOURCE_DIR ${OpenCV_FRAMEWORK})
        FetchContent_MakeAvailable(opencv)
        if(NOT EXISTS ${OpenCV_INCLUDE_DIRS}/opencv2)
            file(COPY ${OpenCV_FRAMEWORK}/Versions/A/Headers DESTINATION ${OpenCV_INCLUDE_DIRS})
            file(RENAME ${OpenCV_INCLUDE_DIRS}/Headers ${OpenCV_INCLUDE_DIRS}/opencv2)
        endif()
        add_library(opencv UNKNOWN IMPORTED GLOBAL)
        set_target_properties(opencv PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${OpenCV_INCLUDE_DIRS}"
                IMPORTED_LOCATION ${OpenCV_FRAMEWORK}/opencv2)
    else()
        find_package(OpenCV 4.9.0 HINTS /Users/caoshiwen/opencv/build/install)
    endif()

    set(${OpenCV_VERSION_MAJOR} 4)
    set(${OpenCV_VERSION_MINOR} 9)
    set(${OpenCV_VERSION_PATCH} 0)
endmacro()
