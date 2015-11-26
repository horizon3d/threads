#include <ctime>
#include "base.h"
#include "util/util.h"

namespace inspire {

   namespace logger {

#ifdef _WINDOWS
      const static char *logFmt = \
         "%04d-%02d-%02d-%02d.%02d.%-25dLevel: %s\r\nPID: %-37lldTID:   %lld" \
         "\r\nFunction: %-32sLine:  %d\r\nFile: %s\r\nMessage:\r\n%s\r\n\r\n";
#else
      const static char *logFmt = \
         "%04d-%02d-%02d-%02d.%02d.%-25dLevel: %s\nPID: %-37lldTID:   %lld" \
         "\nFunction: %-32sLine:  %d\nFile: %s\nMessage:\n%s\n\n";
#endif

      const char* toString(const unsigned priority)
      {
         static const char* typeMap[] = {
            "Always", "Emergency", "Fatal", "Critical", "Error",
            "Event", "Debug", "Warning", "Notice", "Info"
         };

         if (priority < 0 || priority > 900)
            return "Unknown priority";
         else
            return typeMap[priority / 100];
      }

      void append(const unsigned level, const char* func,
                  const char* file, const unsigned line,
                  const char* fmt, ...)
      {
         struct tm otm;
         time_t tt = time(NULL);
#ifdef _WINDOWS
         ::localtime_s(&otm, &tt);
#else
         ::localtime_r(&tt, &otm);
#endif
         char userInfo[LOG_BUFFER_SIZE] = { 0 };
         va_list ap;
         va_start(ap, fmt);
         vsnprintf(userInfo, LOG_BUFFER_SIZE, fmt, ap);
         va_end(ap);

         char buffer[LOG_BUFFER_SIZE] = { 0 };
         utilSnprintf(buffer, LOG_BUFFER_SIZE, logFmt,
                      otm.tm_year + 1900, otm.tm_mon + 1, otm.tm_mday,
                      otm.tm_hour, otm.tm_min, otm.tm_sec, toString(level),
                      utilGetCurrentPid(), utilGetCurrentThreadId(),
                      func, line, file, userInfo);
         loggerWriter()->writeLog(level, buffer);
      }
   }
}