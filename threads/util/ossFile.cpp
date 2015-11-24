#include "ossFile.h"
#include "util.hpp"
#include "charConvertor.h"

namespace inspire {

#ifdef _WIN32
   ossFile::ossFile() : _handle(NULL)
#else
   ossFile::ossFile() : _fd(0)
#endif
   {
   }

   ossFile::~ossFile()
   {
      close();
   }

   bool ossFile::isOpen() const
   {
#ifdef _WIN32
      return INVALID_HANDLE_VALUE != _handle;
#else
      return (0 != _fd) && (-1 != _fd);
#endif
   }

   int ossFile::open(const char* filename, const unsigned mode, const unsigned permission)
   {
      int rc = 0;
      if (NULL == filename)
      {
         return -1;
      }

      int iPermission = _matchPermission(permission);
#ifdef _WIN32
      CharConvertor fname(filename);
      int crMode = 0;
      int rwMode = 0;
      int sharedMode = 0;
      int attri = 0;
      _matchMode(mode, crMode, rwMode, sharedMode, attri);
      _handle = ::CreateFile(fname.toString(), rwMode, sharedMode, NULL, crMode, attri, NULL );
      if (INVALID_HANDLE_VALUE == _handle)
      {
         rc = utilGetLastError();
      }
#else
      int iMode = 0;
      _matchMode(mode, iMode);
      _fd = open(filename, iMode, iPermission);
      if (-1 == _fd)
      {
         return utilGetLastError();
//          uint rc = errno;
//          if (EINVAL == rc)
//          {
//             LogError("Failed to open file, filename = %s, direct io exist", _filename);
//          }
//          else if (ETXTBSY == rc)
//          {
//             LogError("Failed to open file, filename = %s, system busy", _filename);
//          }
//          else if (ENOENT == rc)
//          {
//             LogError("Failed to open file, filename = %s, file not exist", _filename);
//          }
//          else if (EEXIST == rc)
//          {
//             LogError("Failed to open file, filename = %s, file existed", _filename);
//          }
//          else if (EACCES == rc)
//          {
//             LogError("Failed to open file, filename = %s, no permission", _filename);
//          }
//          else
//          {
//             LogError("Failed to open file, filename = %s, io exception", _filename);
//          }
      }
#endif
      return rc;
   }

   int ossFile::read(char* buffer, const unsigned bufferLen,
                     const unsigned toRead, unsigned& totalRead)
   {
      int rc = 0;
      if (NULL == buffer)
      {
         return -1; // INVALID ARGUMENT
      }

      if (bufferLen < toRead)
      {
         return -1;
      }

#ifdef _WIN32
      DWORD bytes = 0;
      BOOL bSuccess = ::ReadFile(_handle, (LPVOID)buffer, toRead, (LPDWORD)&bytes, NULL);
      if (!bSuccess)
      {
         return utilGetLastError();
      }
#else
      int bytes = ::read(_fd, buffer, toRead);
      if (-1 == bytes)
      {
         return utilGetLastError();
      }
#endif
      totalRead = (unsigned)bytes;
      return rc;
   }

   int ossFile::write(const char* buffer, const unsigned bufferLen,
                      const unsigned toWrite, unsigned& totalWrite)
   {
      int rc = 0;
      if (NULL == buffer)
      {
         return -1;
      }

      if (NULL == buffer)
      {
         return 0;
      }

#ifdef _WIN32
      DWORD bytes = 0;
      BOOL bSuccess = ::WriteFile(_handle, buffer, toWrite, (LPDWORD)&bytes, NULL);
      if (!bSuccess)
      {
         return utilGetLastError();
      }
#else
      int bytes = ::write(_fd, buffer, toWrite);
      if (-1 == bytes)
      {
         return utilGetLastError();
      }
#endif
      totalWrite = (unsigned)bytes;
      return rc;
   }

   void ossFile::close()
   {
      if (isOpen())
      {
#ifdef _WIN32
         ::CloseHandle(_handle);
         _handle = INVALID_HANDLE_VALUE;
#else
        ::close(_fd);
        _fd = -1;
#endif
      }
   }

   const unsigned long long ossFile::filesize()
   {
      int64 totalSize = 0;
#ifdef _WIN32
      LARGE_INTEGER li;
      if (!GetFileSizeEx(_handle, &li))
      {
         return utilGetLastError();
      }
      totalSize = li.QuadPart;
#else
      struct stat sb;
      int rc = 0;
      rc = fstat(_fd, &sb);
      if (-1 == rc)
      {
         return utilGetLastError();
      }
      else
      {
         switch (sb.st_mode & S_IFMT)
         {
         case S_IFLNK:
         case S_IFREG:
            totalSize = sb.st_size;
            break;
         default:
            rc = -1;
            break;
         }
      }
#endif
      return totalSize;
   }

