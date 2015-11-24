#include <ctime>
#include "util/util.hpp"
#include "writer.h"
#include "util/condition.h"
#include "util/ossFile.h"

namespace inspire {

   namespace logger {

      writerImpl::writerImpl(const int priority) : _priority(priority)
      {
         initialize();
      }

      writerImpl::~writerImpl()
      {
         if (_logger)
         {
            delete _logger;
            _logger = NULL;
         }
      }

      void writerImpl::writeLog(const unsigned priority, const char* data)
      {
         if (_priority <= priority)
         {
            return;
         }

         unsigned len = strlen(data);
         unsigned written = 0;
         condition_t cond(&_mtx);
         _logger->open(_filename, MODE_CREATE | ACCESS_READWRITE, DEFAULT_FILE_ACCESS);
         _logger->seekToEnd();
         _logger->write(data, len + 1, len, written);
         _logger->close();
      }

      void writerImpl::initialize()
      {
         int rc = 0;
         struct tm otm;
         time_t tt = time(NULL);
#ifdef _WINDOWS
         ::localtime_s(&otm, &tt);
#else
         ::localtime_r(&tt, &otm);
#endif
         utilSnprintf(_filename, MAX_LOG_FILE_NAME, "%04d-%02d-%02d-%02d.%02d.%02d.log",
                      otm.tm_year + 1900, otm.tm_mon + 1, otm.tm_mday,
                      otm.tm_hour, otm.tm_min, otm.tm_sec);
         _logger = new ossFile();
         rc = _logger->open(_filename, MODE_CREATE | ACCESS_READWRITE, DEFAULT_FILE_ACCESS);
         if (rc)
         {
            // TODO:
         }
         _logger->close();
      }

      static writerImpl writer;
      logWriter* loggerWriter()
      {
         return &writer;
      }
   }
}