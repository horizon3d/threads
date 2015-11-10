#include "thdMgr.h"
#include "thdTask.h"

namespace inspire {

   threadMgr* threadMgr::instance()
   {
      static threadMgr mgr;
      return &mgr;
   }

   int threadMgr::initialize()
   {
      return 0;
   }

   int threadMgr::destroy()
   {
      return 0;
   }

   threadMgr::threadMgr()
   {
      _taskMgr = thdTaskMgr::instance();
   }

   threadMgr::~threadMgr()
   {
      _taskMgr = NULL;
   }

   int threadMgr::process()
   {
      int rc = 0;
      // process tasks
      thdTask* task = fetch();
      if (NULL != task)
      {
         threadEntity* entity = fetchIdle();
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
         ::Sleep(100);
      }

      return rc;
   }

   void threadMgr::setIdleCount(const uint maxCount)
   {
      _maxIdleCount = maxCount;
   }

   threadEntity* threadMgr::fetchIdle()
   {
      threadEntity* entity = NULL;
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

   void threadMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
   }

   void threadMgr::over(thdTask* task)
   {
      _taskMgr->over(task);
   }

   threadEntity* threadMgr::create()
   {
      int rc = 0;
      threadEntity* entity = acquire();
      if (NULL == entity)
      {
         entity =  new threadEntity(this);
         if (NULL == entity)
         {
            LogError("Failed to create thread entity, out of memory");
            return NULL;
         }
      }

      rc = entity->initialize();
      if (rc)
      {
         return NULL;
      }

      return entity;
   }

   void threadMgr::deactive(threadEntity* entity)
   {
      over(entity->fetch());
      recycle(entity);
   }

   void threadMgr::enIdle(threadEntity* entity)
   {
      _idleQueue.push_back(entity);
   }

   thdTask* threadMgr::fetch()
   {
      thdTask* task = NULL;
      if (_taskQueue.pop_front(task))
      {
         return task;
      }
      return NULL;
   }

   void threadMgr::recycle(threadEntity* entity)
   {
      entity->assigned(NULL);
      if (_idleQueue.size() < _maxIdleCount)
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

   threadEntity* threadMgr::acquire()
   {
      threadEntity* entity = NULL;
      if (_entityQueue.pop_front(entity))
      {
         return entity;
      }
      return NULL;
   }
}