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

   threadEntity* threadMgr::fetchIdle()
   {
      if (!_idleQueue.size())
      {
         return NULL;
      }

      threadEntity* entity = _idleQueue.pop();
      return entity;
   }

   void threadMgr::pushIdle(threadEntity* entity)
   {
      _idleQueue.push_back(entity);
   }

   void threadMgr::popWorker(threadEntity* entity)
   {
      _workQueue.erase(entity->tid());
   }

   int threadMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
   }

   thdTask* threadMgr::fetchTask()
   {
      if (!_taskQueue.empty())
      {
         return NULL;
      }

      thdTask* task = _taskQueue.pop();
      return task;
   }

   threadEntity* threadMgr::create()
   {
      int rc = 0;
      threadEntity* entity = NULL;
      rc = _createEntity(false, entity);
      if (rc)
      {
         // LogError OOM
         return NULL;
      }
      // insert into idle ?
      rc = entity->initialize();
      if (rc)
      {
         // LogError
         return NULL;
      }

      _thdMap.insert(entity->tid(), entity);
      // lock idle queue
      _idleQueue.push_back(entity);
      return entity;
   }

   int threadMgr::createWorker(const uint w)
   {
      int rc = 0;
      if (0 == w)
         return rc;

      for (int idx = 0; idx < w; ++idx)
      {
         threadEntity* entity = NULL;
         rc = _createEntity(true, entity);
         if (rc)
         {
            // LogError
            return rc;
         }
      }
   }

   void threadMgr::recycle(threadEntity* entity)
   {
      if (entity->poolable())
      {
         // worker do not need pool to idle queue
         return;
      }

      if ( _idleQueue.size() < _maxPooledCount)
      {
         popWorker(entity);
         entity->suspend();
         pushIdle(entity);
      }
      else
      {
         entity->deactive();
         entity->destroy();
         _remove(entity);
      }
   }

   int threadMgr::suspend(threadEntity* entity)
   {
      popWorker(entity);
      entity->state(THREAD_IDLE);
      pushIdle(entity);
   }

   int threadMgr::_createEntity(bool worker, threadEntity*& entity)
   {
      int rc = 0;
      entity = new threadEntity(this, worker);
      if (NULL != entity)
      {
         // LogError
         // rc = -6; OOM
         return NULL;
      }
      // insert into idle ?
      rc = entity->initialize();
      if (rc)
      {
         // LogError
         return NULL;
      }
      // lock map
      _thdMap.insert(entity->tid(), entity);
      // lock idle queue
      _idleQueue.push_back(entity);
      return rc;
   }

   void threadMgr::_remove(threadEntity* entity)
   {
      // ASSERT(entity, "thread entity to be destructed cannot be NULL")
      threadEntity* entity = _thdMap.find(entity->tid());
      if (entity)
      {
         _thdMap.erase(entity->tid());
         delete entity;
         entity = NULL;
      }
   }
}