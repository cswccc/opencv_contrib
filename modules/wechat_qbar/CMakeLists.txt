set(the_description "WeChat QBar")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

ocv_define_module(wechat_qbar opencv_core opencv_imgproc opencv_dnn opencv_calib3d WRAP java objc python js)