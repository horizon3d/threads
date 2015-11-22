#ifndef _INSPIRE_UTIL_MAP_H_
#define _INSPIRE_UTIL_MAP_H_

#include <map>
#include "mutex.h"
#include "condition.h"

namespace inspire {

   template<typename K, typename V>
   class map
   {
      typedef K key_type;
      typedef V value_type;
      typedef V& value_reference;
   public:
      map()  {}
      ~map() {}

   public:

      void insert(const key_type key, value_type value)
      {
         condition_t cond(&_mtx);
         _map.insert(std::make_pair(key, value));
      }

      void erase(const key_type& key)
      {
         condition_t cond(&_mtx);
         _map.erase(key);
      }

      bool fetch(const key_type& key, value_reference v)
      {
         condition_t cond(&_mtx);
         std::map<key_type, value_type>::iterator it = _map.find(key);
         if (_map.end() != it)
         {
            v = it->second;
            return true;
         }
         return false;
      }

      bool empty() const { return _map.empty(); }

      unsigned size() const { return (unsigned)_map.size(); }

      bool find(const key_type& key)
      {
         std::map<key_type, value_type>::iterator it = _map.find(key);
         if (_map.end() != it)
         {
            return true;
         }
         return false;
      }

      std::map<key_type, value_type>& raw() { return _map; }

   private:
      mutex_t _mtx;
      std::map<key_type, value_type> _map;
   };
}
#endif
