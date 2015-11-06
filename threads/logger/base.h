#ifndef _INSPIRE_LOGGER_BASE_H_
#define _INSPIRE_LOGGER_BASE_H_

namespace inspire {
   /*
   * the bigger of value, the more logs
   **/
   enum PRIORITY
   {
      LOG_ALWAYS = 0,   // print always
      LOG_EMERG = 100,
      LOG_FATAL = 200,
      LOG_CRITICAL = 300,
      LOG_ERROR = 400,
      LOG_EVENT = 500,
      LOG_WARNING = 600,
      LOG_DEBUG = 700,
      LOG_NOTICE = 800, // for expected
      LOG_INFO = 900,
      LOG_DEFAULT = LOG_WARNING,
   };

   namespace logger {

      // macros
      #define LOG_BUFFER_SIZE 4096
      #define MAX_LOG_FILE_NAME 255

      #if defined(_WINDOWS)
         #define NEWLINE "\r\n"
         #define SEP "\\"
         #define CHAR_SEP '\\'
      #else
         #define NEWLINE "\n"
         #define SEP "/"
         #define CHAR_SEP '/'
      #endif

      class logWriter
      {
      public:
         //logWriter(const char* filename);
         virtual ~logWriter() {}

         virtual void writeLog(const unsigned priority, const char* data) = 0;
      };

      const char* toString(const unsigned priority);

      void append(const unsigned level, const char* func,
                  const char* file, const unsigned line,
                  const char* fmt, ...);

      extern logWriter* loggerWriter();
   }
}
#endif
