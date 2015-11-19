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
         thread* entity = fetchIdle();
         if (NULL == entity)
         {
            entity = create();
            if (NULL == entity)
            {
               rc = -1; // OOM
               LogError("Failed to create thread entity");
               return rc;
            }
         }
         entity->assigned(task);
         LogEvent("assigned task: %lld to thread: %lld", task->id(), entity->tid());
         entity->active();
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
      thread* entity = NULL;
      if (_idleQueue.pop_front(entity))
      {
         if (THREAD_IDLE != entity->state())
         {
            // when a thread pooled and pushed to idle queue
            // its state may not be THREAD_IDLE
            // so we need to fetch next to support request
            enIdle(entity);
         }
         else
         {
            return entity;
         }
      }
      return entity;
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
      thread* entity = acquire();
      if (NULL == entity)
      {
         entity =  new thread(this);
         if (NULL == entity)
         {
            LogError("Failed to create thread entity, out of memory");
            return NULL;
         }
      }

      rc = entity->create();
      if (rc)
      {
         return NULL;
      }

      return entity;
   }

   void thdMgr::deactive(thread* entity)
   {
      over(entity->fetch());
      recycle(entity);
   }

   void thdMgr::enIdle(thread* entity)
   {
      _idleQueue.push_back(entity);
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

   void thdMgr::recycle(thread* entity)
   {
      entity->assigned(NULL);
      if (_idleQueue.size() < _maxIdleCount && !_exit)
      {
         // push the thread to idle
         enIdle(entity);
         entity->suspend();
      }
      else
      {
         // change state to stop, thread will be exit
         entity->state(THREAD_STOPPED);
         _entityQueue.push_back(entity);
      }
   }

   thread* thdMgr::acquire()
   {
      thread* entity = NULL;
      if (_entityQueue.pop_front(entity))
      {
         return entity;
      }
      return NULL;
   }
}