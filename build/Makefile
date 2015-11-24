all: threads

# which compiler
CC = g++

# include directory
INCLUDE = ../threads

# compile flags
CFLAGS = -O -ggdb -Wall -D_LINUX -D_DEBUG -I$(INCLUDE) -lpthread -lm -ldl

# target
threads: threads.o thdMgr.o thdMgrTask.o thdTaskMgr.o base.o writer.o thread.o \
ossFile.o
	$(CC) -o threads threads.o thdMgr.o thdMgrTask.o thdTaskMgr.o base.o \
	writer.o thread.o ossFile.o

threads.o: ../sample/threads.cpp
	$(CC) -I$(INCLUDE) -c ../sample/threads.cpp $(CFLAGS)

thread.o: ../threads/thread.cpp
	$(CC) -I$(INCLUDE) -c ../threads/thread.cpp $(CFLAGS)

thdMgr.o: ../threads/thdMgr.cpp
	$(CC) -I$(INCLUDE) -c ../threads/thdMgr.cpp $(CFLAGS)

thdMgrTask.o: ../threads/task/thdMgrTask.cpp
	$(CC) -I$(INCLUDE) -c ../threads/task/thdMgrTask.cpp $(CFLAGS)

thdTaskMgr.o: ../threads/task/thdTaskMgr.cpp
	$(CC) -I$(INCLUDE) -c ../threads/task/thdTaskMgr.cpp $(CFLAGS)

base.o: ../threads/logger/base.cpp
	$(CC) -I$(INCLUDE) -c ../threads/logger/base.cpp $(CFLAGS)

writer.o: ../threads/logger/writer.cpp
	$(CC) -I$(INCLUDE) -c ../threads/logger/writer.cpp $(CFLAGS)

ossFile.o: ../threads/util/ossFile.cpp
	$(CC) -I$(INCLUDE) -c ../threads/util/ossFile.cpp $(CFLAGS)

clean:
	rm -f *.o