#include "logger/logger.h"
#include "thdTask.h"
#include "util/spinlock.h"
#include "util/condition.h"
#include "logger/logger.h"
#include "thdMgr.h"

struct mtxnumber
{
   inspire::spinlock_t _spin;
   int index = 0;
};

static mtxnumber no;
static int taskId = 0;

class taskA : public inspire::thdTask
{
public:
   taskA() : thdTask(taskId) { ++taskId; }
   ~taskA() {}

   virtual int run()
   {
      inspire::condition_t cond(&no._spin);
      int tmp = no.index;
      ++no.index;
      LogEvent("from %d ---> %d", tmp, no.index);
      Sleep(10000);
      return 0;
   }
};

/*int main(int argc, char** argv)
{
   inspire::threadMgr* mgr = inspire::threadMgr::instance();
   inspire::thdTask* ts[50];
   for (int idx = 0; idx < 50; ++idx)
   {
      inspire::thdTask* t = new taskA();
      mgr->dispatch(t);
      ts[idx] = t;
   }

   mgr->createWorker(10);

   while (!mgr->handled())
   {
      Sleep(5000);
   }

   for (INT idx = 0; idx < 50; ++idx)
   {
      delete ts[idx];
      ts[idx] = NULL;
   }

   return 0;
}*/