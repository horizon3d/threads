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

      std::deque<threadEntity*>::iterator it = _idleQueue.begin();
      // INSPIRE_ASSERT(_idleQueue.end() != it)
      return *it;
   }

   int threadMgr::create(int64& id)
   {
      int rc = 0;
      threadEntity* entity = new threadEntity();
      if (entity)
      {
         // insert into idle ?
         rc = entity->initialize();
         if (rc)
         {
            // LogError
         }
         id = entity->tid();
         _idleQueue.push_back(entity);
      }
      return rc;
   }

   threadEntity* threadMgr::create()
   {
      int rc = 0;
      threadEntity* entity = new threadEntity();
      if (NULL != entity)
      {
         // LogError
         return NULL;
      }
      // insert into idle ?
      rc = entity->initialize();
      if (rc)
      {
         // LogError
         return NULL;
      }
      _thdMap.insert(std::make_pair(entity->tid(), entity));
      _idleQueue.push_back(entity);
      return entity;
   }

   int threadMgr::release(const int64& id)
   {
      int rc = 0;
      std::deque<threadEntity*>::iterator it = _workQueue.begin();
      while ( _workQueue.end() != it)
      {
         if (id == (*it)->tid())
         {
            threadEntity* entity = *it;
            entity->deactive();
            if (entity->poolable())
            {
               _idleQueue.push_back(entity);
            }
            else
            {
               delete entity;
               entity = NULL;
            }
            break;
         }
      }
      return rc;
   }

   int threadMgr::release(threadEntity* entity)
   {
      int rc = 0;
      std::deque<threadEntity*>::iterator it = _workQueue.begin();
      while (_workQueue.end() != it)
      {
         if (entity == (*it))
         {
            entity->deactive();
            if (entity->poolable())
            {
               _idleQueue.push_back(entity);
            }
            else
            {
               delete entity;
               entity = NULL;
            }
            break;
         }
      }
      return rc;
   }

   void threadMgr::pooled(const int64& id)
   {
      std::map<int64, threadEntity*>::iterator it = _thdMap.find(id);
      if (_thdMap.end() != it)
      {
         threadEntity* entity = it->second;
         entity->poolable(true);
      }
      // LogError
   }

   void threadMgr::pooled(threadEntity* entity)
   {
      pooled(entity->tid());
   }

   int threadMgr::dispatch(thdTask* task)
   {
      int rc = 0;
      threadEntity* entity = NULL;
      if (!_idleQueue.empty())
      {
         entity = *_idleQueue.begin();
      }
      _workQueue.push_back(entity);
      _idleQueue.pop_front();
      task->attach(entity);
      rc = task->run();
      task->detach();
      _idleQueue.push_back(entity);
      _workQueue.pop_back();
   }

   

}