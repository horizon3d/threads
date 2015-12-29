#ifndef _INSPIRE_THREAD_EXTENSION_H_
#define _INSPIRE_THREAD_EXTENSION_H_

#include "thread.h"
#include "util/system/util.h"

namespace inspire {

   /*
    * This function is used as a factory to product extensive thread object
    * When a class inherited on thread, and in order to get object of it,
    * add the product statement here, it must be like thd = new EXT_THREAD(...)
    * A pointer of base(thread) will be return if successfully worked.
   */
   inline thread* create(threadMgr* mgr, uint thdType)
   {
      INSPIRE_ASSERT(NULL != mgr, "thread manager cannot be NULL");
      thread* thd = NULL;
      switch (thdType)
      {
      //case 0:
         //thd = new extThread(mgr);
      default:
         break;
      }

      INSPIRE_ASSERT(NULL != thd, "Failed to create a thread object, type: %d", thdType);
      return thd;
   }
}
#endif
