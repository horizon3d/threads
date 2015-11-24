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
	writer.o thread.o ossFile.o $(CFLAGS)

threads.o: ../sample/threads.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../sample/threads.cpp

thread.o: ../threads/thread.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/thread.cpp

thdMgr.o: ../threads/thdMgr.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/thdMgr.cpp

thdMgrTask.o: ../threads/task/thdMgrTask.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/task/thdMgrTask.cpp

thdTaskMgr.o: ../threads/task/thdTaskMgr.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/task/thdTaskMgr.cpp

base.o: ../threads/logger/base.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/logger/base.cpp

writer.o: ../threads/logger/writer.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/logger/writer.cpp

ossFile.o: ../threads/util/ossFile.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c ../threads/util/ossFile.cpp

clean:
	rm -f *.o