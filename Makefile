# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = g++
CFLAGS  = -g -Wall

default: test

test:  posix_segment.o multithreads.o
	$(CC) $(CFLAGS) -o test posix_segment.o multithreads.o -lpthread -lrt

posix_segment.o:  posix_segment.cpp 
	$(CC) $(CFLAGS) -I. -c posix_segment.cpp
multithreads.o: multithreads.cpp
	$(CC) $(CFLAGS) -I. -c multithreads.cpp

clean: 
	$(RM) test *.o *~