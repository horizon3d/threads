#include "thdEntity.h"
#include <process.h>
#include "thdTask.h"
#include "thdMgr.h"
namespace inspire {

   threadEntity::threadEntity(threadMgr* mgr) : _thdMgr(mgr)
   {
   }

   threadEntity::~threadEntity()
   {
      if (_hThread)
      {
         stop();
      }

      _task = NULL;
      _thdMgr = NULL;
   }

   int threadEntity::initialize()
   {
      int rc = 0;
#ifdef _WINDOWS
      unsigned threadId = 0;
      _hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntity::ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      if (INVALID_HANDLE_VALUE == _hThread)
      {
         LogError("Failed to start a thread, error: %d", FetchLastError());
         rc = FetchLastError();
         return rc;
      }
#else
      pthread_t t;
      rc = pthread_create(&t, NULL, threadEntity::ENTRY_POINT, this);
      if (rc)
      {
         LogError("Failed to start a thread, error: %d", FetchLastError());
         rc = FetchLastError();
         return rc;
      }
#endif
      state(THREAD_IDLE);
      _tid = threadId;
      _task = NULL;
      return rc;
   }

   void threadEntity::active()
   {
      state(THREAD_RUNNING);
      ::ResumeThread(_hThread);
   }

   void threadEntity::suspend()
   {
      state(THREAD_IDLE);
      ::SuspendThread(_hThread);
   }

   void threadEntity::resume()
   {
      state(THREAD_RUNNING);
      ::ResumeThread(_hThread);
   }

   int threadEntity::join()
   {
      int rc = 0;
      _task->attach(this);
      rc = _task->run();
      error(rc);
      _task->detach();
      return rc;
   }

   int threadEntity::stop()
   {
      if (THREAD_IDLE == _state)
      {
         state(THREAD_STOPPED);
         // if thread is suspended, we should state it stopped and active it
         active();
      }
      else if (THREAD_STOPPED != _state)
      {
         state(THREAD_STOPPED);
      }

      return 0;
   }

   int threadEntity::kill()
   {
      ::_endthreadex(-1);
      return 0;
   }

#ifdef _WINDOWS
   unsigned __stdcall threadEntity::ENTRY_POINT(void* arg)
#else
   void* threadEntity::ENTRY_POINT(void* arg)
#endif
   {
      threadEntity* entity = static_cast<threadEntity*>(arg);
      if (entity)
      {
         threadMgr* mgr = entity->thdMgr();
         // assert mgr not NULL
         while (THREAD_RUNNING == entity->state())
         {
            int rc = 0;
            thdTask* task = entity->fetch();
            if (NULL == task)
               continue;
            // keep task to the entity
            // so that we can catch information when handling task
            task->attach(entity);
            rc = task->run();
            if (rc)
            {
               entity->error(rc);
            }
            // now we clean task assigned to entity
            task->detach();
            mgr->deactive(entity);
         }
         entity->close();
      }
#ifdef _WINDOWS
      return 0;
#else
      return NULL;
#endif
   }

}