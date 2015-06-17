# Macros:
CPP= g++
CFLAGS= -Wall -Wextra -pedantic -O3 -g -fopenmp -std=c++11
OBJECTS= main.o CtopDecoder.o
TARGET= sample

# Targets:
main: $(OBJECTS)
	$(CPP) $(CFLAGS) -o $(TARGET) $(OBJECTS)
        
main.o: main.cpp MTRand.h BRKGA.h
	$(CPP) $(CFLAGS) -c main.cpp

CtopDecoder.o: CtopDecoder.cpp CtopDecoder.h MTRand.h
	$(CPP) $(CFLAGS) -c CtopDecoder.cpp

# Remove:
clean:
	rm -f $(OBJECTS) *~

run:
	./$(TARGET) dadosctop.dat
