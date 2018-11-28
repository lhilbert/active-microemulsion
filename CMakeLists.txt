cmake_minimum_required(VERSION 3.5)
project(active-microemulsion)

set(CMAKE_CXX_STANDARD 14)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message(">>> Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -O0 -Wall -pedantic -Werror") #debug
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(">>> Release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -O3 -Wall -pedantic -Werror")
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
