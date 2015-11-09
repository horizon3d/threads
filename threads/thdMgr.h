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
      int process();
      void setIdleCount(const uint maxCount = 10);

      // thread
      threadEntity* fetchIdle();
      // task
      void  dispatch(thdTask* task);

      threadEntity* create();
      void deactive(threadEntity* entity);

   protected:
      thdTask* fetch();
      void enIdle(threadEntity* entity);
      void recycle(threadEntity* entity);
      void store(threadEntity* entity);
      threadEntity* acquire();

   private:
      int destroy(threadEntity* entity);

   private:
      threadMgr() {}
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr() {}

   private:
      uint                 _maxIdleCount = 10;
      deque<threadEntity*> _idleQueue;
      deque<thdTask*>      _taskQueue;
      deque<threadEntity*> _entityQueue;
      deque<threadEntity*> _thdMap;
   };
}
#endif
