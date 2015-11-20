#include "thdMgr.h"
#include "task/thdTask.h"
#include "task/thdMgrTask.h"

namespace inspire {

   thdMgr* thdMgr::instance()
   {
      static thdMgr mgr;
      return &mgr;
   }

   thdMgr::thdMgr()
   {
      _taskMgr = thdTaskMgr::instance();
   }

   thdMgr::~thdMgr()
   {
      _taskMgr = NULL;
   }

   void thdMgr::initialize()
   {
      thdTask* t = new thdMgrTask(this);
      if (NULL == t)
      {
         LogError("Failed to allocate event processing task");
         Panic();
      }

      thread* thd = create();
      if (NULL == thd)
      {
         LogError("cannot start event processing thread, exit");
         Panic();
      }
      _mThd = thd;
      _mThd->assigned(t);
   }

   void thdMgr::active()
   {
      STRONG_ASSERT(NULL != _mThd, "event processing thread is NULL");
      _mThd->active();
   }

   void thdMgr::destroy()
   {
      _mThd->join();
      while (!_eventQueue.empty())
      {
         process();
      }
      LogEvent("event loop exit, program is going stopping");
   }

   void thdMgr::process()
   {
      thdEvent ev;
      if (_eventQueue.pop_front(ev))
      {
         switch (ev.evType)
         {
         case EVENT_DISPATCH_TASK:
            dispatch((thdTask*)ev.evObject);
            break;
         case EVENT_THREAD_SUSPEND:
            thread* thd = (thread*)ev.evObject;
            thd->suspend();
            break;
         case EVENT_THREAD_RUNNING:
            thread* thd = (thread*)ev.evObject;
            thd->active();
            break;
         case EVENT_THREAD_RESUME:
            thread* thd = (thread*)ev.evObject;
            thd->resume();
            break;
         case EVENT_THREAD_STOP:
            thread* thd = (thread*)ev.evObject;
            thd->stop();
            break;
         case EVENT_THREAD_RECYCLE:
            thread* thd = (thread*)ev.evObject;
            recycle(thd);
         case EVENT_DUMMY:
         default:
            LogError("receive a dummy or unknown event, type: %d", ev.evType);
            break;
         }
      }
      else
      {
         inSleep(100);
      }
   }

   void thdMgr::reverseIdleCount(const uint maxCount)
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
      INSPIRE_ASSERT(NULL != thd, "try to deactive a NULL thread")
      notify(EVENT_THREAD_RECYCLE, thd);
   }

   bool thdMgr::notify(const char t, void* pObj)
   {
      INSPIRE_ASSERT(EVENT_DUMMY < t && t < EVENT_THREAD_UPBOUND,
                     "notify with an dummy or unknown type: %d", t);
      INSPIRE_ASSERT(NULL != pObj, "notify with invalid object");
      thdEvent ev(t, pObj);
      if (_mThd->running())
      {
         _eventQueue.push_back(ev);
         return true;
      }
      LogError("a exit signal received, do not accept event any more");
      return false;
   }

   void thdMgr::enIdle(thread* thd)
   {
      _idleQueue.push_back(thd);
   }

   void thdMgr::recycle(thread* thd)
   {
      INSPIRE_ASSERT(NULL != thd, "try to recycle a NULL thread")
      thdTask* task = thd->fetch();
      if (NULL != task)
      {
         // we should notify task manager to release task
         _taskMgr->over(task);
         // clean task attached in thread
         thd->assigned(NULL);
      }

      if (_idleQueue.size() < _maxIdleCount)
      {
         // suspend the thread
         thd->suspend();
         // push the thread to idle
         enIdle(thd);
      }
      else
      {
         // join thread until it exit
         thd->join();
         // now the thread object didn't contains real thread process function
         // we store the thread into thread queue for next use
         _thdQueue.push_back(thd);
      }
   }

   thread* thdMgr::acquire()
   {
      thread* thd = NULL;
      if (_thdQueue.pop_front(thd))
      {
         thd->join();
         thd->state(THREAD_INVALID);
         return thd;
      }
      return NULL;
   }

   void thdMgr::dispatch(thdTask* task)
   {
      INSPIRE_ASSERT(NULL != task, "try to despatch a NULL task");
      if (task)
      {
         thread* thd = fetchIdle();
         if (NULL == thd)
         {
            thd = create();
            if (NULL == thd)
            {
               LogError("cannot allocate a new thread object");
               // Out of memory
               // TODO:
               notify(EVENT_DISPATCH_TASK, task);
            }
         }

         thd->assigned(task);
         thd->active();
      }
   }
}