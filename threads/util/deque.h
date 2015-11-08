#ifndef _INSPIRE_UTIL_DEQUEUE_H_
#define _INSPIRE_UTIL_DEQUEUE_H_

#include <deque>
#include "threads.h"
#include "mutex.h"

namespace inspire {

   template<typename T>
   class deque
   {
   public:
      deque()  {}
      ~deque() {}

      void push_back(T& t)
      {
         condition_t cond(_mtx);
         _deque.push_back(t);
      }

      T& pop()
      {
         condition_t cond(_mtx);
         T& t = _deque.front();
         _deque.pop_front();
         return t;
      }

      bool empty() const { return _deque.empty(); }

      uint size() const { return (uint)_deque.size(); }

      std::deque<T>& raw() { return _deque; }

   private:
      mutex_t _mtx;
      std::deque<T> _deque;
   };
}
#endif
