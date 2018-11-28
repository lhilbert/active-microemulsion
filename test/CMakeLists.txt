# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Catch2)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Prepare "FakeIt" library for other executables
set(FAKEIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FakeIt)
add_library(FakeIt INTERFACE)
target_include_directories(FakeIt INTERFACE ${FAKEIT_INCLUDE_DIR})

# Make test executable
#set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)
set(TEST_SOURCES test_main.cpp
        Grid/RandomGenerator.test.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests
        active-microemulsion-lib
        Catch
        FakeIt)
