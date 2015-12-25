#include "threads.h"
#include "logger/logger.h"
#include "util/system/mutex.h"
#include "util/system/condition.h"
#include <iostream>

struct unit
{
   inspire::mutex_t _mtx;
   int id;
};

unit ticket;
unit taskId;

int64 inc(unit& u)
{
   inspire::condition_t cond(&u._mtx);
   ++u.id;
   return u.id;
}

class taskA : public inspire::thdTask
{
public:
   taskA(int64 id) : thdTask(id, "A task") {  }
   ~taskA() {}

   virtual const int run()
   {
      inspire::condition_t cond(&ticket._mtx);
      int tmp = ticket.id;
      ++ticket.id;
      LogEvent("from %d ---> %d", tmp, ticket.id);
      std::cout << " ---> " << ticket.id << std::endl;
      return 0;
   }
};

int main(int argc, char** argv)
{
   ticket.id = 0;
   taskId.id = 0;

   inspire::threadMgr* mgr = inspire::threadMgr::instance();
   mgr->initialize();
   mgr->active();

   mgr->reverseIdleCount(3);
   for (int idx = 0; idx < 20; ++idx)
   {
      int64 tt = inc(taskId);
      inspire::thdTask* t = INSPIRE_NEW taskA(tt);
      mgr->postEvent(t);
   }

   mgr->destroy();

   return 0;
}