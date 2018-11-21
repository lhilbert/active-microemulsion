cmake_minimum_required(VERSION 3.5)
project(active-microemulsion)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pg -O3 -Wall -pedantic -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pg -O0 -Wall -pedantic -Werror") #debug

# Manage OMP -- ref. https://stackoverflow.com/a/12404666
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(./include)
link_directories(./lib)

add_executable(active-microemulsion
        src/main.cpp
        src/Timing/Timing.cpp src/Timing/Timing.h
        src/Logger/Logger.cpp src/Logger/Logger.h
        src/Utils/BitwiseOperations.cpp src/Utils/BitwiseOperations.h
        src/Grid/Grid.cpp src/Grid/Grid.h
        src/Grid/GridInitializer.cpp src/Grid/GridInitializer.h
        src/Visualization/PgmWriter.cpp src/Visualization/PgmWriter.h
        src/Microemulsion/Microemulsion.cpp src/Microemulsion/Microemulsion.h
        src/Cell/CellData.cpp src/Cell/CellData.h
        src/Chain/ChainConfig.cpp src/Chain/ChainConfig.h
        src/EventSchedule/EventSchedule.cpp src/EventSchedule/EventSchedule.h
        src/Utils/RandomGenerator.cpp src/Utils/RandomGenerator.h)
target_link_libraries(active-microemulsion boost_program_options boost_system boost_filesystem m)
