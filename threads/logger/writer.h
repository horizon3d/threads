#ifndef _INSPIRE_LOGGER_WRITER_H_
#define _INSPIRE_LOGGER_WRITER_H_

#include "base.h"
#include "util/mutex.h"

namespace inspire {

   class ossFile;
   namespace logger {

      class writerImpl : public logWriter
      {
      public:
#ifdef _DEBUG
         writerImpl(const int priority = LOG_DEBUG);
#else
         writerImpl(const int priority = LOG_ERROR);
#endif
         virtual ~writerImpl();

         virtual void writeLog(const unsigned priority, const char* data);

      protected:
         void initialize();

      protected:
         unsigned _priority;
         mutex_t   _mtx;
         ossFile* _logger;
         char     _filename[MAX_LOG_FILE_NAME + 1];
      };
   }
}
#endif