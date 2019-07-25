UNAME := $(shell uname)

# Here the compilation command for Linux
ifeq ($(UNAME), Linux)
CC = g++
CFLAGS = -std=c++14 -O3 -Wall -pedantic -Werror -fopenmp
LIBS = -lboost_program_options -lboost_system -lboost_filesystem -lm
endif

# Here the compilation command for Mac
ifeq ($(UNAME), Darwin)
CC = clang++
CFLAGS = -std=c++14 -O3 -Wall -pedantic -Werror -Xpreprocessor -fopenmp
LIBS = -lboost_program_options -lboost_system -lboost_filesystem -lm -lomp
endif

OBJ = src/main.o \
      src/Timing/Timing.o \
      src/Logger/Logger.o \
      src/Utils/BitwiseOperations.o \
      src/Utils/RandomGenerator.o \
      src/Cell/CellData.o \
      src/Grid/Grid.o \
      src/Grid/GridInitializer.o \
      src/Chain/ChainConfig.o \
      src/Visualization/PgmWriter.o \
      src/Microemulsion/Microemulsion.o \
      src/EventSchedule/EventSchedule.o

all:  $(OBJ)
	$(CC) $(CFLAGS) -o active-microemulsion $(OBJ) $(LIBS)

ifeq ($(UNAME), Linux)
%.o : %.cpp
	$(CC) -c $(CFLAGS) $*.cpp -o $*.o $(LIBS)
endif

ifeq ($(UNAME), Darwin)
%.o : %.cpp
	$(CC) -c $(CFLAGS) $*.cpp -o $*.o
endif

clean:
	rm $(OBJ)

src/Grid/Grid.o                     : src/Grid/Grid.h src/Cell/CellData.h src/Chain/ChainConfig.h src/Utils/BitwiseOperations.h src/Utils/RandomGenerator.h
src/Grid/GridInitializer.o          : src/Grid/Grid.h src/Cell/CellData.h src/Chain/ChainConfig.h
src/Logger/Logger.o                 : src/Logger/Logger.h src/Timing/Timing.h
src/Microemulsion/Microemulsion.o   : src/Microemulsion/Microemulsion.h src/Grid/Grid.h src/Logger/Logger.h src/Utils/RandomGenerator.h
src/Visualization/PgmWriter.o       : src/Visualization/PgmWriter.h src/Cell/CellData.h
src/Timing/Timing.o                 : src/Timing/Timing.h

src/main.o  : src/Logger/Logger.h src/Cell/CellData.h src/Grid/Grid.h src/Grid/GridInitializer.h src/Microemulsion/Microemulsion.h src/Visualization/PgmWriter.h src/Chain/ChainConfig.h src/EventSchedule/EventSchedule.h
