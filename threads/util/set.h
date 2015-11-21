#ifndef _INSPIRE_UTIL_SET_H_
#define _INSPIRE_UTIL_SET_H_

#include <set>
#include "mutex.h"
#include "condition.h"

namespace inspire {

   template<class T>
   class set
   {
   public:
      set() {}
      ~set() {}

   public:
      void insert(T& t)
      {
         condition_t cond(&_mtx);
         _set.insert(t);
      }

      void erase(const T& t)
      {
         condition_t cond(&_mtx);
         _set.erase(t);
      }

      bool empty() const { return _set.empty(); }

      unsigned size() const { return (unsigned)_set.size(); }

      bool find(const T& t)
      {
         std::set<T>::iterator it = _set.find(t);
         if (_set.end() != it)
         {
            return true;
         }
         return false;
      }

      std::set<T>& raw() { return _set; }

   private:
      mutex_t _mtx;
      std::set<T> _set;
   };
}
#endif // !_INSPIRE_UTIL_SET_H_
