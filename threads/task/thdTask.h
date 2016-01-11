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
#ifndef _INSPIRE_THREAD_TASK_H_
#define _INSPIRE_THREAD_TASK_H_

#include "thread.h"
#include "thdTaskMgr.h"

namespace inspire {

   enum
   {
      TASK_UNHANDLED = 0,
      TASK_RUNNING   = 1,
      TASK_HANDLED   = 2,
   };

   typedef void (*TASK_END_FUNC)(void* result);
   class thdTask
   {
   public:
      thdTask(const uint& ttype, const char* name = NULL)
         : _status(TASK_UNHANDLED), _taskType(ttype), _name(name), _thd(NULL), _cb(NULL)
      {
         thdTaskMgr::instance()->registerTask(this);
      }
      explicit thdTask(const uint& ttype, const char* name, thread* thd)
         : _status(TASK_UNHANDLED), _taskType(ttype), _name(name), _thd(thd), _cb(NULL)
      {}
      virtual ~thdTask() {}

      virtual void initialize() {}
      virtual const int run() = 0;

   public:
      const char* name() { return (NULL == _name ? "NULL" : _name); };
      const uint type() const { return _taskType; }
      const uint status() const { return _status; }
      void reinitialize() { status(TASK_UNHANDLED); OnTaskEnd(NULL); }
      void attach(thread* thd = NULL)
      {
         if(NULL != thd)
         {
            _thd = thd;
         }
         status(TASK_RUNNING);
         LogEvent("Task: %d begin handling, thread: %lld", _taskType, _thd->tid());
      }

      void detach()
      {
         if (NULL != _cb)
         {
            // need to be finish parameter
            // _cb(NULL);
         }
         status(TASK_HANDLED);
         LogEvent("Task: %d over", _taskType);
         _thd = NULL;
      }

      TASK_END_FUNC OnTaskEnd(TASK_END_FUNC cb)
      {
         TASK_END_FUNC old = _cb;
         _cb = cb;
         return old;
      }

   protected:
      void status(const uint st) { _status = st; }

   protected:
      uint          _status;
      uint          _taskType;
      const char*   _name;
      thread*       _thd;
      TASK_END_FUNC _cb;
   };
}
#endif
