#include "thdMgr.h"

namespace inspire {

   const int thdMgr::thdInnerTask::run()
   {
      LogEvent("starting MAIN PROCESS LOOP: %s", name());
      while (_thd->running())
      {
         _thdMgr->_process();
      }

      LogEvent("ending MAIN PROCESS LOOP: %s", name());
      return _thd->error();
   }

   thdMgr* thdMgr::instance()
   {
      static thdMgr mgr;
      return &mgr;
   }

   thdMgr::thdMgr() : _maxIdleCount(10)
   {
      _taskMgr = thdTaskMgr::instance();
   }

   thdMgr::~thdMgr()
   {
      _taskMgr = NULL;
   }

   void thdMgr::initialize()
   {
      thdTask* t = new thdInnerTask(this);
      STRONG_ASSERT(NULL != t, "Failed to allocate event processing task");

      thread* thd = create();
      STRONG_ASSERT(NULL != thd, "cannot start event processing thread, exit");

      detach(thd);
      _thdMain = thd;
      _thdMain->assigned(t);
   }

   void thdMgr::active()
   {
      STRONG_ASSERT(NULL != _thdMain, "event processing thread is NULL");
      _thdMain->active();
   }

   void thdMgr::destroy()
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

   void thdMgr::_process()
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

   void thdMgr::reverseIdleCount(const uint maxCount)
   {
      _maxIdleCount = maxCount;
   }

   thread* thdMgr::_fetchIdle()
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

   thread* thdMgr::create()
   {
      int rc = 0;
      thread* thd = _acquire();
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
         postEvent(EVENT_THREAD_RELEASE, thd);
         // suspend the thread
         thd->suspend();
      }
   }

   bool thdMgr::postEvent(const char t, void* pObj)
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

   bool thdMgr::postEvent(thdTask* task)
   {
      return postEvent(EVENT_DISPATCH_TASK, task);
   }

   void thdMgr::_enIdle(thread* thd)
   {
      _idleQueue.push_back(thd);
   }

   void thdMgr::_release(thread* thd)
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

   thread* thdMgr::_acquire()
   {
      thread* thd = NULL;
      if (_thdQueue.pop_front(thd))
      {
         LogEvent("fetch a thread object from thread queue");
         return thd;
      }

      return NULL;
   }

   void thdMgr::_dispatch(thdTask* task)
   {
      INSPIRE_ASSERT(NULL != task, "try to despatch a NULL task");
      if (task)
      {
         thread* thd = _fetchIdle();
         if (NULL == thd)
         {
            thd = create();
            if (NULL == thd)
            {
               LogError("cannot allocate a new thread object");
               // we failed to allocate a thread object,
               // now we should push the task into task queue
               postEvent(EVENT_DISPATCH_TASK, task);
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
      _totalSet.erase(thd);
      thd->detach();
   }

}