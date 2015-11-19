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

   int threadEntity::create()
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
      rc = pthread_create(&_ntid, NULL, threadEntity::ENTRY_POINT, this);
      if (rc)
      {
         LogError("Failed to start a thread, error: %d", rc);
         return rc;
      }
      state(THREAD_IDLE);
      _tid = (int64)_ntid;
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
#else
      pthread_mutex_unlock(&_mtx);
      pthread_cond_signal(_cond);
#endif
   }

   void threadEntity::suspend()
   {
      state(THREAD_IDLE);
#ifdef _WINDOWS
      ::SuspendThread(_hThread);
#else
      pthread_mutex_lock(&_mtx);
#endif
   }

   void threadEntity::resume()
   {
      active();
   }

   void threadEntity::stop()
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
   }

   void threadEntity::join()
   {
      state(THREAD_STOPPED);
#ifdef _WINDOWS
      ::TerminateThread(_hThread, 0);
      DWORD dw = ::WaitForSingleObject(_hThread, INFINITE);
      if (WAIT_OBJECT_0 == dw || WAIT_ABANDONED == dw)
      {
         ::CloseHandle(_hThread);
         _hThread = NULL;
      }
      else
      {
         LogError("thread ending with error");
         error(FetchLastError());
      }
#else
      int ret = &_errno;
      pthread_cancle(_ntid);
      pthread_join(_ntid, &ret);
#endif
   }

#ifdef _WINDOWS
   unsigned __stdcall threadEntity::ENTRY_POINT(void* arg)
#else
   void* threadEntity::ENTRY_POINT(void* arg)
#endif
   {
      LogEvent("Thread starting");
      threadEntity* entity = static_cast<threadEntity*>(arg);
      if (entity)
      {
#ifndef _WINDOWS
         entity->suspend();
#endif
         threadMgr* mgr = entity->thdMgr();
         // assert mgr not NULL
         while (THREAD_RUNNING & entity->state())
         {
#ifndef _WINDOWS
            if (THREAD_IDLE == entity->state())
            {
               inSleep(1000);
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
      }
      entity->join();
      LogEvent("Thread endding");
#ifdef _WINDOWS
      return entity->error();
#else
      return NULL;
#endif
   }
}