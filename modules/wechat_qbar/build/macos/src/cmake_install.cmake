# Install script for directory: /Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

set(CMAKE_BINARY_DIR "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/build/macos")

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/include/QBarLib.h;/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/include/QBarStruct.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/include" TYPE FILE FILES
    "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/src/../include/opencv2/QBarLib.h"
    "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/src/../include/opencv2/QBarStruct.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/lib/libQBar.a")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/lib" TYPE STATIC_LIBRARY FILES "/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/build/macos/src/Debug/libQBar.a")
    if(EXISTS "$ENV{DESTDIR}/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/lib/libQBar.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/lib/libQBar.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/install/macos/lib/libQBar.a")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("/Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/build/macos/src/CMakeFiles/QBar.dir/install-cxx-module-bmi-Debug.cmake" OPTIONAL)
  endif()
endif()

