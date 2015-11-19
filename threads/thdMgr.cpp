#include "thdMgr.h"
#include "thdTask.h"

namespace inspire {

   thdMgr* thdMgr::instance()
   {
      static thdMgr mgr;
      return &mgr;
   }

   int thdMgr::initialize()
   {
      return 0;
   }

   int thdMgr::destroy()
   {
      return 0;
   }

   thdMgr::thdMgr()
   {
      _taskMgr = thdTaskMgr::instance();
   }

   thdMgr::~thdMgr()
   {
      _taskMgr = NULL;
   }

   int thdMgr::process()
   {
      int rc = 0;
      if (_exit)
      {
         std::deque<thread*>& rqueue = _idleQueue.raw();
         std::deque<thread*>::iterator it = rqueue.begin();
         for (; rqueue.end() != it; ++it)
         {
            (*it)->stop();
         }
         return rc;
      }
      // process tasks
      thdTask* task = fetch();
      if (NULL != task)
      {
         thread* thd = fetchIdle();
         if (NULL == thd)
         {
            thd = create();
            if (NULL == thd)
            {
               rc = -1; // OOM
               LogError("Failed to create thread thd");
               return rc;
            }
         }
         thd->assigned(task);
         LogEvent("assigned task: %lld to thread: %lld", task->id(), thd->tid());
         thd->active();
      }
      else
      {
         ::inSleep(100);
      }

      return rc;
   }

   void thdMgr::setIdleCount(const uint maxCount)
   {
      _maxIdleCount = maxCount;
   }

   thread* thdMgr::fetchIdle()
   {
      thread* thd = NULL;
      if (_idleQueue.pop_front(thd))
      {
         if (THREAD_IDLE != thd->state())
         {
            // when a thread pooled and pushed to idle queue
            // its state may not be THREAD_IDLE
            // so we need to fetch next to support request
            enIdle(thd);
         }
         else
         {
            return thd;
         }
      }
      return thd;
   }

   void thdMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
   }

   void thdMgr::over(thdTask* task)
   {
      _taskMgr->over(task);
   }

   thread* thdMgr::create()
   {
      int rc = 0;
      thread* thd = acquire();
      if (NULL == thd)
      {
         thd =  new thread(this);
         if (NULL == thd)
         {
            LogError("Failed to create thread thd, out of memory");
            return NULL;
         }
      }

      rc = thd->create();
      if (rc)
      {
         return NULL;
      }

      return thd;
   }

   void thdMgr::deactive(thread* thd)
   {
      over(thd->fetch());
      recycle(thd);
   }

   void thdMgr::enIdle(thread* thd)
   {
      _idleQueue.push_back(thd);
   }

   thdTask* thdMgr::fetch()
   {
      thdTask* task = NULL;
      if (_taskQueue.pop_front(task))
      {
         return task;
      }
      return NULL;
   }

   void thdMgr::recycle(thread* thd)
   {
      thd->assigned(NULL);
      if (_idleQueue.size() < _maxIdleCount && !_exit)
      {
         // push the thread to idle
         enIdle(thd);
         thd->suspend();
      }
      else
      {
         // change state to stop, thread will be exit
         thd->state(THREAD_STOPPED);
         _thdQueue.push_back(thd);
      }
   }

   thread* thdMgr::acquire()
   {
      thread* thd = NULL;
      if (_thdQueue.pop_front(thd))
      {
         return thd;
      }
      return NULL;
   }
}