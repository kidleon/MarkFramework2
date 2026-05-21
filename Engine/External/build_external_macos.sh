#!/bin/bash

BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
PLATFORM="MacOS"
ARCH=$(uname -m) # x86_64 or arm64
BIN_DIR="${BASE_DIR}/bin/${PLATFORM}/${ARCH}"
INC_DIR="${BASE_DIR}/inc"

mkdir -p "${BIN_DIR}"
mkdir -p "${INC_DIR}"

echo "============================================"
echo "Building External Libraries (${PLATFORM} ${ARCH})"
echo "============================================"

build_cmake_lib() {
    local SRC_DIR=$1
    local EXTRA_ARGS=$2
    
    echo "Building ${SRC_DIR}..."
    
    # Debug
    cmake -S "${BASE_DIR}/${SRC_DIR}" -B "${BASE_DIR}/build/${SRC_DIR}/debug" \
        -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="${BASE_DIR}/build/${SRC_DIR}/install" -DCMAKE_DEBUG_POSTFIX="_d" ${EXTRA_ARGS}
    cmake --build "${BASE_DIR}/build/${SRC_DIR}/debug" --config Debug --parallel
    cmake --install "${BASE_DIR}/build/${SRC_DIR}/debug" --config Debug

    # Release
    cmake -S "${BASE_DIR}/${SRC_DIR}" -B "${BASE_DIR}/build/${SRC_DIR}/release" \
        -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="${BASE_DIR}/build/${SRC_DIR}/install" ${EXTRA_ARGS}
    cmake --build "${BASE_DIR}/build/${SRC_DIR}/release" --config Release --parallel
    cmake --install "${BASE_DIR}/build/${SRC_DIR}/release" --config Release

    # Copy Results
    cp -f "${BASE_DIR}/build/${SRC_DIR}/install"/lib/*.a "${BIN_DIR}/"
    mkdir -p "${INC_DIR}/${SRC_DIR}"
    cp -R "${BASE_DIR}/build/${SRC_DIR}/install"/include/* "${INC_DIR}/${SRC_DIR}/"
}

# 1. iconv
build_cmake_lib "iconv" "-DBUILD_STATIC=ON -DBUILD_EXECUTABLE=OFF -DBUILD_TEST=OFF"

# 2. mimalloc
build_cmake_lib "mimalloc-3.3.1" "-DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_TESTS=OFF"

echo "Cleaning up temporary build files..."
rm -rf "${BASE_DIR}/build"

echo "All External libraries built successfully!"