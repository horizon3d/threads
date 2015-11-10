#include "thdTaskMgr.h"
#include "thdTask.h"

namespace inspire {

   thdTaskMgr* thdTaskMgr::instance()
   {
      static thdTaskMgr mgr;
      return &mgr;
   }

   void thdTaskMgr::registerTask(thdTask* task)
   {
      thdTask* t = NULL;
      if (!_taskMap.fetch(task->id(), t))
      {
         LogEvent("insert a new task into map, id: %lld, name: %s",
                  task->id(), task->name());
         _taskMap.insert(task->id(), task);
      }
#ifdef _DEBUG
      else
      {
         LogError("try to register a duplicated task!!!"
                  " task id: %d, name: %s, existed task: %s",
                  task->id(), task->name(), t->name());
      }
#endif
   }

   void thdTaskMgr::over(thdTask*& task)
   {
      thdTask* t = NULL;
      if (_taskMap.fetch(task->id(), t) && t == task)
      {
         // we have stored the task
         // and the task is the same one we stored
         // let's change the task status
         // or we delete it
         task->status(TASK_UNHANDLED);
         return;
      }
      // we never stored the task
      // or the task is not the one we've stored
      // free it
      delete task;
      task = NULL;
   }

   thdTask* thdTaskMgr::get(const int64& id)
   {
      thdTask* t = NULL;
      if (_taskMap.fetch(id, t) && TASK_UNHANDLED == t->status())
      {
         // the task is idle, we use it
         return t;
      }
      return NULL;
   }

   void thdTaskMgr::clean()
   {
      std::map<int64, thdTask*>& rmap = _taskMap.raw();
      std::map<int64, thdTask*>::iterator it = rmap.begin();
      for (; rmap.end() != it; ++it)
      {
         delete it->second;
         it->second = NULL;
      }
   }
}