cmake_minimum_required(VERSION 3.6.0)

file(GLOB_RECURSE QBAR_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/**/*.c)

file(GLOB_RECURSE QBAR_PUBLIC_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/**)

add_library(QBar STATIC ${QBAR_SOURCES})

if (IOS)
    # Set QBar as a framework
    QBarSetIOSProperties(QBar TRUE)
    set_target_properties(QBar PROPERTIES PUBLIC_HEADER "${QBAR_PUBLIC_HEADERS}")
endif ()

#---------------------------------------------------------------------------------------
# Set QBar_INCLUDE_DIRS

set(QBar_INCLUDE_DIRS)
list(APPEND QBar_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)
list(APPEND QBar_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)
target_include_directories(QBar PUBLIC ${QBar_INCLUDE_DIRS})

target_link_libraries(QBar PUBLIC ${OpenCV_LIBS})
if (MSVC)
    target_link_libraries(QBar PUBLIC IConv)
endif()

#---------------------------------------------------------------------------------------
# install

install(FILES ${QBAR_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS QBar DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if (MSVC)
    install(FILES $<TARGET_FILE_DIR:QBar>/QBar.pdb DESTINATION ${CMAKE_INSTALL_PREFIX}/pdb OPTIONAL)
endif ()
