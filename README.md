# threads
THREADS is a simple, tiny and platform-crossing thread library implement using C++
It is aim at developing multi-threading program faster.
It is a exercise to improve myself and so on.

The threads library contains several modules includeing:
## logger
logger is a simple thread-safe logging module using mutex(Windows) or pthread_mutex_t(Linux) etc.

## util
util contains some useful tools, which are wrappered crossing different platform(but now the AIX and Mac OS X is not supported. You can do it if you will)

## thread
The thread(of os) is wrappered as a object, and it managed by thread mgr.
The duty of the thead mgr also contains dispatching tasks to thread.


---
# Build:
>Linux:
   ```
   cd threads/build
   make && make clean
   ```
   
>Windows:
   ```
   Open the sulotion using visual studio(2015) and build it.
   ```
   >> Ps: The library is developed based on C++(98), You can also NEW a visual studio project and ADD files into, and build it.
   
---   
# Structure
## threadMgr
The manager of thread. It manage all un-detached thread. And it occupied a whole thread to process events.
It combines a inner task, which is used to process events.
Every event was a object, and the event declared as a private class in threadMgr.
MORE DETAIL, PLEASE SEE THE **public function** IN threadMgr.h & threadMgr.cpp

## thdTask
The thdTask is a abstruct class. As a base of user defined task, it is extensive and easy to use.
User just new a task, and post it to the threadMgr, it will be handled by a thread.
If you are never call **detach** to a thread, you do not need to care how the thread going to be pooled or ended.

## taskMgr
The taskMgr is a class which will pool tasks used. The reason to the exist of taskMgr is to prevent a same task from being used repeatly.
When a task is handle over, the taskMgr will decide to pool it or release it.

---
There are more features, and I will add them one by one.
Happy your coding.

Fork me at the GITHUB