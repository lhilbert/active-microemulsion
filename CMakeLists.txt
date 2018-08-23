cmake_minimum_required(VERSION 3.5)
project(active_microemulsion)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror")

add_executable(active_microemulsion
        src/main.cpp
        src/Timing.cpp src/Timing.h
        src/Logger.cpp src/Logger.h
        src/Grid.cpp src/Grid.h src/PgmWriter.cpp src/PgmWriter.h src/Microemulsion.cpp src/Microemulsion.h src/CellData.h)
target_link_libraries(active_microemulsion boost_program_options boost_system boost_filesystem m)
