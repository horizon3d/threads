#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include "util/deque.h"
#include "thread.h"

namespace inspire {

   class thdTask;
   class thdTaskMgr;
   class thdMgr
   {
   public:
      static thdMgr* instance();
      // self
      int initialize();
      int destroy();
      int process();
      void setIdleCount(const uint maxCount = 10);

      // thread
      thread* fetchIdle();
      // task
      void dispatch(thdTask* task);
      void over(thdTask* task);

      thread* create();
      void deactive(thread* entity);

   protected:
      thdTask* fetch();
      void enIdle(thread* entity);
      void recycle(thread* entity);
      void store(thread* entity);
      thread* acquire();

   private:
      thdMgr();
      thdMgr(const thdMgr& rhs) = delete;
      thdMgr& operator=(const thdMgr& rhs) = delete;
      virtual ~thdMgr();

   private:
      bool                 _exit = false;
      uint                 _maxIdleCount = 10;
      thdTaskMgr*          _taskMgr;
      deque<thread*>    _idleQueue;
      deque<thdTask*>      _taskQueue;
      deque<thread*>    _entityQueue;
   };
}
#endif
