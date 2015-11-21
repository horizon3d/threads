#include "thdMgrTask.h"
#include "thdMgr.h"

namespace inspire {

   const int thdMgrTask::run()
   {
      LogEvent("starting MAIN PROCESS LOOP: %s", name());
      while (_thd->running())
      {
         _thdMgr->process();
      }

      return _thd->error();
      LogEvent("ending MAIN PROCESS LOOP: %s", name());
   }
}