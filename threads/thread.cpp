#include "thread.h"
#include "thdMgr.h"
#include "task/thdTask.h"

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
      pthread_mutex_lock(&_mtx);
      pthread_cond_signal(&_cond);
      pthread_mutex_unlock(&_mtx);
#endif
      LogEvent("resume thread");
   }

   void thread::suspend()
   {
      state(THREAD_IDLE);
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         ::SuspendThread(_hThread);
      }
#endif
      LogEvent("suspend thread");
   }

   void thread::resume()
   {
      active();
      LogEvent("resume thread");
   }

   void thread::stop()
   {
      // in order to exit thread safety, we should state thread STOPPED
      // and then wake up thread to continue
      // thread will exit if it fetch NULL task
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         if (running())
         {
            state(THREAD_STOPPED);
         }
         else
         {
            state(THREAD_STOPPED);
            ::ResumeThread(_hThread);
         }
         ::CloseHandle(_hThread);
         _hThread = INVALID_HANDLE_VALUE;
      }
#else
      if (running())
      {
         state(THREAD_STOPPED);
      }
      else
      {
         state(THREAD_STOPPED);
         pthread_mutex_lock(&_mtx);
         pthread_cond_signal(&_cond);
         pthread_mutex_unlock(&_mtx);
      }
      int ret = &_errno;
      int ntid = (pthread_t)_tid;
      pthread_join(ntid, &ret);
#endif
      LogEvent("stop thread");
   }

   void thread::join()
   {
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         if (running())
         {
            state(THREAD_STOPPED);
         }
         else
         {
            state(THREAD_STOPPED);
            ::ResumeThread(_hThread);
         }
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
      if (running())
      {
         state(THREAD_STOPPED);
      }
      else
      {
         state(THREAD_STOPPED);
         pthread_mutex_lock(&_mtx);
         pthread_cond_signal(&_cond);
         pthread_mutex_unlock(&_mtx);
      }
      int ret = &_errno;
      int ntid = (pthread_t)_tid;
      pthread_join(ntid, &ret);
#endif
      state(THREAD_INVALID);
   }

#ifndef _WINDOWS
   bool thread::wait(uint seconds)
   {
      bool ok = false;
      // lock thread, and thread will wait for resume
      pthread_mutex_lock(&_mtx);
      if (seconds > 0)
      {
         struct timespec ts;
         ts.tv_sec = time(NULL) + seconds;
         ts.tv_nsec = 0;
         int rc = pthread_cond_timedwait(&_cond, &_mtx, &ts);
         if (ETIMEDOUT == rc)
         {
            return false;
         }
      }
      else
      {
         pthread_cond_wait(&_cond, &_mtx);
      }
      pthread_mutex_unlock(&_mtx);
      return true;
   }
#endif

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
            if (NULL == task)
            {
               inSleep(100);
            }
            else
            {
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
         }
      }
      LogEvent("Thread endding");
      return thd->error();
   }
#else
   void* thread::ENTRY_POINT(void* arg)
   {
      INSPIRE_ASSERT(NULL != arg, "Thread addition parameter cannot be NULL");
      LogEvent("Thread starting");
      thread* thd = static_cast<thread*>(arg);
      if (thd)
      {
         thdMgr* mgr = thd->thdMgr();
         STRONG_ASSERT(NULL != mgr, "Thread manager is NULL, panic");

         while (THREAD_RUNNING &= thd->state())
         {
            // Linux do not support suspend
            // so we should wait until receive a signal
            if (THREAD_IDLE == thd->state())
            {
               while (!thd->wait())
               {
                  // DO NOTHING and continue WAITING
               }
            }
            thdTask* task = thd->fetch();
            if (NULL == task)
            {
               inSleep(100);
            }
            else
            {
               // keep task to the thd
               // so that we can catch information when handling task
               task->attach(thd);
               int rc = task->run();
               if (rc)
               {
                  thd->error(rc);
               }
               // now we clean task assigned to thread
               task->detach();
               mgr->deactive(thd);
            }
         }
      }
      LogEvent("Thread endding");
      return NULL;
   }
#endif
}