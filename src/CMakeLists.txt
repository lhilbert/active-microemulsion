find_package(Boost COMPONENTS program_options system filesystem REQUIRED)

add_library(active-microemulsion-lib
        Timing/Timing.cpp Timing/Timing.h
        Logger/Logger.cpp Logger/Logger.h
        Utils/BitwiseOperations.cpp Utils/BitwiseOperations.h
        Grid/Grid.cpp Grid/Grid.h
        Grid/GridInitializer.cpp Grid/GridInitializer.h
        Visualization/PgmWriter.cpp Visualization/PgmWriter.h
        Microemulsion/Microemulsion.cpp Microemulsion/Microemulsion.h
        Cell/CellData.cpp Cell/CellData.h
        Chain/ChainConfig.cpp Chain/ChainConfig.h
        EventSchedule/EventSchedule.cpp EventSchedule/EventSchedule.h
        Utils/RandomGenerator.cpp Utils/RandomGenerator.h)
#target_link_libraries(active-microemulsion-lib boost_program_options boost_system boost_filesystem m)
target_link_libraries(active-microemulsion-lib
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        m)

add_executable(active-microemulsion main.cpp)
target_link_libraries(active-microemulsion active-microemulsion-lib)
