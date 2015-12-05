#include <ctime>
#include "util/system/util.h"
#include "writer.h"
#include "util/system/condition.h"
#include "util/assert.h"

namespace inspire {

   namespace logger {

      writerImpl::writerImpl(const int priority) : _priority(priority)
      {
         initialize();
      }

      writerImpl::~writerImpl()
      {
         if (HandleValid(_handle))
         {
            cpsClose(_handle);
         }
      }

      void writerImpl::writeLog(const unsigned priority, const char* data)
      {
         if (_priority < priority)
         {
            return;
         }

         unsigned len = strlen(data);
         condition_t cond(&_mtx);
         int rc = cpsOpen(_handle, _filename, MODE_CREATE | ACCESS_READWRITE, DEFAULT_FILE_ACCESS);
         STRONG_ASSERT(-1 != rc, "Failed to open log file: %s", _filename);
         rc = cpsSeek(_handle, 0, INSPIRE_SEEK_END);
         STRONG_ASSERT(-1 != rc, "Failed to seek to the end of file: %s", _filename);
         rc = cpsWrite(_handle, data, len);
         STRONG_ASSERT(-1 != rc, "Failed to write log to file: %s", _filename);
         rc = cpsClose(_handle);
         STRONG_ASSERT(-1 != rc, "Failed to close file: %s", _filename);
      }

      void writerImpl::initialize()
      {
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
         int rc = cpsOpen(_handle, _filename, MODE_CREATE | ACCESS_READWRITE, DEFAULT_FILE_ACCESS);
         STRONG_ASSERT(-1 != rc, "Failed to open log file: %s", _filename);
         rc = cpsClose(_handle);
      }

      static writerImpl writer;
      logWriter* loggerWriter()
      {
         return &writer;
      }
   }
}