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

   void threadMgr::push(threadEntity* entity)
   {
      _idleQueue.push_back(entity);
   }

   void threadMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
   }

   thdTask* threadMgr::fetch()
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
      threadEntity* entity = new threadEntity(this);
      if (NULL == entity)
      {
         LogError("Failed to create thread entity, out of memory");
         return NULL;
      }
      // insert into idle ?
      rc = entity->initialize();
      if (rc)
      {
         return NULL;
      }

      _thdMap.insert(entity->tid(), entity);
      push(entity);

      return entity;
   }

   void threadMgr::deactive(threadEntity* entity)
   {
      recycle(entity);
   }

   void threadMgr::recycle(threadEntity* entity)
   {
      if ( _idleQueue.size() < _maxPooledCount)
      {
         entity->suspend();
      }
      else
      {
         entity->state(THREAD_STOPPED);
      }
   }

   int threadMgr::_createEntity(bool worker, threadEntity*& entity)
   {
      
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