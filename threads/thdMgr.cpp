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
      // lock idle queue
      threadEntity* entity = _idleQueue.front();
      _idleQueue.pop_front();
      return entity;
   }

   void threadMgr::pushIdle(threadEntity* entity)
   {
      // lock _idle queue
      _idleQueue.push_back(entity);
      // lock work queue
      release(entity->tid());
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
      // lock map
      _thdMap.insert(std::make_pair(entity->tid(), entity));
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
            rc = 
         }
      }
   }

   int threadMgr::release(const int64& id)
   {
      int rc = 0;
      std::map<int64, threadEntity*>::iterator it = _workQueue.find(id);
      if ( _workQueue.end() != it)
      {
         threadEntity* entity = it->second;
         // lock work queue
         _workQueue.erase(id);
         recycle(entity);
      }
      return rc;
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
      // lock task queue
      thdTask* task = _taskQueue.front();
      _taskQueue.pop_front();
      return task;
   }

   void threadMgr::recycle(threadEntity* entity)
   {
      if (entity->poolable())
      {
         entity->deactive();
         // lock idle queue
         pushIdle(entity);
      }
      else
      {
         entity->destroy();
         _remove(entity);
      }
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
      _thdMap.insert(std::make_pair(entity->tid(), entity));
      // lock idle queue
      _idleQueue.push_back(entity);
      return rc;
   }

   void threadMgr::_remove(threadEntity* entity)
   {
      // ASSERT(entity, "thread entity to be destructed cannot be NULL")
      std::map<int64, threadEntity*>::iterator it = _thdMap.find(entity->tid());
      if (_thdMap.end() != it)
      {
         threadEntity* entity = it->second;
         // lock work queue
         _thdMap.erase(entity->tid());
         delete entity;
         entity = NULL;
      }
   }

   threadMgr::threadMgr()
   {

   }

   threadMgr::~threadMgr()
   {
      std::map<int64, threadEntity*> _thdMap;
   }
}