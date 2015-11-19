#ifndef _INSPIRE_THREAD_H_
#define _INSPIRE_THREAD_H_

#include "threads.h"

namespace inspire {

   enum thdState
   {
      THREAD_INVALID = 1 << 0,
      THREAD_RUNNING = 1 << 1,
      THREAD_STOPPED = 1 << 2,
      THREAD_IDLE    = 1 | THREAD_RUNNING, // it means thread is suspended
   };

   class thdMgr;
   class thdTask;
   class thread
   {
#ifdef _WINDOWS
      static unsigned __stdcall ENTRY_POINT(void* arg);
#else
      static void* ENTRY_POINT(void* arg);
#endif
   public:
      thread(thdMgr* mgr);
      virtual ~thread() {}

      int64 tid() { return _tid; }
      
      thdMgr* threadMgr() const { return _thdMgr; }

      void  state(char st) { _state = st; }
      const char state() const { return _state; }
      // task
      void assigned(thdTask* task) { _task = task; }
      thdTask* fetch() const { return _task; }
      // running error
      void error(const int err) { _errno = err; }
      const int error() const { return _errno; }

   public:
      int  create();
      void active();
      void suspend();
      void resume();
      void stop();
      void join();

   private:
      thread(const thread& rhs);
      thread& operator= (const thread& rhs);

   protected:
      char       _state;
      int        _errno;
      uint64     _tid;
      thdTask*   _task;
      thdMgr* _thdMgr;
#ifdef _WINDOWS
      HANDLE _hThread = INVALID_HANDLE_VALUE;
#else
      pthread_mutex_t _mtx  = PTHREAD_MUTEX_INITIALIZER;
      pthread_cond_t  _cond = PTHREAD_COND_INITIALIZER;
#endif
   };
}
#endif
