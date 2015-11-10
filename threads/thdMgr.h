#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include "util/deque.h"
#include "util/map.h"
#include "thdEntity.h"

namespace inspire {

   class thdTask;
   class thdTaskMgr;
   class threadMgr
   {
   public:
      static threadMgr* instance();
      // self
      int initialize();
      int destroy();
      int process();
      void setIdleCount(const uint maxCount = 10);

      // thread
      threadEntity* fetchIdle();
      // task
      void dispatch(thdTask* task);
      void over(thdTask* task);

      threadEntity* create();
      void deactive(threadEntity* entity);

   protected:
      thdTask* fetch();
      void enIdle(threadEntity* entity);
      void recycle(threadEntity* entity);
      void store(threadEntity* entity);
      threadEntity* acquire();

   private:
      threadMgr();
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr();

   private:
      bool                 _exit = false;
      uint                 _maxIdleCount = 10;
      thdTaskMgr*          _taskMgr;
      deque<threadEntity*> _idleQueue;
      deque<thdTask*>      _taskQueue;
      deque<threadEntity*> _entityQueue;
   };
}
#endif
