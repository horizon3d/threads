#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "threads.h"
#include "logger/logger.h"

namespace inspire {

   enum thdState
   {
      THREAD_INVALID  = 1 << 0,
      THREAD_RUNNING  = 1 << 1,
      THREAD_STOPPED  = 1 << 2,
      THREAD_IDLE     = 1 | THREAD_RUNNING, // it means thread is suspended
   };

   class thdTask;
   class threadMgr;
   class threadEntity
   {
#ifdef _WINDOWS
      static unsigned __stdcall ENTRY_POINT(void* arg);
#else
      static void* ENTRY_POINT(void* arg);
#endif
   public:
      threadEntity(threadMgr* mgr);
      virtual ~threadEntity();

      int initialize();
      void active();
      void suspend();
      void resume();
      int  stop();

      void assigned(thdTask* task) { _task = task; }
      thdTask* fetch() const { return _task; }

      bool running() const { return THREAD_RUNNING == _state; }
      const int64 tid() const { return _tid; }

      HANDLE handle() const { return _hThread; }
      void   close()
      {
#ifdef _WINDOWS
         if (_hThread)
         {
            ::CloseHandle(_hThread);
            _hThread = INVALID_HANDLE_VALUE;
         }
#else
#endif
      }

      const int state() const { return _state; }
      void state(const int st) { _state = st;  }

      const int error() const { return _errno; }
      void error(const int e) { _errno = e; }

      threadMgr* thdMgr() const { return _thdMgr; }

   private:
      int        _errno;
      int        _state = THREAD_INVALID;
      thdTask*   _task;
      threadMgr* _thdMgr;
#ifdef _WINDOWS
      HANDLE     _hThread = INVALID_HANDLE_VALUE;
      int64      _tid = -1;
#else
      pthread_t  _tid;
      pthread_cond_t _cond;
      pthread_mutex_t _mtx;
#endif

   private:
      threadEntity(const threadEntity& rhs);
      threadEntity& operator= (const threadEntity& rhs);
   };
}
#endif
