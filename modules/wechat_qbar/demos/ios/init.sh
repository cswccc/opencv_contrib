cmake ../../CMakeLists.txt -B QBar -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=cmake/ios.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPLATFORM=OS64COMBINED \
    -DDEPLOYMENT_TARGET=12.0 \
    -DBUILD_WITHOUT_ASAN=ON \
    -DBUILD_WITHOUT_OPENMP=ON \
    -DQBar_WITH_ICONV=OFF
