#ifndef _INSPIRE_LOGGER_WRITER_H_
#define _INSPIRE_LOGGER_WRITER_H_

#include "base.h"
#include "util/system/mutex.h"
#include "util/file/IO.h"

namespace inspire {

   namespace logger {

      class writerImpl : public logWriter
      {
      public:
#ifdef _DEBUG
         writerImpl(const int priority = LOG_DEBUG);
#else
         writerImpl(const int priority = LOG_EVENT);
#endif
         virtual ~writerImpl();

         virtual void writeLog(const unsigned priority, const char* data);

      protected:
         void initialize();

      protected:
         unsigned  _priority;
         mutex_t   _mtx;
         cpsHandle _handle;
         char      _filename[MAX_LOG_FILE_NAME + 1];
      };
   }
}
#endif