#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include "util/deque.h"
#include "util/map.h"
#include "thdEntity.h"

namespace inspire {

   class thdTask;

   class threadMgr
   {
   public:
      static threadMgr* instance();
      // self
      int initialize();
      int destroy();
      // thread
      threadEntity* fetchIdle();
      void pushIdle(threadEntity* entity);
      void popWorker(threadEntity* entity);
      void pushWorker(threadEntity* entity);
      // task
      void  dispatch(thdTask* task);
      thdTask* fetchTask();

      threadEntity* create();
      int  createWorker(const uint w);
      void recycle(threadEntity* entity);
      void suspend(threadEntity* entity);

      void setIdleQueueSize(const uint count) { _maxPooledCount = count; }

      bool handled() const { return _taskQueue.empty(); }
      void sigExit();

   private:
      int  _createEntity(bool worker, threadEntity*& entity);
      void _remove(threadEntity* entity);

   private:
      threadMgr() {}
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr() {}

   private:
      uint _maxPooledCount = 50;
      deque<threadEntity*>      _idleQueue;
      deque<thdTask*>           _taskQueue;
      map<int64, threadEntity*> _workQueue;
      map<int64, threadEntity*> _thdMap;
   };
}
#endif
