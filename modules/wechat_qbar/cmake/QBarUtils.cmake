#=========================================================================
#
# QBarUtils.cmake
# 
# Copyright (c) 2020-present. All Rights Reserved
# 
# Author: Harding
#
#=========================================================================

#----------------------------------------------------------------------------------------
# Get platform libbraries 
macro(GetPlatformLibs)
    unset(Platform_Libs CACHE) 

    if(ANDROID)
        set(tmp)
        find_library(tmp log)
        list(APPEND Platform_Libs ${tmp})
    endif()

    if(APPLE)
        # Accelerate.framework
        QBarFindFramework(tmp Accelerate)
        list(APPEND Platform_Libs ${tmp})

        # Maybe unused

        # Metal.framework
        QBarFindFramework(tmp Metal)
        list(APPEND Platform_Libs ${tmp})

        # MetalPerformanceShaders.framework
        QBarFindFramework(tmp MetalPerformanceShaders)
        list(APPEND Platform_Libs ${tmp})

        unset(tmp CACHE)
        set(tmp)
        find_library(tmp iconv)
        list(APPEND Platform_Libs ${tmp})
    endif()

    if(Linux)
        set(tmp)
        find_library(tmp dl)
        list(APPEND Platform_Libs ${tmp})
    endif()

endmacro()

#----------------------------------------------------------------------------------------
# Get the architecture for Android build
function(QBarGetAndroidArchitecture ABI ARCH)
    if(ABI STREQUAL armeabi-v7a)
        set(${ARCH} arm PARENT_SCOPE)
    elseif(ABI STREQUAL arm64-v8a)
        set(${ARCH} aarch64 PARENT_SCOPE)
    elseif(ABI STREQUAL x86)
        set(${ARCH} i686 PARENT_SCOPE)
    elseif(ABI STREQUAL x86_64)
        set(${ARCH} x86_64 PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unsupported ABI")
    endif()
endfunction()

#----------------------------------------------------------------------------------------
# [Apple platforms only] Find specified framework
macro(QBarFindFramework framework frameworkName)
    unset(FW_${frameworkName} CACHE)
    set(FW_${frameworkName})
    if(NOT DEFINED IOS_PLATFORM)
        # macOS
        find_library(
            FW_${frameworkName} NAMES ${frameworkName}
        )
    else()
        # iOS or simulators
        find_library(
            FW_${frameworkName} NAMES ${frameworkName}
            PATHS ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks NO_DEFAULT_PATH
        )
    endif()
    if(${FW_${frameworkName}} STREQUAL FW_${frameworkName}-NOTFOUND)
        message(ERROR  "Framework ${frameworkName} not found")
    else()
        message(STATUS "Framework ${frameworkName} found at ${FW_${frameworkName}}")
    endif()
    set(${framework} ${FW_${frameworkName}})
endmacro()

#----------------------------------------------------------------------------------------
# [Apple iOS platforms only] Set iOS properties for Application/Framework target
macro(QBarSetIOSProperties target isFramework)

    # Add debuginfo to lib
    if(${isFramework})
        set_xcode_property(${target} GCC_GENERATE_DEBUGGING_SYMBOLS YES "Release")
        set_xcode_property(${target} STRIP_INSTALLED_PRODUCT NO "Release")
        set_xcode_property(${target} STRIP_STYLE "debugging" "Release")
        set_xcode_property(${target} DEPLOYMENT_POSTPROCESSING NO "Release")
    endif()

    if(IOS)
        if(${isFramework})
            set_target_properties(${target} PROPERTIES
                FRAMEWORK TRUE
                FRAMEWORK_VERSION Aß
                XCODE_ATTRIBUTE_DYLIB_INSTALL_NAME_BASE "@rpath"
            )
        else()
            set_target_properties(${target} PROPERTIES
                XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks"
            )
        endif()

        get_target_property(targetType ${target} TYPE)

        if(NOT targetType STREQUAL STATIC_LIBRARY)
            # Common properties for both Application and non-static Framework
            set_target_properties(${target} PROPERTIES
                MACOSX_BUNDLE YES
                XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
                XCODE_ATTRIBUTE_ENABLE_BITCODE "${ENABLE_BITCODE}"
                XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
                XCODE_ATTRIBUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/Product"
                XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"
                XCODE_ATTRIBUTE_PRODUCT_NAME "${target}"
                XCODE_ATTRIBUTE_LD_GENERATE_MAP_FILE YES
                XCODE_ATTRIBUTE_LD_MAP_FILE_PATH "${IOS_LD_MAP_FILE_PATH}"
            )
        endif()

        # Set the iOS deployment target
        if( IOS_PLATFORM STREQUAL "OS" OR
            IOS_PLATFORM STREQUAL "SIMULATOR" OR IOS_PLATFORM STREQUAL "SIMULATOR64" )
            set_target_properties(
                ${target} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET
                ${DEPLOYMENT_TARGET}
            )
        endif()
    endif()
endmacro()