/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 tynia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author: tynia
Date  : =========
*******************************************************************************/
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
