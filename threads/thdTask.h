#ifndef _INSPIRE_THREAD_TASK_H_
#define _INSPIRE_THREAD_TASK_H_

#include "threads.h"
#include "thdEntity.h"

namespace inspire {

   class taskMgr;

   class thdTask
   {
      enum
      {
         TASK_UNHANDLED = 0,
         TASK_HANDLEING = 1,
         TASK_HANDLED   = 2,
      };
   public:
      thdTask(const int64& id) : _status(TASK_UNHANDLED), _taskId(id), _thd(NULL) {}
      explicit thdTask(const int64& id, threadEntity* thd) : _status(TASK_UNHANDLED), _taskId(id), _thd(thd) {}
      virtual ~thdTask() {}

      virtual int run() = 0;

      virtual const char* name() = 0;

   public:
      void attach(threadEntity* thd = NULL)
      {
         if(NULL != thd)
         {
            _thd = thd;
         }
         OnBegin();
      }
      void detach() { _status = TASK_HANDLED; _thd = NULL; OnEnd(); }

   private:
      void notify() {}

   private:
      void OnBegin() { /* LogEvent begin*/ }
      void OnEnd()   { /* LogEvent end*/ }

   private:
      int           _status;
      int64         _taskId;
      threadEntity* _thd;
      taskMgr*      _mgr;
   };
}
#endif
