# [threads](https://github.com/tynia/threads)
THREADS is a tiny and platform-crossing thread library implement using C++.  
It is aimed at developing multi-threading program faster and safe.  
It is also an exercise to improve myself.  

# Author
**name**  :**tynia Liu**  
**email** : [ekesi@live.com](ekesi@live.com)

# LICENSE
The **threads** library is released under the [**MIT LICENSE**](http://opensource.org/licenses/mit-license.php).  
you can use it without any copyright about it.  

# Besides
I'd like you will leave my name(tynia) and [repository](https://github.com/tynia/threads) in your project, which you use the [**threads**](https://github.com/tynia/threads) in it.  
  
  

---
The threads library contains several modules includeing:
- logger
- util
- thread
- task

# Introduction
**[logger]** 

It is a simple thread-safe logging module using mutex(Windows) or pthread_mutex_t(Linux) etc.  
When using the logger, all you need do is ```#include logger/logger.h```. And then you can using it like:
```
LogError("LogError is used at the place where a error may be occured during running");
LogEvent("LogEvent is used at the place where a event comming");
LogDebug("LogDebug is used during we developping and record some info for debugging");
...
```
More detail, please see logger/logger.h


**[util]**

It contains some useful tools, which are wrappered to crossing different platform(but now the AIX and Mac OS X is not supported).
- ***spinlock***  is a spinlock based on atomic number. It can be used when accessing to a shared member.  
- ***mutex***     is a mutex variable, and it is used widely between threads.  
- ***container*** is a series of container that defined in STL. I wrapped them using mutex to make sure thread safe when used.  
- ***file***      is a wrapped of I/O operation between different operating system.  
- ... more module(s) is completing.

**[thread]**

It is warpped and based on the real thread supported by operating system, which re-uses the name of *thread*.  
It give out a series of control interface such as **create**, **active**, **deactive**, **suspend**, **resume**, **join**, etc.   
It also contains a simple manager. In order to control and manage thread object reasonable , all thread objects must be created by the thread manager in normal condition.   
However, the thread manager may be not satisfied all your need, you also can use *detach* to own a thread. In that time, you need invoke *active* to start the thread, and *join* to return resource to os.  
sample of dispatching task:
```
threadMgr* mgr = inspire::threadMgr::instance();
INSPIRE_ASSERT(NULL != mgr, "Failed to get thread manager");
mgr->initialize();
mgr->active();

...
taskFactory factory;            // taskFactory must be base on ITaskProductor
...

thdTask* task = inspire::thdTaskMgr::instance()->get(THREAD_TASK_EXAMPLE, &factory);
INSPIRE_ASSERT(NULL != task, "Failed to create task, type: %d", THREAD_TASK_EXAMPLE);

mgr->postEvent(task);           // dispatch task to process task
```

sample of create user defined thread:
```
threadFactory thdFactory;       // threadFactory must be base on IThreadProductor
...

threadMgr* mgr = inspire::threadMgr::instance();
INSPIRE_ASSERT(NULL != mgr, "Failed to get thread manager");
mgr->initialize();              // init resource of thread mgr needed
mgr->active();                  // start thread manager to process tasks
mgr->registerFactory(&thdFactory); // register factory to thread mgr
...
taskFactory factory;            // taskFactory must be base on ITaskProductor
...
thread* thd = mgr->create(THREAD_TYPE_EXAMPLE);   // create a thread object user defined
INSPIRE_ASSERT(NULL != thd, "Failed to create thd, type: %d", THREAD_TYPE_EXAMPLE);
thdTask* task = inspire::thdTaskMgr::instance()->get(THREAD_TASK_EXAMPLE, &factory);
INSPIRE_ASSERT(NULL != task, "Failed to create task, type: %d", THREAD_TASK_EXAMPLE);
thd->assigned(task);           // assign a task to the thread
thd->active();                 // start the thread and handle the task assigned
...
thd->join();                   // task handle over, you need to join the thread to return resource to os
...                            // user should never care the leak of thread object, thread mgr will do it
```

**[task]**

It is a abstract of jobs. It designed to be used with the thread conveniently. A simple task manager is included.  
If you have a task named thdTaskA, and you wanna to dispatch to thread manager to handle it. The thdTaskA must be inherited from thdTask.  
Dispatch the task to the thread manager, the manager will choose a thread to handle it.  
And you can also set a callback typed as TASK_END_FUNC to the task, the callback will be invoked if task is handle over.

 
---
# Sample Building:
The code located in sample/threads.cpp is a simple example of thread-synchronization.

Linux:
```
cd threads/build
make && make clean
```
   
Windows:
```
Open the solution using visual studio(2015) and build it.
```
> Ps: The library is developed based on C++(98), You can also NEW a visual studio(2008-2015) project and ADD files into, and build it.
   
---------------------------
### Fork me at GITHUB
There are more features, and shall we add them one by one?

```At last, Happy your coding.```
