all: threads

# which compiler
CC = g++

# include directory
INCLUDE = ../threads

# compile flags
CFLAGS = -O -ggdb -Wall

# target
threads: threads.o
	$(CC) -o threads threads.o

threads.o: ../sample/threads.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../sample/threads.cpp