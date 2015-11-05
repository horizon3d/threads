#ifndef _INSPIRE_UTIL_MAP_H_
#define _INSPIRE_UTIL_MAP_H_

#include "condition.h"

namespace inspire {

   template<typename K, typename V>
   class map
   {
   public:
      typedef iterator std::map<K, V>::iterator;
      typedef const_iterator std::map<K, V>::const_iterator;
      typedef reverse_iterator std::map<K, V>::reverse_iterator;

      map()  { _spin = new spinlock_t(); }
      ~map() { delete _spin; _spin = NULL; }

   public:

      void insert(K key, V value)
      {
         condition_t cond(_spin);
         _map.insert(std::make_pair(key, value));
      }

      void erase(const K& key)
      {
         condition_t cond(_spin);
         _map.erase(key);
      }

      void erase(const_iterator& it, const_iterator& end)
      {
         condition_t cond(_spin);
         _map.erase(it, end);
      }

      void erase(const_iterator& it)
      {
         condition_t cond(_spin);
         _map.erase(it);
      }

      bool empty() const { return _map.empty(); }

      uint size() const { return _map.size(); }

      V& operator[] (const K& key) { return _map.at(key); }

      V& find(const K& key)
      {
         iterator it = _map.find(key);
         if (_map.end() != it)
         {
            return it->second;
         }
      }

      std::map<K, V>& raw() { return _map; }

   private:
      spinlock_t* _spin;
      std::map<K, V> _map;
   };
}
#endif
