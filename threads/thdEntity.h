#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "threads.h"
#include "entry/entry.h"

namespace inspire {

   enum thdState
   {
      THREAD_INVALID  = 0,
      THREAD_CREATING = 1,
      THREAD_IDLE     = 2,
      THREAD_RUNNING  = 3,
      THREAD_SUSPEND  = 4,
      THREAD_STOPPING = 5,
      THREAD_STOPPED  = 6,
   };

   class thdTask;
   class threadMgr;
   class threadEntity
   {
      static unsigned __stdcall WORKER_ENTRY_POINT(void* arg);
      static unsigned __stdcall ENTRY_POINT(void* arg);
   public:
      threadEntity(threadMgr* mgr, bool worker = false);
      virtual ~threadEntity();

      int initialize();
      int active();
      int suspend();
      int resume();
      int join();
      int deactive();
      int destroy();
      void wait(int seconds);

      void assigned(thdTask* task) { _task = task; }

      const int64 tid() const { return _tid; }
      const int state() const { return _state; }
      void state(const int st) { _state = st; }
      bool running() const { return THREAD_RUNNING == _state; }

      bool poolable() const { return _worker; }

      const int err() const { return _errno; }
      void err(const int e) { _errno = e; }
      thdTask* task() const { return _task; }
      threadMgr* thdMgr() const { return _thdMgr; }

   private:
      int _run();

   private:
      bool       _worker;
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
