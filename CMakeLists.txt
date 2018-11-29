cmake_minimum_required(VERSION 3.5)
project(active-microemulsion)

set(CMAKE_CXX_STANDARD 14)

message(">>> Compiler: CC=$ENV{CC}, CXX=$ENV{CXX}")
set(CXX_COMPILER "$ENV{CXX}")

# Base set of flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror")
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message(">>> Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Og") #debug
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(">>> Release mode")
    IF(CXX_COMPILER MATCHES icpc)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ipo -fno-fat-lto-objects")
    ELSE(CXX_COMPILER MATCHES icpc)
        set(FLTO_FLAGS "-flto -fno-fat-lto-objects -fuse-linker-plugin")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 ${FLTO_FLAGS}")  #??? -fprofile-correction
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLTO_FLAGS}")
    ENDIF(CXX_COMPILER MATCHES icpc)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Manage OMP -- ref. https://stackoverflow.com/a/12404666
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(./include)
link_directories(./lib)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
