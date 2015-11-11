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
      if (THREAD_STOPPED != _state)
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
      _tid = threadId;
#else
      pthread_t ntid;
      rc = pthread_create(&ntid, NULL, threadEntity::ENTRY_POINT, this);
      if (rc)
      {
         LogError("Failed to start a thread, error: %d", rc);
         return rc;
      }
      _tid = (int64)ntid;
#endif
      state(THREAD_IDLE);
      _task = NULL;

      return rc;
   }

   void threadEntity::active()
   {
      state(THREAD_RUNNING);
#ifdef _WINDOWS
      ::ResumeThread(_hThread);
#endif
   }

   void threadEntity::suspend()
   {
      state(THREAD_IDLE);
#ifdef _WINDOWS
      ::SuspendThread(_hThread);
#endif
   }

   void threadEntity::resume()
   {
      active();
   }

   int threadEntity::stop()
   {
      if (THREAD_IDLE == _state)
      {
         // if thread is suspended, we should state it stopped and active it
         active();
         state(THREAD_STOPPED);
      }
      else if (THREAD_STOPPED != _state)
      {
         state(THREAD_STOPPED);
      }

      int rc = 0;
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         DWORD dw = ::WaitForSingleObject(_hThread, INFINITE);
         if (WAIT_OBJECT_0 != dw)
         {
            rc = FetchLastError();
            LogError("Thread: %lld exit with error: %d", _tid, rc);
         }
      }
#else
      if (0 != _tid)
      {
         int* err = &_errno;
         rc = pthread_join(_tid, &err);
         if (rc)
         {
            LogError("Thread: %lld exit with error: %d", (int64)_tid, rc);
         }
      }
#endif

      return rc;
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
         while (THREAD_RUNNING & entity->state())
         {
#ifndef _WINDOWS
            if (THREAD_IDLE == entity->state())
            {
               ::inSleep(1000);
               continue;
            }
#endif
            int rc = 0;
            thdTask* task = entity->fetch();
            if (NULL != task)
            {
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
            else
            {
               ::inSleep(100);
               continue;
            }
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