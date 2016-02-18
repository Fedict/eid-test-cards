#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef _WIN32

#include <unistd.h>

int strcpy_s(char *dest, size_t len, const char *src)
{
	if (dest == NULL)
		return -1;

	for (; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

int fopen_s(FILE** pFile, const char *filename, const char *mode)
{
	int r = 0;

	if (pFile == NULL)
		return -1;

	FILE *f = fopen(filename, mode);

	if (f != NULL)
		*pFile = f;
	else
		r = -1;

	return r;
}

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
	va_list args;
	char    *csTmp = NULL;
	int     r      = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = strcpy_s(buffer, sizeOfBuffer, csTmp);
		free(csTmp);
	}

	return r;
}

char *strtok_s(char *strToken, const char *strDelimit, char **context)
{
	return strtok_r(strToken, strDelimit, context);
}

errno_t strcat_s(char *strDestination, size_t numberOfElements, const char *strSource)
{
	strncat(strDestination, strSource, numberOfElements);
	return 0;
}

int fprintf_s(FILE *stream, const char *format, ...)
{
	va_list args;
	char    *csTmp = NULL;
	int     r      = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, "%s", csTmp);
		free(csTmp);
	}

	return r;
}

int vfprintf_s(FILE *stream, const char *format, va_list argptr)
{
	char *csTmp = NULL;
	int  r      = -1;

	r = vasprintf(&csTmp, format, argptr);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, "%s", csTmp);
		free(csTmp);
	}

	return r;
}


void Sleep(unsigned long dwMillisecs)
{
	usleep(1000 * dwMillisecs);
}

#endif

