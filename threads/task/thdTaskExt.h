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
#ifndef _INSPIRE_THREAD_TASK_EXTENSION_H_
#define _INSPIRE_THREAD_TASK_EXTENSION_H_

namespace inspire {

   /*
   * task type must be an enumerated variable
   * in order to pass the management of task to task manager
   * user should to implement a task factory base on interface ITaskProductor
   */
   class ITaskProductor
   {
   public:
      virtual thdTask* createTask(const uint taskType) = 0;

      void destroy(thdTask* task) { delete task; }
   };
}
#endif