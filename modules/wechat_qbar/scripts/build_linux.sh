BUILD_TYPE=Debug
QBar_WITH_ICONV=ON

ROOT_DIR="$(pwd -P)"
BUILD_DIR=${ROOT_DIR}/build/linux
INSTALL_DIR=${ROOT_DIR}/install/linux

if [ -d ${BUILD_DIR} ]; then
    mkdir -p ${BUILD_DIR}
fi

cmake CMakeLists.txt -B ${BUILD_DIR} -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_CONFIGURATION_TYPES=${BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
    -DQBar_CPP14=$QBar_CPP14 \
    -DQBar_WITH_ICONV=$QBar_WITH_ICONV \
    || exit 1

cd ${BUILD_DIR}
cmake --build . --config ${BUILD_TYPE} -j16 || exit 1
cmake --build . --target install --config ${BUILD_TYPE} || exit 1
