#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include <map>
#include "thdEntity.h"

namespace inspire {

   class thdTask;

   class threadMgr
   {
   public:
      static threadMgr* instance();

      threadEntity* fetchIdle();
      int create(int64& id);
      int release(const int64& id);
      int destroy(const int64&& id);
      int dispatch(thdTask* task);

      threadEntity* create();
      int release(threadEntity* entity);

   private:
      threadMgr();
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr();

   private:
      int64 _id = 0;
      std::map<int64, threadEntity*> _workQueue;
      std::map<int64, threadEntity*> _idleQueue;
   };
}
#endif
