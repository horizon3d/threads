#ifndef _INSPIRE_THREAD_TASK_MANAGER_H_
#define _INSPIRE_THREAD_TASK_MANAGER_H_

#include "util/inspire.h"
#include "util/container/map.h"
#include "util/container/deque.h"

namespace inspire {

   class threadMgr;
   class thdTask;
   class ITaskProductor;
   class thdTaskMgr
   {
   public:
      static thdTaskMgr* instance();
      void registerTask(thdTask* task);
      void over(thdTask* task);
      thdTask* get(const uint& id, ITaskProductor* factory);
      uint64 total() const { return _totalCount; }

   protected:
      void clean();

   private:
      thdTaskMgr() : _totalCount(0) {}
      thdTaskMgr(const thdTaskMgr& rhs);
      thdTaskMgr& operator= (const thdTaskMgr& rhs);
      ~thdTaskMgr() { clean(); }

   private:
      uint64              _totalCount;
      deque<thdTask*>     _overQueue;
      map<uint, thdTask*> _taskMap;
   };
}
#endif