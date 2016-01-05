#ifndef _INSPIRE_THREAD_TASK_EXTENSION_H_
#define _INSPIRE_THREAD_TASK_EXTENSION_H_

namespace inspire {

   /*
   * task type must be an enumerated variable
   */
   inline thdTask* createTask(const uint taskType)
   {
      thdTask* task = NULL;
      switch (taskType)
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