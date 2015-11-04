#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include <queue>
#include <map>
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
      threadEntity* create();
      int  createWorker(const uint w);
      int  release(const int64& id);
      void recycle(threadEntity* entity);

      // task
      int  dispatch(thdTask* task);
      thdTask* fetchTask();

   private:
      int  _createEntity(bool worker, threadEntity*& entity);
      void _remove(threadEntity* entity);

   private:
      threadMgr();
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr();

   private:
      std::deque<threadEntity*> _idleQueue;
      std::deque<thdTask*>      _taskQueue;
      std::map<int64, threadEntity*> _workQueue;
      std::map<int64, threadEntity*> _thdMap;
   };
}
#endif
