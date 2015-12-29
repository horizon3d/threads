#ifndef _INSPIRE_THREAD_TASK_EXTENSION_H_
#define _INSPIRE_THREAD_TASK_EXTENSION_H_

namespace inspire {

   inline thdTask* createTask(const int64 taskId)
   {
      thdTask* task = NULL;
      switch (taskId)
      {
      case 0:
         //task = new XXXXTask();
         break;
      default:
         break;
      }
      return task;
   }
}
#endif