# threads
THREADS is a thread lib implement using C++

## logger
logger is a simple thread-safe logging module using mutex(Windows) or pthread_mutex_t(Linux) etc.

## util
util contains some useful tools, which are wrappered crossing different platform(but now the AIX and Mac OS X is not supported. You can do it if you will)

## thread
thread is wrappered as a object, and it managed by threadMgr. the theadMgr also dispatch tasks to thread.
