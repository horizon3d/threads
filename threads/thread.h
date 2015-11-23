#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "inspire.h"
#include "util/assert.h"

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
      virtual ~thread();

      int64 tid() { return _tid; }
      
      thdMgr* threadMgr() const { return _thdMgr; }

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
      * break the relation of thread manager
      * it means the thread manager don't manager it any more
      * you should free it when its work over
      */
      void detach() { _detach = true; }
      /*
      * check the thread is detached or not
      */
      bool detached() const { return _detach; }
      /*
      * check the running state of thread
      */
      bool running() const { return THREAD_RUNNING == _state; }

   public:
      int  create();
      void active();
      void suspend();
      void resume();
      /*void stop();*/
      /*
      * join thread and let operation system recycle the kernel resource
      * every thread need to call join before free
      */
      void join();

   protected:
      bool valid() const
      {
#ifdef _WINDOWS
         return INVALID_HANDLE_VALUE != _hThread;
#else
         return _tid != -1;
#endif
      }

      void  state(char st) { _state = st; }
#ifndef _WINDOWS
      bool wait(uint seconds = 0);
#endif
      /*
      * deactive the thread, the manager will decide it pooled or free
      * if the thread is detached, you still need to manage it
      */
      void deactive();

   private:
      thread(const thread& rhs);
      thread& operator= (const thread& rhs);

   protected:
      char     _state;
      bool     _detach;
      int      _errno;
      uint64   _tid;
      thdTask* _task;
      thdMgr*  _thdMgr;
#ifdef _WINDOWS
      HANDLE _hThread = INVALID_HANDLE_VALUE;
#else
      pthread_mutex_t _mtx  = PTHREAD_MUTEX_INITIALIZER;
      pthread_cond_t  _cond = PTHREAD_COND_INITIALIZER;
#endif
   };
}
#endif // _INSPIRE_THREAD_ENTITY_H_
