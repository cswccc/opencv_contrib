cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)
# CMake version < 3.6.0 uses all upper cases for Protobuf e.g., PROTOBUF_FOUND

project(QBar)

# set OS according to your system environment
if(WIN32 OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC"
    OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC"))
    set(Windows TRUE)
elseif(UNIX AND NOT APPLE) # Linux
    set(Linux TRUE)
elseif(UNIX AND APPLE) # macOS
    set(macOS TRUE)
    set(FOBJC_ARC "-fobjc-arc")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")
endif()

include(cmake/QBarUtils.cmake)

message(STATUS "[QBar] ================================================================")
message(STATUS "[QBar]   CMAKE_GENERATOR = ${CMAKE_GENERATOR}")
message(STATUS "[QBar] ================================================================")
message(STATUS "[QBar]   CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

#########################################################################################
# Options

option(QBar_WITH_ICONV                  "Build QBar with iconv"               ON)

message(STATUS "[QBar] ================================================================")
message(STATUS
"[QBar]   QBar_WITH_ICONV                        = ${QBar_WITH_ICONV}")

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE})

if(IOS)
    string(CONCAT IOS_LD_MAP_FILE_PATH
        "$(TARGET_BUILD_DIR)/"
        "$(PRODUCT_NAME)-LinkMap-$(CURRENT_VARIANT)-$(CURRENT_ARCH).txt"
    )
endif()

if(MSVC_IDE OR CMAKE_GENERATOR MATCHES Xcode)
    option(QBar_ENABLE_SOLUTION_FOLDERS "Enable solution folders of IDE"           ON)
else()
    option(QBar_ENABLE_SOLUTION_FOLDERS "Enable solution folders of IDE"           OFF)
endif()
# Enable solution folders?
if(QBar_ENABLE_SOLUTION_FOLDERS)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
endif()

#----------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------
# Set C/C++ compiler/linker flags

if(NOT QBar_WITH_ICONV)
    add_definitions(-DNO_ICONV)
endif()

if(ANDROID)
    add_definitions(-DHAVE_ANDROID_OS -DHAVE_SYS_UIO_H -DUSE_CODE_DETECTOR)
endif()

if(IOS AND IOS_PLATFORM EQUAL "OS")
    set(CMAKE_XCODE_ATTRIBUTE_SDKROOT "iphoneos")
endif()


if(NOT MSVC)
    if(IOS AND ENABLE_BITCODE)
        # To disable warning: -headerpad_max_install_names is ignored when used with
        # -bitcode_bundle (Xcode setting ENABLE_BITCODE=YES)
        set(CMAKE_C_LINK_FLAGS      "")
        set(CMAKE_CXX_LINK_FLAGS    "")
        set(CMAKE_OBJC_LINK_FLAGS   "")
        set(CMAKE_OBJCXX_LINK_FLAGS "")

        if(HAVE_FLAG_SEARCH_PATHS_FIRST)
            # Insert -search_paths_first back according to Darwin.cmake
            set(CMAKE_C_LINK_FLAGS      "-Wl,-search_paths_first")
            set(CMAKE_CXX_LINK_FLAGS    "-Wl,-search_paths_first")
            set(CMAKE_OBJC_LINK_FLAGS   "-Wl,-search_paths_first")
            set(CMAKE_OBJCXX_LINK_FLAGS "-Wl,-search_paths_first")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    if(QBar_CPP14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    endif()

    if(MINGW)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -posix")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -posix")
    endif()

    if((NOT MINGW) AND (NOT CYGWIN))
        # MinGW and CygWin does not need -fPIC
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

    # Force to always compile with the most pedantic warning level
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unknown-pragmas")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")

    if(ANDROID AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
    endif()

    set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-Os -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -fvisibility=hidden")
else()
    # Force to always compile with warning level: W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    # Code analysis
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze")

    foreach(flagVar
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL 
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
    )
        # NOTE: OpenCV uses /MD by default

        if(MSVCRT_LIBRARY_FLAG STREQUAL "MD" OR MSVCRT_LIBRARY_FLAG STREQUAL "")
            if(${flagVar} MATCHES "/MT")
                string(REGEX REPLACE "/MT" "/MD" ${flagVar} "${${flagVar}}")
            endif()
        elseif(MSVCRT_LIBRARY_FLAG STREQUAL "MT")
            if(${flagVar} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flagVar} "${${flagVar}}")
            endif()
        else()
            message(FATAL_ERROR
                "MSVCRT_LIBRARY_FLAG: ${MSVCRT_LIBRARY_FLAG} is not supported")
        endif()

        set(${flagVar} "${${flagVar}} /MP /bigobj")
    endforeach(flagVar)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")

    set(PDB_LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PDB_LINK_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PDB_LINK_FLAGS}")
endif()

if(OpenMP_CXX_FOUND OR OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if(APPLE)
    # Force treating all C++ files as Objective-C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")
endif()

if(MINGW)
    add_definitions(-D_WIN32_WINNT=0x0501)  # Windows XP, Windows Server 2003
endif()

#----------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------
# Set platform libraries

# set(QBar_Platform_LIBRARIES)

# if(WIN32)
#     if(NOT MSVC)
#         # GCC or Clang
#         list(APPEND QBar_Platform_LIBRARIES ImageHlp)
#     endif()
# elseif(NOT APPLE)
#     list(APPEND QBar_Platform_LIBRARIES dl)
# endif()

# if(ANDROID)
#     list(APPEND QBar_Platform_LIBRARIES log)
# endif()

# if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND NOT MINGW)
#     list(APPEND QBar_Platform_LIBRARIES pthread)
#     list(APPEND QBar_Platform_LIBRARIES rt)
# endif()

if(APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT DEFINED ENV{LDFLAGS} AND EXISTS "/usr/local/lib")
  link_directories("/usr/local/lib")
endif()

if(APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT DEFINED ENV{LDFLAGS} AND EXISTS "/usr/local/include")
  include_directories("/usr/local/include")
endif()

#----------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------
# 3rd

include(cmake/FindOpenCV.cmake)
find_opencv()

add_subdirectory(src)
if (NOT ANDROID AND NOT IOS)
    add_subdirectory(demos/cmd Demo)
endif ()