   int ossFile::seek( const unsigned long long offset, SEEK_MOD whence )
   {
      int rc = 0;
#ifdef _WIN32
      LARGE_INTEGER li;
      li.QuadPart = offset;
      li.LowPart = SetFilePointer( _handle, li.LowPart, &li.HighPart, (DWORD)whence );
      if (INVALID_SET_FILE_POINTER == li.LowPart && NO_ERROR == utilGetLastError())
      {
         // I/O exception occurred when seek
         rc = -1;
      }
#else
      int64 seekoff = 0;
      seekoff = lseek(_fd, offset, whence);
      if (-1 == seekoff)
      {
         rc = -1;
      }
#endif
      return rc;
   }

   int ossFile::seekToEnd()
   {
     return seek(0, INSPIRE_SEEK_END);
   }

#ifdef _WIN32
   void ossFile::_matchMode( const int mode, int& crMode, int& rwMode, int& sharedMode, int& attri)
   {
      // create/open mode
      switch (mode & MODE_CREATE)
      {
      case MODE_DEFAULT:
         crMode = OPEN_EXISTING;
         break;
      case MODE_CREATEONLY:
         crMode = CREATE_NEW;
         break;
      case MODE_REPLACE:
         crMode = CREATE_ALWAYS;
         break;
      case MODE_CREATE:
         crMode = OPEN_ALWAYS;
         break;
      }

      // read/write access
      switch (mode & ACCESS_READWRITE)
      {
      case ACCESS_WRITEONLY:
         if (mode & SHAREDREAD)
         {
            rwMode = GENERIC_READ | GENERIC_WRITE;
         }
         else
         {
            rwMode = GENERIC_WRITE;
         }
         break;
      case ACCESS_READONLY:
         if ((mode | SHAREDREAD) == EXCLUSIVE )
         {
            rwMode = GENERIC_READ | GENERIC_WRITE;
         }
         else
         {
            rwMode = GENERIC_READ;
         }
         break;
      case ACCESS_READWRITE:
         rwMode = GENERIC_READ | GENERIC_WRITE;
         break;
      }

      // shared mode
      switch (mode & SHAREDWRITE)
      {
      case EXCLUSIVE:
         sharedMode = 0;
         break;
      case SHAREDREAD:
         if (mode | ACCESS_WRITEONLY)
         {
            sharedMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
         }
         else
         {
            sharedMode = FILE_SHARE_READ ;
         }
         break;
      case SHAREDWRITE:
         sharedMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
         break;
      }

      // I/O
      if (mode & DIRECTIO)
      {
         attri |= FILE_FLAG_NO_BUFFERING;
      }

      if(mode & WRITETHROUGH)
      {
         attri |= FILE_FLAG_WRITE_THROUGH;
      }
   }
#else
   void ossFile::_matchMode(const int mode, int& iMode)
   {
      iMode = 0;
      // create/open mode
      switch (mode & MODE_CREATE)
      {
      case MODE_DEFAULT:
         iMode = 0;
         break;
      case MODE_CREATEONLY:
         iMode = O_CREAT | O_EXCL;
         break;
      case MODE_REPLACE:
         iMode = O_CREAT | O_TRUNC;
         break;
      case MODE_CREATE:
         iMode = O_CREAT;
      default:
         break;
      }

      // read/write access
      switch (mode & ACCESS_READWRITE)
      {
      case ACCESS_WRITEONLY:
         if(mode | SHAREDREAD)
         {
            iMode |= O_RDWR;
         }
         else
         {
            iMode |= O_WRONLY;
         }
      case ACCESS_READONLY:
         if ((mode | SHAREDREAD) == EXCLUSIVE)
         {
            iMode |= O_RDWR;
         }
         else
         {
            iMode |= O_RDONLY;
         }
      case ACCESS_READWRITE:
         iMode |= O_RDWR;
         break;
      }

      // I/O
      if (mode | DIRECTIO)
      {
         iMode = O_DIRECT;
      }

      if (mode | WRITETHROUGH)
      {
         iMode |= O_SYNC;
      }
   }
#endif

   const int ossFile::_matchPermission( const int permission )
   {
      if ( 0 == permission )
      {
         return DEFAULT_FILE_ACCESS;
      }
      return permission;
   }
}