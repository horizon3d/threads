#include "logger/logger.h"
#include "task/thdTask.h"
#include "util/mutex.h"
#include "util/spinlock.h"
#include "util/condition.h"
#include "logger/logger.h"
#include "thdMgr.h"
#include <iostream>
#include "thdEvent.h"

struct mtxnumber
{
   inspire::mutex_t _spin;
   //inspire::spinlock_t _spin;
   int index = 0;
};

static mtxnumber no;
static mtxnumber taskId;

int64 inc()
{
   inspire::condition_t cond(&taskId._spin);
   ++taskId.index;
   return taskId.index;
}

class taskA : public inspire::thdTask
{
public:
   taskA(int64 id) : thdTask(id, "A task") {  }
   ~taskA() {}

   virtual const int run()
   {
      inspire::condition_t cond(&no._spin);
      int tmp = no.index;
      ++no.index;
      LogEvent("from %d ---> %d", tmp, no.index);
      std::cout << " ---> " << no.index << std::endl;
      return 0;
   }

private:
   const char* _name;
};

int main(int argc, char** argv)
{
   inspire::thdMgr* mgr = inspire::thdMgr::instance();
   mgr->initialize();
   mgr->active();

   mgr->reverseIdleCount(3);
   for (int idx = 0; idx < 20; ++idx)
   {
      int tt = inc();
      inspire::thdTask* t = new taskA(tt);
      mgr->notify(inspire::EVENT_DISPATCH_TASK, t);
   }

   //inSleep(20 * 1000);
   mgr->destroy();
   inSleep(2000);
   return 0;
}