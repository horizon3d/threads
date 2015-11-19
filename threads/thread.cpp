#include "thread.h"
#include "thdMgr.h"
#include "thdTask.h"

namespace inspire {

   thread::thread(thdMgr* mgr)
      : _state(THREAD_INVALID), _errno(0), _thdMgr(mgr), _task(NULL)
   {
      STRONG_ASSERT(NULL != mgr, "thdMgr cannot be NULL");
      state(THREAD_IDLE);
   }

   thread::~thread()
   {
      _errno  = 0;
      _state  = THREAD_INVALID;
      _task   = NULL;
      _thdMgr = NULL;
   }

   int thread::create()
   {
      int rc = 0;
      state(THREAD_IDLE);
#ifdef _WINDOWS
      unsigned threadId = 0;
      _hThread = (HANDLE)_beginthreadex(NULL, 0, thread::ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      if (INVALID_HANDLE_VALUE == _hThread)
      {
         rc = utilGetLastError();
         LogError("Failed to start a thread, error: %d", rc);
         return rc;
      }
      _tid = threadId;
#else
      pthread_t ntid;
      rc = pthread_create(&ntid, NULL, thread::ENTRY_POINT, this);
      if (rc)
      {
         LogError("Failed to start a thread, error: %d", rc);
         return rc;
      }
      _tid = (int64)ntid;
#endif
      return rc;
   }

   void thread::active()
   {
      state(THREAD_RUNNING);
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         ::ResumeThread(_hThread);
      }
#else
      pthread_mutex_unlock(&_mtx);
      pthread_cond_signal(_cond);
#endif
   }

   void thread::suspend()
   {
      state(THREAD_IDLE);
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         ::SuspendThread(_hThread);
      }
#else
      pthread_mutex_lock(&_mtx);
#endif
   }

   void thread::resume()
   {
      active();
   }

   void thread::stop()
   {
      state(THREAD_STOPPED);
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         ::ResumeThread(_hThread);
      }
#else
      pthread_mutex_unlock(&_mtx);
      pthread_cond_signal(_cond);
#endif
   }

   void thread::join()
   {
      state(THREAD_STOPPED);
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         ::ResumeThread(_hThread);
         DWORD dw = ::WaitForSingleObject(_hThread, INFINITE);
         if (WAIT_OBJECT_0 == dw || WAIT_ABANDONED == dw)
         {
            ::CloseHandle(_hThread);
            _hThread = NULL;
         }
         else
         {
            _errno = utilGetLastError();
            LogError("thread ending with error, errno: %d", _errno);
         }
      }
#else
      int ret = &_errno;
      int ntid = (pthread_t)_tid;
      pthread_cancle(ntid);
      pthread_join(ntid, &ret);
#endif
   }

#ifdef _WINDOWS
   unsigned __stdcall thread::ENTRY_POINT(void* arg)
   {
      INSPIRE_ASSERT(NULL != arg, "Thread addition parameter cannot be NULL");
      LogEvent("Thread starting");

      thread* thd = static_cast<thread*>(arg);
      if (thd)
      {
         int rc = 0;
         thdMgr* mgr = thd->threadMgr();
         INSPIRE_ASSERT(NULL != mgr, "Thread manager is NULL, panic");
         while (THREAD_RUNNING == thd->state())
         {
            thdTask* task = thd->fetch();
            INSPIRE_ASSERT(NULL != task, "fetch a task point to NULL");
            // keep task to the entity
            // so that we can catch information when handling task
            task->attach(thd);
            rc = task->run();
            if (rc)
            {
               thd->error(rc);
            }
            // now we clean task assigned to entity
            task->detach();
            mgr->deactive(thd);
         }
         thd->join();
      }
      LogEvent("Thread endding");
      return thd->error();
   }
#else
   void* thread::ENTRY_POINT(void* arg)
   {
      INSPIRE_ASSERT(NULL != arg, "Thread addition parameter cannot be NULL");
      LogEvent("Thread starting");
      int rc = 0;
      thread* thd = static_cast<thread*>(arg);
      if (thd)
      {
         thdMgr* mgr = entity->thdMgr();
         STRONG_ASSERT(NULL != mgr, "Thread manager is NULL, panic");

         while (THREAD_RUNNING &= entity->state())
         {
            // Linux do not support suspend
            // so we should wait until receive a signal
            // TODO:
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
         }
         thd->join();
         rc = entity->error();
      }
      LogEvent("Thread endding");
      return NULL;
   }
#endif
}