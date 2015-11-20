#ifndef _INSPIRE_THREAD_TASK_PROCESS_H_
#define _INSPIRE_THREAD_TASK_PROCESS_H_

#include "thdTask.h"

namespace inspire {

   class thdMgr;
   class thread;
   class thdMgrTask : public thdTask
   {
   public:
      thdMgrTask(thdMgr* mgr)
         : thdTask(0, "Event Process Task"), _thdMgr(mgr) {}
      ~thdMgrTask() { _thdMgr = NULL; }

      virtual const int run();

   private:
      thdMgrTask(const thdMgrTask& rhs);
      thdMgrTask& operator= (const thdMgrTask& rhs);

   private:
      thdMgr* _thdMgr;
   };
}
#endif