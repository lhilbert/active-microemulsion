#!/usr/bin/env bash

if command -v module 1>/dev/null 2>&1; then
#    module load lib/boost devel/cmake
    module load lib/boost/1.56.0 devel/cmake # This is because with boost 1.61 we get a segfault, to be investigated
    module unload compiler/gnu
    module load compiler/intel
fi

export CC=icc
export CXX=icpc

#BUILD_TYPE="Debug"
#BUILD_TYPE="PGO"
BUILD_TYPE="Release"

# Enable the below only if a profile has already been generated using the PGO build type (see docs)
ENABLE_PGO_USE=0
# ENABLE_PGO_USE=1

if [[ "$1" != "" ]]; then
    BUILD_TYPE="$1"
fi

SRC_DIR="$(pwd)"
BUILD_DIR="${SRC_DIR}/cmake-build-${CC}"
TARGET="active-microemulsion"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_PGO_USE=${ENABLE_PGO_USE} -G "CodeBlocks - Unix Makefiles" ${SRC_DIR}
cmake --build ${BUILD_DIR} --target ${TARGET} -- -j 2
popd
ln -sf ${BUILD_DIR}/src/${TARGET} ${SRC_DIR}/.
