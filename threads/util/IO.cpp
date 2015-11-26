#include <iostream>
#include "IO.h"
#include "charConvertor.h"
#include "util.h"
#include "assert.h"

#ifdef _WINDOWS
int interpretMask(const uint mask);
#else
int interpretMask(const uint mask);
#endif

#define MAX_OPEN_RETRY 5

int cpsOpen(cpsHandle& h, char* path, uint mode, uint iPermission)
{
   if (NULL == path)
   {
      LogError("path name is invalid");
      return -1;
   }
   int rc = 0;
   int tryn = 0;
#ifdef _WINDOWS
   inspire::CharConvertor con(path);
   int crMode = 0;  // create mode
   int rwMode = 0;  // read/write mode
   int shMode = 0;  // shared mode
   int attri  = 0;  // direct/sync I/O
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
      if ((mode | SHAREDREAD) == EXCLUSIVE)
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
      shMode = 0;
      break;
   case SHAREDREAD:
      if (mode | ACCESS_WRITEONLY)
      {
         shMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
      }
      else
      {
         shMode = FILE_SHARE_READ;
      }
      break;
   case SHAREDWRITE:
      shMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
      break;
   }

   // I/O
   if (mode & DIRECTIO)
   {
      attri |= FILE_FLAG_NO_BUFFERING;
   }

   if (mode & WRITETHROUGH)
   {
      attri |= FILE_FLAG_WRITE_THROUGH;
   }

   while (true)
   {
      ++tryn;

      h.hHandle = ::CreateFile(con.toString(), rwMode, shMode, NULL, crMode, attri, NULL);
      if (INVALID_HANDLE_VALUE == h.hHandle)
      {
         int err = utilGetLastError();
         if (ERROR_SHARING_VIOLATION == err && tryn < MAX_OPEN_RETRY)
         {
            utilSleep(100);
            continue;
         }
         switch (err)
         {
         case ERROR_SHARING_VIOLATION:
            rc = -1;
            LogError("Permission denied when open file: %s", path);
            break;
         default:
            rc = -1;
            LogError("I/O error when open file: %s", path);
            break;
         }
      }
      break;
   }
#else
   int iMode = 0;
   int direct = 0;
   int perm = ((iPermission != 0) ? iPermission : DEFAULT_FILE_ACCESS);
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
      break;
   default:
      break;
   }

   // read/write access
   switch (mode & ACCESS_READWRITE)
   {
   case ACCESS_WRITEONLY:
      if (mode | SHAREDREAD)
      {
         iMode |= O_RDWR;
      }
      else
      {
         iMode |= O_WRONLY;
      }
      break;
   case ACCESS_READONLY:
      if ((mode & SHAREDREAD) == EXCLUSIVE)
      {
         iMode |= O_RDWR;
      }
      else
      {
         iMode |= O_RDONLY;
      }
      break;
   case ACCESS_READWRITE:
      iMode |= O_RDWR;
      break;
   }

   // I/O
   if (mode & DIRECTIO)
   {
      direct = O_DIRECT;
   }

   if (mode & WRITETHROUGH)
   {
      iMode |= O_SYNC;
   }

   int err = 0;
   while (true)
   {
      ++tryn;
      do 
      {
         h.hHandle = ::open(path, iMode | direct, perm);
      } while ((-1 == h.hHandle) && (EINTR == (err = utilGetLastError())));

      if (-1 == h.hHandle)
      {
         if ((EINVAL == err) && (O_DIRECT == direct))
         {
            direct = 0;
            continue;
         }
         else if ((ETXTBSY == err) && tryn < MAX_OPEN_RETRY)
         {
            utilSleep(100);
            continue;
         }
         rc = -1;
         switch (err)
         {
         case ENOENT:
            LogError("Try to open a file not exist, file: %s", path);
            break;
         case EEXIST:
            LogError("Try to create a file that existed, file: %s", path);
            break;
         case EACCES:
            LogError("Permission denied when open file: %s", path);
            break;
         default:
            LogError("I/O error occurs when open file: %s", path);
            break;
         }
      }
      break;
   }
#endif
   return rc;
}

int cpsRead(cpsHandle& h, char* buffer, const uint64 toRead)
{
   if (NULL == buffer)
   {
      LogError("buffer is point to NULL");
      return -1;
   }
   int rc = 0;

#ifdef _WINDOWS
   if (INVALID_HANDLE_VALUE == h.hHandle)
   {
      LogError("Handle is invalid");
      return -1;
   }
   DWORD bytes = 0;
   BOOL bSuccess = ::ReadFile(h.hHandle, (LPVOID)buffer, (DWORD)toRead, (LPDWORD)&bytes, NULL);
   if (!bSuccess)
   {
      int err = utilGetLastError();
      LogError("Failed to read file: %x, error: %d", h.hHandle, err);
      switch (err)
      {
      case ERROR_HANDLE_EOF:
         LogError("Read at end of file");
         break;
      case ERROR_NOT_ENOUGH_MEMORY:
         LogError("Out of memory when reading");
         break;
      default:
         LogError("I/O error occurs when reading from file");
         break;
      }
      rc = -1;
   }
#else
   if (-1 == h.hHandle)
   {
      LogError("Handle is invalid");
      return -1;
   }
   int bytes = ::read(h.hHandle, buffer, toRead);
   if (-1 == bytes)
   {
      int err = utilGetLastError();
      LogError("Failed to read file: %x, error: %d", h.hHandle, err);
      switch (err)
      {
      case EINTR:
         LogError("Interrupted when reading");
         break;
      case EINVAL:
         LogError("Invalid argument assigned to cpsRead");
         break;
      case ENOMEM:
         LogError("Out of memory when reading");
         break;
      case EBADF:
         LogError("Permission denied when reading");
         break;
      default:
         LogError("I/O error occurs when reading from file");
         break;
      }
      rc = -1;
   }
#endif
   rc = (int)bytes;
   return rc;
}

