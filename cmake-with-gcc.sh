#!/usr/bin/env bash
export CC=gcc
export CXX=g++

SRC_DIR="$(pwd)"
BUILD_DIR="${SRC_DIR}/cmake-build-${CC}"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ${SRC_DIR}
cmake --build ${BUILD_DIR} --target active-microemulsion -- -j 2
popd
