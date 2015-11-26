#ifndef _INSPIRE_UTIL_IO_H_
#define _INSPIRE_UTIL_IO_H_

#include <fcntl.h>
#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#endif
#include "inspire.h"

enum SEEK_MOD
{
   INSPIRE_SEEK_SET = 0,
   INSPIRE_SEEK_CUR,
   INSPIRE_SEEK_END,
};

enum RW_MOD
{
   MODE_DEFAULT    = 0,
   MODE_CREATEONLY = 1 << 0,
   MODE_REPLACE    = 1 << 1,
   MODE_CREATE     = MODE_CREATEONLY | MODE_REPLACE,
};

enum ACCESS_MODE
{
   ACCESS_READONLY  = 1 << 2,
   ACCESS_WRITEONLY = 1 << 3,
   ACCESS_READWRITE = ACCESS_READONLY | ACCESS_WRITEONLY
};

enum SHARE_MODE
{
   EXCLUSIVE = 0,
   SHAREDREAD   = 1 << 4,
   SHAREDWRITE  = 1 << 5 | SHAREDREAD,
   WRITETHROUGH = 1 << 6,
   DIRECTIO     = 1 << 7,
};

#ifdef _WINDOWS
enum ACCESS_PERMITION
{
   ACCESS_STICKY = 0,

   U_READ  = 1 << 10,
   U_WRITE = 1 << 9,
   U_EXEC  = 1 << 8,
   U_RWX   = U_READ | U_WRITE | U_EXEC,

   G_READ  = 1 << 6,
   G_WRITE = 1 << 5,
   G_EXEC  = 1 << 4,
   G_RWX   = G_READ | G_WRITE | G_EXEC,

   O_READ  = 1 << 2,
   O_WRITE = 1 << 1,
   O_EXEC  = 1 << 0,
   O_RWX   = O_READ | O_WRITE | O_EXEC,

   DEFAULT_FILE_ACCESS = U_RWX | G_READ | G_EXEC, // rwxr-x---
   DEFAULT_DIR_ACCESS = U_RWX | G_READ | G_EXEC, // rwxr-x---
};
#else
enum ACCESS_PERMITION
{
   ACCESS_STICKY = S_ISVTX,

   U_READ  = S_IRUSR,
   U_WRITE = S_IWUSR,
   U_EXEC  = S_IXUSR,
   U_RWX   = S_IRWXU,

   G_READ  = S_IRGRP,
   G_WRITE = S_IWGRP,
   G_EXEC  = S_IXGRP,
   G_RWX   = S_IRWXG,

   O_READ  = S_IROTH,
   O_WRITE = S_IWOTH,
   O_EXEC  = S_IXOTH,
   O_RWX   = S_IRWXO,

   DEFAULT_FILE_ACCESS = U_READ | U_WRITE | G_READ, // rw-r-----
   DEFAULT_DIR_ACCESS  = U_RWX | G_READ | G_EXEC,   // rwxr-x---
};
#endif

// cps: cross platform(s)
// difference should be processed differently between operator systems
// so packet them and supply a same interface
// those interfaces will be named cpsXXXX prefix
struct cpsHandle
{
#ifdef _WINDOWS
   HANDLE hHandle;
#else
   int    hHandle;
#endif
};

inline bool HandleValid(cpsHandle& h)
{
#ifdef _WINDOWS
   return INVALID_HANDLE_VALUE == h.hHandle;
#else
   return -1 == h.hHandle;
#endif // _WINDOWS
}

/*
* open a file/device
* h: the handle/fd out if success
* path: the file/device to be open
* mode: how to open the file
* iPermission: how we set the access permission if we create a file
* return: 0 is ok, or errno
*/
int cpsOpen(cpsHandle& h, char* path, uint mode = 0, uint iPermission = 0);

int cpsRead(cpsHandle& h, char* buffer, const uint64 toRead);

int cpsWrite(cpsHandle& h, const char* buffer, const uint64 toWrite);

int cpsClose(cpsHandle& h);

int cpsSeek(cpsHandle& h, int64 offset, SEEK_MOD whence);

int cpsGetFileSize(cpsHandle& h, int64& totalSize);

#endif