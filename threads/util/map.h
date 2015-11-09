#ifndef _INSPIRE_UTIL_MAP_H_
#define _INSPIRE_UTIL_MAP_H_

#include <map>
#include "threads.h"
#include "condition.h"

namespace inspire {

   template<typename K, typename V>
   class map
   {
   public:
      map()  {}
      ~map() {}

   public:

      void insert(K key, V value)
      {
         condition_t cond(_mtx);
         _map.insert(std::make_pair(key, value));
      }

      void erase(const K& key)
      {
         condition_t cond(&_mtx);
         _map.erase(key);
      }

      bool empty() const { return _map.empty(); }

      uint size() const { return (uint)_map.size(); }

      bool find(const K& key)
      {
         std::map<K, V>::iterator it = _map.find(key);
         if (_map.end() == it)
         {
            return false;
         }
         return true;
      }

      std::map<K, V>& raw() { return _map; }

   private:
      mutex_t _mtx;
      std::map<K, V> _map;
   };
}
#endif
