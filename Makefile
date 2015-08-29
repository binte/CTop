# Macros:
CPP= g++
CFLAGS= -Wall -Wextra -pedantic -O3 -g -fopenmp -std=c++11
OBJECTS= CtopDecoder.o Clients.o Vertice.o main.o
TARGET= prog

# Targets:
main: $(OBJECTS)
	$(CPP) $(CFLAGS) -o $(TARGET) $(OBJECTS)

main.o: main.cpp MTRand.h BRKGA.h
	$(CPP) $(CFLAGS) -c main.cpp

CtopDecoder.o: CtopDecoder.cpp CtopDecoder.h MTRand.h
	$(CPP) $(CFLAGS) -c CtopDecoder.cpp

Clients.o: Clients.cpp Clients.h
	$(CPP) $(CFLAGS) -c Clients.cpp

Vertice.o: Vertice.cpp Vertice.h
	$(CPP) $(CFLAGS) -c Vertice.cpp


# Remove:
clean:
	rm -f $(OBJECTS) *~

run:
	./$(TARGET) Cp4.3m.txt false
