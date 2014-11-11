# this is the makefile for k-means-multi

CC = g++
T1 = k-means-multi
.SUFFIXES: .cpp .h .o

all: $(T1)

$(T1): main.o k-means-multi.o
	$(CC) -o k-means-multi main.o k-means-multi.o 

k-means-multi.o: k-means-multi.cpp k-means-multi.h
	$(CC) -c k-means-multi.cpp

main.o: main.cpp k-means-multi.h
	$(CC) -c main.cpp
	
clean:
	/bin/rm -f *.o core 

