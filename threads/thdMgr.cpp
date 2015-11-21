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
      _mThd = NULL;

//       {
//          std::deque<thread*>& rqueue = _idleQueue.raw();
//          std::deque<thread*>::iterator it = rqueue.begin();
//          for (; rqueue.end() != it; ++it)
//          {
//             thread* thd = *it;
//             thd->join();
//             delete thd;
//          }
//       }
// 
//       {
//          std::deque<thread*>& rqueue = _thdQueue.raw();
//          std::deque<thread*>::iterator it = rqueue.begin();
//          for (; rqueue.end() != it; ++it)
//          {
//             thread* thd = *it;
//             delete thd;
//          }
//       }
   }

   void thdMgr::initialize()
   {
      thdTask* t = new thdMgrTask(this);
      STRONG_ASSERT(NULL != t, "Failed to allocate event processing task");

      thread* thd = create();
      STRONG_ASSERT(NULL != thd, "cannot start event processing thread, exit");

      detach(thd);
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
      LogEvent("signal to exit, program is going stopping");
      _mThd->join();

      while (!_eventQueue.empty())
      {
         process();
      }

      {
         std::map<int64, thread*>& rmap = _thdMap.raw();
         std::map<int64, thread*>::iterator it = rmap.begin();
         for (; rmap.end() != it; ++it)
         {
            thread* thd = it->second;
            thd->join();
            delete thd;
         }
      }

      delete _mThd;
      _mThd = NULL;
   }

   void thdMgr::process()
   {
      thdEvent ev;
      if (_eventQueue.pop_front(ev))
      {
         switch (ev.evType)
         {
         case EVENT_DISPATCH_TASK:
         {
            thdTask* task = (thdTask*)ev.evObject;
            dispatch(task);
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
            release(thd);
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
            LogEvent("fetch a idle thread from idle queue, thread id: %lld", thd->tid());
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
         thd = new thread(this);
         if (NULL == thd)
         {
            LogError("failed to create thread, out of memory");
            // if hit there, log had been written
            return NULL;
         }
         LogEvent("allocate a thread object");
         // we should remove it from thdMap
         _thdMap.erase(thd->tid());
      }

      rc = thd->create();
      if (rc)
      {
         LogError("create thread failed in start thread");
         return NULL;
      }
      // let's record it
      _thdMap.insert(thd->tid(), thd);
      return thd;
   }

   void thdMgr::recycle(thread* thd)
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
         notify(EVENT_THREAD_RELEASE, thd);
         // suspend the thread
         thd->suspend();
      }
   }

   bool thdMgr::notify(const char t, void* pObj)
   {
      INSPIRE_ASSERT(EVENT_DUMMY < t && t < EVENT_THREAD_UPBOUND,
                     "notify with an dummy or unknown type: %d", t);
      INSPIRE_ASSERT(NULL != pObj, "notify with invalid object");
      if (!_mThd->running() && EVENT_DISPATCH_TASK == t)
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

   void thdMgr::enIdle(thread* thd)
   {
      _idleQueue.push_back(thd);
   }

   void thdMgr::release(thread* thd)
   {
      INSPIRE_ASSERT(NULL != thd, "try to recycle a NULL thread");
      if (_mThd->running() && _idleQueue.size() < _maxIdleCount)
      {
         // push the thread to idle
         LogEvent("recycle a thread into idle queue, thread id: %lld", thd->tid());
         enIdle(thd);
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

   thread* thdMgr::acquire()
   {
      thread* thd = NULL;
      if (_thdQueue.pop_front(thd))
      {
         LogEvent("fetch a thread object from thread queue");
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

         LogEvent("dispatch task: %lld to thread: %lld", task->id(), thd->tid());
         thd->assigned(task);
         thd->active();
      }
   }

   void thdMgr::detach(thread* thd)
   {
      LogEvent("detach thread: %lld from manager map", thd->tid());
      _thdMap.erase(thd->tid());
      thd->detach();
   }

}