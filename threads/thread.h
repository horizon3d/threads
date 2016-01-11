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
#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "inspire.h"
#include "util/system/condition.h"
#include "util/assert.h"

namespace inspire {

   enum thdState
   {
      THREAD_INVALID = 1 << 0,
      THREAD_RUNNING = 1 << 1,
      THREAD_STOPPED = 1 << 2,
      THREAD_IDLE    = 1 | THREAD_RUNNING, // it means thread is suspended
   };

   class threadMgr;
   class thdTask;
   class thread
   {
      friend class threadMgr;
#ifdef _WINDOWS
      static unsigned __stdcall ENTRY_POINT(void* arg);
#else
      static void* ENTRY_POINT(void* arg);
#endif
   protected:
      thread(threadMgr* mgr, bool detach = true);

   public:
      virtual ~thread();

      int64 tid() { return _tid; }
      
      threadMgr* thdMgr() const { return _threadMgr; }
      /*
      * get the state of thread
      */
      const char state() const { return _state; }
      /*
      * assign a task to thread to handle
      */
      void assigned(thdTask* task) { _task = task; }
      /*
      * fetch the task assigned to that thread
      */
      thdTask* fetch() const { return _task; }
      /*
      * set error code if error occurs
      */
      void error(const int err) { _errno = err; }
      /*
      * get the error code
      */
      const int error() const { return _errno; }
      /*
      * check the thread is detached or not
      */
      bool detached() const { return _detach; }
      /*
      * check the running state of thread
      */
      bool running() const { return THREAD_RUNNING == _state; }

   public:
      void active();
      void suspend();
      void resume();
#ifndef _WINDOWS
      bool wait(uint seconds = 0);
#endif
      /*
      * join thread and let operation system recycle the kernel resource
      * every thread need to call join before free
      */
      void join();
      /*
      * deactive the thread, the manager will decide it pooled or free
      * and the status of the thread will be changed to IDLE, if the thread
      * is not detached, or you still need to manage it
      */
      void deactive();

   protected:
      bool _valid() const
      {
#ifdef _WINDOWS
         return INVALID_HANDLE_VALUE != _hThread;
#else
         return (int64)_ntid != -1;
#endif
      }

      void _setstate(char st) { _state = st; }

      void _reset();

      /*
      * break the relation of thread manager
      * it may make the thread to be a user defined thread
      * it means the thread manager don't manager it any more
      * you should free it when its work over
      */
      void detach() { _detach = true; }

      /*
      * start thread and initialize thread process handling function
      */
      int  create();

   private:
      thread(const thread& rhs);
      thread& operator= (const thread& rhs);

   protected:
      char        _state;
      bool        _detach;
      int         _errno;
      threadMgr*  _threadMgr;
      thdTask*    _task;
#ifdef _WINDOWS
      HANDLE      _hThread;
#else
      pthread_t       _ntid;
      pthread_mutex_t _mtx;
      pthread_cond_t  _cond;
#endif
      int64       _tid;
   };
}
#endif // _INSPIRE_THREAD_ENTITY_H_
