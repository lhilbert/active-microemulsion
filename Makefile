CC = g++
CFLAGS = -std=c++14 -O2 -Wall -pedantic -Werror -fopenmp
LIBS = -l boost_program_options -lm

OBJ = src/main.o \
      src/Timing.o \
      src/Logger.o \
      src/Grid.o \
      src/PgmWriter.o \
      src/Microemulsion.o

all:  $(OBJ)
	$(CC) $(CFLAGS) -o active-microemulsion $(OBJ) $(LIBS)

%.o : %.cpp
	$(CC) -c $(CFLAGS) $*.cpp -o $*.o $(LIBS)

clean:
	rm $(OBJ)

src/Grid.o              : src/Grid.h
src/Logger.o            : src/Logger.h src/Timing.h
src/Microemulsion.o     : src/Microemulsion.h src/Grid.h src/Logger.h
src/PgmWriter.o         : src/PgmWriter.h
src/Timing.o            : src/Timing.h

src/main.o              : src/Logger.h src/Grid.h src/Microemulsion.h src/PgmWriter.h
