#!/usr/bin/env bash
export CC=clang
export CXX=clang++

BUILD_TYPE="Debug"
#BUILD_TYPE="Release"

SRC_DIR="$(pwd)"
BUILD_DIR="${SRC_DIR}/cmake-build-${CC}"
TARGET="active-microemulsion"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G "CodeBlocks - Unix Makefiles" ${SRC_DIR}
cmake --build ${BUILD_DIR} --target ${TARGET} -- -j 2
popd
ln -sf ${BUILD_DIR}/${TARGET} ${SRC_DIR}/.
