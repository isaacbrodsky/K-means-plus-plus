# this is the makefile for k-means-multi

CC = c++
CFLAGS = -Wall
T1 = k-means++
.SUFFIXES: .cpp .h .o

all: $(T1)

$(T1): main.o k-means-multi.o
	$(CC) $(CLFAGS) -o k-means++ main.o k-means-multi.o 

k-means-multi.o: k-means-multi.cpp k-means-multi.h
	$(CC) $(CFLAGS) -c k-means-multi.cpp

main.o: main.cpp k-means-multi.h
	$(CC) $(CFLAGS) -c main.cpp
	
clean:
	/bin/rm -f *.o core 

