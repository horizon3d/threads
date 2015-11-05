#ifndef _INSPIRE_UTIL_DEQUEUE_H_
#define _INSPIRE_UTIL_DEQUEUE_H_

#include <deque>
namespace inspire {

   template<typename T>
   class deque
   {
   public:
      deque()  { _spin = new spinlock_t(); }
      ~deque() { delete _spin; _spin = NULL; }

      void push_back(T& t)
      {
         condition_t cond(_spin);
         _deque.push_back(t);
      }

      T& pop()
      {
         condition_t cond(_spin);
         T& t = _deque.front();
         _deque.pop_front();
         return t;
      }

      bool empty() const { return _deque.empty(); }

      uint size() const { return _deque.size(); }

   private:
      spinlock_t *_spin;
      std::deque<T> _deque;
   };
}
#endif
