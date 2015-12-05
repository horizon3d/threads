#include "AllocatorMgr.h"
#include "util/system/condition.h"
#include "util/assert.h"

namespace inspire {

   AllocatorMgr::AllocatorMgr()
   {
      for (int idx = 0, size = 8; idx < MAX_ALLOCATOR_COUNT; ++idx, size *= 2)
      {
         _fls[idx].size = size;
         _fls[idx].hdr  = NULL;
      }
   }

   AllocatorMgr::~AllocatorMgr()
   {
      pray();
   }

   AllocatorMgr* AllocatorMgr::instance()
   {
      static AllocatorMgr mgr;
      return &mgr;
   }

   void* AllocatorMgr::alloc(const uint size, const char* file, const uint line)
   {
      void* ptr = _alloc(size, file, line);
      if (NULL == ptr)
      {
         pray();
         ptr = _alloc(size, file, line);
      }

      if (NULL == ptr)
      {
         LogError("Failed to allocate memory, size: %d", size);
         return NULL;
      }

      return ptr;
   }
   
   void* AllocatorMgr::realloc(void* ptr, const uint size, const char* file, const uint line)
   {
      void* reptr = alloc(size, file, line);
      if (NULL == reptr)
      {
         return NULL;
      }

      dealloc(ptr);
      return ptr;
   }

   void AllocatorMgr::dealloc(void* ptr)
   {
      bool ok = _checkSanity(ptr);
      if (!ok)
      {
         LogError("pointer: 0x%x sanity checked failed, it may not be "
                  "allocated by allocator", (uint64)ptr);
         //::free(ptr);
#ifdef _DEBUG
         Panic();
#endif
         return;
      }

      header* hdr = (header*)((char*)ptr - sizeof(header));
      uint locate = _locate(hdr->size);
      freelist* fl = &_fls[locate];

      // ensure the safety of multi-thread, we locked the free list
      // and we append the memory block at the end of free list
      // the reason for it to to ensure that every memory block in free list can be used
      // thus we can keep balance of recycling
      condition_t cond(&fl->mtx);
      header* fhdr = fl->hdr;
      if (NULL != fhdr)
      {
         while (NULL != fhdr->next)
         {
            fhdr = fhdr->next;
         }
         fhdr->next = hdr;
      }
      else
      {
         fhdr = hdr;
      }
      // now we clear the expired data in memory block
      ::memset((void*)ptr, 0x0, fl->size);
   }

   void AllocatorMgr::pray()
   {
      for (int idx = 0; idx < MAX_ALLOCATOR_COUNT; ++idx)
      {
         header* hdr = _fls[idx].hdr;
         while (NULL != hdr)
         {
            header* next = hdr;
            hdr = hdr->next;
            ::free((char*)next);
         }
      }
   }

   void* AllocatorMgr::_alloc(const uint size, const char* file, const uint line)
   {
      void* ptr = NULL;
      // first we pick memory block stored in free list
      uint locate = _locate(size);
      freelist* fl = &_fls[locate];
      {
         condition_t cond(&fl->mtx);
         header* hdr = fl->hdr;
         while (NULL != hdr)
         {
            header* toReturn = hdr;
            if (NULL != toReturn)
            {
               hdr = toReturn->next;
               toReturn->next = NULL;
               ptr = (char*)toReturn;
               break;
            }
            else
            {
               hdr = hdr->next;
            }
         }
      }

      if (NULL == ptr)
      {
         // if we find no match block in free list
         // then we use malloc to alloc memory from system
         ptr = ::malloc(fl->size + sizeof(header));
         if (NULL == ptr)
         {
            LogError("Failed to allocate memory, size :%d", fl->size);
            return NULL;
         }
         // fill mete data recorded in header 
         _setSanity(ptr, fl->size);
         ::memset((char*)ptr + sizeof(header), 0, fl->size);
         return (char*)ptr + sizeof(header);
      }

      // means we find memory block in free list
#ifdef _DEBUG
      // now we should resign the file and line to record where calls alloc
      _debug(ptr, file, line);
#endif
      return (char*)ptr + sizeof(header);
   }

   uint AllocatorMgr::_locate(const uint size)
   {
      // we need find which allocator should allocate the size
      // and we use binary search
      // TODO: bug in the code block
      uint high = MAX_ALLOCATOR_COUNT - 1;
      uint low = 0;
      uint locate = 0;
      while (true)
      {
         locate = (high + low) / 2;
         if (size >= 2 * _fls[locate].size)
         {
            low = locate;
         }
         else if (size < _fls[locate].size)
         {
            high = locate;
         }
         else
         {
            return locate;
         }
      }
   }

   void AllocatorMgr::_setSanity(void* ptr, const uint size)
   {
      ::memset(ptr, 0x0, size + sizeof(header));
      header* hdr = (header*)ptr;
      ::memmove(hdr->eyecatcher, "inspire", 8);
      hdr->next = NULL;
      hdr->size = size;
      hdr->magic = magic;
   }

   bool AllocatorMgr::_checkSanity(void* ptr)
   {
      header* hdr = (header*)((char*)ptr - sizeof(header));
      bool eq1 = (0 == ::memcmp(hdr->eyecatcher, "inspire", 8));
      bool eq2 = (magic == hdr->magic);
      return !(eq1 && eq2);
   }

#ifdef _DEBUG
   void AllocatorMgr::_debug(void* ptr, const char* file, const uint line)
   {
      INSPIRE_ASSERT(NULL != ptr, "ptr cannot be NULL");
      header* hdr = (header*)ptr;
      hdr->file = file;
      hdr->line = line;
   }
#endif
}