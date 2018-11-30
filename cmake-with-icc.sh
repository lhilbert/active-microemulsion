#!/usr/bin/env bash

if command -v module 1>/dev/null 2>&1; then
    module load lib/boost devel/cmake/3.11.1
    module unload compiler/gnu
    module load compiler/intel
fi

export CC=icc
export CXX=icpc

#BUILD_TYPE="Debug"
#BUILD_TYPE="PGO"
BUILD_TYPE="Release"

SRC_DIR="$(pwd)"
BUILD_DIR="${SRC_DIR}/cmake-build-${CC}"
TARGET="active-microemulsion"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G "CodeBlocks - Unix Makefiles" ${SRC_DIR}
cmake --build ${BUILD_DIR} --target ${TARGET} -- -j 2
popd
ln -sf ${BUILD_DIR}/src/${TARGET} ${SRC_DIR}/.
