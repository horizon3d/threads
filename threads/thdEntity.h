#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "threads.h"
#include "logger/logger.h"
namespace inspire {

   enum thdState
   {
      THREAD_INVALID  = 0,
      THREAD_IDLE     = 1, // it means thread is suspended
      THREAD_RUNNING  = 2,
      THREAD_STOPPED  = 3,
   };

   class thdTask;
   class threadMgr;
   class threadEntity
   {
      static unsigned __stdcall ENTRY_POINT(void* arg);
   public:
      threadEntity(threadMgr* mgr);
      virtual ~threadEntity();

      int initialize();
      int active();
      int suspend();
      int resume();
      int join();
      int stop();
      int kill();
      void wait(int seconds);

      void assigned(thdTask* task) { _task = task; }
      thdTask* fetch() const { return _task; }

      bool running() const { return THREAD_RUNNING == _state; }
      const int64 tid() const { return _tid; }
      HANDLE handle() const { return _hThread; }

      const int state() const { return _state; }
      void state(const int st) { _state = st;  }

      const int error() const { return _errno; }
      void error(const int e) { _errno = e; }
      
      threadMgr* thdMgr() const { return _thdMgr; }

   private:
      int        _errno;
      int        _state = THREAD_INVALID;
      int64      _tid = -1;
      HANDLE     _hThread = INVALID_HANDLE_VALUE;
      thdTask*   _task;
      threadMgr* _thdMgr;

   private:
      threadEntity(const threadEntity& rhs) = delete;
      threadEntity& operator= (const threadEntity& rhs) = delete;
   };
}
#endif
