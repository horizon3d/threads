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
      int release(const int64& id);
      int dispatch(thdTask* task);

      threadEntity* create();
      int release(threadEntity* entity);

      void pooled(const int64& id);
      void pooled(threadEntity* entity);

   private:
      threadMgr();
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr();

   private:
      std::deque<threadEntity*> _workQueue;
      std::deque<threadEntity*> _idleQueue;
      std::map<int64, threadEntity*> _thdMap;
   };
}
#endif