int cpsWrite(cpsHandle& h, const char* buffer, const uint64 toWrite)
{
   if (NULL == buffer)
   {
      LogError("buffer is point to NULL");
      return -1;
   }

   int rc = 0;
#ifdef _WINDOWS
   if (INVALID_HANDLE_VALUE == h.hHandle)
   {
      LogError("Handle is invalid");
      return -1;
   }

   DWORD bytes = 0;
   BOOL bSuccess = ::WriteFile(h.hHandle, buffer, (DWORD)toWrite, (LPDWORD)&bytes, NULL);
   if (!bSuccess)
   {
      int err = utilGetLastError();
      LogError("Failed to write file: %x, error: %d", h.hHandle, err);
      switch (err)
      {
      case ERROR_HANDLE_EOF:
         LogError("Write at end of file");
         break;
      case ERROR_NOT_ENOUGH_MEMORY:
         LogError("Out of memory when writing");
         break;
      default:
         LogError("I/O error occurs when writing to file");
         break;
      }
      rc = -1;
   }
#else
   int bytes = ::write(h.hHandle, buffer, toWrite);
   if (-1 == bytes)
   {
      int err = utilGetLastError();
      LogError("Failed to write file: %x, error: %d", h.hHandle, err);
      switch (err)
      {
      case EINTR:
         LogError("Interrupted when writing");
         break;
      case EINVAL:
         LogError("Invalid argument assigned to cpsWrite");
         break;
      case ENOSPC:
         LogError("Not enough space left on disk");
         break;
      case EBADF:
         LogError("Permission denied when writing");
         break;
      default:
         LogError("I/O error occurs when writing to file");
         break;
      }
      rc = -1;
   }
#endif
   rc = (int)bytes;
   return rc;
}

int cpsClose(cpsHandle& h)
{
   int rc = 0;
#ifdef _WINDOWS
   if (INVALID_HANDLE_VALUE != h.hHandle)
   {
      BOOL fOk = ::CloseHandle(h.hHandle);
      if (!fOk)
      {
         LogError("I/O error occurs when closing handle: %x", h.hHandle);
         rc = -1;
      }
      else
      {
         h.hHandle = INVALID_HANDLE_VALUE;
      }
   }
#else
   if (-1 != h.hHandle)
   {
      if (0 != ::close(_fd))
      {
         LogError("I/O error occurs when closing handle: %x", h.hHandle);
         rc = -1;
      }
      else
      {
         _fd = -1;
      }
   }
#endif
   return rc;
}

int cpsSeek(cpsHandle& h, int64 offset, SEEK_MOD whence)
{
   int rc = 0;
#ifdef _WINDOWS
   LARGE_INTEGER li;
   li.QuadPart = offset;
   li.LowPart = ::SetFilePointer(h.hHandle, li.LowPart, &li.HighPart, (DWORD)whence);
   if (INVALID_SET_FILE_POINTER == li.LowPart && NO_ERROR == utilGetLastError())
   {
      int err = utilGetLastError();
      LogError("I/O error occurs when seeking, error: %d", err);
      rc = -1;
   }
#else
   int64 seekoff = 0;
   seekoff = lseek(_fd, offset, whence);
   if (-1 == seekoff)
   {
      int err = utilGetLastError();
      LogError("Failed to seek file: %x, error: %d", h.hHandle, err);
      switch (err)
      {
      case EINTR:
         LogError("Interrupted when seeking");
         break;
      case EINVAL:
         LogError("Invalid argument assigned to cpsSeek");
         break;
      case EBADF:
         LogError("Permission denied when seeking");
         break;
      default:
         LogError("I/O error occurs when seeking to file");
         break;
      }
      rc = -1;
   }
#endif
   return rc;
}

int cpsGetFileSize(cpsHandle& h, int64& totalSize)
{
   int rc = 0;
#ifdef _WINDOWS
   LARGE_INTEGER li;
   if (!GetFileSizeEx(h.hHandle, &li))
   {
      int err = utilGetLastError();
      LogError("I/O error occurs when getting file size, error: %d", err);
      rc = -1;
   }
   totalSize = li.QuadPart;
#else
   struct stat sb;
   int rc = 0;
   rc = fstat(_fd, &sb);
   if (-1 == rc)
   {
      int err = utilGetLastError();
      LogError("I/O error occurs when getting file size, error: %d", err);
      switch (err)
      {
      case EINVAL:
         LogError("Invalid argument assigned to %s", __FUNCTION__);
         break;
      default:
         LogError("I/O error occurs when seeking to file");
         break;
      }
      rc = -1;
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
         LogError("Unknown file type");
         rc = -1;
         break;
      }
   }
#endif
   return rc;
}

