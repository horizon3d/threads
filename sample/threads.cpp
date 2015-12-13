#include "threads.h"
#include "logger/logger.h"
#include "util/system/mutex.h"
#include "util/system/condition.h"
#include <iostream>

struct mtxnumber
{
   inspire::mutex_t _spin;
   int index;
};

mtxnumber no;
mtxnumber taskId;

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
   no.index = 0;
   taskId.index = 0;

   inspire::thdMgr* mgr = inspire::thdMgr::instance();
   mgr->initialize();
   mgr->active();

   mgr->reverseIdleCount(3);
   for (int idx = 0; idx < 20; ++idx)
   {
      int64 tt = inc();
      inspire::thdTask* t = new taskA(tt);
      mgr->postEvent(inspire::EVENT_DISPATCH_TASK, t);
   }

   mgr->destroy();

   return 0;
}