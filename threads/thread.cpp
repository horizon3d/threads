/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 tynia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author: tynia
Date  : =========
*******************************************************************************/
#include "thread.h"
#include "threadMgr.h"
#include "task/thdTask.h"

#ifdef _WINDOWS
#include <process.h>
#else
#include <pthread.h>
#endif // _WINDOWS

namespace inspire {

   thread::thread(threadMgr* mgr, bool detach)
      : _state(THREAD_INVALID), _detach(detach), _errno(0), _threadMgr(mgr), _task(NULL), _tid(0)
   {
#ifdef _WINDOWS
      _hThread = INVALID_HANDLE_VALUE;
      _hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
      STRONG_ASSERT(INVALID_HANDLE_VALUE != _hEvent, "Failed to create mutex event");
#else
      _ntid = -1;
      pthread_mutex_init(&_mtx, NULL);
      pthread_cond_init(&_cond, NULL);
#endif
      STRONG_ASSERT(NULL != mgr, "threadMgr cannot be NULL");
   }

   thread::~thread()
   {
      join();
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         LogError("HANDLE of thread didn't release safety");
      }
      _hThread = INVALID_HANDLE_VALUE;
#else
      pthread_mutex_destroy(&_mtx);
      pthread_cond_destroy(&_cond);
#endif
   }

   void thread::active()
   {
      _setstate(THREAD_RUNNING);
#ifdef _WINDOWS
      if (INVALID_HANDLE_VALUE != _hThread)
      {
         ::SetEvent(_hEvent);
         //::ResumeThread(_hThread);
      }
#else
      pthread_mutex_lock(&_mtx);
      pthread_cond_signal(&_cond);
      pthread_mutex_unlock(&_mtx);
#endif
      LogEvent("active thread: %lld", _tid);
   }

   void thread::suspend()
   {
      LogEvent("suspend thread: %lld", _tid);
      _setstate(THREAD_IDLE);
#ifdef _WINDOWS
      //if (INVALID_HANDLE_VALUE != _hThread)
      //{
      //   ::SuspendThread(_hThread);
      //}
#endif
   }

   void thread::resume()
   {
      active();
      LogEvent("resume thread: %lld", _tid);
   }

#ifdef _WINDOWS
   bool thread::wait_util()
   {
      // lock thread, and thread will wait for resume
      ::ResetEvent(_hEvent);
      ::WaitForSingleObject(_hEvent, INFINITE);
      return true;
   }
#else
   bool thread::wait_util()
   {
      // lock thread, and thread will wait for resume
      pthread_mutex_lock(&_mtx);
      pthread_cond_wait(&_cond, &_mtx);
      pthread_mutex_unlock(&_mtx);
      return true;
   }
#endif

   void thread::join()
   {
      if (running())
      {
         LogEvent("join thread: %lld to exit", _tid);
         _setstate(THREAD_STOPPED);
      }

      if (_valid())
      {
         // in order to exit thread safety, we should state thread STOPPED if it is idle
         // and then wake up thread to continue
         // thread under Linux will exit after it fetch a NULL task
#ifdef _WINDOWS
         if (THREAD_IDLE == state())
         {
            _setstate(THREAD_STOPPED);
            LogEvent("awake thread: %lld to exit", _tid);
            //::ResumeThread(_hThread);
            ::SetEvent(_hEvent);
         }

         DWORD dw = ::WaitForSingleObject(_hThread, INFINITE);
         if (WAIT_OBJECT_0 == dw || WAIT_ABANDONED == dw)
         {
            ::CloseHandle(_hThread);
            _hThread = INVALID_HANDLE_VALUE;
         }
         else
         {
            _errno = utilGetLastError();
            LogError("thread ending with error, errno: %d", _errno);
         }
#else
         if (THREAD_IDLE == state())
         {
            _setstate(THREAD_STOPPED);
            pthread_mutex_lock(&_mtx);
            pthread_cond_signal(&_cond);
            pthread_mutex_unlock(&_mtx);
         }
         pthread_join(_ntid, NULL);
         _ntid = -1;
#endif
      }
      _reset();
   }

   void thread::deactive()
   {
      _threadMgr->recycle(this);
   }

   void thread::_reset()
   {
      _setstate(THREAD_INVALID);
      _detach = false;
      _tid    = 0;
      _task   = NULL;
   }

   int thread::create()
   {
      int rc = 0;
      _setstate(THREAD_IDLE);
#ifdef _WINDOWS
      unsigned threadId = 0;
      _hThread = (HANDLE)_beginthreadex(NULL, 0, thread::ENTRY_POINT, this, 0/*CREATE_SUSPENDED*/, &threadId);
      if (INVALID_HANDLE_VALUE == _hThread)
      {
         rc = utilGetLastError();
         LogError("Failed to start a thread, error: %d", rc);
         return rc;
      }
      _tid = threadId;
#else
      rc = pthread_create(&_ntid, NULL, thread::ENTRY_POINT, this);
      if (rc)
      {
         LogError("Failed to start a thread, error: %d", rc);
         return rc;
      }
      _tid = (int64)_ntid;
#endif
      return rc;
   }

#ifdef _WINDOWS
   unsigned __stdcall thread::ENTRY_POINT(void* arg)
   {
      INSPIRE_ASSERT(NULL != arg, "Thread addition parameter cannot be NULL");
      LogDebug("Thread starting");

      thread* thd = static_cast<thread*>(arg);
      if (thd)
      {
         int rc = 0;
         threadMgr* mgr = thd->thdMgr();
         INSPIRE_ASSERT(NULL != mgr, "Thread manager is NULL, panic");
         while (THREAD_RUNNING == thd->state())
         {
            if (THREAD_IDLE == thd->state())
            {
               while (!thd->wait_util())
               {
                  // DO NOTHING and continue WAITING
               }
            }

            thdTask* task = thd->fetch();
            if (NULL == task)
            {
               utilSleep(100);
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
               thd->deactive();
            }
         }
      }
      LogDebug("Thread endding");
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
         threadMgr* mgr = thd->thdMgr();
         STRONG_ASSERT(NULL != mgr, "Thread manager is NULL, panic");

         while (THREAD_RUNNING & thd->state())
         {
            // Linux do not support suspend
            // so we should wait until receive a signal
            if (THREAD_IDLE == thd->state())
            {
               while (!thd->wait_util())
               {
                  // DO NOTHING and continue WAITING
               }
            }
            thdTask* task = thd->fetch();
            if (NULL == task)
            {
               utilSleep(100);
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
               thd->deactive();
            }
         }
      }
      LogEvent("Thread endding");
      return NULL;
   }
#endif
}