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
//       std::deque<threadEntity*>& rqueue = _idleQueue.raw();
//       std::map<int64, threadEntity*>& rmap = _thdMap.raw();
      return 0;
   }

   int threadMgr::destroy()
   {
      return 0;
   }

   int threadMgr::process()
   {
      int rc = 0;
      while (true)
      {
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
               }
               entity->assigned(task);
               rc = entity->active();
               if (rc)
               {
                  LogError("Get an error when processing tash: %s", task->name());
               }
            }
         }
      }
      
   }

   void threadMgr::setIdleCount(const uint maxCount)
   {
      _maxIdleCount = maxCount;
   }

   threadEntity* threadMgr::fetchIdle()
   {
      threadEntity* entity = NULL;
      while (_idleQueue.pop_front(entity))
      {
         if (THREAD_IDLE != entity->state())
         {
            // when a thread pooled and pushed to idle queue
            // its state may not be THREAD_IDLE
            // so we need to fetch next to support request
            enIdle(entity);
            continue;
         }
         else
         {
            break;
            return entity;
         }
      }
      return entity;
   }

   void threadMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
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
      // insert into idle ?
      rc = entity->initialize();
      if (rc)
      {
         return NULL;
      }

      _entityQueue.push_back(entity);

      return entity;
   }

   void threadMgr::deactive(threadEntity* entity)
   {
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
      if ( _idleQueue.size() < _maxIdleCount)
      {
         // push the thread to idle
         enIdle(entity);
         entity->suspend();
      }
      else
      {
         // change state to stop, thread will be exit
         entity->state(THREAD_STOPPED);
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

   int threadMgr::destroy(threadEntity* entity)
   {
      int rc = entity->error();
      DWORD dw = ::WaitForSingleObject(entity->handle(), 10 * 1000);
      if (WAIT_OBJECT_0 != dw)
      {
         LogError("Failed to stop thread in soon");
         entity->kill();
      }
      return rc;
   }
}