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
      if (_idleQueue.empty())
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
      if (_workQueue.find(entity->tid()))
      {
         _workQueue.erase(entity->tid());
      }
   }

   void threadMgr::pushWorker(threadEntity* entity)
   {
      _workQueue.insert(entity->tid(), entity);
   }

   void threadMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
   }

   thdTask* threadMgr::fetchTask()
   {
      if (_taskQueue.empty())
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

      return entity;
   }

   int threadMgr::createWorker(const uint w)
   {
      int rc = 0;
      if (0 == w)
         return rc;

      for (uint idx = 0; idx < w; ++idx)
      {
         threadEntity* entity = NULL;
         rc = _createEntity(true, entity);
         if (rc)
         {
            return rc;
         }
         entity->active();
      }
      // LogError
      return rc;
   }

   void threadMgr::recycle(threadEntity* entity)
   {
      if (entity->isWorker())
      {
         // worker do not need pool to idle queue
         return;
      }

      if ( _idleQueue.size() < _maxPooledCount)
      {
         popWorker(entity);
         pushIdle(entity);
         entity->suspend();
      }
      else
      {
         entity->deactive();
         entity->destroy();
         _remove(entity);
      }
   }

   void threadMgr::suspend(threadEntity* entity)
   {
      popWorker(entity);
      entity->state(THREAD_IDLE);
      pushIdle(entity);
   }

   void threadMgr::sigExit()
   {
   }

   int threadMgr::_createEntity(bool worker, threadEntity*& entity)
   {
      int rc = 0;
      threadEntity* thd = new threadEntity(this, worker);
      if (NULL == thd)
      {
         // LogError
         // rc = -6; OOM
         return NULL;
      }
      // insert into idle ?
      rc = thd->initialize();
      if (rc)
      {
         // LogError
         return NULL;
      }

      _thdMap.insert(thd->tid(), thd);

      if (worker)
      {
         pushWorker(thd);
      }
      else
      {
         pushIdle(thd);
      }
      entity = thd;
      return rc;
   }

   void threadMgr::_remove(threadEntity* entity)
   {
      // ASSERT(entity, "thread entity to be destructed cannot be NULL")
      bool fetch = _thdMap.find(entity->tid());
      if (fetch)
      {
         _thdMap.erase(entity->tid());
      }
      // LogError thread is not create by thread manager
      delete entity;
      entity = NULL;
   }
}