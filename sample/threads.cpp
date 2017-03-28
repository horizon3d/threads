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

enum TASK_TYPE
{
   THREAD_TASK_A = 1,
};

class taskA : public inspire::thdTask
{
public:
   taskA(uint id) : thdTask(id, "A task") {  }
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

class taskFactory : public inspire::ITaskProductor
{
public:
   virtual inspire::thdTask* createTask(const uint taskType)
   {
      inspire::thdTask* task = NULL;
      switch (taskType)
      {
      case THREAD_TASK_A:
         task = new taskA(taskType);
         break;
      default:
         break;
      }
      return task;
   }

public:
   taskFactory() {}
   ~taskFactory() {}
};

int main(int argc, char** argv)
{
   ticket.id = 0;

   taskFactory factory;

   inspire::threadMgr* mgr = inspire::threadMgr::instance();
   mgr->initialize();
   mgr->active();

   for (int idx = 0; idx < 20; ++idx)
   {
      inspire::thdTask* t = inspire::thdTaskMgr::instance()->get(THREAD_TASK_A, &factory);
      INSPIRE_ASSERT(t, "create task failed, out of memory");
      mgr->postEvent(t);
   }

   mgr->destroy();

   return 0;
}