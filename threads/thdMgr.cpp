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
            _recycle(entity);
            break;
         }
      }
      return rc;
   }

   int threadMgr::release(threadEntity* entity)
   {
      int rc = 0;
      _recycle(entity);
      return rc;
   }

   int threadMgr::dispatch(thdTask* task)
   {
      _taskQueue.push_back(task);
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

   void threadMgr::unpooled(const int64& id)
   {
      std::map<int64, threadEntity*>::iterator it = _thdMap.find(id);
      if (_thdMap.end() != it)
      {
         threadEntity* entity = it->second;
         entity->poolable(false);
      }
      // LogError
   }

   void threadMgr::unpooled(threadEntity* entity)
   {
      unpooled(entity->tid());
   }

   void threadMgr::_recycle(threadEntity* entity)
   {
      if (entity->poolable())
      {
         entity->deactive();
         _idleQueue.push_back(entity);
      }
      else
      {
         entity->destroy();
         _remove(entity);
      }
   }

   void threadMgr::_remove(threadEntity* entity)
   {
      // ASSERT(entity, "thread entity to be destructed cannot be NULL")
      delete entity;
      entity = NULL;
   }

   threadMgr::threadMgr()
   {

   }

   threadMgr::~threadMgr()
   {
      std::map<int64, threadEntity*> _thdMap;
   }

   unsigned __stdcall threadMgr::ENTRY_POINT(void* arg)
   {
      threadMgr* mgr = static_cast<threadMgr*>(arg);
      // assert mgr not NULL
      threadEntity* entity = mgr->fetchIdle();
      if (NULL != entity)
      {
         while (THREAD_RUNNING == entity->state())
         {
            thdTask* task = mgr->fetchTask();
            if (NULL == task)
            {
               Sleep(2000);
               continue;
            }

            int rc = 0;
            task->attach(entity);
            rc = task->run();
            if (rc)
            {
               //LogEvent(...);
               entity->err(rc);
            }
            task->detach();
         }
      }
   }
}