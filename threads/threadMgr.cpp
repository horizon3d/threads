/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 tynia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author: tynia
Date  : =========
*******************************************************************************/
#include "threadMgr.h"
#include "threadExt.h"

namespace inspire {

   const int threadMgr::thdInnerTask::run()
   {
      LogEvent("starting MAIN PROCESS LOOP: %s", name());
      while (_thd->running())
      {
         _threadMgr->_process();
      }

      LogEvent("ending MAIN PROCESS LOOP: %s", name());
      return _thd->error();
   }

   threadMgr::threadMgr() : _maxIdleCount(20)
   {
      _taskMgr = thdTaskMgr::instance();
   }

   threadMgr::~threadMgr()
   {
      _taskMgr = NULL;
   }

   void threadMgr::initialize()
   {
      thdTask* t = new thdInnerTask(this);
      STRONG_ASSERT(NULL != t, "Failed to allocate event processing task");

      thread* thd = _create();
      STRONG_ASSERT(NULL != thd, "cannot start event processing thread, exit");

      _detach(thd);
      _thdMain = thd;
      _thdMain->assigned(t);
   }

   void threadMgr::active()
   {
      STRONG_ASSERT(NULL != _thdMain, "event processing thread is NULL");
      _thdMain->active();
   }

   void threadMgr::destroy()
   {
      LogEvent("signal to exit, program is going stopping");
      _thdMain->join();

      while (!_eventQueue.empty())
      {
         _process();
      }

      {
         std::set<thread*>& rset = _totalSet.raw();
         std::set<thread*>::iterator it = rset.begin();
         for (; rset.end() != it; ++it)
         {
            thread* thd = (*it);
            thd->join();
            delete thd;
         }
      }

      delete _thdMain;
      _thdMain = NULL;
   }

   threadMgr* threadMgr::instance()
   {
      static threadMgr mgr;
      return &mgr;
   }

   void threadMgr::idleCount(const uint maxCount)
   {
      _maxIdleCount = maxCount;
   }

   thread* threadMgr::create(const uint thdType, IThreadProductor* factory)
   {
      return factory->create(this, thdType);
   }

   void threadMgr::recycle(thread* thd)
   {
      INSPIRE_ASSERT(NULL != thd, "try to recycle a NULL thread");
      thdTask* task = thd->fetch();
      if (NULL != task)
      {
         // clean task attached in thread
         thd->assigned(NULL);
         // we should notify task manager to release task
         _taskMgr->over(task);
      }

      if (!thd->detached())
      {
         // signal to manager to destroy the thread
         postEvent(EVENT_THREAD_RELEASE, thd);
         // suspend the thread
         thd->suspend();
      }
   }

   bool threadMgr::postEvent(const char t, void* pObj)
   {
      INSPIRE_ASSERT(EVENT_DUMMY < t && t < EVENT_THREAD_UPBOUND,
                     "notify with an dummy or unknown type: %d", t);
      INSPIRE_ASSERT(NULL != pObj, "notify with invalid object");
      if (!_thdMain->running() && EVENT_DISPATCH_TASK == t)
      {
         thdTask* task = (thdTask*)pObj;
         LogError("a exit signal received, do not accept task dispatch event "
                  "any more, task id: %lld, name:[%s]", task->id(), task->name());
      }
      else
      {
         thdEvent ev(t, pObj);
         _eventQueue.push_back(ev);
         return true;
      }

      return false;
   }

   bool threadMgr::postEvent(thdTask* task)
   {
      return postEvent(EVENT_DISPATCH_TASK, task);
   }

   thread* threadMgr::_create()
   {
      int rc = 0;
      thread* thd = _acquire();
      if (NULL == thd)
      {
         thd = new thread(this, false);
         if (NULL == thd)
         {
            LogError("failed to create thread, out of memory");
            // if hit there, log had been written
            return NULL;
         }
         LogEvent("allocate a thread object");
      }

      rc = thd->create();
      if (rc)
      {
         LogError("create thread failed in start thread");
         return NULL;
      }
      // let's record it
      // if object is existed already, it cannot be inserted
      _totalSet.insert(thd);
      return thd;
   }

   void threadMgr::_detach(thread* thd)
   {
      LogEvent("detach thread: %lld from manager map", thd->tid());
      _totalSet.erase(thd);
      thd->detach();
   }

   void threadMgr::_process()
   {
      thdEvent ev;
      if (_eventQueue.pop_front(ev))
      {
         switch (ev.evType)
         {
         case EVENT_DISPATCH_TASK:
         {
            thdTask* task = (thdTask*)ev.evObject;
            _dispatch(task);
         }
         break;
         case EVENT_THREAD_SUSPEND:
         {
            thread* thd = (thread*)ev.evObject;
            thd->suspend();
            break;
         }
         case EVENT_THREAD_RUNNING:
         {
            thread* thd = (thread*)ev.evObject;
            thd->active();
         }
         break;
         case EVENT_THREAD_RESUME:
         {
            thread* thd = (thread*)ev.evObject;
            thd->resume();
         }
         break;
         case EVENT_THREAD_RELEASE:
         {
            thread* thd = (thread*)ev.evObject;
            _release(thd);
         }
         break;
         case EVENT_DUMMY:
         default:
            LogError("receive a dummy or unknown event, type: %d", ev.evType);
            break;
         }
      }
      else
      {
         utilSleep(100);
      }
   }

   thread* threadMgr::_fetchIdle()
   {
      thread* thd = NULL;
      if (_idleQueue.pop_front(thd))
      {
         if (THREAD_IDLE != thd->state())
         {
            // when a thread pooled and pushed to idle queue
            // its state may not be THREAD_IDLE
            // so we need to fetch next to support request
            _enIdle(thd);
         }
         else
         {
            LogEvent("fetch a idle thread from idle queue, thread id: %lld", thd->tid());
            return thd;
         }
      }
      return thd;
   }

   

   void threadMgr::_enIdle(thread* thd)
   {
      _idleQueue.push_back(thd);
   }

   void threadMgr::_release(thread* thd)
   {
      INSPIRE_ASSERT(NULL != thd, "try to recycle a NULL thread");
      if (_thdMain->running() && _idleQueue.size() < _maxIdleCount)
      {
         // push the thread to idle
         LogEvent("recycle a thread into idle queue, thread id: %lld", thd->tid());
         _enIdle(thd);
      }
      else
      {
         // join thread until it exit
         thd->join();
         // now the thread object didn't contains real thread process function
         // we store it into thread queue
         // until it acquired and start new thread process for next use
         _thdQueue.push_back(thd);
      }
   }

   thread* threadMgr::_acquire()
   {
      thread* thd = NULL;
      if (_thdQueue.pop_front(thd))
      {
         LogEvent("fetch a thread object from thread queue");
         return thd;
      }

      return NULL;
   }

   void threadMgr::_dispatch(thdTask* task)
   {
      INSPIRE_ASSERT(NULL != task, "try to despatch a NULL task");
      if (task)
      {
         thread* thd = _fetchIdle();
         if (NULL == thd)
         {
            thd = _create();
            if (NULL == thd)
            {
               LogError("cannot allocate a new thread object");
               // we failed to allocate a thread object,
               // now we should insert the task into head of task queue 
               // postEvent(EVENT_DISPATCH_TASK, task);
               thdEvent ev(EVENT_DISPATCH_TASK, task);
               _eventQueue.push_front(ev);
               return;
            }
         }

         LogEvent("dispatch task: %lld to thread: %lld", task->id(), thd->tid());
         thd->assigned(task);
         thd->active();
      }
   }

   

}