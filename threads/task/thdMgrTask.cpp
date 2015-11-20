#include "thdMgrTask.h"
#include "thdMgr.h"

namespace inspire {

   const int thdMgrTask::run()
   {
      while (_thd->running())
      {
         _thdMgr->process();
      }

      return _thd->error();
   }
}