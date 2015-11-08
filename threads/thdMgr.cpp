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

   void threadMgr::setIdleCount(const uint maxCount)
   {
      _maxIdleCount = maxCount;
   }

   threadEntity* threadMgr::fetchIdle()
   {
      if (_idleQueue.empty())
      {
         return NULL;
      }

      threadEntity* entity = _idleQueue.pop();
      while (THREAD_IDLE != entity->state())
      {
         // when a thread pooled and pushed to idle queue
         // its state may not be THREAD_IDLE
         // so we need to fetch next to support request
         push(entity);
         entity = _idleQueue.pop();
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

   void threadMgr::push(threadEntity* entity)
   {
      _idleQueue.push_back(entity);
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

   void threadMgr::recycle(threadEntity* entity)
   {
      if ( _idleQueue.size() < _maxIdleCount)
      {
         // push the thread to idle
         push(entity);
         entity->suspend();
      }
      else
      {
         // change state to stop, thread will be exit
         entity->state(THREAD_STOPPED);
         delete entity;
         entity = NULL;
      }
   }

   int threadMgr::destroy(threadEntity* entity)
   {
      int rc = entity->error();
      DWORD dw = ::WaitForSingleObject(entity->handle(), INFINITE);
      if (WAIT_OBJECT_0 != dw)
      {
         LogError("Failed to stop thread in soon");
         entity->kill();
      }
      return rc;
   }
}