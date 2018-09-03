#!/usr/bin/env bash
export CC=icc
export CXX=icpc

SRC_DIR="$(pwd)"
BUILD_DIR="${SRC_DIR}/cmake-build-${CC}"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ${SRC_DIR}
cmake --build ${BUILD_DIR} --target active_microemulsion -- -j 2
popd
