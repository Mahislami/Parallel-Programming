CC=g++-10

CFLAGS=-c -fopenmp -march=native 
LNKFLAGS= -fopenmp

all: main

main: main.o 
	$(CC) $(LNKFLAGS) $(LIBDIR) $(LIBFILE) main.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) $(INCDIR) main.cpp

clean:
	rm -f *o main