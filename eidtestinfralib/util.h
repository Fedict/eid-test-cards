#ifndef __UTIL_H__
#define __UTIL_H__

#include <string.h>

#ifdef UNICODE
#undef UNICODE
#endif

#ifndef _WIN32

#include <stdio.h>

typedef int   errno_t;

int fopen_s(FILE** pFile, const char *filename, const char *mode);

int strcpy_s(char *dest, size_t len, const char *src);

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...);

char *strtok_s(char *strToken, const char *strDelimit, char **context);

errno_t strcat_s(char *strDestination, size_t numberOfElements, const char *strSource);

int fprintf_s(FILE *stream, const char *format, ...);

int vfprintf_s(FILE *stream, const char *format, va_list argptr);

void Sleep(unsigned long dwMillisecs);

#endif  // _WIN32

#ifdef __APPLE__
typedef const char * LPCTSTR;
#endif

#endif
