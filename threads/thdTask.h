#ifndef _INSPIRE_THREAD_TASK_H_
#define _INSPIRE_THREAD_TASK_H_

#include "threads.h"
#include "thdEntity.h"
#include "thdTaskMgr.h"

namespace inspire {

   enum
   {
      TASK_UNHANDLED = 0,
      TASK_HANDLEING = 1,
      TASK_HANDLED = 2,
   };
   typedef void (*ON_TASK_END)(void* result);
   class thdTask
   {
   public:
      thdTask(const int64& id, const char* name)
         : _status(TASK_UNHANDLED), _taskId(id), _name(name), _thd(NULL), _cb(NULL)
      {
         thdTaskMgr::instance()->registerTask(this);
      }
      explicit thdTask(const int64& id, const char* name, threadEntity* thd)
         : _status(TASK_UNHANDLED), _taskId(id), _name(name), _thd(thd), _cb(NULL)
      {}
      virtual ~thdTask() {}

      virtual int run() = 0;

      virtual const char* name() { return _name; };

   public:
      const int64 id() const { return _taskId; }
      const uint status() const { return _status; }
      void status(const uint st) { _status = st; }
      void attach(threadEntity* thd = NULL)
      {
         if(NULL != thd)
         {
            _thd = thd;
         }
         status(TASK_HANDLEING);
         LogEvent("Task: %lld begin handling", _taskId, _thd->tid());
      }

      void detach()
      {
         _cb(NULL); // need to be finish parameter
         status(TASK_HANDLED);
         LogEvent("Task: %lld over", _taskId);
         _thd = NULL;
      }

      ON_TASK_END setTaskCallBack(ON_TASK_END cb)
      {
         ON_TASK_END old = _cb;
         _cb = cb;
         return old;
      }

   private:
      uint          _status;
      int64         _taskId;
      const char*   _name;
      threadEntity* _thd;
      ON_TASK_END   _cb;
   };
}
#endif
