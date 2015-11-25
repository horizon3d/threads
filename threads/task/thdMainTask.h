#ifndef _INSPIRE_THREAD_TASK_PROCESS_H_
#define _INSPIRE_THREAD_TASK_PROCESS_H_

#include "thdTask.h"

namespace inspire {

   class thdMgr;
   class thread;
   class thdMainTask : public thdTask
   {
   public:
      thdMainTask(thdMgr* mgr)
         : thdTask(0, "Event Process Task"), _thdMgr(mgr) {}
      ~thdMainTask() { _thdMgr = NULL; }

      virtual const int run();

   private:
      thdMainTask(const thdMainTask& rhs);
      thdMainTask& operator= (const thdMainTask& rhs);

   private:
      thdMgr* _thdMgr;
   };
}
#endif