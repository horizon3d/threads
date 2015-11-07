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
      // task
      void  dispatch(thdTask* task);

      threadEntity* create();
      void deactive(threadEntity* entity);

   protected:
      thdTask* fetch();
      void push(threadEntity* entity);
      void recycle(threadEntity* entity);

   private:
      int  _createEntity(bool worker, threadEntity*& entity);
      void _remove(threadEntity* entity);

   private:
      threadMgr() {}
      threadMgr(const threadMgr& rhs) = delete;
      threadMgr& operator=(const threadMgr& rhs) = delete;
      virtual ~threadMgr() {}

   private:
      deque<threadEntity*>      _idleQueue;
      deque<thdTask*>           _taskQueue;
      map<int64, threadEntity*> _workQueue;
      map<int64, threadEntity*> _thdMap;
   };
}
#endif
