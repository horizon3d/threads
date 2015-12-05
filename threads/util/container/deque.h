#ifndef _INSPIRE_UTIL_DEQUEUE_H_
#define _INSPIRE_UTIL_DEQUEUE_H_

#include "util/inspire.h"
#include "util/system/mutex.h"
#include "util/system/condition.h"

namespace inspire {

   template<typename T>
   class deque
   {
   public:
      deque()  {}
      ~deque() {}

      void push_back(T& t)
      {
         condition_t cond(&_mtx);
         _deque.push_back(t);
      }

      bool pop_front(T& t)
      {
         condition_t cond(&_mtx);
         if (!_deque.empty())
         {
            t = _deque.front();
            _deque.pop_front();
            return true;
         }
         return false;
      }

      bool empty() const { return _deque.empty(); }

      unsigned size() const { return (unsigned)_deque.size(); }

      std::deque<T>& raw() { return _deque; }

   private:
      mutex_t _mtx;
      std::deque<T> _deque;
   };
}
#endif
