#include "thdMainTask.h"
#include "thdMgr.h"

namespace inspire {

   const int thdMainTask::run()
   {
      LogEvent("starting MAIN PROCESS LOOP: %s", name());
      while (_thd->running())
      {
         _thdMgr->process();
      }

      LogEvent("ending MAIN PROCESS LOOP: %s", name());
      return _thd->error();
   }
}