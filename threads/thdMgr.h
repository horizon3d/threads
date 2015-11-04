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
      int initialize();
      int destroy();

      threadEntity* fetchIdle();
      int create(int64& id);
      threadEntity* create();
      int release(const int64& id);
      int release(threadEntity* entity);
      int dispatch(thdTask* task);
      void pooled(const int64& id);
      void pooled(threadEntity* entity);
      void unpooled(const int64& id);
      void unpooled(threadEntity* entity);

      /*
       * process task in task queue
       */
      int handle();

   private:
      void _recycle(threadEntity* entity);

   private:
      threadMgr();
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr();

   private:
      std::deque<threadEntity*> _workQueue;
      std::deque<threadEntity*> _idleQueue;
      std::deque<thdTask*>      _taskQueue;
      std::map<int64, threadEntity*> _thdMap;
   };
}
#endif
